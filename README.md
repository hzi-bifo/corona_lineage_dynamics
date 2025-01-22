# corona_lineage_dynamics

This repository contains scripts to calculate frequences of pangolin lineages based on the GISAID metadata file and to compute and visualize their trajectories.

## Requirements
This README file is designed for linux system with installed conda. 
We tested the installation on Ubuntu 23.10 and conda 24.3.0.

## Installation (approx. 20 min)
To fetch the repository run:
```shell
git clone https://github.com/hzi-bifo/corona_lineage_dynamics.git
cd corona_lineage_dynamics
```

Creating conda environment with required dependencies is the simplest way. 
For making the C/C++ codes, C++ compiler with boost_regex development packages required to be installed.
```
conda env create -f environment.yml
conda activate corona_lineage_dynamics
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
To run this software, run the following:
```shell
bash SDplots_lineages_local.sh \
  <PathTo/MetadataFile> \
  <PathTo/MonthsFile> \
  <OutputFolder> \
  <Threshold>
```

where the Metadata file is the metadata file downloaded from GISAID, the Months file is a file listing all months to be analysed (see the example in the folder testdata), the output folder is the name of the folder generated to store all results in, and the threshold is the frequency threshold for considering a lineage predominant (for SARS-CoV-2, a low value like 0.1 makes sense). 

### Running the pipeline on simulated dataset (approx. 8 min)
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

If everything goes well, the pipeline will print several lines of log. The last line should be
```
ENDING ./SDPlots_lineages.sh
```
Also, besides the files inside the `outputs` folder, the pipeline should create the following files.
```
DE_continuous.interactive.html
DE_BB_continuous.interactive.html
DE_corrected.interactive.html
DE_BB_corrected.interactive.html
DE.interactive.html
DE_BB.interactive.html
DE_BE_continuous.interactive.html
JP_continuous.interactive.html
DE_BE_corrected.interactive.html
JP_corrected.interactive.html
DE_BE.interactive.html
JP.interactive.html
```


