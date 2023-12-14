#!/usr/bin/env Rscript

args = commandArgs(trailingOnly=TRUE)
data_file <- args[1]
lineage_file <- args[2]
zip_mapping_file <- args[3]
output <- args[4]

#data_file <- "SARS-CoV-2-Sequenzdaten_Deutschland.csv"
#lineage_file <- "SARS-CoV-2-Entwicklungslinien_Deutschland.csv"
#zip_mapping_file <- "DE/plz_states_extended.txt"

data <- read.table(data_file, sep = ",", stringsAsFactors = FALSE, header = TRUE)
lineages <- read.table(lineage_file, sep = ",", stringsAsFactors = FALSE, header = TRUE)
zip_mapping <- read.table(zip_mapping_file, sep = "\t", stringsAsFactors = FALSE, header = TRUE)

# data columns needed for coverage: country, state/division, date, pangolin_lineage, originating_lab, submitting_lab, gisaid_epi_isl
data_subset <- data[, c("IMS_ID", "DATE_DRAW", "SENDING_LAB_PC", "SEQUENCING_LAB_PC")]
lineages_subset <- lineages[, c("IMS_ID", "lineage")]

metadata <- merge(data_subset, lineages_subset, by = "IMS_ID")
metadata <- metadata[!is.na(metadata$SENDING_LAB_PC),] # remove entries with missing zip code

# add country column
metadata$country <- "Germany"

# add state column
state <- vector(mode = "character", length = nrow(metadata))

for (i in 1:nrow(metadata)){
  plz <- metadata$SENDING_LAB_PC[i]
  current_state <- zip_mapping[which(zip_mapping$zip == plz),]$state
  if (length(current_state) == 0){
    print(paste0("Zip code ", plz, " can't be matched to a state and will be removed."))
    state[i] <- NA
  } else {
    state[i] <- zip_mapping[which(zip_mapping$zip == plz),]$state
  }
}

metadata$division <- state
metadata <- metadata[!is.na(metadata$division),]

# add host column
metadata$host <- "Human"

# rename
names(metadata) <- c("gisaid_epi_isl", "date", "originating_lab", "submitting_lab", "pangolin_lineage", "country", "division", "host")

write.table(metadata, output, sep = "\t", quote = FALSE, row.names = FALSE, col.names = TRUE)
