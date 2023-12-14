# SDplots_lineages

This repository contains scripts to calculate frequences of pangolin lineages based on the Gisaid metadata file and to evaluate and plot the trajectories using the fishertest and visualitation from the SD plots pipeline.

To run this software, change into this folder and run:

> bash SDplots\_lineages\_local.sh PathTo/GisaidFile PathTo/MonthsFile OutputFolder Threshold

where the Gisaid file is the metadata file downloaded from Gisaid, the Months file is a file listing all months to be analysed (see the example in the folder testdata), the output folder is the name of the folder generated to store all results in, and the threshold is the frequency threshold for considering a lineage predominant (for SARS-CoV-2, a low value like 0.1 makes sense). 

To fetch the repository run:
> git clone git@github.com:hzi-bifo/corona_lineage_dynamics.git
> cd corona_lineage_dynamics
> make

For making the codes, C++ compiler with boost_regex development packages required to be installed.

To analyse the given testdata, run:

> bash SDPlots\_lineages\_local.sh testdata/metadata_subset.tsv testdata/months.txt results 0.1

## Folders
* scripts: source codes
* testdata: a fabricated dataset for testing the codes
* test: some test codes

  
