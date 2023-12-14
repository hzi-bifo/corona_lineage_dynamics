#!/usr/bin/env Rscript
#this script performs a fisher test for all positions and seasons
#input is the prefix used in the AD plot analysis
args <- commandArgs(trailingOnly = TRUE)

# read data

prefix <- args[1]
mutations <- read.table(paste(prefix, ".subtreeMutationMap.txt", sep=""))
numIsolates <- read.table(paste(prefix, ".numIsolates.txt", sep=""))
mutnames <- read.table(paste(prefix, ".mutations.txt", sep=""), sep="\t")

cutoff <- as.numeric(args[2]) 	#defines which frequencies are considered predominant
#min_diff <- 0.5		#defines how large the difference to previous season should be

if (numIsolates[1,1] == 0) {
	#remove first column when no isolates
	#alternative: maybe remove all columns when no isolates??
	mutations = mutations[,-1]
	numIsolates = numIsolates[,-1]
}

# set negative mutation counts to 0
# set mutation counts larger than number of isolates in season to number of isolates
# (both shouldn't happen with a working frequency correction)
mutations[mutations<0] <- 0
for (col in 1:ncol(mutations)) {
	mutations[mutations[,col]>numIsolates[1,col],col] <- numIsolates[1,col]
}

# initiate matix to save pvalues
n_seasons <- ncol(mutations)
n_positions <- nrow(mutations)
pval <- matrix(nrow = n_positions, ncol = n_seasons-1)
#diffs <- matrix(nrow = n_positions, ncol = n_seasons-1)


for (season in 2:n_seasons) {
	num_thisseason = as.integer(numIsolates[season])
	num_prevseason = as.integer(numIsolates[season -1])

	for (position in 1:n_positions) {
		# get values for contigency table
		mut_thisseason <- as.integer(mutations[position, season])
		mut_prevseason <- as.integer(mutations[position, season - 1])
		
		nomut_thisseason <- num_thisseason - mut_thisseason
		nomut_prevseason <- num_prevseason - mut_prevseason
		
		# calculate fisher test and save pvalue in matrix
		data <- matrix(c(mut_thisseason, nomut_thisseason, mut_prevseason, nomut_prevseason), nrow = 2)
		pval[position, season -1] <- fisher.test(data, alternative = "greater")$p.value

		#diffs[position, season -1] <- mut_thisseason/num_thisseason - mut_prevseason/num_prevseason
	}
}

# adjust p-values for multiple testing
#pval_adjusted <- matrix(p.adjust(pval, method = "bonferroni"), nrow = n_positions)
pval_adjusted <- matrix(p.adjust(pval, method = "fdr"), nrow = n_positions)

# add names
pval_names <- cbind(t(mutnames), pval_adjusted)

# save pvalues into new file
write.table(pval_names, file=paste(prefix,".results.pvalues_fishertest.txt", sep=""), sep = "\t", row.names = FALSE, col.names = FALSE, quote = FALSE)


#define frequency matrix instead if counts
frequency <- matrix(nrow = n_positions, ncol = n_seasons)
for (season in 1:n_seasons){
	frequency[,season] <- mutations[,season]/numIsolates[,season]
}

# save frequencies with names
freq_names <- cbind(t(mutnames), frequency)
write.table(freq_names, file=paste(prefix,".results.frequencies.txt", sep=""), sep = "\t", row.names = FALSE, col.names = FALSE, quote = FALSE)

# check if position was predominant before

# for this check: check NaNs in frequency table to 0
frequency[is.nan(frequency)] <- 0

prev_predom <- matrix(nrow = n_positions, ncol = n_seasons)
prev_predom[,1] <- 0

for (season in 2:n_seasons){
	if (season == 2) {
		prev_predom[,season] <- frequency[,1] > cutoff
	} else if (season > 2 && n_positions > 1) {
        	prev_predom[,season] <- rowSums(frequency[,1:season-1] > cutoff)
	} else if (season > 2 && n_positions == 1) {
		prev_predom[,season] <- sum(frequency[,1:season-1] > cutoff)
	}
}

# check which positions and seasons are significant and predominant 
sign_matrix <- cbind(rep(1, n_positions), pval_adjusted)
#diffs <- cbind(rep(0, n_positions), diffs)
#diffs[,2] <- 1
predom_sign <-matrix(nrow=n_positions, ncol=n_seasons)
predom_sign <- frequency > cutoff & sign_matrix < 0.05 & prev_predom == 0

predom_sign <- matrix(as.integer(predom_sign), nrow=n_positions)

# if there's less than x sequences in the latest season, no significance
if (numIsolates[length(numIsolates)] < 50){
  predom_sign[,length(numIsolates)] <- 0
}

# save results of testing
write.table(predom_sign, file=paste(prefix, ".significance.txt", sep=""), sep = "\t", row.names = FALSE, col.names = FALSE, quote = FALSE)
