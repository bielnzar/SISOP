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
int num_aplikasi_dibuka = 0;       

void buka_aplikasi(const char *app, int numWindows) {
    // Fork dan jalankan aplikasi
    for (int i = 0; i < numWindows; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("error fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            execlp(app, app, NULL);
            perror("gagal execlp");
            exit(EXIT_FAILURE);
        } else {
            // Simpan ID proses di array aplikasi_dibuka
            aplikasi_dibuka[num_aplikasi_dibuka].pids = (pid_t *)realloc(aplikasi_dibuka[num_aplikasi_dibuka].pids, (i + 1) * sizeof(pid_t));
            aplikasi_dibuka[num_aplikasi_dibuka].pids[i] = pid;
        }
    }
    // Simpan informasi tentang aplikasi yang dibuka
    strncpy(aplikasi_dibuka[num_aplikasi_dibuka].nama, app, sizeof(aplikasi_dibuka[num_aplikasi_dibuka].nama));
    aplikasi_dibuka[num_aplikasi_dibuka].numWindows = numWindows;
    num_aplikasi_dibuka++;
}

void tutup_semua_aplikasi() {
    for (int i = 0; i < num_aplikasi_dibuka; i++) {
        for (int j = 0; j < aplikasi_dibuka[i].numWindows; j++) {
            kill(aplikasi_dibuka[i].pids[j], SIGKILL); // Kirim sinyal SIGKILL untuk mengakhiri proses
        }
        for (int j = 0; j < aplikasi_dibuka[i].numWindows; j++) {
            waitpid(aplikasi_dibuka[i].pids[j], NULL, 0); // Tunggu proses untuk keluar
        }
        free(aplikasi_dibuka[i].pids);
    }
    num_aplikasi_dibuka = 0;
}

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
    } else if (strcmp(argv[1], "-o") == 0) {
        if ((argc - 2) % 2 != 0 || argc < 4) {
            fprintf(stderr, "Cara penggunaan: %s -o <app1> <num1> <app2> <num2> ... <appN> <numN>\n", argv[0]);
            return EXIT_FAILURE;
        }
        for (int i = 2; i < argc; i += 2) {
            buka_aplikasi(argv[i], atoi(argv[i + 1]));
        }
    } else if (strcmp(argv[1], "-k") == 0) {
        if (argc != 2) {
            fprintf(stderr, "Cara penggunaan: %s -k\n", argv[0]);
            return EXIT_FAILURE;
        }
        tutup_semua_aplikasi();
    } else {
        fprintf(stderr, "Opsi tidak valid '%s'\n", argv[1]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
