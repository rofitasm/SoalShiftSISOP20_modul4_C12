#define FUSE_USE_VERSION 28
#include<fuse.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<dirent.h>
#include<errno.h>
#include<sys/time.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdbool.h>

char dirpath[50] = "/home/hao/Documents/Praktikum4";

char ext[9999];
int id = 0;

void cek_sub(char *s, char *sub, int p, int l) {
   int i = 0;
   while (i < l) 
   {
      sub[i] = s[p + i];
      i++;
   }
   sub[i] = '\0';
}

void logFile(char* command, char* desc)
{
    char now[100];
    char level[30];
    time_t rawtime;
    struct tm *info;
    time(&rawtime);
    info = localtime(&rawtime);
    strftime(now, sizeof(now), "%y%m%d-%H:%M:%S::", info);
    if(strcmp(command, "RMDIR") == 0 || strcmp(command, "UNLINK") == 0){
        strcpy(level, "WARNING");
    }
    else{
        strcpy(level, "INFO");
    }
    char logLine[200];
    sprintf(logLine, "%s::%s%s::%s", level, now, command, desc);

    FILE* fp;
    fp = fopen("/home/hao/fs.log", "a");
    fprintf(fp, "%s\n", logLine);
    fclose(fp);
}


void Ffo(char *text, char* path)
{
    char* info = "INFO";
	char curtime[30];
    time_t t = time(NULL);
    struct tm* p1 = localtime(&t);
	strftime(curtime, 30, "%y%m%d-%H:%M:%S", p1);
    char log[1000];
    sprintf(log, "%s::%s::%s::%s", info, curtime, text, path);
	FILE *out = fopen("/home/hao/fs.log", "a");  
    fprintf(out, "%s\n", log);  
    fclose(out); 
    
}

void writeWarning(char *text, char* path)
{
    char* info = "WARNING";
    char curtime[30];
    time_t t = time(NULL);
    struct tm* p1 = localtime(&t);
	strftime(curtime, 30, "%y%m%d-%H:%M:%S", p1);
    char log[1000];
    sprintf(log, "%s::%s::%s::%s", info, curtime, text, path);
	FILE *out = fopen("/home/hao/fs.log", "a");  
    fprintf(out, "%s\n", log);  
    fclose(out); 
}

char cis[] = "9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";
int key = 10;

char *encrypt(char* str, bool cek)
{
	int i, j, k = 0;
	char *ext = strrchr(str, '.');
	if(cek && ext != NULL) k = strlen(ext);
	for(i = 0; i < strlen(str) - k; i++)
	{
		for(j = 0; j < strlen(cis); j++)
		{
			if(str[i] == cis[j]){
				str[i] = cis[(j + key) % 87];
				break;
			}
		}
	}
	return str;
}

char *decrypt(char* str, bool cek)
{
	int i, j, k = 0;
	char *ext = strrchr(str, '.');
	if(cek && ext != NULL) k = strlen(ext);
	for(i = 0; i < strlen(str) - k; i++)
	{
		for(j = 0; j < 87; j++)
		{
			if(str[i] == cis[j]){
				str[i] = cis[(j + 87 - key) % 87];
				break;
			}
		}
	}
	return str;
}

char *lastSub(char *str)
{
	if(!strcmp(str, "/")) return NULL;
	return strrchr(str, '/') + 1;
}

char *cekPath(char *str)
{
	bool encr;
	int start, id;
	encr = 0; start = 1; 
	id = strchr(str + start, '/') - str - 1;
	char curpos[1024];
	while(id < strlen(str))
	{
		strcpy(curpos, "");
		strncpy(curpos, str + start, id - start + 1);
		curpos[id - start + 1] = '\0';
		if(encr)
		{
			encrypt(curpos, 0);
			strncpy(str + start, curpos, id - start + 1);
		}
		if(!encr && strstr(str + start, "encv1_") == str + start) encr = 1;
		start = id + 2;
		id = strchr(str + start, '/') - str - 1;
	}
	id = strlen(str); id--;
	strncpy(curpos, str + start, id - start + 1);
	curpos[id - start + 1] = '\0';
	if(encr)
	{
		encrypt(curpos, 1);
		strncpy(str + start, curpos, id - start + 1);
	}
	return str;
}

char *catPath(char *fin, char *str1, const char *str2)
{
	strcpy(fin, str1);
	if(!strcmp(str2, "/")) return fin;
	if(str2[0] != '/')
	{
		fin[strlen(fin) + 1] = '\0';
		fin[strlen(fin)] = '/';
	}
	sprintf(fin, "%s%s", fin, str2);
	return fin;
}

void split(char *str)
{
	char finalPath[10000000];
	sprintf(finalPath, "%s.", str);
	pid_t cid;
	cid = fork();
	if(cid == 0)
	{
		char *argv[] = {"split", "-b", "1024", "-d", "-a", "3", str, finalPath, NULL};
		execv("/usr/bin/split", argv);
	}
	int status;
	while(wait(&status) > 0);
	unlink(str);
}

void combine(char *str)
{
	char buff[2048];
	int id = 0;
	char *ext = str + strlen(str) - 4;
	if(strcmp(ext, ".000")) return;
	ext[0] = '\0';
	FILE *combined;
	combined = fopen(str, "wb");
	while(1)
	{
		char name[1000000];
		sprintf(name, "%s.%03d", str, id);
		FILE *each;
		each = fopen(name, "rb");
		if(!each) break;
		fseek(each, 0L, SEEK_END);
		rewind(each);
		fread(buff, sizeof(buff), ftell(each), each);
		fwrite(buff, sizeof(buff), ftell(each), combined);
		fclose(each);
		unlink(name);
		id++;
	}
	fclose(combined);
}

int encrFolder(char *str)
{
	int ans;
	char *fi = strtok(str, "/");
	ans = 0;
	while(fi)
	{
		char sub[1024];
		cek_sub(fi, sub, 0, 6);
		if(!strcmp(sub, "encv1_")) ans |= 1;
		else if(!strcmp(sub, "encv2_")) ans |= 2;
		fi = strtok(NULL, "/");
	}
	return ans;
}

int encrFull(char *str)
{
	int ans;
	char *fi = strtok(str, "/");
	char *sc = strtok(NULL, "/");
	ans = 0;
	while(sc)
	{
		char sub[1024];
		cek_sub(fi, sub, 0, 6);
		if(!strcmp(sub, "encv1_")) ans |= 1;
		else if(!strcmp(sub, "encv2_")) ans |= 2;
		fi = sc;
		sc = strtok(NULL, "/");
	}
	return ans;
}

void whileEnc1(char *str, int flag)
{
	struct dirent *dp;
	DIR *dir = opendir(str);
	
	if(!dir) return;
	
	while((dp = readdir(dir)) != NULL)
	{
		if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
        	char path[2000000], name[1000000], newname[1000000];
        	catPath(path, str, dp->d_name);
			strcpy(name, dp->d_name);
			if(flag == 1) catPath(newname, str, encrypt(name, 1));
			else if(flag == -1) catPath(newname, str, decrypt(name, 1));
			if(dp->d_type == DT_REG) rename(path, newname);
			else if(dp->d_type == DT_DIR)
			{
				rename(path, newname);
				whileEnc1(newname, flag);
			}
        }
	}
}

void encrypt1(char *str, int flag)
{
	struct stat add;
	stat(str, &add);
	if(!S_ISDIR(add.st_mode)) return;
	whileEnc1(str, flag);
}

void whileEnc2(char *str, int flag)
{
	struct dirent *dp;
	DIR *dir = opendir(str);
	
	if(!dir) return;
	
	while((dp = readdir(dir)) != NULL)
	{
		if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
			char path[2000000];
        	catPath(path, str, dp->d_name);
			if(dp->d_type == DT_DIR) whileEnc2(path, flag);
			else if(dp->d_type == DT_REG)
			{
				if(flag == 1) split(path);
				if(flag == -1) combine(path);
			}
        }
	}
}

void encrypt2(char *str, int flag)
{
	struct stat add;
	stat(str, &add);
	if(!S_ISDIR(add.st_mode)) return;
	whileEnc2(str, flag);
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
	int res;
	char fpath[1000];
	catPath(fpath, dirpath, path);
	res = lstat(cekPath(fpath), stbuf);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_access(const char *path, int mask)
{
	int res;
	char fpath[1000];
	catPath(fpath, dirpath, path);
	res = access(cekPath(fpath), mask);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_readlink(const char *path, char *buf, size_t size)
{
	int res;
	char fpath[1000];
	catPath(fpath, dirpath, path);
	res = readlink(cekPath(fpath), buf, size - 1);
	if (res == -1) return -errno;
	buf[res] = '\0';
	return 0;
}


static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];
	catPath(fpath, dirpath, path);
	int res = 0;
	
	DIR *dp;
	struct dirent *de;
	(void) offset;
	(void) fi;
	dp = opendir(cekPath(fpath));
	if (dp == NULL) return -errno;
	
	int flag = encrFolder(fpath);
	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;		
		st.st_mode = de->d_type << 12;
		char nama[1000000];
		strcpy(nama, de->d_name);
		if(flag == 1)
		{
			if(de->d_type == DT_REG) decrypt(nama, 1);
			else if(de->d_type == DT_DIR && strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) decrypt(nama, 0);
			res = (filler(buf, nama, &st, 0));
			if(res!=0) break;
		}
		else
		{
			res = (filler(buf, nama, &st, 0));
			if(res!=0) break;
		}
	}
	closedir(dp);
//    writeInfo("CD", fpath);
//      logFile("CD", fpath);
	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
	char fpath[1000];
	catPath(fpath, dirpath, path);
	cekPath(fpath);
	int res;
	
	if (S_ISREG(mode)) {
		res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0) res = close(res);
	} else if (S_ISFIFO(mode)) res = mkfifo(fpath, mode);
	else res = mknod(fpath, mode, rdev);
	if (res == -1) return -errno;
	
    logFile("CREATE", fpath);
	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	char fpath[1000];
	catPath(fpath, dirpath, path);
	
	int res;

	res = mkdir(cekPath(fpath), mode);
	if (res == -1) return -errno;
	
    char cek_substr[1024];
    if(lastSub(fpath) == 0) return 0;
    char filePath[1000000];
    strcpy(filePath, lastSub(fpath));
    cek_sub(filePath, cek_substr, 0, 6);
	if(strcmp(cek_substr, "encv1_") == 0) //folder encrypt1
	{
		encrypt1(fpath, 1);	
	}
	else if(strcmp(cek_substr, "encv2_") == 0) //folder encrypt2
	{
		encrypt2(fpath, 1);
	}
	logFile("MKDIR", fpath);
	return 0;
}

static int xmp_unlink(const char *path)
{
	char fpath[1000];
	catPath(fpath, dirpath, path);
	int res;

	res = unlink(cekPath(fpath));
    	logFile("UNLINK", fpath);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_rmdir(const char *path)
{
	char fpath[1000];
	catPath(fpath, dirpath, path);
	int res;

	res = rmdir(cekPath(fpath));
    logFile("RMDIR", fpath);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_symlink(const char *from, const char *to)
{
	int res;

	res = symlink(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to)
{    
    char from2[1000];
	catPath(from2, dirpath, from);
	
    char fto[1000];
	catPath(fto, dirpath, to);
	
	int res;

	res = rename(cekPath(from2), cekPath(fto));
	
	if (res == -1)
		return -errno;
	
	int from1 = 0, too = 0;
	char cek_substr[1024], cek2[1024];
    if(lastSub(from2) == 0) return 0;
    char filePath[1000000];
    strcpy(filePath, lastSub(from2));
    cek_sub(filePath, cek_substr, 0, 6);
	if(strcmp(cek_substr, "encv1_") == 0) //folder encrypt1
	{
		from1 = 1;
	}
	else if(strcmp(cek_substr, "encv2_") == 0) //folder encrypt2
	{
		from1 = 2;
	}
	
    if(lastSub(fto) == 0) return 0;
    strcpy(filePath, lastSub(fto));
    cek_sub(filePath, cek_substr, 0, 6);
	if(strcmp(cek2, "encv1_") == 0) //folder decrypt1
	{
		too = 1;
	}
	else if(strcmp(cek2, "encv2_") == 0) //folder decrypt2
	{
		too = 2;
	}
	
	if(from1 == 0 && too == 1) encrypt1(fto, 1);
	else if(from1 == 0 && too == 2) encrypt2(fto, 1);
	else if(from1 == 1 && too != 1) encrypt1(fto, -1);
	else if(from1 == 1 && too == 2) encrypt2(fto, 1);
	else if(from1 == 2 && too != 1) encrypt1(fto, -1);
	else if(from1 == 2 && too == 2) encrypt2(fto, 1);
	logFile("RENAME", from2);

	return 0;
}

static int xmp_link(const char *from, const char *to)
{
	int res;

	res = link(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	char fpath[1000];
	catPath(fpath, dirpath, path);
	int res;

	res = chmod(cekPath(fpath), mode);
	logFile("CHMOD", fpath);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
	char fpath[1000];
	catPath(fpath, dirpath, path);
	int res;

	res = lchown(cekPath(fpath), uid, gid);
	logFile("CHOWN", fpath);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
	char fpath[1000];
	catPath(fpath, dirpath, path);
	int res;

	res = truncate(cekPath(fpath), size);
    logFile("TRUNCATE", fpath);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	char fpath[1000];
	catPath(fpath, dirpath, path);
	int res;
	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(cekPath(fpath), tv);
    logFile("UTIMENS", fpath);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
	char fpath[1000];
	catPath(fpath, dirpath, path);
	int res;

	res = open(cekPath(fpath), fi->flags);
    	logFile("OPEN", fpath);
	if (res == -1) return -errno;
	close(res);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	char fpath[1000];
	catPath(fpath, dirpath, path);
	int fd = 0;
	int res = 0;

	(void) fi;
	fd = open(cekPath(fpath), O_RDONLY);
	if (fd == -1) return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1) res = -errno;
	close(fd);
	
    logFile("CAT", fpath);
	return res;
}

static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];
	catPath(fpath, dirpath, path);
	int fd;
	int res;

	(void) fi;
	fd = open(cekPath(fpath), O_WRONLY);
	if (fd == -1) return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1) res = -errno;

    logFile("WRITE", fpath);
	close(fd);
	return res;
}

static int xmp_statfs(const char *path, struct statvfs *stbuf)
{
	char fpath[1000];
	catPath(fpath, dirpath, path);
	int res;

	res = statvfs(cekPath(fpath), stbuf);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi) 
{
	char fpath[1000];
	catPath(fpath, dirpath, path);
    (void) fi;

    int res;
    res = creat(cekPath(fpath), mode);
    if(res == -1) return -errno;
	
    logFile("CREAT", fpath);
    close(res);
    return 0;
}


static int xmp_release(const char *path, struct fuse_file_info *fi)
{
	(void) path;
	(void) fi;
	return 0;
}

static int xmp_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	(void) path;
	(void) isdatasync;
	(void) fi;
	return 0;
}


static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.access		= xmp_access,
	.readlink	= xmp_readlink,
	.readdir	= xmp_readdir,
	.mknod		= xmp_mknod,
	.mkdir		= xmp_mkdir,
	.symlink	= xmp_symlink,
	.unlink		= xmp_unlink,
	.rmdir		= xmp_rmdir,
	.rename		= xmp_rename,
	.link		= xmp_link,
	.chmod		= xmp_chmod,
	.chown		= xmp_chown,
	.truncate	= xmp_truncate,
	.utimens	= xmp_utimens,
	.open		= xmp_open,
	.read		= xmp_read,
	.write		= xmp_write,
	.statfs		= xmp_statfs,
	.create     = xmp_create,
	.release	= xmp_release,
	.fsync		= xmp_fsync,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}

