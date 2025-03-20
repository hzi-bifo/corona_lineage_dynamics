# corona_lineage_dynamics

This repository contains scripts to calculate the frequencies of Pangolin lineages based on the GISAID metadata file and to compute and visualize their trajectories.

## Requirements
This README file is designed for a Linux system with Conda installed.
We tested the installation on Ubuntu 23.10 and Conda 24.3.0. 

Here, we present three options for installing and working with corona_lineage_dynamics: 1) installing from source, 2) installing the Bioconda package, and 3) using a Singularity container.
# Installation from source
## Installation (approx. 20 min)
To fetch the repository run:
```shell
git clone https://github.com/hzi-bifo/corona_lineage_dynamics.git
cd corona_lineage_dynamics
```

The simplest way to set up the required dependencies is by creating a Conda environment. 
To compile the C/C++ code, a C++ compiler with the `boost_regex` development package must be installed.
```
conda env create -f environment.yml
conda activate corona_lineage_dynamics
```

Then, we compile the C++ code.
```
make
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
If the folder already exists, it is recommended to clear its contents before proceeding.

To analyse the test data, run:
```shell
bash SDPlots_lineages_local.sh testdata/metadata.tsv testdata/months.txt outputs 0.1
```

If everything runs successfully, the pipeline will print several log messages.
The last line should be
```
ENDING ./SDPlots_lineages.sh
```

If you receive any error, please check [Errors](#errors) section

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

# Bioconda package installation
You can install this pipeline using its Bioconda package. To do so, run the following command:
```
conda install bioconda::corona_lineage_dynamics
```
This will install the package. Then, we can get the sample data:
```
mkdir -p testdata/ output/
wget https://github.com/hzi-bifo/corona_lineage_dynamics/raw/refs/heads/main/testdata/aliases.tsv -O testdata/aliases.tsv
wget https://github.com/hzi-bifo/corona_lineage_dynamics/raw/refs/heads/main/testdata/metadata.tsv -O testdata/metadata.tsv
wget https://github.com/hzi-bifo/corona_lineage_dynamics/raw/refs/heads/main/testdata/months.txt -O testdata/months.txt
```

And run the package
```
corona_lineage_dynamics testdata/metadata.tsv testdata/months.txt output 0.1
```

If everything runs successfully, the pipeline will print several log messages. The last line should be
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

# Singularity container installation
Assuming Singularity is installed on the machine, we first assure that `cloud.sylabs.io` is already in the remote list. We can check the remote list with the following command
```
singularity remote list
```
and we expect a result like this
```
NAME           URI                          GLOBAL  ACTIVE
cloud (default)  cloud.sylabs.io           YES     YES
local          ...
```
If cloud.sylabs.io is not in the remote list, you can add it with the following command.
```
singularity remote add --no-login cloud https://cloud.sylabs.io
```

Then, we pull the container with the following command.
```
singularity pull library://hforoughmand/hzi-bifo/corona_lineage_dynamics:latest
```

Then, we can run the entire pipeline using the metadata file. As an example, we download the sample metadata file, an alias file (which contains the expanded names of the lineages), and a months file (which specifies the months for which reports should be generated). These files are hosted in this repository and should be placed in the `testdata` folder.
```
mkdir -p testdata/ output/
wget https://github.com/hzi-bifo/corona_lineage_dynamics/raw/refs/heads/main/testdata/aliases.tsv -O testdata/aliases.tsv
wget https://github.com/hzi-bifo/corona_lineage_dynamics/raw/refs/heads/main/testdata/metadata.tsv -O testdata/metadata.tsv
wget https://github.com/hzi-bifo/corona_lineage_dynamics/raw/refs/heads/main/testdata/months.txt -O testdata/months.txt
```

The following command creates the reports
```
singularity exec corona_lineage_dynamics_latest.sif /opt/corona_lineage_dynamics/SDPlots_lineages_local.sh testdata/metadata.tsv testdata/months.txt output 0.1
```
If everything runs successfully, the pipeline will print several log messages. The last line should be
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

# Errors
During execution, the pipeline retrieves alias and lineage files from cov-lineages.org using an R script (get_alias_list.R). In some cases, the connection may timeout, resulting in an error like:

```
Error in function(type, msg, asError = TRUE) :
Connection timed out after 300040 milliseconds
Calls: getURL -> curlPerform -> -> fun
Execution halted
```

To prevent this issue, we have added automatic fallback mechanisms:
* The script first tries to download the page "https://cov-lineages.org/lineage_list.html".
* If it fails, the script tries to use the pre-downloaded version of the file located at `testdata/lineage_list.html".

### Solution 1: Check Your Internet Connection (Recommended)
If the download fails due to a network issue, follow these steps:
* Make sure you have a stable internet connection.
* Try accessing the link "https://cov-lineages.org/lineage_list.html" directly in your browser:
* If you cannot access the link, your network may have firewall restrictions.
  * Try running the script on a different network.
  * Use a VPN if necessary.

### Solution 2: Pre-Download Required Files
To avoid any network-related issues, we recommend manually downloading the alias file before running the pipeline.

Step 1: Download the required files manually
```
mkdir -p testdata
wget -O testdata/lineage_list.html https://cov-lineages.org/lineage_list.html
```

Step 2: Run the pipeline using local files
```
bash SDPlots_lineages_local.sh testdata/metadata.tsv testdata/months.txt outputs 0.1
```

### Solution 3: Run With the Current Version of the Data
If none of the previous solutions work, a current version of the file [lineage_list.html](https://cov-lineages.org/lineage_list.html) is already available in the `testdata/` folder.
This allows the pipeline to run without any errors. However, please ensure that the file is up-to-date if you intend to run the pipeline on recent datasets.

To execute the pipeline, run:
```
bash SDPlots_lineages_local.sh testdata/metadata.tsv testdata/months.txt outputs 0.1
```
