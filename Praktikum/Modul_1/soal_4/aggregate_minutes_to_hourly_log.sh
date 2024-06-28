#!/bin/bash
#Membuat temporary file yang berisi semua yang telah dicatat di file .log
for files in "/home/PuroFuro/log/metrics_2*.log"; do
    for texts in $files; do
        cat $texts >> temp.txt
    done
done

#Variable path pembantu
takedir="/home/PuroFuro/soal4/temp.txt"
savedir="/home/PuroFuro/log/metrics_agg_$(date +'%Y%m%d%H').log"
dir=~


echo "type,mem_total,mem_used,mem_free,mem_shared,mem_buff,mem_available,swap_total,swap_used,swap_free,path,path_size" >> $savedir

#for max search
echo -n "minimum," >> $savedir
j=1
while [ 1 ]; do 

    nu="$"$j""
    nextcol="$"$((j+1))""
    #Mencari data paling besar di column j dan hanya di line kedua saja
    logs=$(awk -F, 'NR%2==0 {if('$nu' >= max) {max='$nu'}} END {print max}' $takedir)
    nextlogs=$(awk -F, 'NR%2==0 {if('$nextcol' >= max) {max='$nextcol'}} END {print max}' $takedir)

    #Jika output kosong maka loop berhenti
    if [ -z $logs ]; then
        break;
    else
        #Jika next outputnya kosong maka print output sekarang tanpa koma
        if [ -z $nextlogs ]; then
            echo -e "$logs" >> $savedir
        else
            echo -n "$logs," >> $savedir
        fi
        j=$(( j + 1 ))
    fi
done

#Untuk min search
echo -n "maximum," >> $savedir
i=1
while [ 1 ]; do


    nu="$"$i""
    nextcol="$"$((i+1))""
    #Mencari data paling kecil di column i
    logs=$(awk -F, 'min=="" || '$nu' < min {min='$nu'} END {OFS=",";print min}' $takedir)
    nextlogs=$(awk -F, 'min=="" || '$nextcol' < min {min='$nextcol'} END {OFS=",";print min}' $takedir)

    #Jika output kosong maka loop berhenti
    if [ -z $logs ]; then
        break
    else
        #Jika next outputnya kosong maka print output sekarang tanpa koma
        if [ -z $nextlogs ]; then
            echo -e "$logs" >> $savedir
        else
            echo -n "$logs," >> $savedir
        fi
        i=$(( i + 1 ))
    fi
    
done

#for comparing
i=1
echo -n "average," >> $savedir
while [ 1 ]; do

    #min
    nu="$"$i""
    nextcol="$"$((i+1))""
    logs1=$(awk -F, 'min=="" || '$nu' < min {min='$nu'} END {OFS=",";print min}' $takedir)
    nextlogs=$(awk -F, 'min=="" || '$nextcol' < min {min='$nextcol'} END {OFS=",";print min}' $takedir)
    #max  
    logs2=$(awk -F, 'NR%2==0 {if('$nu' >= max) {max='$nu'}} END {print max}' $takedir)

    if [ -z $logs2 ]; then
        break;

    #Print directory saat i==10
    elif [[ $i == 10 ]]; then
        echo -n "$dir," >> $savedir

    else
        #Jika next output kosong, maka mengubah output sekarang (yaitu integer dengan huruf) menjadi hanya integer untuk perhitungan
        if [ -z $nextlogs ]; then

            #Mengambil huruf dari salah satu min
            logslet=${logs1//[0-9]/}
            logs1=$(echo "$logs1" | grep -o '[0-9]\+')
            logs2=$(echo "$logs2" | grep -o '[0-9]\+')
        fi

        #Pertambahan
        let plus=$logs1+$logs2
        #Pengecekan angka genap atau ganjil
        mod=$(($plus%2))

        if [ $mod == 0 ];then

            #Jika genap perhitungan normal
            avg=`expr $plus / 2`

            if [ -z $nextlogs ]; then
                echo -n "$avg$logslet" >> $savedir
            else
            echo -n "$avg," >> $savedir
            fi
        else

            #Jika ganjil perhitungan menggunakan float
            avg=$(awk 'BEGIN {printf "%.1f", '$plus' / 2}')

            if [ -z $nextlogs ]; then
                echo -n "$avg$logslet" >> $savedir
            else
                echo -n "$avg," >> $savedir
            fi
        fi
    fi
    i=$(( i + 1 ))
done

#remove temp.txt
rm -rf temp.txt

#crontab
#0 * * * * /home/PuroFuro/soal4/aggregate_minutes_to_hourly_log.sh