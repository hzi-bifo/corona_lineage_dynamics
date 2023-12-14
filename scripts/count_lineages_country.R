#!/usr/bin/env Rscript

library("countrycode")
library("stringr")
library("tidyr")
library("readr")
library(Rcpp)
sourceCpp("scripts/count_lineages_country.cpp")


args = commandArgs(trailingOnly=TRUE)
file <- args[1]
months_file <- args[2]
output_folder <- args[3]
corrected <- as.logical(args[4])
alias_file <- args[5]
lineage_date_file <- args[6]

countrycode_simple <- function(country) {
  return (countrycode(country, origin = 'country.name', destination = 'iso2c', nomatch = NULL))
}

count_lineages_country(file, months_file, output_folder, corrected, alias_file, lineage_date_file, 
  countrycode)
