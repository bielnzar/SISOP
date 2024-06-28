## LAPORAN RESMI

## Praktikum [Modul 1](https://github.com/lab-kcks/Modul-Sisop/tree/main/Modul-1)

Mata Kuliah Sistem Operasi

Dosen pengampu : Ir. Muchammad Husni, M.Kom.


## Kelompok Praktikum [IT-30]

- [Fico Simhanandi - 50272310](https://github.com/PuroFuro)
- [Jody Hezekiah - 5027221050](https://github.com/imnotjs)
- [Nabiel Nizar Anwari - 5027231087](https://github.com/bielnzar)



## Soal 2

Paul adalah seorang mahasiswa semester 4 yang diterima magang di perusahaan XYZ. Pada hari pertama magang, ia diberi tugas oleh atasannya untuk membuat program manajemen file sederhana. Karena kurang terbiasa dengan bahasa C dan environment Linux, ia meminta bantuan kalian untuk mengembangkan program tersebut.

**A)** Atasannya meminta agar program tersebut dapat berjalan secara daemon dan dapat mengunduh serta melakukan unzip terhadap file berikut. Atasannya juga meminta program ini dibuat tanpa menggunakan command system()

Pertama-tama kita membuat function untuk melakukan pengecekan kalau file/folder sudah ada atau tidak.
```c
int ada(const char *fname){
    char fixname[100] = "/home/purofuro/Fico/Soal_2/";
    strcat(fixname, fname);
    if (access(fixname, F_OK) == 0) {
        return 1;
    } else {    
        return 0;
    }
}
```
Function ini akan meletekan menggabungkan path dan nama dari file agar daemon bisa mengetahui dimana letak file tersebut.
Lalu membuat function yang akan mengdownload file yang tertera di link yang diberikan menggunakan daemon.
```c
int default_act(){
    int status;
    if(ada("library.zip") == 0){
        pid_t download_child = fork();
        if(download_child < 0){
            printf("garpu gagal");
            exit(EXIT_FAILURE);
        }
        else if(download_child == 0){
            backup_mode = 0;
            char *argv[6] = {"Download", "--content-disposition", "https://docs.google.com/uc?export=download&id=1rUIZmp10lXLtCIH3LAZJzRPeRks3Crup", "-P", "/home/purofuro/Fico/Soal_2", NULL};
            execv("/bin/wget", argv);
        }
        
    } 
    wait(&status);
    if(WIFEXITED(status)){
        if(ada("library") == 0){
            pid_t unzip_child = fork();
            if(unzip_child == 0){
                char *argv[5] = {"unzip", "/home/purofuro/Fico/Soal_2/library.zip", "-d", "/home/purofuro/Fico/Soal_2", NULL};
                execv("/bin/unzip", argv);
            }
            
        } else{
            pid_t lib_delete_child = fork();
            if(lib_delete_child < 0){
                printf("garpu gagal");
                exit(EXIT_FAILURE);
            }
            else if(lib_delete_child == 0){
                backup_mode = 0;
                if(ada("library.zip") != 0){
                    pid_t del_childchild = fork();
                    if(del_childchild == 0){
                        char *argv[4] = {"remove", "-rf", "/home/purofuro/Fico/Soal_2/library", NULL};
                        execv("/bin/rm", argv);
                    }
                    wait(&status);
                    if(WIFEXITED(status)){
                        pid_t zip_child = fork();
                        if(zip_child == 0){
                            char *argv[5] = {"unzip", "/home/purofuro/Fico/Soal_2/library.zip", "-d", "/home/purofuro/Fico/Soal_2", NULL};
                            execv("/bin/unzip", argv);
                        }
                        else{
                            wait(&status);
                            if(WIFEXITED(status)){
                                rename_stuff();
                            }
                        }
                    }
                }
            }
        }
    }
}
```
Inti atau secara simpelnya, kode diatas melakukan pengecekan terlebih dahulu, apakah file/folder tersebut ada atau tidak, dan bila ada maka file tersebut akan didownload dan bila tidak ada maka akan dihapus.

Bagian awal dari `./management` saat di-run

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/management.png)

Proses "delete" dan "Rename"

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/management_proses.png)

Proses `./management` selesai

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/management_done.png)

**B)** Setelah ditelusuri, ternyata hanya 6 file teratas yang nama filenya tidak dienkripsi. Oleh karena itu, bantulah Paul untuk melakukan dekripsi terhadap nama file ke-7 hingga terakhir menggunakan algoritma ROT19.

Disini mungkin code terlihat sangat panjang tetapi yang dilakukan oleh code ini hanyalah pertama-tama adalah membuka directory dan mengecek apakah huruf pertama dari file merupakan angka atau bukan.

```c
int rename_stuff(){
    char *basePath = "/home/purofuro/Fico/Soal_2/library";
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (!dir)
        return 0;

    while ((dp = readdir(dir)) != NULL)
    {   
        
        if(default_mode == 1){
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){

                char to_change[1000] = "/home/purofuro/Fico/Soal_2/library/";
                char original_name[1000] = "/home/purofuro/Fico/Soal_2/library/";
                char text[100], ch;
                int key = 19;
                strcpy(text, dp->d_name);
                if(text[0] >= '0' && text[0] <= '9'){

                    if(strstr(dp->d_name, "d3Let3") != NULL){
                        strcat(to_change, dp->d_name);
                        remove(to_change);
                        timelog(text, "delete");

                    } else if(strstr(text, "r3N4mE") != NULL){
                        //rename the files
                        strcat(to_change, dp->d_name);

                        if(strstr(text, ".ts") != NULL){
                            rename(to_change, "/home/purofuro/Fico/Soal_2/library/helper.ts");
                            timelog("helper.ts", "rename");
                        }
                        else if(strstr(text, ".py") != NULL){
                            rename(to_change, "/home/purofuro/Fico/Soal_2/library/calculator.py");
                            timelog("calculator.py", "rename");
                        }
                        else if(strstr(text, ".go") != NULL){
                            rename(to_change, "/home/purofuro/Fico/Soal_2/library/server.go");
                            timelog("server.go", "rename");
                        }
                        else{
                            rename(to_change, "/home/purofuro/Fico/Soal_2/library/renamed.file");
                            timelog("renamed.file", "rename");
                        }
                    }
```
Apabila bukan merupakan angka, maka code akan menjalankan decrypt sekaligus menjalankan permintaan di bagian **C)** yang berbunyi.
Setelah dekripsi selesai, akan terlihat bahwa setiap file memuat salah satu dari kode berikut: r3N4mE, d3Let3, dan m0V3. Untuk setiap file dengan nama yang memuat kode d3Let3, hapus file tersebut. Sementara itu, untuk setiap file dengan nama yang memuat kode r3N4mE, lakukan hal berikut:
- Jika ekstensi file adalah “.ts”, rename filenya menjadi “helper.ts”
- Jika ekstensi file adalah “.py”, rename filenya menjadi “calculator.py”
- Jika ekstensi file adalah “.go”, rename filenya menjadi “server.go”
- Jika file tidak memuat salah satu dari ekstensi diatas, rename filenya menjadi “renamed.file”
```c
                } else{
                    for (int i = 0; text[i] != '\0'; ++i) {
                        ch = text[i];
                        // Check for valid characters.
                        if (isalnum(ch)) {
                            //Lowercase characters.
                            if (islower(ch)) {
                                ch = (ch - 'a' - key + 26) % 26 + 'a';
                            }
                            // Uppercase characters.
                            if (isupper(ch)) {
                                ch = (ch - 'A' - key + 26) % 26 + 'A';
                            }
                            // Numbers.
                        }
                        // Adding decoded character back.
                        text[i] = ch;
                    }
                    if(strstr(text, "d3Let3") != NULL){
                        strcat(to_change, dp->d_name);
                        remove(to_change);
                        timelog(dp->d_name, "delete");
                    } else if(strstr(text, "r3N4mE") != NULL){
                        //rename the files
                        strcat(to_change, dp->d_name);
                        if(strstr(text, ".ts") != NULL){
                            rename(to_change, "/home/purofuro/Fico/Soal_2/library/helper.ts");
                            timelog("helper.ts", "rename");
                        }
                        else if(strstr(text, ".py") != NULL){
                            rename(to_change, "/home/purofuro/Fico/Soal_2/library/calculator.py");
                            timelog("calculator.py", "rename");
                        }
                        else if(strstr(text, ".go") != NULL){
                            rename(to_change, "/home/purofuro/Fico/Soal_2/library/server.go");
                            timelog("server.go", "rename");
                        }
                        else{
                            rename(to_change, "/home/purofuro/Fico/Soal_2/library/renamed.file");
                            timelog("renamed.file", "rename");
                        }
                    } else if(strstr(text, "m0V3") != NULL){
                        strcat(original_name, dp->d_name);
                        strcat(to_change, text);
                        rename(original_name, to_change);
                        timelog(dp->d_name, "rename");
                    }
                }  
            }
        }
        sleep(1);
    }
    closedir(dir);
}
```

Hal ini dilakukan dengan menggunakan `strstr` untuk mengecek apakah string memiliki kata tersebut dan melakukan aksi sesuai dengan apa yang diinginkan dan algoritman untuk aksi sesuai dengan nama ini juga dilakukan kepada 6 file pertama tadi.

**D)** Atasan Paul juga meminta agar program ini dapat membackup dan merestore file. Oleh karena itu, bantulah Paul untuk membuat program ini menjadi 3 mode, yaitu:
- default: program berjalan seperti biasa untuk me-rename dan menghapus file. Mode ini dieksekusi ketika program dijalankan tanpa argumen tambahan, yaitu dengan command ./management saja
- backup: program memindahkan file dengan kode m0V3 ke sebuah folder bernama “backup”
- restore: program mengembalikan file dengan kode m0V3 ke folder sebelum file tersebut dipindahkan
`Contoh penggunaan: ./management -m backup`


Hal ini dilakukan dengan membuat `int main` memiliki opsi untuk diberikan argumen

```c
int main(int argc, char *argv[]){

    signal(SIGRTMIN,signal_mode);
    signal(SIGUSR1, signal_mode);
    signal(SIGUSR2, signal_mode);
    signal(SIGTERM, signal_mode);
    pid_t pid, sid;        // Variabel untuk menyimpan PID

    pid = fork();     // Menyimpan PID dari Child Process

    /* Keluar saat fork gagal
    * (nilai variabel pid < 0) */
    if (pid < 0) {
    exit(EXIT_FAILURE);
    }

    /* Keluar saat fork berhasil
    * (nilai variabel pid adalah PID dari child process) */
    if (pid > 0) {
    exit(EXIT_SUCCESS);
    }

    umask(0);

    sid = setsid();
    if (sid < 0) {
    exit(EXIT_FAILURE);
    }

    if ((chdir("/")) < 0) {
    exit(EXIT_FAILURE);
    }
    printf("The PID %d\n", getpid());

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    while(1){
        //check
        if(argc == 1){
            if(default_mode == 1) default_act();
        }
        if(argc >= 2){
            if(strcmp(argv[1], "-m") == 0){
                if(backup_mode == 0){
                    if(strcmp(argv[2], "backup") == 0){
                        backup_act();
                    }
                }
```
Dimana jika `./management -m backup` diketik, maka fungsi `backup_act` akan dipanggil yang dimana isi dari fungsi tersebut adalah.
```c
int backup_act(){
    int status;
    char *basePath = "/home/purofuro/Fico/Soal_2/library";
    struct dirent *dp;
    DIR *dir = opendir(basePath);
    if (!dir) return 0;
    while ((dp = readdir(dir)) != NULL){
        
        char filename[1000] = "/home/purofuro/Fico/Soal_2/library/";
        if(backup_mode == 1){
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
            
                if(strstr(dp->d_name, "m0V3") != NULL){
                    strcat(filename, dp->d_name);
                    pid_t multi_child = fork();
                    if(multi_child == 0){
                        char *argv[4] = {"backing up", filename, "/home/purofuro/Fico/Soal_2/library/backup", NULL}; 
                        execv("/bin/mv", argv);
                    }
                }
                timelog(dp->d_name, "backup");
                
            }
            sleep(1);
        }
    }
    closedir(dir);
}
```

Jika kita lihat, konsep penggunaannya sama dengan `default_act` hanya saja, untuk memindahkannya menggunakan `execv` yang memuat aksi `mv` kedalam folder backup yang sudah dibuat disaat melakukan `default_act` tadi.


```c
                else if(strcmp(argv[2], "restore") == 0){
                    if(restore_mode){
                        restore_act();
                    }
                }
            }
        }
        sleep(1000);
    }
    return 0;
}
```

Apabila yang diketik adalah `./management -m restore` maka fungsi `restore_act` akan dipanggil dan fungsinya sendiri berisi hal berikut.

```c
int restore_act(){

    int status;
    char *basePath = "/home/purofuro/Fico/Soal_2/library/backup";
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (!dir) return 0;

    while ((dp = readdir(dir)) != NULL){

        
        char filename[1000] = "/home/purofuro/Fico/Soal_2/library/backup/";
        if(restore_mode == 1){
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
                
                if(strstr(dp->d_name, "m0V3") != NULL){
                    strcat(filename, dp->d_name);
                    pid_t multi_child = fork();
                    if(multi_child == 0){
                        char *argv[4] = {"backing up", filename, "/home/purofuro/Fico/Soal_2/library", NULL}; 
                        execv("/bin/mv", argv);
                    }
                }
                timelog(dp->d_name, "restore");

            }  
            sleep(1);
        }
    }
    closedir(dir);
}
```

Isinya sendiri sama seperti yang ada di `backup` hanya saja ia meletakkan file dari folder *backup* ke folder awal yaitu *library*.

**E)** Terkadang, Paul perlu mengganti mode dari program ini tanpa menghentikannya terlebih dahulu. Oleh karena itu, bantulan Paul untuk mengintegrasikan kemampuan untuk mengganti mode ini dengan mengirim sinyal ke daemon, dengan ketentuan:
- SIGRTMIN untuk mode default
- SIGUSR1 untuk mode backup
- SIGUSR2 untuk mode restore
`Contoh penggunaan: kill -SIGUSR2 <pid_program>`

Untuk soal ini, telah kita lihat bahwa tadi di `int main` terdapat beberapa kode yang memanggil fungsi `signal_mode`, dimana fungsi itu sendiri berisi dengan

```c
void signal_mode(int signal){
    if(signal == SIGRTMIN){
        write(STDOUT_FILENO, "Default chosen", 13);
        default_mode = 1;
        restore_mode = 0;
        backup_mode = 0;

    } else if(signal == SIGUSR1){
        write(STDOUT_FILENO, "Backup chosen", 13);
        backup_mode = 1;
        default_mode = 0;
        restore_mode = 0;
        backup_act();


    } else if(signal == SIGUSR2){
        write(STDOUT_FILENO, "Restore chosen", 13);
        restore_mode = 1;
        backup_mode = 0;
        default_mode = 0;
        restore_act();


    } else if(signal == SIGTERM){
        exit(EXIT_SUCCESS);
    }
}
```
Disini terlihat bahwa terdapat semacam switch yang bila signal tersebut dipanggil maka ia akan menyalakan switchnya sendiri dan mematikan switch dari variabel yang lain. Setelah itu akan dipanggil fungsi yang sesuai dengan signal mana yang di-kill. Untuk variabel switchnya sendiri ada dibawah kode-kode `#include` yang memuat isi.
```c
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
int default_mode = 1;
int backup_mode = 1;
int restore_mode = 1; 
int exiting = 0; 
```

Menjalankan `./management -m backup` via `kill -USR1 PID`

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/kill_usr1.png)

Menjalankan `./management -m restore` via `kill -USR2 PID`

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/restore.png)

**F)** Program yang telah dibuat ini tidak mungkin akan dijalankan secara terus-menerus karena akan membebani sistem. Maka dari itu, bantulah Paul untuk membuat program ini dapat dimatikan dengan aman dan efisien.

```c
    } else if(signal == SIGTERM){
        exit(EXIT_SUCCESS);
    }
}
```

Sebelum dan setelah mematikan daemon menggunakan via `kill -TERM PID`

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/management_show.png)

![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/management_gone.png)

Untuk bagian ini, terlihat bahwa di `signal_mode` terdapat SIGTERM yang bila dipanggil maka akan mengeluarkan daemon secara aman dengan `exit(EXIT_SUCCESS)`

**G)** Terakhir, program ini harus berjalan setiap detik dan mampu mencatat setiap peristiwa yang terjadi ke dalam file .log yang bernama “history.log” dengan ketentuan:
- Format: [nama_user][HH:MM:SS] - <nama_file> - <action>
- nama_user adalah username yang melakukan action terhadap file
- Format action untuk setiap kode:
    - kode r3N4mE: Successfully renamed.
    - kode d3Let3: Successfully deleted.
    - mode backup: Successfully moved to backup.
    - mode restore: Successfully restored from backup.
Contoh pesan log:
- `[paul][00:00:00] - r3N4mE.ts - Successfully renamed.`
- `[paul][00:00:00] - m0V3.xk1 - Successfully restored from backup.`

Apabila kita melihat ke semua fungsi kita sebelumnya contohnya `default_act`, maka akan terlihat bahwa di akhir fungsi terdapat pemanggilan fungsi yaang bernama `timeog`, dimana `timelog` itu tersebut berisi dengan.

```c
void timelog(char *name, char *action){

    char result[1000];
    time_t my_time;
    struct tm * timeinfo; 
    time (&my_time);
    timeinfo = localtime (&my_time);

    if(strstr(action, "restore") != NULL ){
        sprintf(result, "[PuroFuro][%d:%d:%d] - %s - Succesfully restored from backup.", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, name);

    } else if(strstr(action, "backup") != NULL ){
        sprintf(result, "[PuroFuro][%d:%d:%d] - %s - Succesfully moved to backup.", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, name);


    } else if(strstr(action, "delete") != NULL ){
        sprintf(result, "[PuroFuro][%d:%d:%d] - %s - Succesfully Deleted.", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, name);


    } else if(strstr(action, "rename") != NULL ){
        sprintf(result, "[PuroFuro][%d:%d:%d] - %s - Succesfully Renamed.", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, name);

    }
    FILE *file = fopen("/home/purofuro/Fico/Soal_2/history.log", "a"); 
    if (file != NULL) {  
        fprintf(file, "%s\n", result); 
        fclose(file); 
    } else { 
        printf("Error opening the file\n"); 
    } 
}
```
Menunjukan isi dari history.log
![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/management_history.png)
Disini terdapat if else yang akan mengecek sesuai dari argumen yang diberikan saat fungsi ini berjalan dan akan melakukan aksi sesuai dari if else mana yang bekerja. Kode ini menggunakan `sprintf` itu menggabungkan string-string yang ada seperti contohnya yang berasal dari `timeinfo`.
**H)** Berikut adalah struktur folder untuk pengerjaan nomor 2:
```   
    soal_2/
    ├── history.log
    ├── management.c
    └── library/
        └── backup/
```
## Hasil akhir repository
![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/repo1.png)
![github-small](https://github.com/PuroFuro/image_for_sisop/blob/main/repo2.png)


# Soal 2

Pak Heze adalah seorang admin yang baik. Beliau ingin membuat sebuah program admin yang dapat memantau para pengguna sistemnya. Bantulah Pak Heze untuk membuat program  tersebut!

**A. Nama program tersebut dengan nama admin.c**

**B. Program tersebut memiliki fitur menampilkan seluruh proses yang dilakukan oleh seorang user dengan menggunakan command:
./admin <user>**

![github-small](https://github.com/bielnzar/sisop/blob/main/Modul2/soal3/Screenshot_1.png)

**c. Program dapat memantau proses apa saja yang dilakukan oleh user. Fitur ini membuat program berjalan secara daemon dan berjalan terus menerus. Untuk menjalankan fitur ini menggunakan command:**

**./admin -m <user>**

![github-small](https://github.com/bielnzar/sisop/blob/main/Modul2/soal3/Screenshot_2.png)

**Dan untuk mematikan fitur tersebut menggunakan:** 

**./admin -s <user>**

![github-small](https://github.com/bielnzar/sisop/blob/main/Modul2/soal3/Screenshot_3.png)

**Program akan mencatat seluruh proses yang dijalankan oleh user di file <user>.log dengan format:
[dd:mm:yyyy]-[hh:mm:ss]_pid-process_nama-process_GAGAL/JALAN**

**d. Program dapat menggagalkan proses yang dijalankan user setiap detik secara terus menerus dengan menjalankan:**

**./admin -c user**

**sehingga user tidak bisa menjalankan proses yang dia inginkan dengan baik. Fitur ini dapat dimatikan dengan command:**

**./admin -a user**

**e. Ketika proses yang dijalankan user digagalkan, program juga akan melog dan menset log tersebut sebagai GAGAL. Dan jika di log menggunakan fitur poin c, log akan ditulis dengan JALAN**

Changelog : untuk point c bagian logging, point d dan e masih belum selesai


## [SOAL 4](https://docs.google.com/document/d/1tJdfzPwhWOJTU_xgI9ATl88raufFcRKuo6noh91jvN4/edit)

#### a. Salomo memiliki passion yang sangat dalam di bidang sistem operasi. Saat ini, dia ingin mengotomasi kegiatan-kegiatan yang ia lakukan agar dapat bekerja secara efisien. Bantulah Salomo untuk membuat program yang dapat mengotomasi kegiatan dia!

Buat file bernama setup.cpp sesuai kriteria soal nomor 4

Definisikan library, variabel, dan buat struct untuk menyimpan nilai" yang akan dibutuhkan kedepannya

```
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_APPS 100 // 

typedef struct {
    char nama[100];
    int numWindows; 
    pid_t *pids;   
} Aplikasi;

Aplikasi aplikasi_dibuka[MAX_APPS]; // Array untuk menyimpan informasi tentang aplikasi yang dibuka
int num_app_dibuka = 0;   
```
Disini juga dibuat array untuk menyimpan informasi tentang aplikasi yang dibuka oleh program nantinya.


#### b. Program dapat membuka berbagai macam aplikasi dan banyak jendela aplikasi sekaligus (bisa membuka 1 atau lebih aplikasi dengan 1 atau lebih window (kecuali aplikasi yang tidak bisa dibuka dengan banyak window seperti discord) dengan menjalankan: 
`./setup -o <app1> <num1> <app2> <num2>.....<appN> <numN>`
Contoh penggunaannya adalah sebagai berikut: 
`./setup -o firefox 2 wireshark 2`
Program akan membuka 2 jendela aplikasi firefox dan 2 jendela aplikasi wireshark.

#### c. Program juga dapat membuka aplikasi dengan menggunakan file konfigurasi dengan menggunakan 
`./setup -f file.conf`

Format file konfigurasi dibebaskan, namun pastikan dapat menjalankan fitur dari poin 2.
Contoh isi file.conf:
`Firefox 2
Wireshark 3`
Ketika menjalankan command contoh, program akan membuka 2 jendela aplikasi firefox dan 3 jendela aplikasi wireshark.

Fungsi pertama digunakan untuk membuka aplikasi. Tentunya menggunakan fork() dan execlp()

Pada fungsi ini juga mengecek feedback dari fork, apabila feedback yang dikirim berupa nilai (-1), maka akan memberitahukan error, begitu juga jika (0) karena tidak memuat ppid, tetapi jika yg diinput itu bilangan positif. maka input dinyatakan benar. dan proses dilanjutkan.

Di fungsi ini juga ID Proses atau PID dari aplikasi yang dibuka oleh program akan disimpan, agar kedepannya apabila ingin menutup aplikasi, bisa kill pid yang telah disimpan di variabel tertentu.

Namun core dari fitur buka aplikasi juga terletak di fungsi main(). yang akan mengatur alur dari proses membuka aplikasi yang diinginkan.

berikut code dari fungsi buka_aplikasi() :

```

void buka_aplikasi(const char *app, int numWindows) {
    // Fork dan jalankan aplikasi
    for (int i = 0; i < numWindows; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("error fork");
            exit(EXIT_FAILURE);
        } 
        else if (pid == 0) {
            execlp(app, app, NULL);
            perror("gagal execlp");
            exit(EXIT_FAILURE);
        } 
        else {
            // Simpan ID proses di array aplikasi_dibuka
            aplikasi_dibuka[num_app_dibuka].pids = (pid_t *)realloc(aplikasi_dibuka[num_app_dibuka].pids, (i + 1) * sizeof(pid_t));
            aplikasi_dibuka[num_app_dibuka].pids[i] = pid;
        }
    }
    // Simpan informasi tentang aplikasi yang dibuka
    strncpy(aplikasi_dibuka[num_app_dibuka].nama, app, sizeof(aplikasi_dibuka[num_app_dibuka].nama));
    aplikasi_dibuka[num_app_dibuka].numWindows = numWindows;
    num_app_dibuka++;
}
```
![github-small](https://github.com/bielnzar/sisop/blob/main/Modul2/ab.png)

Bisa dilihat pada gambar diatas bahwasannya aplikasi yang diinginkan sudah bisa dibuka dengan sempurna melalui command `./setup -o code 2 wireshark 2`. yang sudah berjalan membuka 2 vscode dan 2 wireshark secara langsung. 

![github-small](https://github.com/bielnzar/sisop/blob/main/Modul2/c.png)

![github-small](https://github.com/bielnzar/sisop/blob/main/Modul2/c2.png)

Bisa dilihat pada gambar diatas bahwasannya aplikasi yang diinginkan sudah bisa dibuka dengan sempurna melalui command `./setup -f file.conf` 


#### d. Program dapat mematikan semua aplikasi yg dijalankan oleh program tersebut dengan: 
`./setup -k`
#### e. Program juga dapat mematikan aplikasi yang dijalankan sesuai dengan file konfigurasi. 
Contohnya: 
`./setup -k file.conf` 
Command ini hanya mematikan aplikasi yang dijalankan dengan 
`./setup -f file.conf`

Disini ada fungsi tutup_aplikasi(); untuk menutup semua aplikasi yang telah dibuka oleh program sebelumnya. Dalam fungsi ini di lakukan interpolasi untuk kill program aplikasi yang telah dibuka. kill mengakhiri proses pid yang sebelumnya sudah di inputkan dan disimpan oleh fungsi buka_aplikasi. disini kami memakai SIGKILL untuk kill program. Namun ada k endala, yaitu aplikasi belum juga tertutup ketika kami menjalankan fungsi tutup_aplikasi. Padahal proses sudah berjalan, tidak ada yang error, yang ditandakan oleh terminal yang tidak menampilkan error comment. Banyak yang harus kami teliti lagi, agar fungsi untuk menutup aplikasi ini bisa berjalan dengan sempurna.

```
void tutup_aplikasi() {
    for (int i = 0; i < num_app_dibuka; i++) {
        for (int j = 0; j < aplikasi_dibuka[i].numWindows; j++) {
            kill(aplikasi_dibuka[i].pids[j], SIGKILL); // Kirim sinyal SIGKILL untuk mengakhiri proses
        }
        for (int j = 0; j < aplikasi_dibuka[i].numWindows; j++) {
            waitpid(aplikasi_dibuka[i].pids[j], NULL, 0); // Tunggu proses untuk keluar
        }
        free(aplikasi_dibuka[i].pids);
    }
    num_app_dibuka = 0;
}
```
![github-small](https://github.com/bielnzar/sisop/blob/main/Modul2/de.png)

seperti yang sudah dijelaskan sebelunya. Program belum bisa menutup aplikasi dengan sempurna. padahal command sudah berjalan dan tidak ditemukan error.

Core dari semua fungsi diatas ialah int main(); 

bermula dari parameter `argc` dan `argv` yaitu argument count dan argument vector untuk menangkap dan memproses argumen baris perintah yang diberikan kepada program. 
agar program bisa berjalan sesuai keinginan, maka argc dan argv akan memeriksa apakah input pengguna sudah benar atau belum, juga berfungsi untuk membedakan input pengguna sesuai fungsi yang diharapkan.

```
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Cara penggunaan: %s -f <file.conf> atau %s -o <app1> <num1> ...\n", argv[0], argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-f") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Cara penggunaan: %s -f <file.conf>\n", argv[0]);
            return EXIT_FAILURE;
        }
        char app[100];
        int numWindows;
        
        FILE *file = fopen(argv[2], "r");
        if (!file) {
            perror("Gagal membuka file");
            return EXIT_FAILURE;
        }
        while (fscanf(file, "%99s %d", app, &numWindows) == 2) {
            buka_aplikasi(app, numWindows);
        }
        fclose(file);
    } 

    else if (strcmp(argv[1], "-o") == 0) {
        if ((argc - 2) % 2 != 0 || argc < 4) {
            fprintf(stderr, "Cara penggunaan: %s -o <app1> <num1> <app2> <num2> ... <appN> <numN>\n", argv[0]);
            return EXIT_FAILURE;
        }
        for (int i = 2; i < argc; i += 2) {
            buka_aplikasi(argv[i], atoi(argv[i + 1]));
        }
    }
     
    else if (strcmp(argv[1], "-k") == 0) {
        if (argc != 2) {
            fprintf(stderr, "Cara penggunaan: %s -k\n", argv[0]);
            return EXIT_FAILURE;
        }
        tutup_aplikasi();
    } 
    else {
        fprintf(stderr, "Opsi tidak valid '%s'\n", argv[1]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

```

![github-small](https://github.com/bielnzar/sisop/blob/main/Modul2/main.png)

int main(), pusat dari program ini. yang dimana akan mengatur dan mengolah input pengguna. bisa dilihat di gambar, jika input yang dimasukkan itu salah, ngawur, ataupun asal"an. Maka terminal akan menampilkan error message berupa apa yang sebenarnya harus diinputkan oleh pengguna
