#!/usr/bin/env Rscript

library("XML")
library("RCurl")
library(readr)

args = commandArgs(trailingOnly=TRUE)
output_alias <- args[1]
output_dates <- args[2]


parse_html <- function(lineage_html) {
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
}

# read html page
tryCatch( 
    {
        lineage_html <- getURL("https://cov-lineages.org/lineage_list.html", .opts = list(ssl.verifypeer = FALSE))
        parse_html(lineage_html)
    }, 
    error=function(err.msg) {
        message(toString(err.msg))
        message("Please make sure of your network connection. Please check if you can access 'https://cov-lineages.org/lineage_list.html'.")
        
        message(paste0("Since there are network problems, we try to use an other version of this file. Please don't forget to update this file."))
        # lineage_information_backup_file = "testdata/lineage_list.html"
        if (length(args) < 3) {
            stop(paste("Since I received a network problem, I will try to load a local file, but you need to provide me the actual file address.") )

        }
        lineage_information_backup_file = args[3]

        # message("Please make sure of your network connection. Please check if you can access 'https://github.com/cov-lineages/pango-designation/raw/refs/heads/master/lineage_notes.txt'.")
        if (!file.exists(lineage_information_backup_file)) {
            stop(paste("File", lineage_information_backup_file, "does not exists. Please download 'https://cov-lineages.org/lineage_list.html' and move it here. Please finally make sure that the file located there correctly.") )
        }
        parse_html(read_file(lineage_information_backup_file))
    }
)



