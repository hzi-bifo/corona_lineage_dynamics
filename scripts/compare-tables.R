suppressPackageStartupMessages(library("tidyr"))
library("stringr")
library("readr")
suppressPackageStartupMessages(library("dplyr"))

remove_empty_rows <- function(d) {
 d[rowSums(d[,2:ncol(d)] != 0 & !is.na(d[,2:ncol(d)])) > 0,]
}


args = commandArgs(trailingOnly=TRUE)
file1 <- args[1]
file2 <- args[2]
#file1 <- '/home/hforoughmand/debug_coverage_server/t1//SDPlots_covlineages/AT/AT_corrected_counts.txt'
#file2 <- '/home/hforoughmand/debug_coverage_server/t1//SDPlots_covlineages-reliable0/AT/AT_corrected_counts.txt'
#file1 <- '/home/hforoughmand/debug_coverage_server/t1//SDPlots_covlineages/AT/AT_counts.txt'
#file2 <- '/home/hforoughmand/debug_coverage_server/t1//SDPlots_covlineages-reliable0/AT/AT_counts.txt'

d1 <- read_tsv(file1, show_col_types = FALSE)
d2 <- read_tsv(file2, show_col_types = FALSE)

d1.r <- remove_empty_rows(d1) %>% arrange(1) %>% apply(1, toString) %>% str_replace_all(" ", "")
d2.r <- remove_empty_rows(d2) %>% arrange(1) %>% apply(1, toString) %>% str_replace_all(" ", "")

setdiff(d1.r, d2.r)
setdiff(d2.r, d1.r)

