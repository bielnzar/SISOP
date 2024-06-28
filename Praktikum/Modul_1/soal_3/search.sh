#!/bin/bash
path="/home/PuroFuro/gingseng/genshin_character"

#Melakukan pengecekan untuk setiap file di masing-masing folder region
for folder in $path/*; do
  for file in $folder/*.jpg; do

    #Fix untuk menghilangkan .jpg, name untuk menghilangkan path
    fix=$(echo "${file%.*}")
    name=$(echo "${file##*/}")

    #Extract .txt dari .jpg tersebut
    steghide extract -sf "$file" -xf "$fix".txt -p "" -q
    string=$(cat "$fix.txt")

    #Waktu saat command dilakukan
    time=$(date '+%d/%m/%y %H:%M:%S')   

    #aHR0cHM merupakan encode dari kata 'http'
    if [[ $string == *aHR0cHM* ]]
    then
        #Decode file dan letakan waktu ke image.log
        found=$(echo $string | base64 -d )
        echo -e "\n$time: Found the secret file at $fix\nThe URL is $found"
        echo "$time [FOUND] $fix" >> image.log
        
        #Download dari URL
        wget --content-disposition $found
        echo "$found" >> "/home/PuroFuro/gingseng/$name.txt"
        rm -rf "$fix.txt"
        exit 0
    else
        echo "$time Not this one.."
        echo "$time [NOT FOUND] $fix" >> image.log
        rm -rf "$fix.txt"
    fi

    sleep 1
    done 

done