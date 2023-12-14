#!/usr/bin/env Rscript

library("plotly")
library("htmlwidgets")
library("binom")

args = commandArgs(trailingOnly=TRUE)
infile.prefix <- args[1]
months_file <- args[2]
filter_sign <- as.logical(args[3])
error_bars <- as.logical(args[4])

output <- paste(infile.prefix, "_continuous.interactive.html", sep = "")

mutations <- paste(infile.prefix, ".mutations.txt", sep="")
mutmap <- paste(infile.prefix, ".subtreeMutationMap.txt", sep="")
numiso <- paste(infile.prefix, ".numIsolates.txt", sep="")
significance <- paste(infile.prefix, ".significance.txt", sep="")    
pvalues <- paste(infile.prefix, ".results.pvalues_fishertest.txt", sep="")

continuous <- paste(infile.prefix, "_sign_continuous.txt", sep = "")

d.names <- read.table(mutations, sep="\t", stringsAsFactors=FALSE)
d.data <- read.table(mutmap, sep="\t", stringsAsFactors=FALSE)
d.isolates <- read.table(numiso, sep=" ", stringsAsFactors=FALSE)
d.isolates <- as.numeric(d.isolates)
d.sign <- read.table(significance, sep="\t")
d.pvalues <- read.table(pvalues, sep="\t")
months <- as.vector(t(read.table(months_file, stringsAsFactors = FALSE)))

sign.continuous <- read.table(continuous, sep = "\t", stringsAsFactors = FALSE, header = TRUE)
# adjust d.sign according to continuous significance
d.sign[,] <- 0

if (nrow(sign.continuous) > 0){
  if (d.isolates[1] == 0){
    count_months <- months[-1]
  } else{
    count_months <- months
  }
  for (i in 1:nrow(sign.continuous)){
    row <- which(d.names == sign.continuous$lineage[i])
    col <- which(count_months == substr(sign.continuous$date[i], 1, 7))
  
    d.sign[row, col] <- 1
  }
}

# remove substitutions with problematic names (containing brackets or spaces) for plotting
keep_names <- which(!(grepl("(", d.names, fixed = TRUE) | grepl(")", d.names, fixed = TRUE) | grepl(" ", d.names, fixed = TRUE)))
d.names <- d.names[keep_names]
d.data <- d.data[keep_names,]
d.sign <- d.sign[keep_names,]
d.pvalues <- d.pvalues[keep_names,]

if(filter_sign == TRUE) {
  d.names <- d.names[rowSums(d.sign)!=0]
  d.data <- d.data[rowSums(d.sign)!=0,]
  d.pvalues <- d.pvalues[rowSums(d.sign)!=0,]
  d.sign <- d.sign[rowSums(d.sign)!=0,]       
 
  if(ncol(d.names) == 0) {
    #print("ERROR: no significant results. Use the option '-f false' when calling the SD plot pipeline to plot other results.")
    quit(status=10)
  }
}

months <- months[d.isolates > 0]
plotYears <- months

#------------decide, which curves should be highlighted----------------------#

# get frequencies
norm.data <-round(d.data[, 1:length(d.isolates)] / 
                    matrix(d.isolates, nrow=nrow(d.data), ncol=length(d.isolates), byrow=TRUE), 4)
test.data <- norm.data[, d.isolates > 0]


seli <- rep(FALSE, nrow(test.data)) # select important alleles
for (i in 1:nrow(test.data)){
  if (sum(d.sign[i,]) > 0) seli[i] <- TRUE
}

#### set colours
colVec <- rep("darkgray", nrow(test.data))

crp.rg <- colorRampPalette(c("red", "orange","green", "cyan", "blue", "purple","magenta"))
colours <- crp.rg(sum(seli))
colVec [seli] <- sample(colours)

# set line width
lwdVec <- rep(0.5, nrow(test.data))
lwdVec [seli] <- 1.5#3 


### create ggplot ###

# transform into dataframe
df_test <- as.data.frame(t(test.data))
colnames(df_test) <- d.names
df_test <- cbind(df_test, Month = months)

# create basic plot
plot <- ggplot(df_test, aes(x = Month, group = 1)) + theme_bw(base_size = 15) + theme(text = element_text(size = 15, family = "Arial")) +
  ylim(0,1) + xlab("Month") + ylab("Frequency") + theme(axis.text.x = element_text(angle = 45, hjust = 1)) #+ geom_line(aes_string(y = toString(d.names[1]))) + geom_line(aes_string(y = toString(d.names[2])))

# add lines
for (i in 1:length(d.names)){
  plot <- plot + geom_line(aes_string(y = toString(d.names[i])), col = colVec[i], size = lwdVec[i])
}

# optionally: add error bars
sign_ids <- which(seli)

if (error_bars == TRUE){
  total <- d.isolates[d.isolates > 0]
  d.data <- d.data[,d.isolates > 0]
  for (i in sign_ids){
    variant <- as.numeric(d.data[i,])
    # get confidence interval using Wilson method
    interval <- binom.confint(variant, total, methods = "wilson")
    # add upper and lower bounds to dataframe for plotting
    df_test$upper <- interval$upper
    df_test$lower <- interval$lower
    plot <- plot + geom_errorbar(df_test, mapping = aes(x = Month, ymin = lower, ymax = upper), color = colVec[i], width = 0.3)
  }
}

# transform to interactive plot
interactive_plot <- ggplotly(plot)
interactive_plot

## add text with links and asterisks if there are significant results

if (d.isolates[1] == 0){
  # first season was removed in significance file if there's no isolates
  d.isolates <- d.isolates[-1]
}
d.sign <- d.sign[, d.isolates > 0]

if (length(sign_ids) > 0){
  sign_per_season <- d.sign[sign_ids,]
  rownames(sign_per_season) <- sign_ids
  #sign_names <- d.names[as.integer(rownames(sign_per_season))]
  sign_names <- d.names[sign_ids]
  
  # go through seasons to allow plotting of more than one name per month/season
  for (i in 1:ncol(sign_per_season)){
    if (sum(sign_per_season[i]) == 0){
      # no sign lineage in this season: skip
      next
    } else {
      current_lin <- sign_names[which(sign_per_season[i] == 1)]
      current_lin_id <- as.integer(rownames(sign_per_season)[which(sign_per_season[i] == 1)])
      current_freq <- df_test[i,current_lin_id]
      current_link <- paste("<a href = 'https://cov-lineages.org/lineages/lineage_", current_lin, ".html' style=\"color:", colVec[current_lin_id], ";font-weight:bold;font-family:Arial\">", current_lin, "</a>", sep = "")
      current_y <- seq(1 - (length(current_lin)-1)*0.05, 1, by = 0.05)
      interactive_plot <- add_annotations(interactive_plot, x = i, y = current_y, text = current_link, showarrow = FALSE, font = list(size = 20))
      interactive_plot <- add_annotations(interactive_plot, x = i, y = current_freq, text = "<b>*</b>", showarrow = FALSE, font = list(size = 30))
    }
  }
}

# save as html
saveWidget(partial_bundle(interactive_plot), basename(output), selfcontained = FALSE, libdir = "lib")
