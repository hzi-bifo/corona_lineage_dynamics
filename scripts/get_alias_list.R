#!/usr/bin/env Rscript

library("XML")
library("RCurl")

args = commandArgs(trailingOnly=TRUE)
output_alias <- args[1]
output_dates <- args[2]

# read html page
lineage_html <- getURL("https://cov-lineages.org/lineage_list.html",.opts = list(ssl.verifypeer = FALSE))

# extract table and convert to data frame
lineage_list <- readHTMLTable(lineage_html, stringsAsFactors = FALSE)[[1]]
#data_alias <- lineage_list[which(grepl("Alias of", lineage_list$Description, fixed = TRUE)),]
data_alias <- lineage_list[which(startsWith(lineage_list$Description, "Alias of")),]
alias_list <- data.frame(lineage = trimws(data_alias$Lineage), 
           alias = gsub("Alias of ", "", unlist(lapply(strsplit(data_alias$Description, ",", fixed = TRUE), function(x) x[[1]])), fixed = TRUE), 
           stringsAsFactors = FALSE)

# if there is a space somewhere in the alias, the description has not been parsed correctly. Remove those entries
alias_list <- alias_list[!grepl(" ", alias_list$alias, fixed = TRUE),]

# write alias table
write.table(alias_list, file = output_alias, sep = "\t", quote = FALSE, row.names = FALSE, col.names = FALSE)

# additionally: get information when lineages first occured
lineage_dates <- data.frame(lineage = lineage_list$Lineage, month = lineage_list$`Earliest date`, stringsAsFactors = FALSE)
lineage_dates = lineage_dates[-1,]
lineage_dates$month[which(lineage_dates$month == "")] <- NA

write.table(lineage_dates, file = output_dates, sep = "\t", quote = FALSE, row.names = FALSE, col.names = TRUE)
