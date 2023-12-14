#!/usr/bin/env Rscript

library("countrycode")
library("plyr")
library("tidyr")
library("stringr")
library("readr")
library('dplyr')
if(!require("tictoc")){
  install.packages("tictoc")
}

library("tictoc")
library(Rcpp)

# For parallelization
library(foreach)
library(doParallel)

stub <- function() {}
thisPath <- function() {
  cmdArgs <- commandArgs(trailingOnly = FALSE)
  if (length(grep("^-f$", cmdArgs)) > 0) {
    # R console option
    normalizePath(dirname(cmdArgs[grep("^-f", cmdArgs) + 1]))[1]
  } else if (length(grep("^--file=", cmdArgs)) > 0) {
    # Rscript/R console option
    scriptPath <- normalizePath(dirname(sub("^--file=", "", cmdArgs[grep("^--file=", cmdArgs)])))[1]
  } else if (Sys.getenv("RSTUDIO") == "1") {
    # RStudio
    dirname(rstudioapi::getSourceEditorContext()$path)
  } else if (is.null(attr(stub, "srcref")) == FALSE) {
    # 'source'd via R console
    dirname(normalizePath(attr(attr(stub, "srcref"), "srcfile")$filename))
  } else {
    stop("Cannot find file path")
  }
}

# Sys.setlocale("LC_CTYPE", "en_US.UTF-8")
sourceCpp(paste0(thisPath(), "/fishertest_continuous_sliding_fisher.cc"))

args = commandArgs(trailingOnly=TRUE)
file <- args[1]
freq_threshold <- as.numeric(args[2])
output_folder <- args[3]

#file <- "~/testdata/metadata_subset_50.tsv"
#freq_threshold <- 0.1
#output_folder <- "~/testdata/results/"

# variants of concern
# might be more conventient to read via a file in case this list needs to be updated in the future
voc <- c("B.1.1.7", "B.1.351", "P.1", "B.1.617", "B.1.617.1", "B.1.617.2", "B.1.617.3") 
# variants of concern: uk, south africa, brazil, india
# additionally maybe: B.1.427, B.1.429, B.1.526, B.1.526.1, B.1.526.2 (all US), P.2 (Brazil)?


# function to perform sliding window fisher test on all data used as input
sliding_fisher <- function(gisaid_data, w, step, freq_threshold, voc){ 
  all_lineages <- sort(unique(gisaid_data$pangolin_lineage))
  all_lineages <- all_lineages[all_lineages != "" & all_lineages != "None"]
  
  if (nrow(gisaid_data) < w*2){
    print("Not enough data for chosen window size.")
    return(data.frame(lineage = character(),
      date = character(),
      pval = numeric(),
      odds = numeric(),
      freq = numeric(),
      stringsAsFactors = FALSE))
  }
  window_steps <- seq(w, (nrow(gisaid_data)-w), step) # respective start of the current window
  
  pval_matrix <- matrix(data = NA, ncol = length(window_steps), nrow = length(all_lineages))
  odds_matrix <- matrix(data = NA, ncol = length(window_steps), nrow = length(all_lineages))
  freq_matrix <- matrix(data = NA, ncol = length(window_steps), nrow = length(all_lineages))
  
  print("Lineages to be analyzed: ") 
  print(paste(all_lineages, collapse = ", "))
  for (l in 1:length(all_lineages)){
    # print(all_lineages[l])
    # get 0/1 vector for current lineage
    lineage_vector <- as.integer(gisaid_data$pangolin_lineage == all_lineages[l])
    # print(lineage_vector[is.na(lineage_vector)])
    counter <- 1
    print(paste0("Analyzing lineage: ", all_lineages[l]))
    for (i in window_steps){
      start <- i + 1
      end <- i + w
      current_window <- lineage_vector[start:end]
      # previous window the same size as the current one
      previous_window <- lineage_vector[(start - w):(start - 1)] 

      # get values for contigency table
      mut_thisseason <- sum(current_window)
      mut_prevseason <- sum(previous_window)
      
      nomut_thisseason <- sum(current_window == 0)
      nomut_prevseason <- sum(previous_window == 0)
      
      # if no the specified lineage in current window and previous window
      if (mut_thisseason == 0 && mut_prevseason == 0) {
        pval_matrix[l, counter] <- 1
        odds_matrix[l, counter] <- 0
        freq_matrix[l, counter] <- 0
      }
      else{
        # calculate fisher test and save pvalue in matrix
        data <- matrix(c(mut_thisseason, nomut_thisseason, mut_prevseason, nomut_prevseason), nrow = 2)
        test <- fisher.test(data, alternative = "greater")
        pval_matrix[l, counter] <- test$p.value
        odds_matrix[l, counter] <- test$estimate
        freq_matrix[l, counter] <- mut_thisseason / w
      }
      # calculate fisher test and save pvalue in matrix
      #data <- matrix(c(mut_thisseason, nomut_thisseason, mut_prevseason, nomut_prevseason), nrow = 2)
      #test <- fisher.test(data, alternative = "greater")
      #pval_matrix[l, counter] <- test$p.value
      #odds_matrix[l, counter] <- test$estimate
      #freq_matrix[l, counter] <- mut_thisseason / w
      counter <- counter + 1
    }
  }
  
  # correction for multiple testing
  print("Correction for multiple testing")
  # FDR correction using Benjamini & Yekutieli for dependent tests
  pval_adjusted <- matrix(p.adjust(pval_matrix, method = "BY"),
    nrow = length(all_lineages)) 
  sign_results <- data.frame(lineage = character(),
    date = character(),
    pval = numeric(),
    odds = numeric(),
    freq = numeric(),
    stringsAsFactors = FALSE)

  for (j in 1:nrow(pval_adjusted)) {
    if (all_lineages[j] %in% voc) {
      # if variant of concern: look for significant results without frequency threshold
      results_test <- pval_adjusted[j,] < 0.05
    } else {
      # otherwise: use frequency threshold
      results_test <- pval_adjusted[j,] < 0.05 & freq_matrix[j,] > freq_threshold
    }
    first_sign <- which(results_test == TRUE)[1]

    if (! is.na(first_sign)) {
      sign_results <- rbind(sign_results,
      data.frame(lineage = all_lineages[j],
        date = gisaid_data[(window_steps + w)[first_sign],]$date,
        pval = pval_adjusted[j, first_sign],
        odds = odds_matrix[j, first_sign],
        freq = freq_matrix[j, first_sign])
      )
    }
  }
  print("Complete")
  return(sign_results) #Commented out this line
}

# read data
#columns <- c("gisaid_epi_isl", "date", "country", "division", "host", "pango_lineage")
#columns <- c("Accession.ID", "Collection.date", "Location", "Host", "Pango.lineage")
#gisaid_data_all <- read.csv2(file, sep = "\t", header = TRUE, stringsAsFactors = FALSE)
#gisaid_data_all <- read_tsv_chunked(
#  file,
#  DataFrameCallback$new(f),
#  chunk_size = 10000,
#  col_names = TRUE,
#  col_types = NULL,
#  locale = default_locale(),
#  na = c("", "NA"),
#  quoted_na = TRUE,
#  quote = "\"",
#  comment = "",
#  trim_ws = TRUE,
#  skip = 0,
#  guess_max = 10000,
#  skip_empty_rows = TRUE
#)
gisaid_data_all_tibble <- read_tsv(
  file,
  col_names = TRUE,
  col_types = NULL,
  locale = default_locale(),
  na = c("", "NA"),
  quoted_na = TRUE,
  quote = "\"",
  comment = "",
  trim_ws = TRUE,
  skip = 0,
  n_max = Inf,
  guess_max = min(1000, Inf),
  skip_empty_rows = TRUE,
)

gisaid_data_all <- as.data.frame(gisaid_data_all_tibble) %>% 
  select(`Pango lineage`, `Collection date`, `Location`, `Host`) %>%
  rename(pangolin_lineage = `Pango lineage`,
        date = `Collection date`
  ) %>%
  filter(Host=="Human") %>%
  filter(!is.na(pangolin_lineage),
    pangolin_lineage != "Unassigned",
    pangolin_lineage != "",
    !grepl("/", pangolin_lineage, fixed = TRUE)) %>%
  filter(grepl("^20\\d{2}-\\d{2}-\\d{2}$", date))

# gisaid_data_all <- gisaid_data_all[order(gisaid_data_all$date),]


# add country and division column
#metadata %>% drop_na(Location)
#metadata %>% separate(Location, c("region", "country", "division"), " / ")

# added city
gisaid_data_all[c("region", "country", "division", "city")] <- str_split_fixed(gisaid_data_all$Location, " / ", 4)


print(head(gisaid_data_all))

# create list of dataframes for all country
list_gisaid_data_country <- split(gisaid_data_all, gisaid_data_all$country)
# list_gisaid_data_country <- gisaid_data_all %>%
#   group_split(country)


# Filter out country dataframes with fewer than 2000 rows
list_gisaid_data_country <- list_gisaid_data_country[sapply(list_gisaid_data_country, nrow) >= 2000]

countries <- names(list_gisaid_data_country)

print("Analysis for countries: ")
print(paste(countries, collapse = ", "))

w = 1000
step = 100


## Use detectCores() to find the total number of cores
#num_cores <- parallel::detectCores()
#
## Register the parallel backend
#cl <- makeCluster(num_cores-4)
#registerDoParallel(cl)
#
## Process the dataframes in parallel
#results <- foreach(country = names(list_gisaid_data_country),
#                  # .combine = 'list',
#                  .packages = c('dplyr', 'countrycode')) %dopar% {
#  country_df <- list_gisaid_data_country[[country]] %>%
#    arrange(date)
#  country_code <- countrycode(country, origin = 'country.name', destination = 'iso2c', nomatch = NULL)
#  # skip the country without a country code
#  if (is.na(country_code)) {
#    next
#  }
#
#  sign_results <- sliding_fisher(country_df, w, step, freq_threshold = freq_threshold, voc) # Changed freq_threshold from 0.1 to freq_threshold 
#
#  # create folder and save significant results
#  output_path <- paste(output_folder, "/", country_code, sep = "")
#  if (!dir.exists(output_path)) {
#    dir.create(output_path)
#  }
#  write.table(sign_results,  paste(output_path, "/", country_code, "_sign_continuous.txt", sep = ""), 
#    sep = "\t", quote = FALSE, row.names = FALSE, col.names = TRUE)
#}
#
#stopCluster(cl)

#sliding_fisher_all_c(gisaid_data_all$pangolin_lineage, countrycode(gisaid_data_all$country, origin = 'country.name', destination = 'iso2c', nomatch = NULL), gisaid_data_all$date, countries, w, step, freq_threshold, voc, output_folder, p.adjust)
sliding_fisher_all_c(gisaid_data_all$pangolin_lineage, countrycode(gisaid_data_all$country, origin = 'country.name', destination = 'iso2c', nomatch = NULL), gisaid_data_all$date, countrycode(countries, origin = 'country.name', destination = 'iso2c', nomatch = NULL), w, step, freq_threshold, voc, output_folder, p.adjust)

# for (country in names(list_gisaid_data_country)){
#   country_df <- list_gisaid_data_country[[country]] %>%
#     arrange(date)
#   country_code <- countrycode(country, origin = 'country.name', destination = 'iso2c', nomatch = NULL)
#   # skip the country without a country code
#   if (is.na(country_code)){
#     next
#   }
#   print("Analyzing country: ")
#   print(country)
#   # gisaid_data <- gisaid_data_all[gisaid_data_all$country_code == countries[c],]
#   sign_results <- sliding_fisher(country_df, w, step, freq_threshold = freq_threshold, voc) # Changed freq_threshold from 0.1 to freq_threshold 

#   # create folder and save significant results
#   output_path <- paste(output_folder, "/", country_code, sep = "")
#   if (!dir.exists(output_path)) {
#     dir.create(output_path)
#   }

#   write.table(sign_results,  paste(output_path, "/", country_code, "_sign_continuous.txt", sep = ""), 
#     sep = "\t", quote = FALSE, row.names = FALSE, col.names = TRUE)
# }

# gisaid_data_all$country_code <- countrycode(gisaid_data_all$country, origin = 'country.name', destination = 'iso2c', nomatch = NULL)
# gisaid_data_all <- gisaid_data_all[!is.na(gisaid_data_all$country_code),] # remove those without a country code

# #start <- Sys.time()
# for (c in 1:length(countries)){
#   print(c)
#   gisaid_data <- gisaid_data_all[gisaid_data_all$country_code == countries[c],]
#   sign_results <- sliding_fisher(gisaid_data, w, step, freq_threshold = freq_threshold, voc) # Changed freq_threshold from 0.1 to freq_threshold 
  
#   # create folder and save significant results
#   output_path <- paste(output_folder, "/", countries[c], sep = "")
#   dir.create(output_path)
  
#   write.table(sign_results,  paste(output_path, "/", countries[c], "_sign_continuous.txt", sep = ""), sep = "\t", quote = FALSE, row.names = FALSE, col.names = TRUE)
# }

#stop <- Sys.time()
#print(stop - start)

### analysis on German state-wise data ###
print("Analysis for German State-wise data")
w = 200
step = 10

if ("Germany" %in% countries) {
  gisaid_data_germany <- list_gisaid_data_country[["Germany"]]  %>%
    arrange(date)
  
  states_de <- list("Baden-Wuerttemberg" = "DE_BW", "Bavaria" = "DE_BY", "Berlin" = "DE_BE", 
                    "Brandenburg" = "DE_BB", "Bremen" = "DE_HB", "Hamburg" = "DE_HH", "Hesse" = "DE_HE", 
                    "Mecklenburg-Vorpommern" = "DE_MV", "Mecklenburg-Western Pomerania"  = "DE_MV",
                    "Niedersachsen" = "DE_NI", "Lower Saxony" = "DE_NI",
                    "North Rhine Westphalia" = "DE_NW", "North Rhine-Westphalia" = "DE_NW", 
                    "Rheinland-Pfalz" = "DE_RP", "Rhineland-Palatinate" = "DE_RP",
                    "Saarland" = "DE_SL", "Saxony" = "DE_SN",
                    "Saxony-Anhalt" = "DE_ST", "Schleswig-Holstein" = "DE_SH", "Thuringia" = "DE_TH")
  gisaid_data_germany$state_code <- as.character(states_de[gisaid_data_germany$division])
  #states <- c("DE_BB", "DE_BE", "DE_BW", "DE_BY", "DE_HB", "DE_HE", "DE_HH", "DE_NI", "DE_NW", "DE_RP", "DE_SH", "DE_SL", "DE_SN", "DE_ST", "DE_TH")
  states <- c("DE_BB", "DE_BE", "DE_BW", "DE_BY", "DE_HB", "DE_HE", "DE_HH", "DE_MV" 
            ,"DE_NI", "DE_NW", "DE_RP", "DE_SH", "DE_SL", "DE_SN", "DE_ST", "DE_TH")

  sliding_fisher_all_c(gisaid_data_germany$pangolin_lineage, gisaid_data_germany$state_code, gisaid_data_germany$date, states, w, step, freq_threshold, voc, output_folder, p.adjust)

  list_gisaid_data_state <- split(gisaid_data_germany, gisaid_data_germany$state_code)
  # Filter out state dataframes with fewer than 200 rows
  list_gisaid_data_state <- list_gisaid_data_state[sapply(list_gisaid_data_state, nrow) >= 200]
  ## Use detectCores() to find the total number of cores
  #num_cores <- parallel::detectCores()

  ## Register the parallel backend
  #cl <- makeCluster(num_cores-4)
  #registerDoParallel(cl)

  ## Process the dataframes in parallel
  #results <- foreach(state_code = names(list_gisaid_data_state),
  #                  # .combine = 'list',
  #                  .packages = c('dplyr')) %dopar% {
  #  state_df <- list_gisaid_data_state[[state_code]]
  #  sign_results <- sliding_fisher(state_df, w, step, freq_threshold = freq_threshold, voc)
  #  output_path <- paste(output_folder, "/", state_code, sep = "")
  #  if (!dir.exists(output_path)) {
  #    dir.create(output_path)
  #  }
  #  write.table(sign_results,  paste(output_path, "/", state_code, "_sign_continuous.txt", sep = ""), 
  #    sep = "\t", quote = FALSE, row.names = FALSE, col.names = TRUE)
  #}
  #stopCluster(cl)

  # for (i in 1:length(states)){
  #   # gisaid_data <- gisaid_data_germany[gisaid_data_germany$state_code == states[i],]
  #   sign_results <- sliding_fisher(gisaid_data, w, step, freq_threshold = freq_threshold, voc) # changed freq_threshold from 0.1 to freq_threshold

  #   # create folder and save significant results
  #   output_path <- paste(output_folder, "/", states[i], sep = "")
  #   if (!dir.exists(output_path)) {
  #     dir.create(output_path)
  #   }

  #   write.table(sign_results,  paste(output_path, "/", states[i], "_sign_continuous.txt", sep = ""), 
  #     sep = "\t", quote = FALSE, row.names = FALSE, col.names = TRUE)
  # }
}
