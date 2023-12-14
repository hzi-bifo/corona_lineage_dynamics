# SDplots_lineages

This repository contains scripts to calculate frequences of pangolin lineages based on the Gisaid metadata file and to evaluate and plot the trajectories using the fishertest and visualitation from the SD plots pipeline.

To run this software, change into this folder and run:

> bash SDplots\_lineages.sh PathTo/GisaidFile PathTo/MonthsFile OutputFolder Threshold

where the Gisaid file is the metadata file downloaded from Gisaid, the Months file is a file listing all months to be analysed (see the example in the folder testdata), the output folder is the name of the folder generated to store all results in, and the threshold is the frequency threshold for considering a lineage predominant (for SARS-CoV-2, a low value like 0.1 makes sense). 

To analyse the given testdata, run:

> bash SDPlots\_lineages.sh testdata/metadata_subset.tsv testdata/months.txt results 0.1


Additionally, this repository contains scripts to download the most recent RKI metadata from GitHub, transform these into the correct input, and run the above analysis for Germany and German states. To run the wrapper including all these steps, run:

> bash SDPlots\_lineages_RKI.sh testdata/plz\_states_extended.txt PathTo/MonthsFile DownloadFolder OutputFolder Threshold
