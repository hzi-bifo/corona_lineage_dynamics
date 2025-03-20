# corona_lineage_dynamics

This repository contains scripts to calculate frequences of pangolin lineages based on the GISAID metadata file and to compute and visualize their trajectories.

## Requirements
This README file is designed for linux system with installed conda. 
We tested the installation on Ubuntu 23.10 and conda 24.3.0. 

Here we present three options for installing and working with corona_lineage_dynamics: 1) installing from source, 2) installing bioconda package, and 3) singularity container.

# Installation from source
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
Note that if you receive any error related to the network problem, please try to fix your network problem and make sure you can access this like inside your network. Some potential network errors could be
```
Error in function (type, msg, asError = TRUE)  : 
  Failed to connect to cov-lineages.org port 443 after 129736 ms: Could not connect to server
Calls: getURL -> curlPerform -> <Anonymous> -> fun
Execution halted
```
Or
```
Error in function (type, msg, asError = TRUE)  : 
  Could not resolve host: cov-lineages.org
Calls: getURL -> curlPerform -> <Anonymous> -> fun
Execution halted
```
If the pipeline encounter these errors, it will automatically switch to a version of the code inside the repository. Note that in this case, the data could not be up-to-date. Fixing the network connection will fix this issue.

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
You can install this pipeline through installing its bioconda package. For that, run the following command:
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

# Singularity container installation
Assuming the singularity is installed on the machine, first we assure that `cloud.sylabs.io` is already in the remote list. We can check the remote list with the following command
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


Then, we can run the whole pipeline given the metadata file. As an example, we download the sample metadata file, alias file shows the expanded names of the lineages, and the months file representing the months we want the reports to be generated for them, hosted in this repository and put it into the `testdata` folder.
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

