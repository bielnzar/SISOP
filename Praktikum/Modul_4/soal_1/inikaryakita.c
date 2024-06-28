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
#include <stdlib.h>

static const char *dp = "/home/bosmuda/Kuliah/Semester_2/SISOP/Praktikum/Modul_4/soal_1";

void this_reverse_content(char *content, size_t length){
    for (size_t i = 0; i < length / 2; ++i) {
        char temp = content[i];
        content[i] = content[length - i - 1];
        content[length - i - 1] = temp;
    }
}

int this_test_prefix(const char *path){
    const char *filename = strrchr(path, '/');
    if (filename) {
        filename++;
    } else {
        filename = path;
    }
    return strncmp(filename, "test", 4) == 0;
}

static int this_getattr(const char *path, struct stat *stbuf){
    int res;
    char fp[1024];

    sprintf(fp, "%s%s", dp, path);
    res = lstat(fp, stbuf);

    if (res == -1)
        return -errno;

    return 0;
}

static int this_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    char fp[1024];

    if (strcmp(path, "/") == 0) {
        path = dp;
        sprintf(fp, "%s", path);
    } else {
        sprintf(fp, "%s%s", dp, path);
    }

    int res = 0;
    DIR *dp;
    struct dirent *de;

    (void)offset;
    (void)fi;

    dp = opendir(fp);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        res = (filler(buf, de->d_name, &st, 0));
        if (res != 0)
            break;
    }

    closedir(dp);
    return 0;
}

static int this_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    char fp[1024];
    sprintf(fp, "%s%s", dp, path);

    int fd = open(fp, O_RDONLY);
    if (fd == -1)
        return -errno;

    int res = pread(fd, buf, size, offset);
    if (res == -1) {
        close(fd);
        return -errno;
    }

    if (this_test_prefix(path)) {
        this_reverse_content(buf, res);
    }

    close(fd);
    return res;
}

static int this_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
    char fp[1024];
    sprintf(fp, "%s%s", dp, path);

    int fd = open(fp, O_WRONLY);
    if (fd == -1)
        return -errno;

    char *mod_buf = malloc(size);
    if (mod_buf == NULL) {
        close(fd);
        return -ENOMEM;
    }
    memcpy(mod_buf, buf, size);

    if (this_test_prefix(path)) {
        this_reverse_content(mod_buf, size);
    }

    int res = pwrite(fd, mod_buf, size, offset);
    if (res == -1)
        res = -errno;

    free(mod_buf);
    close(fd);
    return res;
}

static int this_mkdir(const char *path, mode_t mode){
    char fp[1024];
    sprintf(fp, "%s%s", dp, path);

    int res = mkdir(fp, mode);
    if (res == -1)
        return -errno;

    return 0;
}

static int this_rmdir(const char *path){
    char fp[1024];
    sprintf(fp, "%s%s", dp, path);

    int res = rmdir(fp);
    if (res == -1)
        return -errno;

    return 0;
}

static int this_unlink(const char *path){
    char fp[1024];
    sprintf(fp, "%s%s", dp, path);

    int res = unlink(fp);
    if (res == -1)
        return -errno;

    return 0;
}

static int this_rename(const char *from, const char *to){
    char sp[1024], dest[1024];
    sprintf(sp, "%s%s", dp, from);
    sprintf(dest, "%s%s", dp, to);

    if (strstr(dest, "/wm") != NULL) {
        int src_fd = open(sp, O_RDONLY);
        if (src_fd == -1) {
            perror("Error: Gagal membuka berkas sumber untuk dibaca.");
            return -errno;
        }
        int dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (dest_fd == -1) {
            perror("Error: Gagal membuka berkas tujuan untuk ditulis.");
            close(src_fd);
            return -errno;
        }

        char watermark_text[] = "inikaryakita.id";
        char command[2048];
        sprintf(command, "convert -gravity south -geometry +0+20 /proc/%d/fd/%d -fill white -pointsize 36 -annotate +0+0 '%s' /proc/%d/fd/%d", getpid(), src_fd, watermark_text, getpid(), dest_fd);

        int res = system(command);
        if (res == -1) {
            perror("Error: Gagal menjalankan perintah ImageMagick.");
            close(src_fd);
            close(dest_fd);
            return -errno;
        }

        close(src_fd);
        close(dest_fd);

        if (unlink(sp) == -1) {
            perror("Error: Gagal menghapus berkas sumber.");
            return -errno;
        }
    } else {
        int res = rename(sp, dest);
        if (res == -1) {
            perror("Error: Gagal memindahkan berkas.");
            return -errno;
        }
    }
    return 0;
}

static int this_create(const char *path, mode_t mode, struct fuse_file_info *fi){
    char fp[1024];
    sprintf(fp, "%s%s", dp, path);

    int fd = open(fp, fi->flags, mode);
    if (fd == -1)
        return -errno;

    if (this_test_prefix(path)) {
        const char *initial_content = "";
        size_t len = strlen(initial_content);
        char *reversed_content = malloc(len);

        if (reversed_content == NULL) {
            close(fd);
            return -ENOMEM;
        }
        memcpy(reversed_content, initial_content, len);
        this_reverse_content(reversed_content, len);
        int res = write(fd, reversed_content, len);
        free(reversed_content);
        if (res == -1) {
            close(fd);
            return -errno;
        }
    }

    close(fd);
    return 0;
}

static int this_chmod(const char *path, mode_t mode){
    char fp[1024];
    sprintf(fp, "%s%s", dp, path);

    int res = chmod(fp, mode);
    if (res == -1)
        return -errno;

    return 0;
}

static int this_chown(const char *path, uid_t uid, gid_t gid) {
    char fp[1024];
    sprintf(fp, "%s%s", dp, path);

    int res = lchown(fp, uid, gid);
    if (res == -1)
        return -errno;

    return 0;
}

static struct fuse_operations this_oper = {
    .getattr = this_getattr,
    .readdir = this_readdir,
    .read = this_read,
    .write = this_write,
    .mkdir = this_mkdir,
    .rmdir = this_rmdir,
    .unlink = this_unlink,
    .rename = this_rename,
    .create = this_create,
    .chmod = this_chmod,
    .chown = this_chown,
};

int main(int argc, char *argv[]) 
{
    umask(0);

    int fuse_stat = fuse_main(argc, argv, &this_oper, NULL);

    return fuse_stat;
}
