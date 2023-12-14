#!/usr/bin/env Rscript

library("countrycode")
library("stringr")
library("tidyr")
library("readr")

args = commandArgs(trailingOnly=TRUE)
file <- args[1]
months_file <- args[2]
output_folder <- args[3]
corrected <- as.logical(args[4])
alias_file <- args[5]
lineage_date_file <- args[6]

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
  show_col_types = FALSE # TODO: REMOVE THIS LINE
)
gisaid_data_all <- as.data.frame(gisaid_data_all_tibble)
months <- as.vector(t(read.table(months_file, stringsAsFactors = FALSE)))
alias <- read.table(alias_file, stringsAsFactors = FALSE)
lineage_dates <- read.table(lineage_date_file, stringsAsFactors = FALSE, header = TRUE)

# splitting location column
gisaid_data_all[c("region", "country", "division")] <- str_split_fixed(gisaid_data_all$Location, " / ", 3)

# add country code column
gisaid_data_all$country_code <- countrycode(gisaid_data_all$country, origin = 'country.name', destination = 'iso2c', nomatch = NULL)

# rename column
colnames(gisaid_data_all)[colnames(gisaid_data_all) == "Pango lineage"] = "pangolin_lineage"
colnames(gisaid_data_all)[colnames(gisaid_data_all) == "Collection date"] = "collection_date"
colnames(gisaid_data_all)[colnames(gisaid_data_all) == "Accession ID"] = "Accession_ID"

# removing NA values in pangolin_lineage column
gisaid_data_all <- gisaid_data_all[!is.na(gisaid_data_all$pangolin_lineage),]

# remove entries where the sequencing date is prior to the first observed date for the lineage
date_rm <- c()
regex <- "[[:digit:]][[:digit:]][[:digit:]][[:digit:]][[:punct:]][[:digit:]][[:digit:]][[:punct:]][[:digit:]][[:digit:]]" # regular expression to match complete dates
for(i in 1:nrow(gisaid_data_all)){
  #date_seq <- gisaid_data_all[i, "Collection date"]
  date_seq <- gisaid_data_all$collection_date[i]
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
# message("DEBUG ", gisaid_data_all[gisaid_data_all$Accession_ID == "EPI_ISL_11785477",])

# replace alias in lineage names
for (i in 1:nrow(alias)){
  gisaid_data_all$pangolin_lineage[gisaid_data_all$pangolin_lineage == alias[i,1]] <- alias[i,2]
}

# remove lineage that contain / in the name
gisaid_data_all <- gisaid_data_all[!grepl("/", gisaid_data_all$pangolin_lineage, fixed = TRUE),] 

# get countries with more than 2k sequences
numbers <- table(gisaid_data_all$country_code)
countries <- names(numbers[numbers > 2000])

# message("DEBUG ", gisaid_data_all[gisaid_data_all$Accession_ID == "EPI_ISL_11785477",])

#DEBUG:
# countries <- c("AT")

#acknowledgements <- data.frame(Accession.ID = character(), country = character(), pangolin_lineage = character(), stringAsFactors = FALSE) #, originating_lab = character(), submitting_lab = character(), stringsAsFactors = FALSE)
acknowledgements <- subset(gisaid_data_all, select=c("Accession_ID", "country", "pangolin_lineage"))

write.table(gisaid_data_all[gisaid_data_all$Accession_ID == "EPI_ISL_12906015",])

# count lineages for each countries > 2k
for (c in 1:length(countries)){
  # get data subset for the current country
  gisaid_data <- gisaid_data_all[gisaid_data_all$country_code == countries[c],]
  # TODO: Following is definitly a bug!
  acknowledgements <- rbind(acknowledgements, gisaid_data[c("Accession_ID", "country", "pangolin_lineage")])
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

  if (countries[c] == "US") {
    # message(gisaid_data[startsWith(gisaid_data$pangolin_lineage, "B.1.1.28."),"collection_date"])
    # message(unique(sort(gisaid_data$pangolin_lineage)))
  }
  
  # loop over months and lineages to calculate counts and frequencies
  for (m in 1:length(months)){
    gisaid_data_subset <- gisaid_data[grep(months[m], gisaid_data$collection_date),]


    n_seq <- nrow(gisaid_data_subset)
    n_seqs[m] <- n_seq
    for (i in 1:length(all_lineages)){
      if (countries[c] == "US" & all_lineages[i] == "B.1.1") {
        message("", countries[c], " ", all_lineages[i], " ", months[m], " ", 
          gisaid_data_subset$Accession_ID[gisaid_data_subset$pangolin_lineage==all_lineages[i]]);
      }
      if (corrected == FALSE){
        # count only lineage,  without sublineages
        # if (all_lineages[i] == "B.1.1.529.2.9" & countries[c] == "BE" & months[m] == "2022-03" ) {
        #   write.table(gisaid_data_subset[gisaid_data_subset$pangolin_lineage == all_lineages[i],])
        # }
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
}

#acknowledgements_aggregated <- aggregate(data = acknowledgements, gisaid_epi_isl~originating_lab + submitting_lab + authors, paste, collapse = ", ")
write.table(acknowledgements, paste(output_folder, "/acknowledgements.tsv", sep = ""), sep = "\t", quote = FALSE, row.names = FALSE, col.names = TRUE)
#write.table(acknowledgements_aggregated, paste(output_folder, "/acknowledgements.tsv", sep = ""), sep = "\t", quote = FALSE, row.names = FALSE, col.names = TRUE)
