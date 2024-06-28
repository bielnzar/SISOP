#!/bin/bash

# fungsi untuk menampilkan nama pembeli dengan total penjualan tertinggi
tampilkan_top_pembeli() {
    # Mendapatkan nama pembeli dengan total penjualan paling tinggi
    top_sales=$(tail -n +2 Sandbox.csv | awk -F ',' '{print $6 "," $17}' | sort -t ',' -k2 -nr | head -n 1)

    # Memisahkan nama pembeli dan total penjualan
    customer=$(echo $top_sales | cut -d ',' -f1)
    total_sales=$(echo $top_sales | cut -d ',' -f2)

    # Menampilkan hasil
    echo "Nama pembeli dengan total penjualan paling tinggi adalah: $customer dengan total penjualan sebesar $total_sales"
}

segmentprofit_terendah() {
    # Mendapatkan total profit untuk setiap customer segment
    corporate_profit=$(awk -F ',' '$7 == "Corporate" { sum += $20 } END { printf "%.2f", sum }' Sandbox.csv)
    homeoffice_profit=$(awk -F ',' '$7 == "Home Office" { sum += $20 } END { printf "%.2f", sum }' Sandbox.csv)
    consumer_profit=$(awk -F ',' '$7 == "Consumer" { sum += $20 } END { printf "%.2f", sum }' Sandbox.csv)

    # Menampilkan total profit untuk setiap customer segment
    echo "Total profit untuk Corporate segment: $corporate_profit"
    echo "Total profit untuk Home Office segment: $homeoffice_profit"
    echo "Total profit untuk Consumer segment: $consumer_profit"

    # Mencari profit paling rendah di antara ketiga customer segment

    profit_terendah=$(echo -e "$corporate_profit\n $homeoffice_profit\n $consumer_profit" | sort -n | head -n 1)

    # Menampilkan hasil
    if [ "$(bc <<<"$profit_terendah == $corporate_profit")" -eq 1 ]; then
        echo "Segment dengan profit paling rendah adalah Corporate dengan total profit sebesar $profit_terendah"
    elif [ "$(bc <<<"$profit_terendah == $homeoffice_profit")" -eq 1 ]; then
        echo "Segment dengan profit paling rendah adalah Home Office dengan total profit sebesar $profit_terendah"
    else
        echo "Segment dengan profit paling rendah adalah Consumer dengan total profit sebesar $profit_terendah"
    fi
}

kategoriprofit_tertinggi() {
    # Mendapatkan total profit untuk setiap category
    technology_profit=$(awk -F ',' '$14 == "Technology" { sum += $20 } END { printf "%.2f", sum }' Sandbox.csv)
    furniture_profit=$(awk -F ',' '$14 == "Furniture" { sum += $20 } END { printf "%.2f", sum }' Sandbox.csv)
    officesupplies_profit=$(awk -F ',' '$14 == "Office Supplies" { sum += $20 } END { printf "%.2f", sum }' Sandbox.csv)

    # Menggabungkan profit dan nama kategori dalam satu string dengan tab sebagai pemisah
    profit=$(echo -e "$technology_profit\t Technology\n $furniture_profit\t Furniture\n $officesupplies_profit\t Office Supplies")

    # Mengurutkan total profit dari yang paling tinggi
    profit=$(echo "$profit" | sort -nr)

    # Menampilkan hasil
    echo "Total profit untuk kategori Technology: $technology_profit"
    echo "Total profit untuk kategori Furniture: $furniture_profit"
    echo "Total profit untuk kategori Office Supplies: $officesupplies_profit"

    echo "Top 3 categories dengan total profit paling tinggi adalah:"
    echo "$profit" | head -n 3 | awk -F '\t' '{print $2}'
}

mencaripesanan_adriaens() {
    # Mencari purchase date dan amount (quantity) dari pelanggan dengan nama "adriaens" berdasarkan firstname saja
    pesanan=$(awk -F ',' '{split(tolower($6), a, " "); for(i in a) if(a[i] == "adriaens") print $2 "," $18}' Sandbox.csv)

    # Menampilkan hasil
    if [ -z "$pesanan" ]; then
        echo "Pesanan untuk pelanggan dengan nama adriaens tidak ditemukan."
    else
        echo "Purchase date dan amount (quantity) untuk pelanggan dengan nama adriaens adalah:"
        echo "$pesanan"
    fi
}

main() {

    while true; do
        clear
        # Pilihan user untuk menjalankan fungsi yang diinginkan
        echo
        echo "Pilih fungsi yang ingin dijalankan:"
        echo "a) Tampilkan nama pembeli dengan total penjualan paling tinggi"
        echo "b) Cari customer segment dengan profit paling rendah"
        echo "c) Tampilkan 3 kategori dengan total profit paling tinggi"
        echo "d) Cari purchase date dan amount (quantity) dari pelanggan dengan nama 'adriaens'"
        read -p "Masukkan pilihan (a/b/c/d): " pilihan
        echo

        case $pilihan in
        a)
            tampilkan_top_pembeli
            echo
            read -p "Tekan Enter untuk melanjutkan..."
            ;;
        b)
            segmentprofit_terendah
            echo
            read -p "Tekan Enter untuk melanjutkan..."
            ;;
        c)
            kategoriprofit_tertinggi
            echo
            read -p "Tekan Enter untuk melanjutkan..."
            ;;
        d)
            mencaripesanan_adriaens
            echo
            read -p "Tekan Enter untuk melanjutkan..."
            ;;
        *) echo "Pilihan tidak valid." && break ;;
        esac

        clear
    done
}

main
