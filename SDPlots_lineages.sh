#!/bin/bash -x

echo "STARTING ./SDPlots_lineages.sh - 1"
date

gisaid_file=$1
time=$2
output_folder=$3
threshold=$4


# stop execution if an error occurs
set -e

# input for test data:
#gisaid_file="testdata/gisaid_Germany_France.tsv"
#time="testdata/months.txt"
#output_folder="results"
#threshold=0.1

proj_folder=$(dirname "$0")
proj_folder=.

mkdir -p $output_folder

# make

# get current alias list and lineage dates
alias=$output_folder"/aliases.tsv"
lineage_dates=$output_folder"/lineage_dates.tsv"
Rscript $proj_folder/scripts/get_alias_list.R $alias $lineage_dates


echo "STARTING ./SDPlots_lineages.sh - 2"
date

# export LD_LIBRARY_PATH=$CONDA_PREFIX/lib:$LD_LIBRARY_PATH
# count lineages for countries based on Gisaid metadata file
# uncorrected counts
#Rscript scripts/count_lineages_country.R $gisaid_file $time $output_folder "false" $alias $lineage_dates
$proj_folder/scripts/count_lineages_country $gisaid_file $time $output_folder "false" $alias $lineage_dates

echo "STARTING ./SDPlots_lineages.sh - 3"
date
# corrected counts
#Rscript scripts/count_lineages_country.R $gisaid_file $time $output_folder "true" $alias $lineage_dates
$proj_folder/scripts/count_lineages_country $gisaid_file $time $output_folder "true" $alias $lineage_dates

echo "STARTING ./SDPlots_lineages.sh - 4"
date
# count lineages for German states
Rscript $proj_folder/scripts/count_lineages_states.R "$(dirname $gisaid_file)/DE.metadata.tsv" $time $output_folder "false" $alias $lineage_dates
Rscript $proj_folder/scripts/count_lineages_states.R "$(dirname $gisaid_file)/DE.metadata.tsv" $time $output_folder "true" $alias $lineage_dates

echo "STARTING ./SDPlots_lineages.sh - 5"
date
# in addition to monthly analysis: sliding window fisher test
Rscript $proj_folder/scripts/fishertest_continuous.R $gisaid_file $threshold $output_folder

echo "STARTING ./SDPlots_lineages.sh - 6"
date
mkdir -p $output_folder/control
rm -f $output_folder/control/*.lock
set +e
# data for countries are written in separate folders; loop over all folders to perform analysis
for folder in $output_folder/*/ ; do
	country=$(basename "$folder")
	if [ "$country" == "control" ]; then continue; fi
	if [[ "$country" =~ \ |\' ]]; then continue; fi

	echo "$folder"
	echo "STARTING ./SDPlots_lineages.sh - $folder"
	date

	touch $output_folder/control/$country.lock
	sbatch -o $output_folder/control/$country.out -e $output_folder/control/$country.err -q long -t 10-00:00:00 --mem-per-cpu 10g --cpus-per-task 5 $proj_folder/SDPlots_country.sh $country $folder $threshold
	if [ "$?" != "0" ]
	then
		echo "sbatch failed"
		exit 1
	fi
done
set -e

set -- $output_folder/control/*.lock
while [ -f "$1" ]
do
	ls $output_folder/control/*.lock | wc -l
	echo "Sleeping"
	sleep 30
	set -- $output_folder/control/*.lock
done

echo "STARTING ./SDPlots_lineages.sh - 7"
date
# create summary statistics
Rscript $proj_folder/scripts/summary_statistics.R $output_folder $time 0.1
echo "ENDING ./SDPlots_lineages.sh"
date
# remove additional folders that are not needed
#rm -r $output_folder"/summary_heatmap_files" $output_folder"/summary_states_heatmap_files"
