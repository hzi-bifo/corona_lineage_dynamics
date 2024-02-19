# SDplots_lineages

This repository contains scripts to calculate frequences of pangolin lineages based on the Gisaid metadata file and to evaluate and plot the trajectories using the fishertest and visualitation from the SD plots pipeline.

## Installation
To fetch the repository run:
```shell
git clone git@github.com:hzi-bifo/corona_lineage_dynamics.git
cd corona_lineage_dynamics
make
```

For making the codes, C++ compiler with boost_regex development packages required to be installed.

## Run the pipeline
To run this software, change into the folder:
```shell
cd corona_lineage_dynamics
```

then run:
```shell
bash SDplots_lineages_local.sh \
  <PathTo/MetadataFile> \
  <PathTo/MonthsFile> \
  <OutputFolder> \
  <Threshold>
```

where the Metadata file is the metadata file downloaded from Gisaid, the Months file is a file listing all months to be analysed (see the example in the folder testdata), the output folder is the name of the folder generated to store all results in, and the threshold is the frequency threshold for considering a lineage predominant (for SARS-CoV-2, a low value like 0.1 makes sense). 


To analyse the given test data, run:
```shell
bash SDPlots_lineages_local.sh testdata/metadata_subset.tsv testdata/months.txt results 0.1
```

## Folders
* `scripts`: source codes
* `testdata`: a fabricated dataset for testing the codes
* `test`: some test codes

  
## Running on sample data
A sample (simulated) dataset is provided in folder `testdata`. Run all the following commands from inside the github repository folder.

To execute the pipeline, we need an output folder. It could be created with the following command
```
mkdir out
```
Note that if the folder already exists, it would be better to erase its contents.

Some packages are required by R for the pipeline. These could be installed with the following command (this could take several minutes).
```
Rscript -e 'install.packages(c("XML", "binom", "plotly", "htmlwidgets", "countrycode", "doParallel", "dplyr", "foreach", "htmltools", "plyr", "Rcpp", "RCurl", "readr", "stringr", "tictoc", "tidyr", "xtable"), repos="https://cloud.r-project.org"); if (!require("devtools")) install.packages("devtools", repos="https://cloud.r-project.org"); devtools::install_github("rstudio/d3heatmap");'
```

The whole pipeline could be executed on this dataset with the following command
```
./SDPlots_lineages_local.sh testdata/metadata.tsv testdata/months.txt out/ 0.1
```
  

