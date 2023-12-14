#!/usr/bin/env Rscript

library("countrycode")

args = commandArgs(trailingOnly=TRUE)
file <- args[1]
output_folder <- args[3]

#file <- "/testdata/metadataSubset_50.tsv"
#output_folder <- "/testdata/parallelization/results/"

gisaid_data_all <- read.csv2(file, sep = "\t", header = TRUE, stringsAsFactors = FALSE)
numbers <- table(gisaid_data_all$country)
countries <- names(numbers[numbers > 2000])

countries_df <- as.data.frame(countries, row.names = NULL, optional = FALSE)
names(countries_df) <- NULL
write.csv(countries_df, file = paste(output_folder, "countries_list.csv", sep = ""), row.names = FALSE)

# All analysis scripts will need to input an argument with country
# All analysis scripts will need to filter the gisaid_data_all by country and then apply their own unique filters to that
# For the count_lineages_states.R script just add if country does not equal Germany exit command at the top
