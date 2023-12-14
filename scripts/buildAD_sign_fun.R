#!/usr/bin/env Rscript
buildAD <- function(infile.prefix, cutoff = 0, thresh_color, drucke, filter_sign, plotwidth, format, months_file){

    #library(shape)
   
    mutations <- paste(infile.prefix, ".mutations.txt", sep="")
    mutmap <- paste(infile.prefix, ".subtreeMutationMap.txt", sep="")
    numiso <- paste(infile.prefix, ".numIsolates.txt", sep="")
    filename <- paste(infile.prefix, ".", format, sep="")
    sumyfile <- paste(infile.prefix, "summary.txt", sep="")
    significance <- paste(infile.prefix, ".significance.txt", sep="")    
    pvalues <- paste(infile.prefix, ".results.pvalues_fishertest.txt", sep="")
#    results_sign <- paste(infile.prefix, ".summed_isolates.results.txt", sep="")
#    results <- paste(infile.prefix, ".summed_isolates.txt", sep="")

    # read data
    d.names <- read.table(mutations, sep="\t", stringsAsFactors=FALSE)
    d.data <- read.table(mutmap, sep="\t", stringsAsFactors=FALSE)
    d.isolates <- read.table(numiso, sep=" ", stringsAsFactors=FALSE)
    d.isolates <- as.numeric(d.isolates)
    d.sign <- read.table(significance, sep="\t")
    d.pvalues <- read.table(pvalues, sep="\t")
    months <- as.vector(t(read.table(months_file, stringsAsFactors = FALSE)))
    
#    d.results <- read.table(results, sep="\t")              # all substitutions with leafs
#    colnames(d.results) <- c("substitution", "sign", "leafs")
#    d.results <- cbind(order=seq(1:nrow(d.results)), d.results)
    # merge all substitutions and support values
#    d.results_support <- merge(d.results, d.support, by=c("substitution", "leafs"), all.x=TRUE)
#    d.results_support <- d.results_support[order(d.results_support$order),]
#    d.results_support[is.na(d.results_support)] <- -1   # replace NAs (i.e. support values for nonsignificant substitutions) with -1
    
    #d.data <- d.data[, -ncol(d.data)]
    d.years <- 1:ncol(d.data)

    #tmp_years <- seq(minyear, maxyear+1, 0.5)

    # filter data to plot only significant positions with support value over given cutoff
    # non-significant substitutions have been assigned support value -1, i.e. setting the cutoff to 0 prints all significant values
    if(filter_sign=="true") {
      d.names <- d.names[rowSums(d.sign)!=0]
      d.data <- d.data[rowSums(d.sign)!=0,]
      d.pvalues <- d.pvalues[rowSums(d.sign)!=0,]
      d.sign <- d.sign[rowSums(d.sign)!=0,]       
      #d.names <- d.names[d.results_support$support >= support_cutoff]
      #d.data <- d.data[d.results_support$support >= support_cutoff,]
	    #d.pvalues <- d.pvalues[d.results_support$support >= support_cutoff,]
      #d.sign <- d.sign[d.results_support$support >= support_cutoff,]
	if(ncol(d.names) == 0) {
		#print("ERROR: no significant results. Use the option '-f false' when calling the SD plot pipeline to plot other results.")
		quit(status=10)
	}
    }

    # # define the plot window
    # if(startingSeason == "S"){
    #     plotYears <- ceiling(tmp_years)
    #     plotYears[tmp_years %% 1 == 0] <- paste(plotYears[tmp_years %% 1 == 0], "S",
    #                                         sep="")
    #     plotYears[tmp_years %% 1 > 0] <- paste(plotYears[tmp_years %% 1 > 0], "N",
    #                                        sep="")
    # }else{
    #     plotYears <- floor(tmp_years)
    #     plotYears[tmp_years %% 1 == 0] <- paste(plotYears[tmp_years %% 1 == 0], "N",
    #                                         sep="")
    #     plotYears[tmp_years %% 1 > 0] <- paste(plotYears[tmp_years %% 1 > 0], "S",
    #                                        sep="")
    # }
    # 
    # plotYears <- plotYears[1:length(d.years)]
    months <- months[d.isolates > 0]
    plotYears <- months
    print("read data, now setting names...")
    
#----------------------------------------------------------------------------#
# set names, for AD-Plots with longer sequences
    interNames <- rep("", ncol(d.names))
    for (i in 1:ncol(d.names)){
        # split main mutations from parent mutations      
      	tmpWholeSplit <- strsplit(as.character(d.names[1, i]), "\\{")[[1]] 
      	tmpSplit <- strsplit(tmpWholeSplit[1], " ")[[1]]
      	
      	# to remove ancestral state, set start=2
      	for (j in tmpSplit){ 
            interNames[i] <- paste(interNames[i], substring(j, 1, nchar(j)), sep=" ")
      	}
      	
      	interNames[i] <- substring(interNames[i], 2, nchar(interNames[i]))
      	if (length(tmpWholeSplit) > 1){
            tmpSplit <- strsplit(substring(tmpWholeSplit[2], 1,
                                 nchar(tmpWholeSplit[2]) - 1), " ")[[1]]
            tmpNames <- ""
            for (j in tmpSplit) {
                tmpNames <- paste(tmpNames, substring(j, 2, nchar(j)), sep=", ")
            }
            if (tmpNames != ""){
                tmpNames <- substring(tmpNames, 3, nchar(tmpNames))
                interNames[i] <- paste(interNames[i], " *", tmpNames, "*", sep="")
            }
      	}
    }

    used <- rep(FALSE, ncol(d.names))
    d.names.old <- sapply(d.names[1, ], function(x) substr(x, 2, nchar(x)))

    print ("names set, now defining important alleles...")


#------------decide, which curves should be highlighted----------------------#
    test.data <- d.data
    norm.data <-round(d.data[, 1:length(d.isolates)] / 
              matrix(d.isolates, nrow=nrow(d.data), ncol=length(d.isolates), byrow=TRUE), 4)
    test.data <- norm.data[, d.isolates > 0]
    d.years <- d.years[d.isolates > 0]
    #plotYears <- plotYears[d.isolates > 0]
    plotYears <- plotYears[1:length(d.years)]

    seli <- rep(FALSE, nrow(test.data)) # select important alleles
    for (i in 1:nrow(test.data)){
	if (sum(d.sign[i,]) > 0) seli[i] <- TRUE
    }

    print ("important ones set, now building the plot...")
    
#--------basic framework for the allele dynamics plot------------------------#
    colVec <- rep("darkgray", nrow(test.data))

    crp.rg <- colorRampPalette(c("red", "orange","green", "cyan", "blue", "purple","magenta"))
    colours <- crp.rg(sum(seli))
    colVec [seli] <- sample(colours)

    ltyVec <- rep(1, nrow(test.data))
    lwdVec <- rep(1, nrow(test.data))
    #lwdVec [seli] <- 1.5
    lwdVec [seli] <- 5#3 
    pchVec <- rep(2, nrow(test.data))
#    pchVec <- rep(3, nrow(test.data))
    pchVec2 <- rep("*", nrow(test.data))
    sizeVec <- rep(3, nrow(test.data))

    textSize <- 1.3 

    m <- ncol(test.data)
    n <- nrow(test.data)

    #define region which should be added to plot (in inches), for mutation names
    max_per_season <- max(colSums(d.sign))
    top <- 0.7+0.11*max_per_season  

    #use dev.new() for pdf output, png() for png output
    if(format == "pdf") {
	dev.new(file=filename, width=plotwidth, height=8+top)
    }else if(format == "png") {
	png(file=filename, width=plotwidth, height=8+top, units="in", res=300)
    }else {
        print("invalid file format; format should be pdf or png")
    }

    par(mai=c(1.02,0.82,0.82+top,0.42)) #bottom, left, top, right

    plot(d.years[d.years > cutoff], test.data[1, d.years > cutoff], type="n",
         col=colVec[1], ylim=c(0, 1), axes=FALSE, xlab="Month", ylab="Frequency",
         lwd=lwdVec[1], cex.lab=textSize)
    axis(1,at=d.years[d.years>cutoff],labels=plotYears[d.years>cutoff],cex.axis=textSize)
    axis(2, seq(0, 1, 0.1), cex.axis=textSize)
    #title(main="PB2", cex.main=textSize+0.5, line=12)   
 
    #plot dotlines vertically at each season and two horizontally at 0 and 1
    segments((cutoff + 1), 0.0, max(d.years), 0.0, col="gray", lwd=1, lty=3)
    segments((cutoff + 1), 1.0, max(d.years), 1.0, col="gray", lwd=1, lty=3)
    for (i in d.years[d.years > cutoff]){
        segments(i, 0.0, i, 1.0, col="gray", lwd=1, lty=3)
    }
 
    for (i in (1:n)) {
	tmpData <- test.data[i, d.years>cutoff]
        lines((d.years[d.years > cutoff]), tmpData, lwd=lwdVec[i], col=colVec[i],
        type="l", lty=ltyVec[i], pch=pchVec[i]) 
    }

    #plot significant positions and frequencies
    resultfile <- matrix(nrow=sum(seli), ncol=4)
    frequencyfile <- matrix(nrow=sum(seli), ncol=1+length(d.years))
    c <- 1

    par(xpd=NA)
    factor <- (m-1)/2
    max_counter <- 0
    for (j in (1:m)) {
	
	yvalues <- test.data[d.sign[,j]==1,j]
	if (length(yvalues)==0) {
		next
	}

	
	index <- which(d.sign[,j]==1)
	cluster <- c()
	counter <- 1
	cluster[1] <- counter

	#divide values into clusters
	for (a in (2:length(yvalues))) {
		diffs <- abs(yvalues[a] - yvalues[1:a-1])
		check <- which(diffs < 0.1)
		if (length(check) == 0) {
			counter <- counter+1
			cluster[a] <- counter
		} else {
			cluster[a] <- cluster[check[1]]
			cluster[check] <- cluster[check[1]]
		}	
	}
	
	#plot arrows and names (for each cluster)
	for (i in unique(cluster)) {
		maxvalue <- max(yvalues[cluster==i])
		#plot arrow at highest value
		#Arrows(j-(0.05*factor), maxvalue+0.05,  j-(0.03*factor), maxvalue+0.03, lwd=2, 
                #arr.type="curved", arr.length=0.3)
		#plot names
		mutnamevec <- interNames[index[cluster==i]]
		colours <- colVec[index[cluster==i]]
		counter <- 0
		for (name in mutnamevec) {
			#text(j-(0.05*factor),maxvalue+0.07+0.025*counter, 
			#labels=name, cex=1, font=2, col=colours[counter+1])
			counter <- counter + 1
		}
	}

	sorting <- sort(test.data[index,j], decreasing=TRUE, index.return=TRUE)$ix

	#plot stars and names above plot
	counter <- max_per_season-1
	for (i in index[sorting]) {
#		if (j==4 && counter < 3) { #only for creating Switzerland HongKong plot
#			#plot names
#			text(d.years[j]+0.5, test.data[1,j]+0.03*counter-0.03, labels=interNames[i],
#               	cex=textSize, font=2, col = colVec[i])
#			counter <- counter - 1
#			next
#		}
		points(d.years[j], test.data[i,j], lwd=lwdVec[i], col="black",
        	lty=ltyVec[i], pch=pchVec2[i], cex=sizeVec[i])

		text(d.years[j], 1.08+0.03*counter, labels=interNames[i],
                cex=textSize, font=2, col = colVec[i])
                counter <- counter - 1

		resultfile[c,1] <- plotYears[j]		#season
		resultfile[c,2] <- interNames[i]	#mutation
		resultfile[c,3] <- test.data[i,j] 	#frequency
		resultfile[c,4] <- d.pvalues[i,j]	#pvalue

		frequencyfile[c,1] <- interNames[i]
		frequencyfile[c,2:ncol(frequencyfile)] <- as.matrix(test.data[i,])		

		c <- c+1
	}
    } 

    axis(3,at=d.years[d.years>cutoff],labels=plotYears[d.years>cutoff], 
    cex.axis=textSize, pos=1.1+0.03*max_per_season)

    write.table(resultfile, file=paste(infile.prefix,".results.txt", sep=""), sep = "\t", row.names = FALSE, col.names = FALSE, quote = FALSE)
    write.table(frequencyfile, file=paste(infile.prefix,".significant_frequencies.txt", sep=""), sep = "\t", row.names = FALSE, col.names = FALSE, quote = FALSE)

#print("debug 11")

    resList <- vector("list", ncol(test.data) - 1)
    for (i in 2:ncol(test.data)) {
        sel <- test.data[, i] > 0.05 & test.data[, i] < 9.5
        resList [[i - 1]] <- cbind(interNames[sel], test.data[sel, i],
                                   test.data[sel, i] - test.data[sel, i - 1])
    }
    
    names(resList) <- plotYears[2:(length(plotYears))]
    
#print("debug 33")

    for (i in 1:length(resList)){
        cat(names(resList)[i], "\n", file=sumyfile, append=T, sep="")
        write.table(resList[[i]], sumyfile, row.names=FALSE, col.names=F, sep="\t",
                    quote=FALSE, append=TRUE)
    }
   
    dev.off 
    print("done!")

}
