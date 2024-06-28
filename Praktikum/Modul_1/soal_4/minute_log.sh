#!/bin/bash

dir=~
#Variabel untuk command subs dengan nama date
savedir="/home/PuroFuro/log/metrics_$(date +'%Y%m%d%H%M%S').log"

#Command untuk cek memory dan disk
C_disk=$(du -sh $dir)
C_mem=$(free -m)
echo "mem_total,mem_used,mem_free,mem_shared,mem_buff,mem_available,swap_total,swap_used,swap_free,path,path_size" >> $savedir

#Mengambil hanya output yang memiliki integer
for output in $C_mem; do
    if [[ $output =~ ^[0-9]+$ ]]; then
        echo -n "$output," >> $savedir
    fi
done

#Memasukan output c_disk ke log
echo "$dir,${C_disk%%/*}" >> $savedir
#Read only permission
chmod 400 $savedir

#crontab
#* * * * * /home/PuroFuro/soal4/minute_log.sh