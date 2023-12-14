#!/usr/bin/env Rscript

args = commandArgs(trailingOnly=TRUE)
file <- args[1]
months_file <- args[2]
output <- args[3]

gisaid_data <- read.table(file, sep = "\t", header = TRUE, stringsAsFactors = FALSE)
months <- as.vector(t(read.table(months_file, stringsAsFactors = FALSE)))

all_lineages <- sort(unique(gisaid_data$Lineage))
all_lineages <- all_lineages[all_lineages != "" & all_lineages != "None"]

all_data_counts <- data.frame(lineage = all_lineages, stringsAsFactors = FALSE)
all_data_freqs <- data.frame(lineage = all_lineages, stringsAsFactors = FALSE)

# set timeframe that should be calculated (alternative: read from file)
#months <- c("2020-01", "2020-02", "2020-03", "2020-04", "2020-05", "2020-06", "2020-07", "2020-08", "2020-09", "2020-10", "2020-11", "2020-12", "2021-01")
n_seqs <- vector(mode = "integer", length = length(months))

# loop over months and lineages to calculate counts and frequencies
for (m in 1:length(months)){
  gisaid_data_subset <- gisaid_data[grep(months[m], gisaid_data$Collection.date),]
  n_seq <- nrow(gisaid_data_subset)
  n_seqs[m] <- n_seq
  for (i in 1:length(all_lineages)){
    all_data_counts[i, 1+m] <- sum(gisaid_data_subset$Lineage == all_lineages[i])
    all_data_freqs[i, 1+m] <- sum(gisaid_data_subset$Lineage == all_lineages[i])/n_seq
  }
}

# set names in dataframe
names(all_data_counts) <- c("lineage", months)
names(all_data_freqs) <- c("lineage", months)

# write tables
write.table(all_data_counts, paste(output, "_counts.txt", sep = "") , sep = "\t", quote = FALSE, row.names = FALSE, col.names = TRUE)
write.table(all_data_freqs, paste(output, "_frequencies.txt", sep = "") , sep = "\t", quote = FALSE, row.names = FALSE, col.names = TRUE)

### write output as input for SD plot evaluation&visualization

# numIsolates.txt
# number of seqs per timeframe, tab separated
sink(paste(output, ".numIsolates.txt", sep = ""))
cat(paste(n_seqs, collapse = " "))
cat("\n")
sink()

# mutations.per_position.txt
# mutations/labels for frequencies; tab separated
sink(paste(output, ".mutations.txt", sep = ""))
cat(paste(all_lineages, collapse = "\t"))
cat("\n")
sink()

# subtreeMutationMap.per_position.txt
# counts with time in columns and mutations in rows
write.table(all_data_counts[,-1], paste(output, ".subtreeMutationMap.txt", sep = "") , sep = "\t", quote = FALSE, row.names = FALSE, col.names = FALSE)
