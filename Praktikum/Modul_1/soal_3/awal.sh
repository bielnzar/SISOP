#!/bin/bash

#Mendownload Zip dari link
wget --content-disposition 'https://docs.google.com/uc?export=download&id=1oGHdTf4_76_RacfmQIV4i7os4sGwa9vN'
unzip genshin.zip && unzip genshin_character.zip

path="/home/PuroFuro/gingseng/genshin_character"
mkdir "$path/Inazuma" && mkdir "$path/Mondstat" && mkdir "$path/Liyue" && mkdir "$path/Sumeru" && mkdir "$path/Fontaine"

#For loop untuk mengganti nama file dan memasukan ke folder menurut region
for file in $path/*.jpg; do
  
  #change hex to ascii
  name=$(echo "${file%.*}")
  fix=$(echo "${name##*/}")
  anjay=$(echo $fix | xxd -r -p)
  mv -- "$path/$fix".jpg "$anjay".jpg

  #change name and move to region
  region=$(awk -F,  "/$anjay/"'{OFS=","; print $2}' list_character.csv)
  change=$(awk -F,  "/$anjay/"'{OFS=",";print $2 "-" $1 "-" $3 "-" $4}' list_character.csv)
  mv -- "$anjay.jpg" "$change".jpg
  mv "$change".jpg "$path/$region"

done

#Untuk menghitung jumlah weapon yang ada sesuai dengan tipe mereka
awk '
BEGIN { print "Weapon Count:" }
/Claymore/  { ++l }
/Polearm/ { ++m }
/Catalyst/ { ++n }
/Bow/ { ++o }
/Sword/ { ++p }
END { print "Claymore:"l "\nPolearm:"m "\nCatalyst:"n "\nBow:"o "\nSword:"p }' list_character.csv

#Hapus file yang diminta
rm -rf genshin_character.zip && rm -rf list_character.csv && rm -rf genshin.zip