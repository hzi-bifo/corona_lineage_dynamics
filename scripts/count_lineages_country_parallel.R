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

#file <- "/testdata/metadataSubset_50.tsv"
#months_file <- "/testdata/months.txt"
#output_folder <- "/testdata/parallelization/results/"
#corrected <- FALSE
#alias_file <- "/testdata/aliases.tsv"
#lineage_date_file <- "/parallelization/results/lineage_dates.tsv"
#country <- "Italy"

gisaid_data_all <- read.csv2(file, sep = "\t", header = TRUE, stringsAsFactors = FALSE) #changed from gisaid_data_all to data
months <- as.vector(t(read.table(months_file, stringsAsFactors = FALSE)))
alias <- read.table(alias_file, stringsAsFactors = FALSE)
lineage_dates <- read.table(lineage_date_file, stringsAsFactors = FALSE, header = TRUE)

# Filtering by country
gisaid_data <- gisaid_data_all[gisaid_data_all$country == country,]

# add country code column
gisaid_data$country_code <- countrycode(gisaid_data$country, origin = 'country.name', destination = 'iso2c', nomatch = NULL)

# rename column
names(gisaid_data)[which(names(gisaid_data) == "pango_lineage")] <- "pangolin_lineage"

# remove entries where the sequencing date is prior to the first observed date for the lineage
date_rm <- c()
regex <- "[[:digit:]][[:digit:]][[:digit:]][[:digit:]][[:punct:]][[:digit:]][[:digit:]][[:punct:]][[:digit:]][[:digit:]]" # regular expression to match complete dates
for(i in 1:nrow(gisaid_data)){
  date_seq <- gisaid_data[i,"date"]
  date_first <- lineage_dates[lineage_dates$lineage == gisaid_data[i,"pangolin_lineage"],]$month
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
  gisaid_data <- gisaid_data[-date_rm,]
}

# replace alias in lineage names
for (i in 1:nrow(alias)){
  gisaid_data$pangolin_lineage[gisaid_data$pangolin_lineage == alias[i,1]] <- alias[i,2]
}

# remove lineage that contain / in the name
gisaid_data <- gisaid_data[!grepl("/", gisaid_data$pangolin_lineage, fixed = TRUE),] 

# get countries with more than 2k sequences
#numbers <- table(gisaid_data_all$country_code)
#countries <- names(numbers[numbers > 2000])

#acknowledgements <- data.frame(gisaid_epi_isl = character(), stringsAsFactors = FALSE, originating_lab = character(), submitting_lab = character(), authors = character())
acknowledgements <- data.frame(gisaid_epi_isl = character(), country = character(), pangolin_lineage = character(), originating_lab = character(), submitting_lab = character(), stringsAsFactors = FALSE)

############


# count lineages for each countries > 2k


#for (c in 1:length(countries)){
  # get data subset for the current country
  #gisaid_data <- gisaid_data_all[gisaid_data_all$country_code == countries[c],]
  #acknowledgements <- rbind(acknowledgements, gisaid_data[c("gisaid_epi_isl", "originating_lab", "submitting_lab", "authors")])
acknowledgements <- rbind(acknowledgements, gisaid_data[c("gisaid_epi_isl", "country", "pangolin_lineage", "originating_lab", "submitting_lab")])
  
all_lineages <- sort(unique(gisaid_data$pangolin_lineage))
all_lineages <- all_lineages[all_lineages != "" & all_lineages != "None"]
print("Unique Pango lineages")
print(length(all_lineages))
print(all_lineages)

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
#country_name <- gsub(" ", "", countries[c], fixed = TRUE)
country_name <- as.character(gisaid_data[1, "country_code"])
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
  
# months actually used in the analysis
write.table(months, paste(output, ".months.txt", sep = ""), quote = FALSE, row.names = FALSE, col.names = FALSE)
  
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

#acknowledgements_aggregated <- aggregate(data = acknowledgements, gisaid_epi_isl~originating_lab + submitting_lab + authors, paste, collapse = ", ")
write.table(acknowledgements, paste(output_folder, "/acknowledgements.tsv", sep = ""), sep = "\t", quote = FALSE, row.names = FALSE, col.names = TRUE)
#write.table(acknowledgements_aggregated, paste(output_folder, "/acknowledgements.tsv", sep = ""), sep = "\t", quote = FALSE, row.names = FALSE, col.names = TRUE)
