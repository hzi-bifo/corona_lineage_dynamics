#!/bin/bash -x

country=$1
folder=$2
threshold=$3

proj_folder=.


# if there's only one season with more than 0 sequences in the results left, don't analyze data and skip to next folder
n_months=$(tr -s ' '  '\n' < $folder"/"$country".numIsolates.txt" | awk '{ if($1>0) print$1 }' | wc -l) 
if [ $n_months -eq 1 ]; then continue; fi

# perform fishertest
Rscript $proj_folder/scripts/fishertest.R $folder"/"$country $threshold
Rscript $proj_folder/scripts/fishertest.R $folder"/"$country"_corrected" $threshold

# build SD plot
Rscript $proj_folder/scripts/buildAD_sign.r $folder"/"$country "scripts/" "false" 12 "png" $folder"/"$country".months.txt"
Rscript $proj_folder/scripts/buildAD_sign.r $folder"/"$country"_corrected" "scripts/" "false" 12 "png" $folder"/"$country".months.txt" 

# build interactive SD plot
Rscript $proj_folder/scripts/buildAD_interactive.R $folder"/"$country $folder"/"$country".months.txt" "false" "false"
Rscript $proj_folder/scripts/buildAD_interactive.R $folder"/"$country"_corrected" $folder"/"$country".months.txt" "false" "false"

# build interactive SD plot with signfificant results from sliding window test
Rscript $proj_folder/scripts/buildAD_interactive_continuous.R $folder"/"$country $folder"/"$country".months.txt" "false" "false"

sleep $[ ( $RANDOM % 60 )  + 1 ]s
rm "$folder"/../control/$country.lock

