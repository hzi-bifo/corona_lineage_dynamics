#!/usr/bin/env Rscript

library("countrycode")
library("plyr")

args = commandArgs(trailingOnly=TRUE)
file <- args[1]
freq_threshold <- as.numeric(args[2])
output_folder <- args[3]
country <- args[4]

#file <- "testdata/metadata_subset_50.tsv"
#freq_threshold <- 0.1
#output_folder <- "testdata/parallelization/results/"
#country <- "USA"

# variants of concern
# might be more conventient to read via a file in case this list needs to be updated in the future
voc <- c("B.1.1.7", "B.1.351", "P.1", "B.1.617", "B.1.617.1", "B.1.617.2", "B.1.617.3") # variants of concern: uk, south africa, brazil, india
# additionally maybe: B.1.427, B.1.429, B.1.526, B.1.526.1, B.1.526.2 (all US), P.2 (Brazil)?


# function to perform sliding window fisher test on all data used as input
sliding_fisher <- function(gisaid_data, w, step, freq_threshold, voc){ 
  all_lineages <- sort(unique(gisaid_data$pangolin_lineage))
  all_lineages <- all_lineages[all_lineages != "" & all_lineages != "None"]
  
  if (nrow(gisaid_data) < w*2){
    print("Not enough data for chosen window size.")
    print(nrow(gisaid_data))
    return(data.frame(lineage = character(), date = character(), pval = numeric(), odds = numeric(), freq = numeric(), stringsAsFactors = FALSE))
  }
  window_steps <- seq(w, (nrow(gisaid_data)-w), step) # respective start of the current window
  
  pval_matrix <- matrix(data = NA, ncol = length(window_steps), nrow = length(all_lineages))
  odds_matrix <- matrix(data = NA, ncol = length(window_steps), nrow = length(all_lineages))
  freq_matrix <- matrix(data = NA, ncol = length(window_steps), nrow = length(all_lineages))
  
  for (l in 1:length(all_lineages)){
    # get 0/1 vector for current lineage
    lineage_vector <- as.integer(gisaid_data$pangolin_lineage == all_lineages[l])
    
    counter <- 1
    
    for (i in window_steps){
      start <- i + 1
      end <- i + w
      current_window <- lineage_vector[start:end]
      previous_window <- lineage_vector[(start - w):(start - 1)] # previous window the same size as the current one
      
      # get values for contigency table
      mut_thisseason <- sum(current_window)
      mut_prevseason <- sum(previous_window)
      
      nomut_thisseason <- sum(current_window == 0)
      nomut_prevseason <- sum(previous_window == 0)
      
      # calculate fisher test and save pvalue in matrix
      data <- matrix(c(mut_thisseason, nomut_thisseason, mut_prevseason, nomut_prevseason), nrow = 2)
      test <- fisher.test(data, alternative = "greater")
      pval_matrix[l, counter] <- test$p.value
      odds_matrix[l, counter] <- test$estimate
      freq_matrix[l, counter] <- mut_thisseason / w
      counter <- counter + 1
    }
  }
  
  # correction for multiple testing
  pval_adjusted <- matrix(p.adjust(pval_matrix, method = "BY"), nrow = length(all_lineages)) # FDR correction using Benjamini & Yekutieli for dependent tests
  sign_results <- data.frame(lineage = character(), date = character(), pval = numeric(), odds = numeric(), freq = numeric(), stringsAsFactors = FALSE)

  for (j in 1:nrow(pval_adjusted)){
    if (all_lineages[j] %in% voc){
      # if variant of concern: look for significant results without frequency threshold
      results_test <- pval_adjusted[j,] < 0.05
    } else {
      # otherwise: use frequency threshold
      results_test <- pval_adjusted[j,] < 0.05 & freq_matrix[j,] > freq_threshold
    }
    first_sign <- which(results_test == TRUE)[1]
    if (! is.na(first_sign)){
      sign_results <- rbind(sign_results, data.frame(lineage = all_lineages[j], date = gisaid_data[(window_steps + w)[first_sign],]$date, pval = pval_adjusted[j, first_sign], odds = odds_matrix[j, first_sign], freq = freq_matrix[j, first_sign]))
    }
  }
  
  return(sign_results)

}

# read data
columns <- c("gisaid_epi_isl", "date", "country", "division", "host", "pango_lineage")
gisaid_data_all <- read.csv2(file, sep = "\t", header = TRUE, stringsAsFactors = FALSE)
gisaid_data_all <- gisaid_data_all[gisaid_data_all$host == "Human",] # only sequences sampled from human hosts
gisaid_data <- gisaid_data_all[gisaid_data_all$country == country,] # Samples by country parameter

# add country code column
gisaid_data$country_code <- countrycode(gisaid_data$country, origin = 'country.name', destination = 'iso2c', nomatch = NULL)
gisaid_data <- gisaid_data[!is.na(gisaid_data$country_code),] # remove those without a country code

# rename column
names(gisaid_data)[which(names(gisaid_data) == "pango_lineage")] <- "pangolin_lineage"
# remove lineage that contain / in the name
gisaid_data <- gisaid_data[!grepl("/", gisaid_data$pangolin_lineage, fixed = TRUE),]
	
# take only data with the complete date to allow sorting
gisaid_data <- gisaid_data[grep("20..-..-..", gisaid_data$date),]
gisaid_data <- gisaid_data[order(gisaid_data$date),]


### analysis for countries ###

print("Analysis for countries")
w = 1000
step = 100

sign_results <- sliding_fisher(gisaid_data, w, step, freq_threshold = freq_threshold, voc)

# create folder and save significant results
country_name <- as.character(gisaid_data[1, "country_code"])
output_path <- paste(output_folder, "/", country_name, sep = "")
dir.create(output_path)
write.table(sign_results,  paste(output_path, "/", country_name, "_sign_continuous.txt", sep = ""), sep = "\t", quote = FALSE, row.names = FALSE, col.names = TRUE)

### analysis on German state-wise data ###

#gisaid_data_germany <- gisaid_data_all[gisaid_data_all$country_code == "DE",]

if (country == "Germany") {

	print("Analysis for German State-wise data")
	w = 200 
	step = 10

	gisaid_data_germany <- gisaid_data[gisaid_data$country_code == "DE",]
	states_de <- list("Baden-Wuerttemberg" = "DE_BW", "Bavaria" = "DE_BY", "Berlin" = "DE_BE", "Brandenburg" = "DE_BB", "Bremen" = "DE_HB",
                 "Hamburg" = "DE_HH", "Hesse" = "DE_HE", "Mecklenburg-Vorpommern" = "DE_MV", "Niedersachsen" = "DE_NI",
                 "North Rhine Westphalia" = "DE_NW", "Rheinland-Pfalz" = "DE_RP", "Saarland" = "DE_SL", "Saxony" = "DE_SN",
                 "Saxony-Anhalt" = "DE_ST", "Schleswig-Holstein" = "DE_SH", "Thuringia" = "DE_TH")
	gisaid_data_germany$state_code <- as.character(states_de[gisaid_data_germany$division])
	states <- c("DE_BB", "DE_BE", "DE_BW", "DE_BY", "DE_HB", "DE_HE", "DE_HH", "DE_MV" ,"DE_NI", "DE_NW", "DE_RP", "DE_SH", "DE_SL", "DE_SN", "DE_ST", "DE_TH")
	
	for (i in 1:length(states)){
 		gisaid_data <- gisaid_data_germany[gisaid_data_germany$state_code == states[i],]
		sign_results <- sliding_fisher(gisaid_data, w, step, freq_threshold = freq_threshold, voc) # changed freq_threshold from 0.1 to freq_threshold

 		# create folder and save significant results
 		output_path <- paste(output_folder, "/", states[i], sep = "")
		dir.create(output_path)

 		write.table(sign_results,  paste(output_path, "/", states[i], "_sign_continuous.txt", sep = ""), sep = "\t", quote = FALSE, row.names = FALSE, col.names = TRUE)
	}
}
