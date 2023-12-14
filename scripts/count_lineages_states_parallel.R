#!/usr/bin/env Rscript

library("countrycode")

args = commandArgs(trailingOnly=TRUE)
file <- args[1]
months_file <- args[2]
output_folder <- args[3]
corrected <- as.logical(args[4])
alias_file <- args[5]
lineage_date_file <- args[6]
country <- args[7]

if (country != "Germany") {
	quit()}

input_data_all <- read.csv2(file, sep = "\t", header = TRUE, stringsAsFactors = FALSE)
months <- as.vector(t(read.table(months_file, stringsAsFactors = FALSE)))
alias <- read.table(alias_file, stringsAsFactors = FALSE)
lineage_dates <- read.table(lineage_date_file, stringsAsFactors = FALSE, header = TRUE)

sequence_cutoff <- 50

# filtering by country
gisaid_data_all <- input_data_all[input_data_all$country == country,]

# add country code column
gisaid_data_all$country_code <- countrycode(gisaid_data_all$country, origin = 'country.name', destination = 'iso2c', nomatch = NULL)

# subset by country code
gisaid_data_all <- gisaid_data_all[gisaid_data_all$country_code == "DE",]

# add abbreviation for german states (used also for filenames)
#states_de <- list("Baden-Wuerttemberg" = "DE_BW", "Bavaria" = "DE_BY", "Berlin" = "DE_BE", "Brandenburg" = "DE_BB", "Bremen" = "DE_HB",
#                  "Hamburg" = "DE_HH", "Hesse" = "DE_HE", "Mecklenburg-Vorpommern" = "DE_MV", "Niedersachsen" = "DE_NI", 
#                  "North Rhine Westphalia" = "DE_NW", "Rheinland-Pfalz" = "DE_RP", "Saarland" = "DE_SL", "Saxony" = "DE_SN",                
#                  "Saxony-Anhalt" = "DE_ST", "Schleswig-Holstein" = "DE_SH", "Thuringia" = "DE_TH")
states_de <- list("Baden-Wuerttemberg" = "DE_BW", "Bavaria" = "DE_BY", "Berlin" = "DE_BE",
                    "Brandenburg" = "DE_BB", "Bremen" = "DE_HB", "Hamburg" = "DE_HH", "Hesse" = "DE_HE",
                    "Mecklenburg-Vorpommern" = "DE_MV", "Mecklenburg-Western Pomerania"  = "DE_MV",
                    "Niedersachsen" = "DE_NI", "Lower Saxony" = "DE_NI",
                    "North Rhine Westphalia" = "DE_NW", "North Rhine-Westphalia" = "DE_NW",
                    "Rheinland-Pfalz" = "DE_RP", "Rhineland-Palatinate" = "DE_RP",
                    "Saarland" = "DE_SL", "Saxony" = "DE_SN",
                    "Saxony-Anhalt" = "DE_ST", "Schleswig-Holstein" = "DE_SH", "Thuringia" = "DE_TH")
gisaid_data_all$state_code <- as.character(states_de[gisaid_data_all$division])
# rename column
names(gisaid_data_all)[which(names(gisaid_data_all) == "pango_lineage")] <- "pangolin_lineage"

# remove entries where the sequencing date is prior to the first observed date for the lineage
# remove entries where the sequencing date is prior to the first observed date for the lineage
date_rm <- c()
regex <- "[[:digit:]][[:digit:]][[:digit:]][[:digit:]][[:punct:]][[:digit:]][[:digit:]][[:punct:]][[:digit:]][[:digit:]]" # regular expression to match complete dates
for(i in 1:nrow(gisaid_data_all)){
  date_seq <- gisaid_data_all[i,"date"]
  date_first <- lineage_dates[lineage_dates$lineage == gisaid_data_all[i,"pangolin_lineage"],]$month
  if (length(date_first) == 0){ # if lineage isn't found in lineage date list, assign NA (no filtering will be performed)
    date_first <- NA
  }
  if (grepl(regex, date_first) & grepl(regex, date_seq)){ # comparison only if both dates are complete to avoid errors
    if(as.Date(date_seq) < as.Date(date_first)){
      date_rm <- c(date_rm, i)
    }
  }
}
if (length(date_rm) > 0){
  gisaid_data_all <- gisaid_data_all[-date_rm,]
}

# replace alias in lineage names
for (i in 1:nrow(alias)){
  gisaid_data_all$pangolin_lineage[gisaid_data_all$pangolin_lineage == alias[i,1]] <- alias[i,2]
}

# remove lineage that contain / in the name
gisaid_data_all <- gisaid_data_all[!grepl("/", gisaid_data_all$pangolin_lineage, fixed = TRUE),]

# get countries with more than 2k sequences
numbers <- table(gisaid_data_all$state_code)
numbers <- numbers[which(names(numbers) != "NULL")]
countries <- names(numbers[numbers > 0])

# count lineages for each state
for (c in 1:length(countries)){
  # get data subset for the current country
  gisaid_data <- gisaid_data_all[gisaid_data_all$state_code == countries[c],]
  
  all_lineages <- sort(unique(gisaid_data$pangolin_lineage))
  all_lineages <- all_lineages[all_lineages != "" & all_lineages != "None"]
  
  # get alias to write in output
  all_lineages_alias <- all_lineages
  for (i in 1:nrow(alias)){
    all_lineages_alias[all_lineages_alias == alias[i,2]] <- alias[i,1]
  }
  
  all_data_counts <- data.frame(lineage = all_lineages, stringsAsFactors = FALSE)
  all_data_freqs <- data.frame(lineage = all_lineages, stringsAsFactors = FALSE)
  
  # set timeframe that should be calculated (alternative: read from file)
  #months <- c("2020-01", "2020-02", "2020-03", "2020-04", "2020-05", "2020-06", "2020-07", "2020-08", "2020-09", "2020-10", "2020-11", "2020-12", "2021-01")
  n_seqs <- vector(mode = "integer", length = length(months))
  
  # loop over months and lineages to calculate counts and frequencies
  for (m in 1:length(months)){
    gisaid_data_subset <- gisaid_data[grep(months[m], gisaid_data$date),]
    n_seq <- nrow(gisaid_data_subset)
    n_seqs[m] <- n_seq
    for (i in 1:length(all_lineages)){
      if (corrected == FALSE){
        # count only lineage,  without sublineages
        count <- sum(gisaid_data_subset$pangolin_lineage == all_lineages[i])
      } else if (corrected == TRUE){
        # count lineage with sublineages
        count <- sum(gisaid_data_subset$pangolin_lineage == all_lineages[i] | startsWith(gisaid_data_subset$pangolin_lineage, paste(all_lineages[i], ".", sep = "")))
      }
      all_data_counts[i, 1+m] <- count
      all_data_freqs[i, 1+m] <- count/n_seq
    }
  }
  
  # set names in dataframe
  names(all_data_counts) <- c("lineage", months)
  names(all_data_freqs) <- c("lineage", months)
  
  # rename aliases
  all_data_counts[,1] <- all_lineages_alias
  all_data_freqs[,1] <- all_lineages_alias
  
  # add total counts
  all_data_counts_sum <- rbind(c("all", t(colSums(all_data_counts[,2:ncol(all_data_counts)]))), all_data_counts)
  
  # for creating folder and filenames: country without spaces
  country_name <- gsub(" ", "", countries[c], fixed = TRUE)
  output_path <- paste(output_folder, "/", country_name, sep = "")
  dir.create(output_path)

  if (corrected == FALSE){
    output <- paste(output_path, "/", country_name, sep = "")
  } else if (corrected == TRUE){
    output <- paste(output_path, "/", country_name, "_corrected", sep = "")
  }

  # write tables
  write.table(all_data_counts_sum, paste(output, "_counts.txt", sep = "") , sep = "\t", quote = FALSE, row.names = FALSE, col.names = TRUE)
  write.table(all_data_freqs, paste(output, "_frequencies.txt", sep = "") , sep = "\t", quote = FALSE, row.names = FALSE, col.names = TRUE)
  
  ### write output as input for SD plot evaluation&visualization
  
  # only start in season with more than x sequences
  start_month <- which(n_seqs >= sequence_cutoff)[1]
  n_seqs <- n_seqs[start_month:length(n_seqs)]
  all_data_counts <- all_data_counts[, c(1, (start_month +1):ncol(all_data_counts))]
  all_data_counts[rowSums(as.data.frame(all_data_counts[,-1])) > 0,]
  all_lineages_alias <- all_data_counts[,1]
  current_months <- months[start_month:length(months)]
  
  # months actually used in the analysis
  write.table(current_months, paste(output, ".months.txt", sep = ""), quote = FALSE, row.names = FALSE, col.names = FALSE)
  
  # numIsolates.txt
  # number of seqs per timeframe, tab separated
  sink(paste(output, ".numIsolates.txt", sep = ""))
  cat(paste(n_seqs, collapse = " "))
  cat("\n")
  sink()
  
  # mutations.per_position.txt
  # mutations/labels for frequencies; tab separated
  sink(paste(output, ".mutations.txt", sep = ""))
  cat(paste(all_lineages_alias, collapse = "\t"))
  cat("\n")
  sink()
  
  # subtreeMutationMap.per_position.txt
  # counts with time in columns and mutations in rows
  write.table(all_data_counts[,-1], paste(output, ".subtreeMutationMap.txt", sep = "") , sep = "\t", quote = FALSE, row.names = FALSE, col.names = FALSE)
}
