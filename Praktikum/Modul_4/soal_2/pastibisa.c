#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <libgen.h> // for dirname()
#include <openssl/bio.h>
#include <openssl/evp.h>

static const char *dirpath = "/home/hezekiah/sensitif";
int password_entered;

char program_name[1024]; // Store the program name for logging

// Function to create the log file if it doesn't exist
static void ensure_log_file() {
    char log_path[512];
    snprintf(log_path, sizeof(log_path), "%s/logs-fuse.log", program_name);
    FILE *log_file = fopen(log_path, "a");
    if (log_file == NULL) {
        // Log file doesn't exist, create it
        log_file = fopen(log_path, "w");
        if (log_file == NULL) {
            perror("Error creating log file");
            return;
        }
        // Close the newly created log file
        fclose(log_file);
    }
}

// Function to log activities to a log file in the program's directory
static void logging(const char *activity, const char *path, const char *result) {
    ensure_log_file(); // Ensure the log file exists
    char log_path[512];
    snprintf(log_path, sizeof(log_path), "%s/logs-fuse.log", program_name);
    FILE *log_file = fopen(log_path, "a");
    if (log_file == NULL) {
        perror("Error opening log file");
        return;
    }

    // Get the current time
    time_t now;
    time(&now);
    struct tm *local_time = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%d/%m/%Y-%H:%M:%S", local_time);

    // Write to the log file
    fprintf(log_file, "[%s]::%s::[%s]::[%s]\n", result, timestamp, activity, path);

    fclose(log_file);
}

char secret_password[100] = "bebas";

// Function to check and allow access to the "rahasia" folder
int passtest(const char *path) {
    printf("Entering passtest function\n"); // Debugging: Print when function is entered
    if (!password_entered) {
        printf("Prompting for password\n"); // Debugging: Print when prompting for password
        char input_password[100];
        printf("Masukkan kata sandi: ");
        scanf("%s", input_password);
        if (strcmp(input_password, secret_password) != 0) {
            printf("Kata sandi salah. Akses ditolak.\n");
            logging("DENIED ACCESS", path, "Akses folder rahasia ditolak");
            return 0;
        }
        password_entered = 1;
        logging("GRANTED ACCESS", path, "Akses folder rahasia diterima");
        printf("Access granted\n"); // Debugging: Print when access is granted
    }
    return 1;
}

// Implementation of fuse_getattr to get file/directory attributes
static int getattr_fuse(const char *path, struct stat *stbuf) {
    int res;
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    res = lstat(fpath, stbuf);
    if (res == -1)
        return -errno;
    return 0;
}

// Implementation of fuse_readdir to read directory contents
static int readdir_fuse(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    int res;
    char fpath[1000];
    if (strcmp(path, "/") == 0) {
        path = dirpath;
        sprintf(fpath, "%s", path);
    } else {
        sprintf(fpath, "%s%s", dirpath, path);
    }

    if(strstr(fpath, "rahasia") != NULL){
        if (!passtest(path)) {
            return -EACCES;
        }
    }

    DIR *dp = opendir(fpath);
    if (dp == NULL)
        return -errno;
    struct dirent *de;
    while ((de = readdir(dp)) != NULL){
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        res = (filler(buf, de->d_name, &st, 0));
        if (res != 0)
            break;
    }
    closedir(dp);
    password_entered = 0;
    return 0;
}

// Implementation of fuse_open to open files
static int open_fuse(const char *path, struct fuse_file_info *fi) {
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    if(strstr(fpath, "rahasia") != NULL){
        if (!passtest(path)) {
            return -EACCES;
        }
    }
    int res = open(fpath, fi->flags);
    if (res == -1) return -errno;
    close(res);
    return 0;
}

// Implementation of fuse_mkdir to create directories
static int mkdir_fuse(const char *path, mode_t mode) {
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    int res = mkdir(fpath, mode);
    if (res == -1) return -errno;
    logging("MKDIR", path, "Berhasil membuat direktori");
    return 0;
}

// Implementation of fuse_rmdir to remove directories
static int rmdir_fuse(const char *path) {
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    int res = rmdir(fpath);
    if (res == -1) return -errno;
    logging("RMDIR", path, "Berhasil menghapus direktori");
    return 0;
}

// Implementation of fuse_rename to rename files/directories
static int rename_fuse(const char *from, const char *to) {
    char fromPath[1000], toPath[1000];
    sprintf(fromPath, "%s%s", dirpath, from);
    sprintf(toPath, "%s%s", dirpath, to);
    int res = rename(fromPath, toPath);
    if (res == -1) return -errno;
    logging("RENAME/MOVE", from, "Berhasil mengganti nama/memindahkan file");
    return 0;
}

// Implementation of fuse_create to create files
static int create_fuse(const char *path, mode_t mode, struct fuse_file_info *fi) {
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    int res = creat(fpath, mode);
    if (res == -1) return -errno;
    close(res);
    logging("CREATE", path, "Berhasil membuat file");
    return 0;
}

// Implementation of fuse_unlink to remove files
static int rm_fuse(const char *path) {
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    int res = unlink(fpath);
    if (res == -1) return -errno;
    logging("REMOVE", path, "Berhasil menghapus file");
    return 0;
}

// Implementation of fuse_chmod to change file/directory permissions
static int chmod_fuse(const char *path, mode_t mode) {
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    int res = chmod(fpath, mode);
    if (res == -1) return -errno;
    logging("CHMOD", path, "Berhasil mengubah mode akses file/direktori");
    return 0;
}

// Decrypt file content
static void decrypt_file_content(const char *path, char *buf, size_t size) {
    char *filename = strrchr(path, '/');
    if (filename != NULL) {
        filename++;
        if (strstr(filename, "base64") != NULL) {
            BIO *bio, *b64;
            bio = BIO_new(BIO_s_mem());
            b64 = BIO_new(BIO_f_base64());
            BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
            BIO_push(b64, bio);

            // Write text to be decrypted to the BIO
            BIO_write(bio, buf, strlen(buf));

            // Create buffer for decrypted text
            char *decoded_text = (char *)malloc(strlen(buf));
            memset(decoded_text, 0, strlen(buf));

            // Perform decryption and store result in buffer
            BIO_read(b64, decoded_text, strlen(buf));

            // Clean up BIO and close
            BIO_free_all(b64);
            if (decoded_text != NULL) {
                strncpy(buf, decoded_text, size);
                free(decoded_text);
            }
        } else if (strstr(filename, "rot13") != NULL ) {
            // Decrypt ROT13
            for (int i = 0; i < size; i++) {
                if (isalpha(buf[i])) {
                    if (islower(buf[i])) {
                        buf[i] = 'a' + (buf[i] - 'a' + 13) % 26;
                    } else {
                        buf[i] = 'A' + (buf[i] - 'A' + 13) % 26;
                    }
                }
            }
        } else if (strstr(filename, "hex") != NULL) {
            // Decrypt Hexadecimal
            size_t decoded_size = size / 2;
            char *decoded_text = (char *)malloc(decoded_size);
            memset(decoded_text, 0, decoded_size);

            for (int i = 0, j = 0; i < size; i += 2, j++) {
                char hex[3] = {buf[i], buf[i + 1], '\0'};
                decoded_text[j] = strtol(hex, NULL, 16);
            }

            strncpy(buf, decoded_text, size);
            free(decoded_text);
        }
    }
}

// Read file content
static int read_fuse(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    char fpath[1000];
    if (strcmp(path, "/") == 0) {
        path = dirpath;
        sprintf(fpath, "%s", path);
    } else {
        sprintf(fpath, "%s%s", dirpath, path);
    }
    if(strstr(fpath, "rahasia") != NULL){
        if (!passtest(path)) {
            return -EACCES;
        }
    }

    int res = 0;
    int fd = open(fpath, O_RDONLY);
    if (fd == -1) return -errno;
    res = pread(fd, buf, size, offset);
    if (res == -1) res = -errno;
    close(fd);

    decrypt_file_content(path, buf, size);

    logging("READ", path, "Berhasil membaca file");
    return res;
}

// Write file content
static int write_fuse(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    char fpath[1000];
    if (strcmp(path, "/") == 0) {
        path = dirpath;
        sprintf(fpath, "%s", path);
    } else {
        sprintf(fpath, "%s%s", dirpath, path);
    }
    if(strstr(fpath, "rahasia") != NULL){
        if (!passtest(path)) {
            return -EACCES;
        }
    }

    int fd;
    int res;
    (void) fi;
    fd = open(fpath, O_WRONLY);
    if (fd == -1) return -errno;

    res = pwrite(fd, buf, size, offset);
    if (res == -1) res = -errno;

    close(fd);

    logging("WRITE", path, "Berhasil menulis file");
    return res;
}

// FUSE operations struct
static struct fuse_operations operations_fuse = {
    .getattr = getattr_fuse,
    .readdir = readdir_fuse,
    .open = open_fuse,
    .mkdir = mkdir_fuse,
    .rmdir = rmdir_fuse,
    .rename = rename_fuse,
    .create = create_fuse,
    .unlink = rm_fuse,
    .chmod = chmod_fuse,
    .read = read_fuse,
    .write = write_fuse,
};

int main(int argc, char *argv[]) {
    strncpy(program_name, argv[0], sizeof(program_name) - 1);
    return fuse_main(argc, argv, &operations_fuse, NULL);
}
