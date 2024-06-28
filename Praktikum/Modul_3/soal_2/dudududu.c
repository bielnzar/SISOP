imnotHZ, [5/11/2024 8:04 PM]
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <math.h>

int stringToNumber(char *word) {
    // Dictionary for converting Indonesian numbers to integers
    char *number_words[] = {"nol", "satu", "dua", "tiga", "empat", "lima", "enam", "tujuh", "delapan", "sembilan"};

    for (int i = 0; i < 10; i++) {
        if (strcmp(word, number_words[i]) == 0) {
            return i;
        }
    }
    return -1; // If word is not a valid number, return -1
}

void angkaKeKata(int num, char *result) {
    char *number_words[] = {"", "satu", "dua", "tiga", "empat", "lima", "enam", "tujuh", "delapan", "sembilan"};
    
    if (num >= 1 && num <= 9) {
        strcpy(result, number_words[num]);
    } else if (num >= 10 && num <= 19) {
        switch(num) {
            case 10: strcpy(result, "sepuluh"); break;
            case 11: strcpy(result, "sebelas"); break;
            default: {
                strcpy(result, number_words[num % 10]);
                strcat(result, " belas");
                break;
            }
        }
    } else if (num >= 20 && num <= 99) {
        strcpy(result, number_words[num / 10]);
        strcat(result, " puluh");
        if (num % 10 != 0) {
            strcat(result, " ");
            strcat(result, number_words[num % 10]);
        }
    }
}

void outputLog(int logNumber, char *operation, char *expression, char *timestamp, char *result) {
    FILE *logFile;
    logFile = fopen("histori.log", "a");
    if (logFile == NULL) {
        printf("Error: Cannot open log file.\n");
        exit(EXIT_FAILURE);
    }
    if (strcmp(result, "ERROR") == 0) {
        fprintf(logFile, "[%s] [%s] ERROR pada %s\n", timestamp, operation, expression);
    } else {
        fprintf(logFile, "[%s] [%s] %s %s\n", timestamp, operation, expression, result);
    }
    fclose(logFile);
}

void format_time(char *buffer) {
    time_t now;
    struct tm *info;
    time(&now);
    info = localtime(&now);
    strftime(buffer, 20, "%d/%m/%Y %H:%M:%S", info);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <operation> <operand1> <operand2>\n", argv[0]);
        return 1;
    }

    char *opsi = argv[1];
    char *input1 = argv[2];
    char *input2 = argv[3];
    int pipefd[2];
    int logNumber = 1;
    int num1, num2;
    int hasil;
    char words[100];

    if (pipe(pipefd) == -1) {
        perror("Pipe failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipefd[0]);

        num1 = stringToNumber(input1);
        num2 = stringToNumber(input2);

        if (num1 == -1 || num2 == -1) {
            printf("Invalid input\n");
            close(pipefd[1]);
            exit(EXIT_FAILURE);
        }

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

            outputLog(logNumber, "KALI", strcat(strcat(strcat(input1, " kali "), input2), " sama dengan "), timestamp, words);
        } else if (strcmp(opsi, "-tambah") == 0) {
            hasil = num1 + num2;
            if (hasil < 0) {
                strcpy(words, "ERROR");
            } else {
                hasil = floor(hasil);
                angkaKeKata(hasil, words);
            }
            char timestamp[20];
            format_time(timestamp);

            outputLog(logNumber, "TAMBAH", strcat(strcat(strcat(input1, " tambah "), input2), " sama dengan "), timestamp, words);
        } else if (strcmp(opsi, "-kurang") == 0) {
            hasil = num1 - num2;
            if (hasil < 0) {
                strcpy(words, "ERROR");
            } else {
                hasil = floor(hasil);
                angkaKeKata(hasil, words);
            }

imnotHZ, [5/11/2024 8:04 PM]
char timestamp[20];
            format_time(timestamp);

            outputLog(logNumber, "KURANG", strcat(strcat(strcat(input1, " kurang "), input2), " sama dengan "), timestamp, words);

        } else if (strcmp(opsi, "-bagi") == 0) {
            if (num2 == 0) {
                printf("Division by zero is not allowed.\n");
                close(pipefd[1]);
                exit(EXIT_FAILURE);
            }
            hasil = num1 / num2;
            if (hasil < 0) {
                strcpy(words, "ERROR");
            } else {
                hasil = floor(hasil);
                angkaKeKata(hasil, words);
            }
            char timestamp[20];
            format_time(timestamp);

            outputLog(logNumber, "BAGI", strcat(strcat(strcat(input1, " bagi "), input2), " sama dengan "), timestamp, words);
        } else {
            printf("Invalid operation\n");
            close(pipefd[1]);
            exit(EXIT_FAILURE);
        }

        write(pipefd[1], words, strlen(words) + 1);
        close(pipefd[1]);
        exit(EXIT_SUCCESS);
    } else {
        close(pipefd[1]);

        char hasilKalimat[100];
        read(pipefd[0], hasilKalimat, sizeof(hasilKalimat));

        if (strcmp(opsi, "-kali") == 0) {
            printf("Hasil perkalian %s dan %s adalah %s.\n", input1, input2, hasilKalimat);
        } else if (strcmp(opsi, "-tambah") == 0) {
            printf("Hasil penjumlahan %s dan %s adalah %s.\n", input1, input2, hasilKalimat);
        } else if (strcmp(opsi, "-kurang") == 0) {
            printf("Hasil pengurangan %s dan %s adalah %s.\n", input1, input2, hasilKalimat);
        } else if (strcmp(opsi, "-bagi") == 0) {
            printf("Hasil pembagian %s dan %s adalah %s.\n", input1, input2, hasilKalimat);
        } else {
            printf("Invalid operation\n");
            exit(EXIT_FAILURE);
        }

        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    }

    return 0;
}