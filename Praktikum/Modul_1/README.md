
# WRITE UP
## Praktikum [Modul 1](https://github.com/lab-kcks/Modul-Sisop/tree/main/Modul-1)

Mata Kuliah Sistem Operasi

Dosen pengampu : Ir. Muchammad Husni, M.Kom.



## Kelompok Praktikum [IT-30]

- [Fico Simhanandi - 50272310](https://github.com/PuroFuro)
- [Jody Hezekiah - 5027221050](https://github.com/imnotjs)
- [Nabiel Nizar Anwari - 5027231087](https://github.com/bielnzar)


## [SOAL 1](https://docs.google.com/document/d/140T6O_YsbBcnblkKqQ5lpN1ji_XQzSMEpAkkqHrtTyU/edit)

Cipung dan abe ingin mendirikan sebuah toko bernama “SandBox”, sedangkan kamu adalah manajer penjualan yang ditunjuk oleh Cipung dan Abe untuk melakukan pelaporan penjualan dan strategi penjualan kedepannya yang akan dilakukan.

Setiap tahun Cipung dan Abe akan mengadakan rapat dengan kamu untuk mengetahui laporan dan strategi penjualan dari “SandBox”. Buatlah beberapa kesimpulan dari data penjualan [“Sandbox.csv”](https://drive.google.com/file/d/1cC6MYBI3wRwDgqlFQE1OQUN83JAreId0/view?usp=sharing) untuk diberikan ke cipung dan abe.

#### A. Karena Cipung dan Abe baik hati, mereka ingin memberikan hadiah kepada customer yang telah belanja banyak. Tampilkan nama pembeli dengan total sales paling tinggi


Untuk menyelesaikan soal diatas, jalankan code dibawah ini : 

```bash
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
```

#### Fungsi untuk Menampilkan Nama Pembeli dengan Total Penjualan Tertinggi

Skrip shell ini memiliki sebuah fungsi bernama `tampilkan_top_pembeli()` yang berfungsi untuk menampilkan nama pembeli dengan total penjualan tertinggi dari data yang terdapat dalam file `Sandbox.csv`.



Mendapatkan Nama Pembeli dengan Total Penjualan Tertinggi
bash

`top_sales=$(tail -n +2 Sandbox.csv | awk -F ',' '{print $6 "," $17}' | sort -t ',' -k2 -nr | head -n 1)`

Untuk Mendapatkan Nama Pembeli dengan Total Penjualan Tertinggi

1. Baris `#!/bin/bash` menunjukkan bahwa skrip akan dijalankan oleh program Bash, yang merupakan program yang digunakan untuk mengeksekusi perintah-perintah dalam sistem operasi Unix/Linux.

2. Fungsi `tampilkan_top_pembeli()` isi dan penjelasannya sebagai berikut:
  
Untuk mendapatkan nama pembeli dengan total penjualan tertinggi, skrip melakukan operasi berikut pada file `Sandbox.csv`:

``` # Mendapatkan nama pembeli dengan total penjualan paling tinggi
top_sales=$(tail -n +2 Sandbox.csv | awk -F ',' '{print $6 "," $17}' | sort -t ',' -k2 -nr | head -n 1)

```
Baris ini melakukan beberapa operasi pada file Sandbox.csv:

- `tail -n +2 Sandbox.csv` menampilkan semua baris dalam file Sandbox.csv kecuali baris pertama.

- `awk -F ',' '{print $6 "," $17}'` memisahkan setiap baris berdasarkan koma (,) dan mencetak kolom ke-6 (nama pembeli) dan kolom ke-17 (total penjualan) yang dipisahkan dengan koma.

- `sort -t ',' -k2 -nr` mengurutkan hasil dari *awk* berdasarkan kolom ke-2 (total penjualan) secara descending (dari yang terbesar ke terkecil).

- `head -n 1` mengambil baris pertama dari hasil pengurutan, yaitu nama pembeli dengan total penjualan tertinggi.

- Hasilnya disimpan dalam variabel `top_sales`.

3. Untuk memisahkan nama pembeli dan total penjualan menggunakan skrip berikut : 
```
# Memisahkan nama pembeli dan total penjualan
customer=$(echo $top_sales | cut -d ',' -f1)
total_sales=$(echo $top_sales | cut -d ',' -f2)
```
Baris ini memisahkan nilai top_sales menjadi dua variabel:

- `customer` berisi nama pembeli yang diambil dari kolom pertama (sebelum koma).

- `total_sales` berisi total penjualan yang diambil dari kolom kedua (setelah koma).

4. Selanjutnya, Menampilkan hasil dengan config berikut : 
```
# Menampilkan hasil
echo "Nama pembeli dengan total penjualan paling tinggi adalah: $customer dengan total penjualan sebesar $total_sales"
```

Baris terakhir ini menampilkan hasil dari fungsi `tampilkan_top_pembeli` dengan mencetak "Nama pembeli dengan total penjualan paling tinggi adalah: [nama pembeli] dengan total penjualan sebesar [total penjualan]".

Jadi, fungsi `tampilkan_top_pembeli` ini akan mencari nama pembeli dengan total penjualan tertinggi dari data dalam file `Sandbox.csv`, lalu menampilkannya ke layar.





![github-small](https://github.com/bielnzar/Sisop/blob/main/a.png)

#### B. Karena karena Cipung dan Abe ingin mengefisienkan penjualannya, mereka ingin merencanakan strategi penjualan untuk customer segment yang memiliki profit paling kecil. Tampilkan customer segment yang memiliki profit paling kecil

Untuk menyelesaikan soal diatas, jalankan code dibawah ini : 

```bash
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
```
### Berikut adalah penjelasan untuk skrip shell yang diberikan :

1. Fungsi ini menggunakan perintah awk untuk menghitung total profit untuk setiap customer segment :

- `awk -F ',' '$7 == "Corporate" { sum += $20 } END { printf "%.2f", sum }' Sandbox.csv` menghitung total profit untuk customer segment "Corporate" dengan menjumlahkan kolom ke-20 (profit) dari baris-baris yang memiliki nilai "Corporate" pada kolom ke-7 (segment). Hasil diformat menjadi dua angka desimal dan disimpan dalam variabel `corporate_profit`.
- `homeoffice_profit` dan `consumer_profit` dihitung dengan cara yang sama untuk customer segment "Home Office" dan "Consumer".

2. Menampilkan Total Profit untuk Setiap Customer Segment
```
echo "Total profit untuk Corporate segment: $corporate_profit"
echo "Total profit untuk Home Office segment: $homeoffice_profit"
echo "Total profit untuk Consumer segment: $consumer_profit"
```
Kemudian, fungsi menampilkan total profit untuk setiap customer segment menggunakan perintah echo.

3. Mencari Profit Paling Rendah di Antara Ketiga Customer Segment
```
profit_terendah=$(echo -e "$corporate_profit\n $homeoffice_profit\n $consumer_profit" | sort -n | head -n 1)
```
Fungsi menggabungkan nilai `corporate_profit`, `homeoffice_profit`, dan `consumer_profit` menjadi satu string dengan dipisahkan oleh baris baru (\n). Kemudian, string tersebut diurutkan secara ascending (dari yang terkecil ke yang terbesar) menggunakan perintah `sort -n`. Nilai terkecil (profit terendah) diambil menggunakan perintah `head -n 1` dan disimpan dalam variabel `profit_terendah`.

4. Menampilkan Hasil
```
if [ "$(bc <<<"$profit_terendah == $corporate_profit")" -eq 1 ]; then
    echo "Segment dengan profit paling rendah adalah Corporate dengan total profit sebesar $profit_terendah"
elif [ "$(bc <<<"$profit_terendah == $homeoffice_profit")" -eq 1 ]; then
    echo "Segment dengan profit paling rendah adalah Home Office dengan total profit sebesar $profit_terendah"
else
    echo "Segment dengan profit paling rendah adalah Consumer dengan total profit sebesar $profit_terendah"
fi
```
Selanjutnya, fungsi menampilkan customer segment dengan profit terendah beserta nilai profitnya menggunakan pernyataan kondisional i`f...elif...else`. Perbandingan nilai `profit_terendah` dengan nilai `corporate_profit`, `homeoffice_profit`, dan `consumer_profit` dilakukan menggunakan perintah `bc`. yang digunakan untuk membandingkan dua nilai numerik.

Dalam contoh di atas, `-eq 1` digunakan untuk memeriksa apakah hasil evaluasi dari `bc` sama dengan 1. Jika benar (true, atau sama dengan 1), maka blok kode di dalam if akan dieksekusi.

Jadi, fungsi `segmentprofit_terendah` ini akan mencari customer segment dengan total profit terendah dari data dalam file `Sandbox.csv`, lalu menampilkannya ke layar.

![github-small](https://github.com/bielnzar/Sisop/blob/main/b.png)
#### C. Cipung dan Abe hanya akan membeli stok barang yang menghasilkan profit paling tinggi agar efisien. Tampilkan 3 category yang memiliki total profit paling tinggi 
```
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
```

1. Untuk mendapatkan total profit dari setiap kategori

Fungsi ini menggunakan perintah awk untuk menghitung total profit untuk setiap kategori:

- `awk -F ',' '$14 == "Technology" { sum += $20 } END { printf "%.2f", sum }' Sandbox.csv` menghitung total profit untuk kategori "Technology" dengan menjumlahkan kolom ke-20 (profit) dari baris-baris yang memiliki nilai "Technology" pada kolom ke-14 (category). Hasil diformat menjadi dua angka desimal dan disimpan dalam variabel `technology_profit`.

- `furniture_profit` dan `officesupplies_profit` dihitung dengan cara yang sama untuk kategori "Furniture" dan "Office Supplies".

2. Menggabungkan Profit dan Nama Kategori
```
profit=$(echo -e "$technology_profit\t Technology\n $furniture_profit\t Furniture\n $officesupplies_profit\t Office Supplies")
```
Fungsi menggabungkan nilai `technology_profit`, `furniture_profit`, dan `officesupplies_profit` dengan nama kategori masing-masing dalam satu string yang dipisahkan oleh tab (`\t`) dan baris baru (`\n`).

3. Mengurutkan total profit dari yang paling tinggi
```
profit=$(echo "$profit" | sort -nr)
```
Kemudian, string yang berisi profit dan nama kategori diurutkan secara descending (dari yang terbesar ke yang terkecil) menggunakan perintah `sort -nr`.

4. Menampilkan hasil

Fungsi menampilkan total profit untuk setiap kategori menggunakan perintah `echo`. Selanjutnya, fungsi menampilkan "Top 3 categories dengan total profit paling tinggi adalah : " diikuti dengan nama tiga kategori dengan profit tertinggi yang diperoleh dari mengambil tiga baris pertama `head -n 3` dari string `profit` yang telah diurutkan, kemudian mencetak kolom kedua `awk -F '\t' '{print $2}'` yang berisi nama kategori.

Kesimpulan
Jadi, fungsi `kategoriprofit_tertinggi` ini akan mencari tiga kategori dengan total profit tertinggi dari data dalam file Sandbox.csv, lalu menampilkannya ke layar.

![github-small](https://github.com/bielnzar/Sisop/blob/main/c.png)
#### D. Karena ada seseorang yang lapor kepada Cipung dan Abe bahwa pesanannya tidak kunjung sampai, maka mereka ingin mengecek apakah pesanan itu ada. Cari purchase date dan amount (quantity) dari nama adriaens
```
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
```
Fungsi shell script `mencaripesanan_adriaens` ini dirancang untuk mencari dan menampilkan tanggal pembelian (purchase date) dan jumlah (quantity) barang yang dibeli oleh pelanggan dengan nama depan "adriaens" dari sebuah file CSV. Mari kita bahas setiap bagian dari skrip ini untuk memahami cara kerjanya:

1. Mencari Data dengan `awk`:
```
pesanan=$(awk -F ',' '{split(tolower($6), a, " "); for(i in a) if(a[i] == "adriaens") print $2 "," $18}' Sandbox.csv)
```
- `awk -F ','` menggunakan awk dengan field separator (pemisah kolom) berupa koma (`,`), mengindikasikan bahwa file CSV dipisahkan oleh koma.
~ `{split(tolower($6), a, " "); for(i in a) if(a[i] == "adriaens") print $2 "," $18}` merupakan blok kode awk yang melakukan hal berikut:

- `split(tolower($6), a, " ")`: Memecah nilai dari kolom ke-6 menjadi array a berdasarkan spasi. Fungsi `tolower` digunakan untuk mengubah teks menjadi huruf kecil agar pencarian menjadi case-insensitive.
- `for(i in a) if(a[i] == "adriaens")`: Melakukan iterasi pada array a dan mencari jika ada elemen yang sama dengan "adriaens".
- `print $2 "," $18` : Jika kondisi terpenuhi, maka mencetak nilai dari kolom ke-2 (tanggal pembelian) dan kolom ke-18 (jumlah/quantity), dipisahkan oleh koma.

- Hasil pencarian disimpan dalam variabel `pesanan`.

2. Menampilkan hasil:
- Bagian ini memeriksa apakah variabel `pesanan` kosong atau tidak. Jika kosong, berarti tidak ada pesanan yang ditemukan untuk pelanggan dengan nama "adriaens".
```
if [ -z "$pesanan" ]; then
    echo "Pesanan untuk pelanggan dengan nama adriaens tidak ditemukan."
else
    echo "Purchase date dan amount (quantity) untuk pelanggan dengan nama adriaens adalah:"
    echo "$pesanan"
fi
```
- `[ -z "$pesanan" ]`: Memeriksa apakah string `pesanan` kosong.
    -z: Operator ini digunakan untuk memeriksa "string length is zero". Jika string yang diperiksa memang kosong, kondisi ini bernilai true.

- Jika kosong, mencetak pesan bahwa tidak ada pesanan yang ditemukan.
- Jika tidak kosong, mencetak tanggal pembelian dan jumlah barang yang dibeli oleh pelanggan dengan nama "adriaens".

![github-small](https://github.com/bielnzar/Sisop/blob/main/d.png)


# Soal 2

Oppie merupakan seorang peneliti bom atom, ia ingin merekrut banyak peneliti lain untuk mengerjakan proyek bom atom nya, Oppie memiliki racikan bom atom rahasia yang hanya bisa diakses penelitinya yang akan diidentifikasi sebagai user, Oppie juga memiliki admin yang bertugas untuk memanajemen peneliti,  bantulah oppie untuk membuat program yang akan memudahkan tugasnya 

**A. Buatlah 2 program yaitu login.sh dan register.sh**

**B. Setiap admin maupun user harus melakukan register terlebih dahulu menggunakan email, username, pertanyaan keamanan dan jawaban, dan password**

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/Screenshot_1.png)

**c. Username yang dibuat bebas, namun email bersifat unique. setiap email yang mengandung kata admin akan dikategorikan menjadi admin**

**d. Karena resep bom atom ini sangat rahasia Oppie ingin password nya memuat keamanan tingkat tinggi**

Pada register.sh, terdapat bagian pengecekan password. Dari atas ke bawah, huruf kapital, huruf kecil, dan angka. Berikut kodenya :
```bash
is_valid_password() {
    local password="$1"
    local length=${#password}
    local has_uppercase=$(echo "$password" | grep -q '[[:upper:]]' && echo true || echo false)
    local has_lowercase=$(echo "$password" | grep -q '[[:lower:]]' && echo true || echo false)
    local has_digit=$(echo "$password" | grep -q '[[:digit:]]' && echo true || echo false)

    if [ $length -ge 8 ] && [ "$has_uppercase" == true ] && [ "$has_lowercase" == true ] && [ "$has_digit" == true ]; then
        return 0   # valid
    else
        return 1   # invalid
    fi
}
```

Dan untuk enkripsi menggunakan base64 pada password :
```bash
encrypted_password=$(echo -n "$password" | base64)
```

**e. Karena Oppie akan memiliki banyak peneliti dan admin ia berniat untuk menyimpan seluruh data register yang ia lakukan ke dalam folder users file users.txt. Di dalam file tersebut, terdapat catatan seluruh email, username, pertanyaan keamanan dan jawaban, dan password hash yang telah ia buat.**

Penginputan email, username, pertanyaan keamanan serta jawaban, dan password yang telah dienkripsi menggunakan base 64 pada users.txt :
```bash
echo "$email:$username:$security_question:$security_answer:$encrypted_password" >> users.txt
```

**f. Setelah melakukan register, program harus bisa melakukan login. Login hanya perlu dilakukan menggunakan email dan password.**

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/Screenshot_2.png)

Mengecek apakah user ada dalam list users.txt :
```bash
user_exists() {
    local email="$1"
    if grep -q "^$email:" users.txt; then
        return 0   # exists
    else
        return 1   # does not exist
    fi
}
```
Mengambil password dari user yang dimaksud dari users.txt lalu di dekripsi :
```bash
get_stored_password() {
    local email="$1"
    local stored_password=$(grep "^$email:" users.txt | cut -d ':' -f 5)
    echo "$stored_password"
}

decrypt_password() {
    local encrypted_password="$1"
    echo "$encrypted_password" | base64 -d
}
```
**g. Karena peneliti yang di rekrut oleh Oppie banyak yang sudah tua dan pelupa maka Oppie ingin ketika login akan ada pilihan lupa password dan akan keluar pertanyaan keamanan dan ketika dijawab dengan benar bisa memunculkan password**

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/Screenshot_3.png)

Recovery password akan mengecek email dan jawaban dari pertanyaan keamanan, lalu menampilkan password. Tidak hanya itu, recovery juga akan mencatat sebuah log pada auth.log :
```bash
if user_exists "$email"; then
        # Security question verification
        stored_security_question=$(grep "^$email:" users.txt | cut -d ':' -f 3)
        echo "Security Question: $stored_security_question"
        echo "Enter your answer: "
        read security_answer

        stored_security_answer=$(grep "^$email:" users.txt | cut -d ':' -f 4)
        if [ "$security_answer" == "$stored_security_answer" ]; then
            stored_password=$(get_stored_password "$email")
            decrypted_stored_password=$(decrypt_password "$stored_password")
            echo "Your password is: $decrypted_stored_password"
            log_message "PASSWORD RECOVERY SUCCESS" "Password for user with email $email recovered successfully"
            exit 0
        else
            log_message "PASSWORD RECOVERY FAILED" "ERROR Incorrect security answer for user with email $email"
            echo "Error: Incorrect security answer."
            exit 1
        fi
```                     
h. Setelah user melakukan login akan keluar pesan sukses, namun setelah seorang admin melakukan login Oppie ingin agar admin bisa menambah, mengedit (username, pertanyaan keamanan dan jawaban, dan password), dan menghapus user untuk memudahkan kerjanya sebagai admin. 

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/Screenshot_4.png)

i. Ketika admin ingin melakukan edit atau hapus user, maka akan keluar input email untuk identifikasi user yang akan di hapus atau di edit

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/Screenshot_5.png)

**j. Oppie ingin programnya tercatat dengan baik, maka buatlah agar program bisa mencatat seluruh log ke dalam folder users file auth.log, baik login ataupun register.**

Contoh log berhasil pada register.sh
```bash
log_message "REGISTER SUCCESS" "User with email $email registered successfully"
```

Log yang telah dicatat selama pengetesan :

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/Screenshot_6.png)

## Revisi

- Penambahan fungsi admin pada login.sh
- Perbaikan fungsi recovery password
- Perbaikan fungsi login pada login.sh (kesalahan pada password yang belum di decrypt pada pencocokan)

## [SOAL 3](https://docs.google.com/document/d/140T6O_YsbBcnblkKqQ5lpN1ji_XQzSMEpAkkqHrtTyU/edit)
### Penjelasan awal.sh
Hal awal yang dilakukan adalah download zip dari URL yang diberikan, melakukan unzip, membuat folder sesuai region dan membuat variabel $path agar bisa disubstitute.
```
#!/bin/bash

#Mendownload Zip dari link
wget --content-disposition 'https://docs.google.com/uc?export=download&id=1oGHdTf4_76_RacfmQIV4i7os4sGwa9vN'
unzip genshin.zip && unzip genshin_character.zip

path="/home/PuroFuro/gingseng/genshin_character"
mkdir "$path/Inazuma" && mkdir "$path/Mondstat" && mkdir "$path/Liyue" && mkdir "$path/Sumeru" && mkdir "$path/Fontaine"
```
For loop dibawah ini digunakan untuk:
- Mengubah nama file di dalam folder genshin_character dari hex menjadi ascii
- Mengubah lagi namanya sesuai format yang ditentukan dengan bantuan file "list_character.csv". Secara garis besar, cara yang dipakai adalah mencari nama karakter dari file yang telah di-decode di "list_character.csv" lalu mencari region dari karakter tersebut dan diletakan ke dalam variabel. Lalu mengganti nama dari file menggunakan awk dengan mengganti urutan column. Akhirnya, letakan file tersebut ke folder region dengan bantuan variabel tadi.
```
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
```
Menggunakan awk untuk mencari setiap tipe weapon yang muncul dan meng-assign setiap weapon dengan variabel yang akan di-increment setiap kali weapon tersebut muncul lalu di print. Setelah itu menghapus file-file yang diminta soal.
```
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
```
### Terbentuknya folder dengan folder region tiap karakter yang sudah difilter saat awal.sh dijalankan serta perhitungan weapon di terminal
![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/Screenshot_20240330_192228.png)

### Folder Region setiap karakter
![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/Screenshot_20240330_192239.png)

### Penjelasan search.sh
Simpelnya, yang dilakukan oleh kode ini adalah untuk setiap mengecek setiap file yang ada di setiap folder region dan menghilangkan path dan extension (.jpg) dari file tersebut lalu outputnya dimasukan ke dalam variabel. variabel "time" nanti digunakan untuk printing waktu saja (date, month, year, hour, minute, second).
```
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
```
Ide yang dipakai untuk mencari secret urlnya adalah tulisan "aHR0cHM" yang merupakan kata "http" dalam base64. Condition di awal itu merupakan basis untuk menemukan secret image tersebut. Sleep 1 hanya untuk menjalankan kode setiap 1 detik.
```
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
```
### Proses pencarian file secret file
![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/Screenshot_20240330_192301.png)

### Ditemukannya secret file dan juga secret image
![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/Screenshot_20240330_192332.png)
## [SOAL 4](https://docs.google.com/document/d/140T6O_YsbBcnblkKqQ5lpN1ji_XQzSMEpAkkqHrtTyU/edit)

### Penjelasan minute_log.sh
Membuat banyak variabel yang akan membantu dalam substitution lalu echo tulisan (mem_total, dst) ke dalam path yang ditentukan dengan nama berupa waktu sesuai format.
```
#!/bin/bash

dir=~
#Variabel untuk command subs dengan nama date
savedir="/home/PuroFuro/log/metrics_$(date +'%Y%m%d%H%M%S').log"

#Command untuk cek memory dan disk
C_disk=$(du -sh $dir)
C_mem=$(free -m)
echo "mem_total,mem_used,mem_free,mem_shared,mem_buff,mem_available,swap_total,swap_used,swap_free,path,path_size" >> $savedir
```
Loop dibawah ini mengambil setiap output yang ada di $C_mem yaitu variabel yang menyimpan output dari free -m dan meng-echo hanya hasil int saja ke dalam $savedir. Setelah itu output dari du -sh juga dimasukan ke $savedir dan file diberi permission untuk user read dengan command "chmod 400"
```
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
```
Bawah ini hanyalah konfigurasi dari crontab yang jalan per menit
```
#crontab
#* * * * * /home/PuroFuro/soal4/minute_log.sh
```
### Terbentuknya folder log di home directory
![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/Screenshot_20240330_192609.png)

### Terisinya folder tersebut dengan log file yang terbentuk dari minute_log.sh
![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/Screenshot_20240330_192636.png)

### Isi dari salah satu file yang dihasilkan minute_log.sh
![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/Screenshot_20240330_192649.png)

### Penjelasan aggregate_minutes_to_hourly_log.sh
Karena di dalam folder log terdapat banyak file, maka isi dari file-file tersebut dimasukan ke dalam "file.txt" sementara untuk diproses. Lalu membuat variabel-variabel path yang membantu dalam substitution nanti.
```
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
```
Untuk mencari nilai Maximum, digunakan while loop yang akan berhenti jika output file berupa kosong. Di variabel logs, terdapat command awk untuk mencari nilai max dari file text sementara tadi di baris kedua saja yang berisi integer (NR%2==0). Lalu digunakan conditional statement untuk setiap column agar mendapatkan nilai maximum dari setiap column tersebut.
```
#for max search
echo -n "minimum," >> $savedir
j=1
while [ 1 ]; do 

    nu="$"$j""
    nextcol="$"$((j+1))""
    #Mencari data paling besar di column j dan hanya di line kedua saja
    logs=$(awk -F, 'NR%2==0 {if('$nu' >= max) {max='$nu'}} END {print max}' $takedir)
    nextlogs=$(awk -F, 'NR%2==0 {if('$nextcol' >= max) {max='$nextcol'}} END {print max}' $takedir)
```
If statement dibawah mengecek apabila output dari $logs merupakan kosong makan loop akan dihentikan dan "if [ -z $nextlogs ]" disini berguna untuk mengecek jika output $logs selanjutnya kosong atau tidak, apabila iya maka output $logs sekarang diprint tanpa koma.

```
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
```
Untuk algoritma yang digunakan untuk mencari minimum sebenarnya sama saja dengan algoritma untuk mencari maximum, dan cara echonya ke folder path yang dituju juga sama
```
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
```
Untuk bagian comparing, diulang penggunaan algoritma untuk mencari min dan max lagi dengan kode dibawah ini jadi hanya meng-copas dari kode sebelumnya.
```
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
```
Mengecek apabila salah satu output dari logs merupakan kosong maka loop berhenti dan pada saat i==10 maka akan print home directory dikarenakan directory merupakan string yang akan merusak kode perhitungan dibawah nanti
```
    if [ -z $logs2 ]; then
        break;

    #Print directory saat i==10
    elif [[ $i == 10 ]]; then
        echo -n "$dir," >> $savedir
```
Bagian ini hanya untuk cek bila output selanjutnya merupakan kosong dan jika iya maka output yang ada sekarang (e.g 29G) akan diambil dan dipisah integer dan stringnya agar dapat dipakai untuk perhitungan.
```
    else
        #Jika next output kosong, maka mengubah output sekarang (yaitu integer dengan huruf) menjadi hanya integer untuk perhitungan
        if [ -z $nextlogs ]; then

            #Mengambil huruf dari salah satu min
            logslet=${logs1//[0-9]/}
            logs1=$(echo "$logs1" | grep -o '[0-9]\+')
            logs2=$(echo "$logs2" | grep -o '[0-9]\+')
        fi
```
Intinya, disini melakukan operasi pertamabahan dan juga pembagian dari output $logs2 dan $logs2 yang masing-masing merupakan min dan max. Modulo disini hanya dipakai untuk cek apakah jumlah dari kedua int merupakan ganjil atau tidak, karena jika ganjil maka hasilnya harus float.
```
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
```
Disini adalah kode yang digunakan apabila hasil jumlah dari kedua integer adalah ganjil, operasi pembagian yang dipakai menggunakan awk dengan "printf "%.1f"" ini ada angka tersebut memiliki 1 angka dibelakang koma.
```
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
```
Setelah semua itu selesai dilakukan maka setiap hasil output yang sudah difilter tadi akan diletakan pada $savedir dan file.txt sementara tadi dihapus
```
#remove temp.txt
rm -rf temp.txt
```
Konfigurasi crontab yang menjalankan aggregate_minutes_to_hourly_log.sh setiap satu jam
```
#crontab
#0 * * * * /home/PuroFuro/soal4/aggregate_minutes_to_hourly_log.sh
```
### Terbentuknya file metrics_agg setelah menjalankan .sh aggregate
![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/Screenshot_20240330_192824.png)

### Isi dari file tersebut yang berisi Minimum, Maximum, dan juga Average dari semua file metrics minute
![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/Screenshot_20240330_192832.png)
