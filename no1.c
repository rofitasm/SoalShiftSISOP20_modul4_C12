
#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <libgen.h>



static  const  char *dirpath = "/home/rofita/Documents";

int flag,flag1;
char path_copy[1000];
int key = 10;



void logEnc(const char arg1[], const char arg2[], int num_encv) {
    time_t current_time;
    time(&current_time);
    struct curr_time = localtime(&current_time);
    char str[1000];

    if(!strlen(arg2))
        sprintf(str,"%02d%02d%02d-%02d:%02d:%02d::%s",
            curr_time->tm_year % 100,
            curr_time->tm_mon + 1,
            curr_time->tm_mday,
            curr_time->tm_hour,
            curr_time->tm_min,
            curr_time->tm_sec,
            arg1
        );
    else
        sprintf(str,"%02d%02d%02d-%02d:%02d:%02d::%s::%s",
            curr_time->tm_year % 100,
            curr_time->tm_mon,
            curr_time->tm_mday,
            curr_time->tm_hour,
            curr_time->tm_min,
            curr_time->tm_sec,
            arg1,
            arg2
        );
    FILE *log;
    if(num_encv == 1)
        log = fopen("/home/rofita/encv1.log","a");
    else 
        log = fopen("/home/rofita/encv2.log","a");
    if(log == NULL) printf("Error");
    fprintf(log, "%s\n", str);
    fclose(log);
}

void cut_ext(char *fname)
{
    char *end = fname + strlen(fname);

    while (end > fname && *end != '.') {
        --end;
    }

    if (end > fname) {
        *end = '\0';
    }
}

const char *get_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";

    return dot;
}

int isDirectory(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}

void enc1(const char* str, char *res) {
    char *cis = "9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";
    int lenCis = strlen(cis);
    int length_str = strlen(str);
    int i,j;
    int key2 = key % lenCis;

    for (i = 0; i < length_str; i++)
    {
        for ( j = 0; j < lenCis; j++)
        {
            if(str[i] == cis[j]) {
                res[i] = cis[(j+key2) % lenCis];
                break;
            }   
        }
    }
}

void dec1(const char* str, char *res) {
    char *cis = "9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";
    int lenCis = strlen(cis);
    int length_str = strlen(str);
    int i,j;
    int key2 = key % lenCis;
    
    char temp[1000];
    strcpy(temp,str);

    for (i = 0; i < length_str; i++)
    {
        for ( j = 0; j < lenCis; j++)
        {
            if(temp[i] == cis[j]) {
                res[i] = cis[(j >= key2)? j-key2:lenCis-(key2-j)];
                break;
            }   
        }
    }
}

void loopDir1(const char *name, int encrypt)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;


    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            loopDir1(path, encrypt);
            
            char res[1000],to[1000];

            memset(res, 0, sizeof(res));
            memset(to, 0, sizeof(res));

            if(encrypt) {
                enc1(entry->d_name, res);
            }else{
                dec1(entry->d_name, res);
            }
            snprintf(to, sizeof(to),"%s/%s", name, res);
            rename(path, to);
        } else {
            char res[1000], to[2000], path[1024],filename[100],ext[100];
            
            memset(res, 0, sizeof(res));
            memset(to, 0, sizeof(res));

            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);

            strcpy(filename, entry->d_name);
            strcpy(ext, get_ext(filename));
            cut_ext(filename);

            if(encrypt) {
                enc1(filename, res);
            }else{
                dec1(filename, res);
            }
            snprintf(to, sizeof(to),"%s/%s", name, res);

            if(strlen(ext) > 0)
                strcat(to, ext);
            
            rename(path, to);
        }
    }
    closedir(dir);
}


void loopDir2(const char *name, int encrypt)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;


    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            loopDir2(path, encrypt);

        } else {
            char res[1000], to[2000], path[1024],filename[100],ext[100];
            
            memset(res, 0, sizeof(res));
            memset(to, 0, sizeof(res));

            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
          
            if(encrypt) {
                splitfile(path);
            }else{
                unsplitfile(path);
            }

        }
    }
    closedir(dir);
}



void substring(char s[], char sub[], int p, int l) {
   int c = 0;
   
   while (c < l) {
      sub[c] = s[p+c-1];
      c++;
   }
   sub[c] = '\0';
}


//no2

void splitfile(char* path) {
    char prefix[strlen(path) + 5];
    sprintf(prefix, "%s.", path);
    pid_t child = fork();
    if (child == 0) execl("/usr/bin/split", "split", "-b", "1024", "-d", "-a", "3", path, prefix, NULL);
    int status;
    while (wait(&status) > 0);
    unlink(path);
}

void unsplitfile(char* path) {
    char* last4 = path + strlen(path) - 4;
    if (strcmp(last4, ".000")) return;
    last4[0] = '\0';
    FILE* out = fopen(path, "wb");
    char buffer[1500];
    int index = 0;
    while (1) {
        char blockname[strlen(path) + 10];
        sprintf(blockname, "%s.%03d", path, index);
        FILE* f = fopen(blockname, "rb");
        if (!f) break;
        fseek(f, 0L, SEEK_END);
        int size = ftell(f);
        rewind(f);
        fread(buffer, sizeof(char), size, f);
        fwrite(buffer, sizeof(char), size, out);
        fclose(f);
        unlink(blockname);
        index++;
    }
    fclose(out);
}


static  int  xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);


    if(flag1){
        memset(path_copy,0,sizeof(path_copy));
        strcpy(path_copy, fpath);
        flag1 = 0;
    }
    res = lstat(fpath, stbuf);
    if (res == -1)
        return -errno;


    if(flag && !flag1) {
        char res[1000], pathh[1024],filename[100],ext[100], name[1000], to[1000];
        memset(filename,0,sizeof(filename));
        memset(ext, 0, sizeof(ext));
        strcpy(filename, basename(path_copy));
        if(flag == 2) {
            strcpy(ext, get_ext(filename));
            cut_ext(filename);
        }
        strcpy(name, dirname(path_copy));

        snprintf(pathh, sizeof(pathh), "%s/%s", name, filename);

        enc1(filename, res);

        res[strlen(filename)] = '\0';
        if(strlen(ext) > 0)
            strcat(res, ext);

        memset(to, 0, sizeof(to));
        snprintf(to, sizeof(to),"%s/%s", name, res);

        rename(pathh, to);

        flag = 0;
    }


    return 0;
}  

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];
    if(strcmp(path,"/") == 0)
    {
        path=dirpath;
        sprintf(fpath,"%s",path);
    }
    else sprintf(fpath, "%s%s",dirpath,path);

    int res = 0;

    DIR *dp;
    struct dirent *de;  

    (void) offset;
    (void) fi;


    dp = opendir(fpath);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        res = (filler(buf, de->d_name, &st, 0));
        if(res!=0) break;

    }

    closedir(dp);
    return 0;
}

  

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,struct fuse_file_info *fi)
{
    char fpath[1000];

    if(strcmp(path,"/") == 0)
    {
        path=dirpath;
        sprintf(fpath,"%s",path);
    }
    else sprintf(fpath, "%s%s",dirpath,path);

    int res = 0;
    int fd = 0 ;

    (void) fi;


    fd = open(fpath, O_RDONLY);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);

    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

static int xmp_rename(const char *from, const char *to)
{
	int res;
    char ffrom[1000];
    char fto[1000];

    if(strcmp(from,"/") == 0)
    {
        from=dirpath;
        sprintf(ffrom,"%s",from);
    }
    else sprintf(ffrom, "%s%s",dirpath,from);

    if(strcmp(to,"/") == 0)
    {
        to=dirpath;
        sprintf(fto,"%s",to);
    }
    else sprintf(fto, "%s%s",dirpath,to);


    if(strlen(basename(fto)) >= 6) {
        char sub[1000];
        substring(basename(fto), sub, 1, 6);
        if(strcmp(sub, "encv1_") == 0) {
            loopDir1(ffrom,1);
            logEnc(ffrom,fto,1);
        }
    }

    if(strlen(basename(fto)) >= 6) {
        char sub[1000];
        substring(basename(fto), sub, 1, 6);
        if(strcmp(sub, "encv2_") == 0) {
            loopDir2(ffrom,1);
            logEnc(ffrom,fto,2);
        }
    }

    if(strlen(basename(ffrom)) >= 6) {
        char sub[1000],sub2[1000];
        substring(basename(ffrom), sub, 1, 6);
        substring(basename(fto), sub2, 1, 6);
        if(!strcmp(sub, "encv1_") && strcmp(sub2, "encv1_"))
            loopDir1(ffrom,0);
    }
    
	res = rename(ffrom, fto);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	int res;

    char fpath[1000];

    if(strcmp(path,"/") == 0)
    {
        path=dirpath;
        sprintf(fpath,"%s",path);
    }
    else sprintf(fpath, "%s%s",dirpath,path);

    int local_flag = 0;

    char fpath_copy[1000], dir[1000], dir_name[1000];
    strcpy(fpath_copy, fpath);

    if(strlen(basename(path)) >= 6) {
        char sub[1000];
        substring(basename(path), sub, 1, 6);
        if(strcmp(sub, "encv1_") == 0) {
            logEnc(fpath,"",1);
        }
    }

    if(strlen(basename(path)) >= 6) {
        char sub[1000];
        substring(basename(path), sub, 1, 6);
        if(strcmp(sub, "encv2_") == 0) {
            logEnc(fpath,"",2);
        }
    }

    do
    {
        strcpy(dir, dirname(fpath_copy));

        if(strlen(dir) > 2){
            strcpy(dir_name, basename(dir));

            if(strlen(dir_name) >= 6 ) {
                dir_name[6] = '\0';

                if(!strcmp(dir_name, "encv1_")) {
                    local_flag = 1;
                    break;
                }
            }
        }
    } while (strlen(dir) > 1);


	res = mkdir(fpath, mode);
	if (res == -1)
		return -errno;

    flag = flag1 = local_flag;
    
	return 0;
}


static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int res;

    char fpath[1000];

    if(strcmp(path,"/") == 0)
    {
        path=dirpath;
        sprintf(fpath,"%s",path);
    }
    else sprintf(fpath, "%s%s",dirpath,path);

    int local_flag = 0;

    char fpath_copy[1000], dir[1000], dir_name[1000];
    strcpy(fpath_copy, fpath);

    do
    {
        strcpy(dir, dirname(fpath_copy));

        if(strlen(dir) > 2){
            strcpy(dir_name, basename(dir));

            if(strlen(dir_name) >= 6 ) {
                dir_name[6] = '\0';

                if(!strcmp(dir_name, "encv1_")) {
                    local_flag = 2;
                    break;
                }
            }
        }
    } while (strlen(dir) > 1);
    
    if(flag) {
            char res[1000], to[1000], path[1024],filename[100],ext[100], name[1000];
            strcpy(name, dirname(fpath_copy));
            strcpy(filename, basename(fpath_copy));
            snprintf(path, sizeof(path), "%s/%s", name, filename);

            strcpy(ext, get_ext(filename));
            cut_ext(filename);

            enc1(filename, res);
            snprintf(to, sizeof(to),"%s/%s", name, res);

            if(strlen(ext) > 0)
                snprintf(fpath, sizeof(fpath), "%s.%s", to, ext);
    }

	if (S_ISREG(mode)) {
		res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(fpath, mode);
	else
		res = mknod(fpath, mode, rdev);
	if (res == -1)
		return -errno;

    flag = flag1 = local_flag;

	return 0;
}

static int xmp_rmdir(const char *path)
{
	int res;

    char fpath[1000];

    if(strcmp(path,"/") == 0)
    {
        path=dirpath;
        sprintf(fpath,"%s",path);
    }
    else sprintf(fpath, "%s%s",dirpath,path);


	res = rmdir(fpath);
	if (res == -1)
		return -errno;

	return 0;
}


static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .rename = xmp_rename,
	.mkdir = xmp_mkdir,
    .rmdir = xmp_rmdir,
    .mknod = xmp_mknod
};

int  main(int  argc, char *argv[])
{
    flag=0;
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
