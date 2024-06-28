#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/stat.h>

static const char *base_dir = "/home/purofuro/Fico/M4S3/relics";
#define MAX_BYTES 1024
#define MAX_SIZE 10240
#define MAX_SUFFIX_LEN 5  // Length for ".%03d" + null terminator
#ifndef S_IFREG
#define S_IFREG 0100000
#endif
#ifndef DT_REG
#define DT_REG 8
#endif
#ifndef S_IFDIR
#define S_IFDIR 0040000
#endif

// to make the full file path
static void construct_full_path(char *destination, const char *relative_path) {
    strcpy(destination, base_dir);
    strncat(destination, relative_path, MAX_BYTES - strlen(destination) - 1);
    printf("The string is %s\n", destination);
}

// to get the size of a file part
static size_t get_file_part_size(const char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if (!file) return 0;
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fclose(file);
    return size;
}

// to get the full list of file parts
static void get_all_file_parts(const char *base_path, char part_paths[][MAX_BYTES], int *part_count) {
    int index = 0;
    char part_index[5]; // for ".%03d"
    char part_path[MAX_BYTES];

    while (1) {
        sprintf(part_index, ".%03d", index);
        strcpy(part_path, base_path);
        strcat(part_path, part_index);
        FILE *file = fopen(part_path, "rb");
        if (!file) break;
        fclose(file);
        strcpy(part_paths[*part_count], part_path);
        (*part_count)++;
        index++;
    }
}

static int my_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    (void) fi;
    char full_path[MAX_BYTES];
    construct_full_path(full_path, path);

    char part_paths[100][MAX_BYTES];
    int part_count = 0;
    get_all_file_parts(full_path, part_paths, &part_count);
    printf("The full path is %s\n", full_path);

    size_t read_bytes = 0;

    for (int i = 0; i < part_count && size > 0; ++i) {
        FILE *file = fopen(part_paths[i], "rb");
        if (!file) break;

        fseek(file, 0L, SEEK_END);
        size_t part_size = ftell(file);
        fseek(file, 0L, SEEK_SET);

        if (offset >= part_size) {
            offset -= part_size;
            fclose(file);
            continue;
        }

        fseek(file, offset, SEEK_SET);
        size_t to_read = size;
        if (to_read > part_size - offset) {
            to_read = part_size - offset;
        }
        size_t temp = fread(buf, 1, to_read, file);
        fclose(file);

        buf += temp;
        size -= temp;
        read_bytes += temp;
        offset = 0;
    }
    return read_bytes;
}

static int my_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    (void) offset;
    (void) fi;

    DIR *dir = opendir(base_dir);
    if (!dir) return -errno;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            filler(buf, entry->d_name, NULL, 0);
        } else if (entry->d_type == DT_REG) {
            char *dot = strrchr(entry->d_name, '.');
            if (dot && strcmp(dot, ".000") == 0) {
                char name[500];
                size_t name_length = dot - entry->d_name;
                memcpy(name, entry->d_name, name_length);
                name[name_length] = '\0'; // Add null terminator manually
                printf("Filename is %s\n", entry->d_name);
                filler(buf, name, NULL, 0);
            }
        }
    }
    closedir(dir);
    return 0;
}

static int my_getattr(const char *path, struct stat *stbuf) {
    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    } else {
        char full_path[MAX_BYTES];
        construct_full_path(full_path, path);

        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = 0;

        char part_paths[100][MAX_BYTES];
        int part_count = 0;
        get_all_file_parts(full_path, part_paths, &part_count);

        for (int i = 0; i < part_count; ++i) {
            stbuf->st_size += get_file_part_size(part_paths[i]);
            printf("Iterations: %d\n", i);
        }

        if (part_count == 0) return -ENOENT;
    }
    return 0;
}
static int my_open(const char *path, struct fuse_file_info *fi) {
    return 0;
}

static int my_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    (void) fi;
    char full_path[MAX_BYTES - MAX_SUFFIX_LEN];
    int part_index = offset / MAX_SIZE;
    size_t part_offset = offset % MAX_SIZE;
    size_t written_bytes = 0;
    char part_path[MAX_BYTES];
    construct_full_path(full_path, path);
    printf("Full path is %s\n", full_path);

    while (size > 0) {
        strcpy(part_path, full_path);
        sprintf(part_path + strlen(full_path), ".%03d", part_index);
        printf("Path: %s\n", part_path);

        FILE *file = fopen(part_path, "r+b");
        if (!file) {
            file = fopen(part_path, "wb");
            if (!file) return -errno;
        }

        fseek(file, part_offset, SEEK_SET);
       size_t to_write;
        if (size > (MAX_SIZE - part_offset)) {
            to_write = MAX_SIZE - part_offset;
            size -= to_write;
        } else {
            to_write = size;
            size -= to_write;
        }
        printf("size to write %zu\n", to_write);
        part_offset = 0;
        fwrite(buf, 1, to_write, file);
        fclose(file);

        buf += to_write;
        part_index++;
        written_bytes += to_write;
    }
    return written_bytes;
}

static int my_truncate(const char *path, off_t size) {
    char full_path[MAX_BYTES - MAX_SUFFIX_LEN];
    construct_full_path(full_path, path);
    int part_index = 0;
    char part_path[MAX_BYTES];
    off_t remaining_size = size;

    while (remaining_size > 0) {
        strcpy(part_path, full_path);
        sprintf(part_path + strlen(full_path), ".%03d", part_index++);
        size_t part_size;
        if (remaining_size > MAX_SIZE) {
            part_size = MAX_SIZE;
        } else {
            part_size = remaining_size;
        }
        int res = truncate(part_path, part_size);
        if (res == -1) return -errno;
        remaining_size -= part_size;
    }

    while (1) {
        strcpy(part_path, full_path);
        sprintf(part_path + strlen(full_path), ".%03d", part_index);
        int res = unlink(part_path);
        if (res == -1 && errno == ENOENT) break;
        else if (res == -1) return -errno;
        part_index++;
    }

    return 0;
}

static int my_unlink(const char *path) {
    char full_path[MAX_BYTES - MAX_SUFFIX_LEN];
    construct_full_path(full_path, path);

    int part_index = 0;
    char part_path[MAX_BYTES];

    while (1) {
        strcpy(part_path, full_path);
        sprintf(part_path + strlen(full_path), ".%03d", part_index);
        int res = unlink(part_path);
        if (res == -1 && errno == ENOENT) break;
        else if (res == -1) return -errno;
        part_index++;
    }
    return 0;
}

static int my_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    (void) fi;
    char full_path[MAX_BYTES - MAX_SUFFIX_LEN];
    strcpy(full_path, base_dir);
    strcat(full_path, path);
    char suffix[10];
    sprintf(suffix, ".%03d", 0);  // to allocate the .000 or .xxx and so on
    strcat(full_path, suffix);
    FILE *file = fopen(full_path, "wb");
    if (!file) return -errno;
    fclose(file);
    return 0;
}

static struct fuse_operations operations = {
    .getattr = my_getattr,
    .readdir = my_readdir,
    .open = my_open,
    .read = my_read,
    .write = my_write,
    .truncate = my_truncate,
    .unlink = my_unlink,
    .create = my_create,
};

int main(int argc, char *argv[]) {
    umask(0);
    return fuse_main(argc, argv, &operations, NULL);
}
