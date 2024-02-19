#!/usr/bin/env Rscript

library(d3heatmap)
library(htmlwidgets)
library(countrycode)
library(htmltools)
library(xtable)

args = commandArgs(trailingOnly=TRUE)
folder <- args[1]
months_file <- args[2]
cutoff <- as.numeric(args[3])

get_heatmap <- function(folder, countries, current_month, output_name, rename, cutoff){
  
  current_month_colname <- paste0("X", gsub("-", ".", current_month, fixed = TRUE))
  # get significant lineages from all countries
  sign_lineages <- vector(mode = "character")
  
  country_counts <- c()
  country_names <- c()
  for (country in countries){
    current_file <- paste(folder, "/", country, "/", country, ".results.txt", sep = "")
    if (file.exists(current_file) && file.info(current_file)$size > 0){
      tmp <- read.table(paste(folder, "/", country, "/", country, ".results.txt", sep = ""), stringsAsFactors = FALSE)
      sign_lineages <- c(sign_lineages, tmp$V2)
    }
    current_count_file <- paste(folder, "/", country, "/", country, "_counts.txt", sep = "")
    if (rename && file.exists(current_count_file) && file.info(current_count_file)$size > 0){
        #load country_counts.txt and choose the top 50 countries in terms of counts to display in the heatmap
        #sort the country by name instead of iso2c
        count_data <- read.table(current_count_file, stringsAsFactors = FALSE, header=T, row.names=1)
    	country_counts <- c(country_counts, count_data['all', current_month_colname])
	country_names <- c(country_names, country)
    }
  }
  if (rename){
  	names(country_counts) <- country_names
  	topn <- ifelse(length(country_names)>50, 50, length(country_names))
	topn_count_countries <- names(country_counts[order(country_counts, decreasing = TRUE)][1:topn])
  }
  
  # remove duplicates
  sign_lineages <- sort(unique(sign_lineages))
  
  # get data
  data <- matrix(data = 0, nrow = length(sign_lineages), ncol = length(countries))
  colnames(data) <- countries
  rownames(data) <- sign_lineages

  current_month_all_hifreq_data_list <- list() #matrix(data = 0, nrow = length(sign_lineages), ncol = length(countries))

  for (country in countries){
    if (file.exists(paste(folder, "/", country, "/", country, "_frequencies.txt", sep = ""))) {
      tmp <- read.csv(paste(folder, "/", country, "/", country, "_frequencies.txt", sep = ""), stringsAsFactors = FALSE, sep = "\t", header = TRUE) #, na.strings=c("NA"))
   #   tmp[is.na(tmp)] <- 0
      #current_month_colname <- paste0("X", gsub("-", ".", current_month, fixed = TRUE))
      current_month_country_data <- tmp[, c('lineage', current_month_colname), drop=FALSE]
    } else {
      tmp <- data.frame(matrix(ncol = length(current_month_colname) + 1, nrow = 0))
      colnames(tmp) <- c('lineage', current_month_colname)
    }

    # get the lineages with freq >=0.1
    current_month_all_hifreq_data_list[[country]] <- current_month_country_data[current_month_country_data[,current_month_colname]>=0.01&(!is.na(current_month_country_data[,current_month_colname])),,drop=F]
    for (lineage in sign_lineages){
      freq <- tmp[which(tmp$lineage == lineage), paste("X", gsub("-", ".", current_month, fixed = TRUE), sep = "")]
      if (length(freq) > 0 && ! is.na(freq)){
        data[lineage, country] <- freq
      } else {
        data[lineage, country] <- 0
      }
    }
  }

  #print(current_month_country_data)
  current_month_all_hifreq_data <- as.data.frame(Reduce(function(x,y) merge(x = x, y = y, by = "lineage", all=T),
       current_month_all_hifreq_data_list))
  #current_month_all_data <- as.data.frame(do.call(cbind, current_month_all_data_list))
  colnames(current_month_all_hifreq_data) <- c('lineage', countries)
  rownames(current_month_all_hifreq_data) <- current_month_all_hifreq_data$lineage
  current_month_all_hifreq_data <- current_month_all_hifreq_data[,-1]
  
  # remove lineages with frequency 0 in all countries
  data <- data[rowSums(data) > 0,,drop=FALSE]
  
  # write table
  write.csv(data, paste(folder, "/", output_name, "_statistics.csv", sep = ""), quote = FALSE)

  # further filtering using the cutoff for plotting
  data <- data[rowSums(data) >= cutoff,,drop=FALSE]
 
   # plot heatmap and save as html
  if (rename == TRUE){
    country_names <- countrycode(colnames(data), origin = 'iso2c', destination = 'country.name')
    country_names[country_names == "Hong Kong SAR China"] <- "Hong Kong" # rename Hong Kong to make it shorter
    colnames(data) <- country_names
    # show only countries with high freq VOI lineages
    
    sums <- colSums(data)
    topn <- ifelse(ncol(data)>50, 50, ncol(data))
    topn_freq_countries <- sort(names(sums)[order(sums, decreasing = TRUE)][1:topn])
    #data <- data[, topn_freq_countries]
    
    topn_count_countries <- sort(countrycode(topn_count_countries, origin = 'iso2c', destination = 'country.name'))
    topn_count_countries[topn_count_countries == "Hong Kong SAR China"] <- "Hong Kong" # rename Hong Kong to make it shorter
    data <- data[, topn_count_countries]
  }
  data <- data[order(rowSums(data), decreasing = TRUE), , drop = FALSE]

  xaxis_height <- ifelse(rename, 150, 120)
  if (rename == TRUE) {
  height <- 780
  } else if (nrow(data) == 0) {
    height <- 780 # This is the case where it will use the current_month_all_hifreq_data instead of selected pVOIs
  } else if (nrow(data) < 5) {
    height <- nrow(data)*80
  } else if (nrow(data) >= 5 & nrow(data) < 10) {
    height <- nrow(data)*45
  } else if (nrow(data) >= 10 & nrow(data) < 20) {
    height <- nrow(data)*35
  } else {
    height <- nrow(data)*30
  }

  if (nrow(data)>1){
        base::print(dim(data))
        base::print('test in sig lineage')
        row.names(data) <- paste0(row.names(data), '*')
        heatmap <- d3heatmap(data, Rowv = NULL, Colv = NULL, colors = "Blues", xaxis_font_size = "12pt", xaxis_height = xaxis_height, yaxis_width = 120, yaxis_font_size = "11pt", height = height, width = "100%")
        heatmap_final <- tagList(
          tags$h3(style = "text-align: left;", current_month),
          div(heatmap))
        htmltools::save_html(heatmap_final, paste(folder, "/", output_name, "_heatmap.html", sep = ""))

        # other option?
        #library(heatmaply)
        #data[is.na(data)] <- 0
        #heatmap2 <- heatmaply(data, Rowv = NULL, Colv = NULL, cluster_rows = FALSE, cluster_cols = FALSE)#, k_row = NA, k_col = NA)
        #saveWidget(heatmap2, paste0(folder, "/", output_name, "_heatmap_plotly.html"))
  } else if (nrow(data)==1) { # Heatmap cannot be built with only one row, saving a table instead
      base::print("1 sig lineage, making table")
      base::print(xtable(data), type = "html", file = paste(folder, "/", output_name, "_heatmap.html", sep = ""))
  } else if (nrow(current_month_all_hifreq_data)>1) { #else if
        #print(current_month_all_hifreq_data)
        base::print(dim(current_month_all_hifreq_data))
        current_month_all_hifreq_data[is.na(current_month_all_hifreq_data)] <- 0
        base::print(current_month_all_hifreq_data)
        base::print('test in hi freq lineage')
        heatmap <- d3heatmap(current_month_all_hifreq_data, Rowv = NULL, Colv = NULL, colors = "Blues", xaxis_font_size = "12pt", yaxis_font_size = "11pt", xaxis_height = xaxis_height, yaxis_width = 120, height = height, width = "100%")
        heatmap_final <- tagList(
          tags$h3(style = "text-align: left;", current_month),
          div(heatmap))
        htmltools::save_html(heatmap_final, paste(folder, "/", output_name, "_heatmap.html", sep = ""))

        # other option?
        #library(heatmaply)
        #current_month_all_hifreq_data[is.na(current_month_all_hifreq_data)] <- 0
        #heatmap2 <- heatmaply(current_month_all_hifreq_data, Rowv = NULL, Colv = NULL)#, k_row = NA, k_col = NA)
        #saveWidget(heatmap2, paste0(folder, "/", output_name, "_heatmap_plotly.html"))#, selfcontained = TRUE)
  } else {
      base::print('no data available')
      data_available <- FALSE
      return(data_available)
  }

  #htmltools::save_html(heatmap_final, paste(folder, "/", output_name, "_heatmap.html", sep = ""))
  #saveWidget(heatmap, paste(folder, "/", output_name, "_heatmap.html", sep = ""), selfcontained = TRUE)
}

# get all countries to be analyzed based on the existing folders
#countries <- dir(pattern = "^[A-Z][A-Z]") # countries and states (all folders with two upper case letters)
countries <- dir(path = folder, pattern = "^[A-Z][A-Z]$") # only countries, not states
states <- dir(path = folder, pattern = "^DE_") # only German states

months <- as.vector(t(read.table(months_file, stringsAsFactors = FALSE)))
month_len <- 1
current_month <- months[length(months)-month_len]
#current_month <- months[length(months)-1] # look at frequencies of the previous month to get representative values
x <- get_heatmap(folder, countries, current_month, "summary", TRUE, cutoff)
if (!is.null(x)){
  while (!is.null(x)){ # x == FALSE
    month_len <- month_len + 1
    base::print(current_month)
    x <- get_heatmap(folder, countries, current_month, "summary", TRUE, cutoff)
  }
}

month_len <- 1
current_month <- months[length(months)-month_len]
y <- get_heatmap(folder, states, current_month, "summary_states", FALSE, cutoff)
if (!is.null(y)){
  while (!is.null(y)){
    month_len <- month_len + 1
    current_month <- months[length(months)-month_len]
    base::print(current_month)
    y <- get_heatmap(folder, states, current_month, "summary_states", FALSE, cutoff)
  }
}

#get_heatmap(folder, countries, current_month, "summary", TRUE, cutoff)
#get_heatmap(folder, states, current_month, "summary_states", FALSE, cutoff)

# other option?
#library("heatmaply")
#heatmap <- heatmaply(data, Rowv = NULL, Colv = NULL)
#saveWidget(heatmap, "heatmap_test.html")
