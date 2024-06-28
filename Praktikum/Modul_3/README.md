# Sisop-3-2024-MH-IT30
Praktikum Sistem Operasi Modul 3

## Praktikum [Modul 3](https://github.com/lab-kcks/Modul-Sisop/tree/main/Modul-3)

Mata Kuliah Sistem Operasi

Dosen pengampu : Ir. Muchammad Husni, M.Kom.


## Kelompok Praktikum [IT-30]

- [Fico Simhanandi - 50272310](https://github.com/PuroFuro)
- [Jody Hezekiah - 5027221050](https://github.com/imnotjs)
- [Nabiel Nizar Anwari - 5027231087](https://github.com/bielnzar)

## Soal 1

Pada zaman dahulu pada galaksi yang jauh-jauh sekali, hiduplah seorang Stelle. Stelle adalah seseorang yang sangat tertarik dengan Tempat Sampah dan Parkiran Luar Angkasa. Stelle memulai untuk mencari Tempat Sampah dan Parkiran yang terbaik di angkasa. Dia memerlukan program untuk bisa secara otomatis mengetahui Tempat Sampah dan Parkiran dengan rating terbaik di angkasa. Programnya berbentuk microservice sebagai berikut:

a) Dalam auth.c pastikan file yang masuk ke folder new-entry adalah file csv dan berakhiran  trashcan dan parkinglot. Jika bukan, program akan secara langsung akan delete file tersebut. 
Contoh dari nama file yang akan diautentikasi:

- belobog_trashcan.csv
- osaka_parkinglot.csv

Berikut adalah isi `code` untuk `auth.c`.

**Code:**

```c
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#define NEWDATADIR "/home/purofuro/Fico/M3S1/newdata/"

int main(){
    key_t key = 1234;
    int shmid = shmget(key, sizeof(char)* 1024, IPC_CREAT | 0666);
    char *csvdata = shmat(shmid, NULL, 0);

    DIR *dr;
    struct dirent *newdata;
    dr = opendir("/home/purofuro/Fico/M3S1/newdata/."); 
    if (dr) {
        while ((newdata = readdir(dr)) != NULL) {
            if((strstr(newdata->d_name, "trashcan.csv") != NULL || strstr(newdata->d_name, "parkinglot.csv") != NULL)){
                strcpy(csvdata, newdata->d_name);
                csvdata += strlen(newdata->d_name) + 1; //Go to the next line or array
            }
            else if(strcmp(newdata->d_name, ".") != 0 && strcmp(newdata->d_name, "..") != 0){
                char temp[100] = NEWDATADIR;
                strcat(temp, newdata->d_name);
                remove(temp);
            }
        }
        
    }
    closedir(dr); 
    *csvdata = '\0';
    shmdt(csvdata);
    return 0;

}
```

Proses pertama yang dilakukan adalah mengambil inspirasi dari kode yang ada di modul dan menggabungkan kode itu dengan kode yang telah dibuat, dimana di bagian awal menggunakan `opendir` untuk membukan directory yang berisi file `.csv`.

**Code:**

```c
if((strstr(newdata->d_name, "trashcan.csv") != NULL || strstr(newdata->d_name, "parkinglot.csv") != NULL)){
    strcpy(csvdata, newdata->d_name);
    csvdata += strlen(newdata->d_name) + 1; //Go to the next line or array
}
else if(strcmp(newdata->d_name, ".") != 0 && strcmp(newdata->d_name, "..") != 0){
    char temp[100] = NEWDATADIR;
    strcat(temp, newdata->d_name);
    remove(temp);
}
```

If else disini berguna untuk meletakan file yang bertulisan `parkinglot` atau `trashcan` kedalam anggapannya satu variable yang diletakan di shared memory dan apabila file bukan merupakan `.` atau `..` maka file tersebut akan dihapus.

**Sebelum auth.c dijalankan**

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/foto_modul3/before.png)

**Setelah auth.c dijalankan**

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/foto_modul3/after.png)

b) Format data (Kolom)  yang berada dalam file csv adalah seperti berikut:

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/foto_modul3/contoh1.png)

atau

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/foto_modul3/contoh2.png)

c) File csv yang lolos tahap autentikasi akan dikirim ke shared memory. 

Disini menggunakan gabungan antara `shmget`, `shmat`, dan `shmdt`.

d) Dalam rate.c, proses akan mengambil data csv dari shared memory dan akan memberikan output Tempat Sampah dan Parkiran dengan Rating Terbaik dari data tersebut.

**Code:**

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAXCHAR 1000
#define NEWDATADIR "/home/purofuro/Fico/M3S1/newdata/"
#define TRASH "Trash Can"
#define PARK "Parking Lot"

int main() {
    key_t key = 1234;
    int shmid = shmget(key, sizeof(char) * 1024, IPC_CREAT | 0666);
    char *csvdata = shmat(shmid, NULL, 0);

    while (*csvdata != '\0') {
        char datadir[MAXCHAR]; // Initialize datadir
        char filename[100];
        strcpy(datadir, NEWDATADIR); // Set the base directory
        strcpy(filename, csvdata);
        strcat(datadir, csvdata); // Concatenate the filename

        csvdata += strlen(csvdata) + 1;

        FILE *open = fopen(datadir, "r");
        if (open == NULL) {
            printf("Error opening file: %s\n", datadir);
            return 1;
        }

        char row[MAXCHAR];
        char *token, best[100], type[20]; // Initialize best and type
        int skip = 0;
        float max = 0.0;

        if (strstr(datadir, "trashcan") != NULL) strcpy(type, TRASH);
        else if(strstr(datadir, "parkinglot") != NULL) strcpy(type, PARK);

        while (fgets(row, MAXCHAR, open) != NULL) {
            if (skip == 0) {
                skip++;
                continue;
            }
            token = strtok(row, ",");
            while (token != NULL) {
                char tempname[100];
                if (isdigit(token[0]) || isdigit(token[1])) {
                    float temp = atof(token);
                    if (temp >= max) {
                        max = temp;
                        strcpy(best, tempname);
                    }
                } else {
                    strcpy(tempname, token);
                }
                token = strtok(NULL, ",");
            }
        }
        printf("Type: %s\nFilename: %s\n-----------------------------------\nName: %s\nRating: %0.1f\n\n",type, filename, best, max);

        fclose(open); // Close the file
    }

    shmdt(csvdata);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
```

Secara simpelnya, cara kerja dari kode ini adalah ia membaca isi dari variable `csvdata` yang diambil dari shared memory sampai ditemukannya `\0` atau null terminator, mengecek terlebih dahulu apakah file bernama `parkinglot` atau `trashcan`. Lalu menggunakan strtok untuk memisahkan setiap data apabila terdapat tanda koma `,`. Disana juga terdapat `temp` yang berguna untuk menyimpan data berupa float yang nanti akan dibandingkan dengan float lainnya yang berada di `.csv` tersebut untuk mencari maxnya. Setelah itu, maka diprint sesuai dengan yang disuruh oleh soal.

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/foto_modul3/rate.png)

e) Pada db.c, proses bisa memindahkan file dari new-data ke folder microservices/database, WAJIB MENGGUNAKAN SHARED MEMORY.

**Code**

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <time.h>
#include <unistd.h>

#define NEWDATADIR "/home/purofuro/Fico/M3S1/newdata/"
#define DATABASEDIR "/home/purofuro/Fico/M3S1/microservices/database/"

int main() {
    key_t key = 1234;
    int shmid = shmget(key, sizeof(char) * 1024, IPC_CREAT | 0666);
    char* csvdata = shmat(shmid, NULL, 0);
    char type[20]; // Initialize type
    time_t my_time;
    struct tm* timeinfo;
    time(&my_time);
    timeinfo = localtime(&my_time);
    FILE* log = fopen("/home/purofuro/Fico/M3S1/microservices/database/db.log", "a");

    while (*csvdata != '\0') {
        char filename[100];
        strcpy(filename, csvdata);
        csvdata += strlen(csvdata) + 1;
        if (strstr(filename, "trashcan") != NULL) strcpy(type, "Trash Can");
        else strcpy(type, "Parking Lot");

        char NDname[100] = NEWDATADIR; // Initialize NDname
        char DDname[100] = DATABASEDIR; // Initialize DDname
        strcat(NDname, filename);
        strcat(DDname, filename);

        if (rename(NDname, DDname) != 0) {
            perror("Error renaming file");
            return 1;
        }

        fprintf(log, "[%d/%d/%d %d:%d:%d] [%s] [%s]\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, type, filename);
    }

    shmdt(csvdata);
    shmctl(shmid, IPC_RMID, NULL);
    fclose(log); // Close the log file
    return 0;
}
```

Dibagian `db.c` konsep yang dipakai hampir sama dengan yang digunakan di `rate.c`, yang membedakan hanyalah bagian tengah sampai ke akhir.

**Code**

```c
char NDname[100] = NEWDATADIR; // Initialize NDname
char DDname[100] = DATABASEDIR; // Initialize DDname
strcat(NDname, filename);
strcat(DDname, filename);

if (rename(NDname, DDname) != 0) {
    perror("Error renaming file");
    return 1;
}
```

yang dimana kegunaan kode ini hanya meletakan file ke folder yang disuruh oleh soal (microservice/database).

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/foto_modul3/db.png)

f) Log semua file yang masuk ke folder microservices/database ke dalam file db.log dengan contoh format sebagai berikut:

- [DD/MM/YY hh:mm:ss] [type] [filename]

ex : `[07/04/2024 08:34:50] [Trash Can] [belobog_trashcan.csv]`

**Code**

```c
fprintf(log, "[%d/%d/%d %d:%d:%d] [%s] [%s]\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, type, filename);
```

Karena tadi dituliskan `fopen` untuk file `db.log` dengan tambah `'a'` atau append, maka fprint ini akan menuliskan waktu, tipe file, dan nama file kedalam file db.log.

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/foto_modul3/dblog.png)


# Soal 2

Max Verstappen 🏎️ seorang pembalap F1 dan programer memiliki seorang adik bernama Min Verstappen (masih SD) sedang menghadapi tahap paling kelam dalam kehidupan yaitu perkalian matematika, Min meminta bantuan Max untuk membuat kalkulator perkalian sederhana (satu sampai sembilan). Sembari Max nguli dia menyuruh Min untuk belajar perkalian dari web (referensi) agar tidak bergantung pada kalkulator.
(Wajib menerapkan konsep pipes dan fork seperti yang dijelaskan di modul Sisop. Gunakan 2 pipes dengan diagram seperti di modul 3).

**A. Sesuai request dari adiknya Max ingin nama programnya dudududu.c. Sebelum program parent process dan child process, ada input dari user berupa 2 string. Contoh input: tiga tujuh.**

**B. Pada parent process, program akan mengubah input menjadi angka dan melakukan perkalian dari angka yang telah diubah. Contoh: tiga tujuh menjadi 21**

**C. Pada child process, program akan mengubah hasil angka yang telah diperoleh dari parent process menjadi kalimat. Contoh: `21` menjadi “dua puluh satu”.**

**D. Max ingin membuat program kalkulator dapat melakukan penjumlahan, pengurangan, dan pembagian, maka pada program buatlah argumen untuk menjalankan program :**

Perkalian	: ./kalkulator -kali
Penjumlahan	: ./kalkulator -tambah
Pengurangan	: ./kalkulator -kurang
Pembagian	: ./kalkulator -bagi

**Beberapa hari kemudian karena Max terpaksa keluar dari Australian Grand Prix 2024 membuat Max tidak bersemangat untuk melanjutkan programnya sehingga kalkulator yang dibuatnya cuma menampilkan hasil positif jika bernilai negatif maka program akan print “ERROR” serta cuma menampilkan bilangan bulat jika ada bilangan desimal maka dibulatkan ke bawah.**

Dapat kita lihat pada kode, program akan membaca komentar yang digunakan pada kode. Seperti pada operasi perkalian :

```
        if (strcmp(opsi, "-kali") == 0) {
            hasil = num1 * num2;
            if (hasil < 0) {
                strcpy(words, "ERROR");
            } else {
                hasil = floor(hasil);
                angkaKeKata(hasil, words);
            }
            char timestamp[20];
            format_time(timestamp);
```

**E. Setelah diberi semangat, Max pun melanjutkan programnya dia ingin (pada child process) kalimat akan di print dengan contoh format :**

Perkalian	: “hasil perkalian tiga dan tujuh adalah dua puluh satu.”
Penjumlahan	: “hasil penjumlahan tiga dan tujuh adalah sepuluh.”
Pengurangan	: “hasil pengurangan tujuh dan tiga adalah empat.”
Pembagian	: “hasil pembagian tujuh dan tiga adalah dua.”

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/foto_modul3/foto_modul3/Screenshot_1.png)

**F. Max ingin hasil dari setiap perhitungan dicatat dalam sebuah log yang diberi nama histori.log. Pada parent process, lakukan pembuatan file log berdasarkan data yang dikirim dari child process.**

Format: [date] [type] [message]
Type: KALI, TAMBAH, KURANG, BAGI
Ex:
[10/03/24 00:29:47] [KALI] tujuh kali enam sama dengan empat puluh dua.
[10/03/24 00:30:00] [TAMBAH] sembilan tambah sepuluh sama dengan sembilan belas.
[10/03/24 00:30:12] [KURANG] ERROR pada pengurangan.

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/foto_modul3/foto_modul3/Screenshot_2.png)

Changelog :

- Perbaikan pada output di terminal
- Perbaikan pada output di log
- Perubahan pada operasi yang membuat program dijalankan dengan 3 strings

## Soal 3

Shall Leglerg🥶 dan Carloss Signs 😎 adalah seorang pembalap F1 untuk tim Ferrari 🥵. Mobil F1 memiliki banyak pengaturan, seperti penghematan ERS, Fuel, Tire Wear dan lainnya. Pada minggu ini ada race di sirkuit Silverstone. Malangnya, seluruh tim Ferrari diracun oleh Super Max Max pada hari sabtu sehingga seluruh kru tim Ferrari tidak bisa membantu Shall Leglerg🥶 dan Carloss Signs 😎 dalam race. Namun, kru Ferrari telah menyiapkan program yang bisa membantu mereka dalam menyelesaikan race secara optimal. Program yang dibuat bisa mengatur pengaturan - pengaturan dalam mobil F1 yang digunakan dalam balapan. Programnya ber ketentuan sebagai berikut:

a) Pada program actions.c, program akan berisi function function yang bisa di call oleh paddock.c

b)  Action berisikan sebagai berikut:

- Gap [Jarak dengan driver di depan (float)]: Jika Jarak antara Driver dengan Musuh di depan adalah < 3.5s maka return Gogogo, jika jarak > 3.5s dan 10s return Push, dan jika jarak > 10s maka return Stay out of trouble.
- Fuel [Sisa Bensin% (string/int/float)]: Jika bensin lebih dari 80% maka return Push Push Push, jika bensin di antara 50% dan 80% maka return You can go, dan jika bensin kurang dari 50% return Conserve Fuel.
- Tire [Sisa Ban (int)]: Jika pemakaian ban lebih dari 80 maka return Go Push Go Push, jika pemakaian ban diantara 50 dan 80 return Good Tire Wear, jika pemakaian di antara 30 dan 50 return Conserve Your Tire, dan jika pemakaian ban kurang dari 30 maka return Box Box Box.
- Tire Change [Tipe ban saat ini (string)]: Jika tipe ban adalah Soft return Mediums Ready, dan jika tipe ban Medium return Box for Softs.

		Contoh:
		[Driver] : [Fuel] [55%]
		[Paddock]: [You can go]

**Code**

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

char *GapFunc(char *gappy){
    char temp[10], temp1[10];
    sscanf(gappy, "%s %s", temp, temp1);
    float jarak = atof(temp1);
    if(jarak <= 3.5) return "Gogogo";
    else if(jarak > 3.5 && jarak <= 10) return "Push";
    else if(jarak > 10) return "Stay out of trouble";
}

char *FuelFunc(char *gingseng){
    char temp[10], temp1[10];
    sscanf(gingseng, "%s %s", temp, temp1);
    float bengsing = atof(temp1);
    if(bengsing > 80) return "Push Push Push";
    else if(bengsing >= 50 && bengsing <= 80) return "You can go";
    else if(bengsing < 50) return "Conserve Fuel";
}

char *TireFunc(char *tired){
    char temp[10], temp1[10];
    sscanf(tired, "%s %s", temp, temp1);
    int capek = atoi(temp1);
    if(capek > 80) return "Go Push Go Push";
    else if(capek > 50 && capek <= 80) return "Good Tire Wear";
    else if(capek >= 30 && capek <= 50) return "Conserve Your Tire";
    else if(capek < 30) return "Box Box Box";
}

char *ChangeFunc(char *SebutkanTipePasanganKamu){
    if(strstr(SebutkanTipePasanganKamu, "Soft") != NULL) return "Mediums Ready";
    else if(strstr(SebutkanTipePasanganKamu, "Medium") != NULL) return" Box for Soft";
}

char *NoneOfYourInputIsCorrectYouKnow(){
    return "Kesalahan Berpikir";
}   

void Printlog(char *source, char *action, char *afteraction) {
    char log[2048] = "/home/purofuro/Fico/M3S3/race.log";
    FILE *race = fopen(log, "a");
    if (race == NULL) {
        printf("Error opening log file\n");
        return;
    }
    time_t my_time;
    struct tm * timeinfo; 
    time (&my_time);
    timeinfo = localtime (&my_time);
    fprintf(race, "[%s] [%02d/%02d/%04d %02d:%02d:%02d]: [%s] [%s]\n", source,timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900,timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,action, afteraction);
    fclose(race);
}
```

Disini terdapat 4 fungsi utama yang akan dipanggil, 1 fungsi untuk mencatat log, dan 1 fungsi tambahan untuk mengakomodasikan penulisan yang salah. Didalam 4 fungsi utama ini, intinya mengambil data yang dimasukan saat function dipanggil dan mengecek antara perbandingan angka atau tidak tulisan apa yang ditulis disana `Soft` atau `Medium` dan function akan return tulisan sesuai dengan kriteria yang telah dipenuhi.

Untuk bagian `Printlog()`, ia membuka file yang terdapat di folder yang telah ditentukan lalu dengan bantuan `time` ia akan meng-append informasi mengenai waktu dan juga tambahan informasi lainnya kedalam `race.log` 



c) Pada paddock.c program berjalan secara daemon di background, bisa terhubung dengan driver.c melalui socket RPC.

**Code**

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
//Aksi dari action anjay
#include "action.c"

void doraemon(){
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }

    umask(0);

    chdir("/");

    printf("PID: %d\n", getpid());
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

}

int main(int argc, char *argv[]) {
    doraemon();
    if(argc < 2){
        fprintf(stderr, "No port provided!\n");
        exit(1);
    }
    
    int server_fd, new_socket, n, port;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char baper[1024];
      
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    port = atoi(argv[1]);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );
      
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while(1){
        printf("Waiting for incoming connections...\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
    
        while(1){
            bzero(baper, 1024);
            char *sendback;
            n = read(new_socket, baper, 1024);
            char FirstWord[100], SecondWord[100], ThirdWord[100];
            sscanf(baper, "%s %s %s", FirstWord, SecondWord, ThirdWord);
            if(strcmp(FirstWord, "Gap") == 0 && isdigit(SecondWord[0])){
                Printlog("Driver", FirstWord, SecondWord);
                Printlog("Paddock", FirstWord, GapFunc(baper));
                sendback = GapFunc(baper);
                send(new_socket, sendback, strlen(sendback), 0);
            }
            else if(strcmp(FirstWord, "Fuel") == 0 && isdigit(SecondWord[0])){
                Printlog("Drive]", FirstWord, SecondWord);
                Printlog("Paddock", FirstWord, FuelFunc(baper));
                sendback = FuelFunc(baper);
                send(new_socket, sendback, strlen(sendback), 0);
                
            }
            else if(strcmp(FirstWord, "Tire") == 0 && isdigit(SecondWord[0])){
                Printlog("Driver", FirstWord, SecondWord);
                Printlog("Paddock", FirstWord, TireFunc(baper));
                sendback = TireFunc(baper);
                send(new_socket, sendback, strlen(sendback), 0);
            
            } 
            else if(strcmp(FirstWord, "Tire") == 0 && strcmp(SecondWord, "Change") == 0 && (strcmp(ThirdWord, "Soft") == 0 || strcmp(ThirdWord, "Medium") == 0)){
                char allword[1000];
                sprintf(allword, "%s %s", SecondWord, ThirdWord);
                Printlog("Driver", FirstWord, allword);
                Printlog("Paddock", FirstWord, ChangeFunc(baper));
                sendback = ChangeFunc(baper);
                send(new_socket, sendback, strlen(sendback), 0);
            }
            else{
                sendback = NoneOfYourInputIsCorrectYouKnow();
                Printlog("Driver", "Wrong", "Input");
                send(new_socket, sendback, strlen(sendback), 0);
            }
        
        }
        close(new_socket);
        
    }
    close(server_fd);

    return 0;
}
```
Bagian ini merupakan kode yang telah dipakai dalam modul sebelumnya untuk men-daemonkan proses

**Code**

```c
void doraemon(){
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }

    umask(0);

    chdir("/");

    printf("PID: %d\n", getpid());
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

}
```

Dan bagian untuk merupakan bagian RPCnya yang akan menghubungkan `paddock.c` dengan `driver.c`

**Code**

```c
int main(int argc, char *argv[]) {
    doraemon();
    if(argc < 2){
        fprintf(stderr, "No port provided!\n");
        exit(1);
    }
    
    int server_fd, new_socket, n, port;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char baper[1024];
      
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    port = atoi(argv[1]);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );
      
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while(1){
        printf("Waiting for incoming connections...\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
```

Yang spesial dari kode ini adalah kita dapat input port apapun kedalamnya karena pada saat dijalankan, program akan meminta argumen tambahan berupa port. Contoh penggunaannya `./paddock 8080`

**Paddock berjalan secara daemon**

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/foto_modul3/paddock.png)

d) Program paddock.c dapat call function yang berada di dalam actions.c.

**Code**

```c
bzero(baper, 1024);
char *sendback;
n = read(new_socket, baper, 1024);
char FirstWord[100], SecondWord[100], ThirdWord[100];
sscanf(baper, "%s %s %s", FirstWord, SecondWord, ThirdWord);
if(strcmp(FirstWord, "Gap") == 0 && isdigit(SecondWord[0])){
    Printlog("Driver", FirstWord, SecondWord);
    Printlog("Paddock", FirstWord, GapFunc(baper));
    sendback = GapFunc(baper);
    send(new_socket, sendback, strlen(sendback), 0);
}
else if(strcmp(FirstWord, "Fuel") == 0 && isdigit(SecondWord[0])){
    Printlog("Drive]", FirstWord, SecondWord);
    Printlog("Paddock", FirstWord, FuelFunc(baper));
    sendback = FuelFunc(baper);
    send(new_socket, sendback, strlen(sendback), 0);
    
}
else if(strcmp(FirstWord, "Tire") == 0 && isdigit(SecondWord[0])){
    Printlog("Driver", FirstWord, SecondWord);
    Printlog("Paddock", FirstWord, TireFunc(baper));
    sendback = TireFunc(baper);
    send(new_socket, sendback, strlen(sendback), 0);

} 
else if(strcmp(FirstWord, "Tire") == 0 && strcmp(SecondWord, "Change") == 0 && (strcmp(ThirdWord, "Soft") == 0 || strcmp(ThirdWord, "Medium") == 0)){
    char allword[1000];
    sprintf(allword, "%s %s", SecondWord, ThirdWord);
    Printlog("Driver", FirstWord, allword);
    Printlog("Paddock", FirstWord, ChangeFunc(baper));
    sendback = ChangeFunc(baper);
    send(new_socket, sendback, strlen(sendback), 0);
}
else{
    sendback = NoneOfYourInputIsCorrectYouKnow();
    Printlog("Driver", "Wrong", "Input");
    send(new_socket, sendback, strlen(sendback), 0);
}
```

Disini digunakan `n = read()` untuk membaca input yang diberikan dari driver dan logika yang dipakai adalah mendeklarasikan beberapa variable terlebih dahulu yaitu `FirstWord[100], SecondWord[100], ThirdWord[100]` yang akan berguna sebagai informasi `Command` dan `Additional-info` saat printing nanti dan apabila `Tire Change` dipanggil, maka `FirstWord` dan `SecondWord` digabung. Semua ini nanti akan dimasukan kedalam fungsi sesuai dengan kriteria. 

e) Program paddock.c tidak keluar/terminate saat terjadi error dan akan log semua percakapan antara paddock.c dan driver.c di dalam file race.log

- Format log:

    `[Source] [DD/MM/YY hh:mm:ss]: [Command] [Additional-info]`
ex :

    [Driver] [07/04/2024 08:34:50]: [Fuel] [55%]

    [Paddock] [07/04/2024 08:34:51]: [Fuel] [You can go]

Bagian ini sudah dijelaskan pada bagian `action.c` tadi. 

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/foto_modul3/racelog.png)

f) Program driver.c bisa terhubung dengan paddock.c dan bisa mengirimkan pesan dan menerima pesan serta menampilan pesan tersebut dari paddock.c sesuai dengan perintah atau function call yang diberikan.

**Code**

```c
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
  
int main(int argc, char *argv[]) {
    struct hostent *address;
    int sock, n, port;
    struct sockaddr_in serv_addr;
    char baper[1024];
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
    port = atoi(argv[2]);
    memset(&serv_addr, '0', sizeof(serv_addr));
    if(sock < 0){
        perror("error opening socket");
        address = gethostbyname(argv[1]);
        if(address == NULL) fprintf(stderr, "no such host");
    }
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
      
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    else printf("Connected!\n");

    char untukprint[100][100] = {"(a) Gap", "(b) Fuel", "(c) Tire", "(d) Tire Change"};
    sleep(1);
    while(1){

        printf("Puro says choose:\n");
        for(int i = 0; i < 4; i++){
            printf("%s\n", untukprint[i]);
        }
        memset(baper, 0, sizeof(baper));
        fgets(baper, sizeof(baper), stdin);
        baper[strcspn(baper, "\n")] = '\0';
        send(sock, baper, strlen(baper), 0);
        memset(baper, 0, sizeof(baper));
        recv(sock, baper, sizeof(baper), 0);
        printf("[PakDok Said]: [%s]\n", baper);
    }
    close(sock);
    return 0;
}
```

Code dari `driver.c` akan meminta beberapa argumen saat di run, yang pertama merupakan IP Adress dan juga Port, contoh pemakaian `./driver 192.23.212.12 8080` dan untuk IP harus disesuaikan dengan ip yang ada di laptop server atau di laptop yang menjalankan `paddock.c`. Kode ini saat di-run akan meminta user untuk memberikan input sesuai dengan yang tertulis di kode dan kode akan return hasil proses sesuai dengan tuliskan yang user tersebut ketik tadi. 

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/foto_modul3/driver.png)

g) Jika bisa digunakan antar device/os (non local) akan diberi nilai tambahan.

Berikut adalah video dimana bagian opsional ini berhasil:

[Video Bagian G) dari Modul 3 Soal 3](https://drive.google.com/file/d/1jZkra_tP_DSHwD9VBG3l71N3UXsyCOeC/view?usp=sharing)

h) untuk mengaktifkan RPC call dari driver.c, bisa digunakan in-program CLI atau Argv (bebas) yang penting bisa send command seperti poin B dan menampilkan balasan dari paddock.c
		
ex:
- Argv: 
**./driver -c Fuel -i 55%** 
- in-program CLI:
**Command: Fuel**

**Info: 55%**

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/foto_modul3/pakdok.png)

## [SOAL 4](https://docs.google.com/document/d/11l-PLiVaPjNwZawNiakcBfg39T_s_yluUffeMMb--Ac)

Lewis Hamilton 🏎 seorang wibu akut dan sering melewatkan beberapa episode yang karena sibuk menjadi asisten. Maka dari itu dia membuat list anime yang sedang ongoing (biar tidak lupa) dan yang completed (anime lama tapi pengen ditonton aja). Tapi setelah Lewis pikir-pikir malah kepikiran untuk membuat list anime. Jadi dia membuat [file](https://drive.google.com/file/d/10p_kzuOgaFY3WT6FVPJIXFbkej2s9f50/view?usp=drive_link) (harap diunduh) dan ingin menggunakan socket yang baru saja dipelajarinya untuk melakukan CRUD pada list animenya. 

a. Client dan server terhubung melalui socket. 

b. client.c di dalam folder client dan server.c di dalam folder server

c. Client berfungsi sebagai pengirim pesan dan dapat menerima pesan dari server.

d. Server berfungsi sebagai penerima pesan dari client dan hanya menampilkan pesan perintah client saja. 

e. Server digunakan untuk membaca myanimelist.csv. Dimana terjadi pengiriman data antara client ke server dan server ke client.
- Menampilkan seluruh judul
- Menampilkan judul berdasarkan genre
- Menampilkan judul berdasarkan hari
- Menampilkan status berdasarkan berdasarkan judul
- Menambahkan anime ke dalam file myanimelist.csv
- Melakukan edit anime berdasarkan judul
- Melakukan delete berdasarkan judul
- Selain command yang diberikan akan menampilkan tulisan “Invalid Command”

f. Karena Lewis juga ingin track anime yang ditambah, diubah, dan dihapus. Maka dia membuat server dapat mencatat anime yang dihapus dalam sebuah log yang diberi nama change.log.
- Format: [date] [type] [massage]
- Type: ADD, EDIT, DEL
- Ex:
[29/03/24] [ADD] Kanokari ditambahkan.

[29/03/24] [EDIT] Kamis,Comedy,Kanokari,completed diubah menjadi Jumat,Action,Naruto,completed.

[29/03/24] [DEL] Naruto berhasil dihapus.

g. Koneksi antara client dan server tidak akan terputus jika ada kesalahan input dari client, cuma terputus jika user mengirim pesan “exit”. Program exit dilakukan pada sisi client.

h. Hasil akhir:
soal_4/
    ├── change.log
    ├── client/
    │   └── client.c
    ├── myanimelist.csv
    └── server/
        └── server.c



### Membuat Source Code client.c

Program ini berfungsi untuk mengirim pesan ke server.c yang mana server bertugas sebagai pengeksekusi perintah, jadi dia yang akan membaca file myanimelist.csv dan memiliki kendali penuh atasnya. Nah, program client.c juga memiliki kemampuan untuk menampilkan hasil dari pengolahan perintah oleh server

client.c dan server.c terhubung menggunakan socket, yang mana socket tersebut harus di config dari kedua program agar bisa terhubung satu sama lain. Isi dari program client.c kurang lebih hanya config socket beserta jajarannya, dan config untuk menerima input dari pengguna, mengirim serta menerima pesan dari server, lalu menampilkannya.

berikut adalah isi dari source code client.c

```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define MAXLINE 1024

int main() {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[MAXLINE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    while (1) {
        printf("You : ");
        memset(buffer, 0, sizeof(buffer));
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        send(sock, buffer, strlen(buffer), 0);

        if (strcmp(buffer, "exit") == 0) {
            printf("Exiting the client\n");
            break;
        }

        memset(buffer, 0, sizeof(buffer));
        valread = read(sock, buffer, sizeof(buffer) - 1);
        printf("Server : \n%s\n", buffer);
    }

    close(sock);
    return 0;
}
```

Kami juga menmabahkan pendeteksi" error di beberapa kondisi, agar code bisa berjalan dengan maksimal dan mudah untuk melakukan manajemen dan maintenance error code.



### Membuat server.c

Seperti yang sudah dikatakan sebelumnya, server.c berfungsi sebagai code pengeksekusi perintah dari client.c

banyak sekali fungsi yang berada dalam code server.c ini, karena kami membuat sebuah fungsi untuk satu command. Kami menggunakan struct untuk menyimpan isi dari myanimelist.csv agar mudah dalam mengelola data, digunakan juga fopen untuk membaca melakukan dan melakukan perubahan pada file .csv, diluar header config socker, kami menambahkan `#include <time.h>`, yang berguna untuk mengetahui waktu, yang akan dipakai di penulisan change.log sesuai permintaan soal.

kurang lebih alur dari program server.c itu seperti ini :

membaca file .csv dan menyimpan isinya di struct -> menerima input dari client -> mengeksekusi perintah sesuai command -> menampilkan command dari client ke terminal -> mengirim hasil eksekusi command ke server -> menuliskan hasil perubahan ke change.log apabila sesuai kriteria yang ditentukan

code server.c tidak akan kami cantumkan disini karena terlalu panjang, bisa di baca dan di lihat di repo kami
### Hasil Kinerja

Hasil kolaborasi code client.c dengan server.c berjalan dengan maksimal, semua sudah sesuai dengan apa yang diminta soal, code juga tidak ada yang error, tidak ada revisi dari soal no. 4 ini. Kemudian kami akan menampilkan hasil program apabila dijalankan di terminal.

![github-small](https://github.com/bielnzar/sisop/blob/main/Modul3/1.png)

![github-small](https://github.com/bielnzar/sisop/blob/main/Modul3/2.png)

program sudah bisa menampilkan anime dengan mantap, baik secara keseluruhan, sesuai hari, genre, dan permintaan soal lainnya juga sudah terpenuhi seperti add, edit, delete, dkk. Program juga bisa menampilkan invalid command apabila command yang dimasukkan tidak sesuai, untuk memberitahu pengguna.

![github-small](https://github.com/bielnzar/sisop/blob/main/Modul3/3.png)

sesuai permintaan soal yaitu : "Koneksi antara client dan server tidak akan terputus jika ada kesalahan input dari client, cuma terputus jika user mengirim pesan “exit”. Program exit dilakukan pada sisi client."

![github-small](https://github.com/bielnzar/sisop/blob/main/Modul3/4.png)

Hasil akhir sudah sesuai dengan apa yang diminta soal, bisa dilihat isi dari file change.log dan struktur repo yang sudah sangat ajib dan mantap, alhamdulillah.

## Sekian Laporan Resmi Praktikum Sisop Modul 3
## TERIMAKASIH :)
