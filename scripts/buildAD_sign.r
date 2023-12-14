#!/usr/bin/env Rscript 
args <- commandArgs(trailingOnly = TRUE)
set.seed(28123)


# read parameters
#min_year <- as.numeric(args[1])
#max_year <- as.numeric(args[2])
#thresh_color <- as.numeric(args[2])
width <- as.numeric(args[4])

source(paste(args[2],"/buildAD_sign_fun.R", sep=""))
buildAD(infile.prefix = args[1], thresh_color=thresh_color, drucke=TRUE,
	filter_sign=args[3], plotwidth=width, format=args[5], months_file=args[6])
#source(paste(args[5],"R/plot_seasons.R", sep=""))
#plot_seasons(infile.prefix = args[3], cutoff = cutoff, thresh_color=.5, 
#        minyear=min_year, maxyear=max_year)


#-p -n ../../Analyse/segment1/ADPlot_human/humantree_bifurcating -t ../../Analyse/segment1/ADPlot_human/human_aa.fa -m ../../Analyse/segment1/ADPlot_human/human_aa.map -f AccTran -type PROTEIN -plot 0 -time bc
#-p -n ../../Analyse/segment1/ADPlot_human/humantree_bifurcating -t ../../Analyse/segment1/ADPlot_human/human_aa.fa -m ../../Analyse/segment1/ADPlot_human/human_aa.map -f AccTran -type PROTEIN -plot 0 -time bc
#-p -n ../../gisaid_analysis/segment1/ADPlot/humantree_bifurcating -t ../../gisaid_analysis/segment1/ADPlot/alignedAA.fa -m ../../gisaid_analysis/segment1/aa.map -f AccTran -type PROTEIN -time bw -plot 1
