# SDplots_lineages

This repository contains scripts to calculate frequences of pangolin lineages based on the GISAID metadata file and to compute and visualize their trajectories.

## Installation
To fetch the repository run:
```shell
git clone git@github.com:hzi-bifo/corona_lineage_dynamics.git
cd corona_lineage_dynamics
```

Creating conda environment with required dependencies is the simplest way. 
For making the C/C++ codes, C++ compiler with boost_regex development packages required to be installed.
```
conda env create -f environment.yml
conda activate cld
```

Then, we compile the C++ codes.
```
make
```

Some packages are required by R for the pipeline. These could be installed with the following command (this could take several minutes).
```
Rscript -e 'install.packages(c("XML", "binom", "plotly", "htmlwidgets", "countrycode", "doParallel",
  "dplyr", "foreach", "htmltools", "plyr", "Rcpp", "RCurl", "readr", "stringr", "tictoc", "tidyr",
  "xtable"), repos="https://cloud.r-project.org");
  if (!require("devtools")) install.packages("devtools",
    repos="https://cloud.r-project.org");
    devtools::install_github("rstudio/d3heatmap");'
```


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

where the Metadata file is the metadata file downloaded from GISAID, the Months file is a file listing all months to be analysed (see the example in the folder testdata), the output folder is the name of the folder generated to store all results in, and the threshold is the frequency threshold for considering a lineage predominant (for SARS-CoV-2, a low value like 0.1 makes sense). 

A sample (simulated) dataset is provided in folder `testdata` for testing. 
First we need to create an output folder:
```
mkdir -p outputs
```
Note that if the folder already exists, it would be better to erase its contents.

To analyse the test data, run:
```shell
bash SDPlots_lineages_local.sh testdata/metadata.tsv testdata/months.txt outputs 0.1
```


