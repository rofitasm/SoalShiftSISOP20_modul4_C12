# SoalShiftSISOP20_modul4_C12
1. 18-0034 Rofita Siti Musdalifah
2. 18-0086 Calvin Wijaya
# Soal 1
Diperintahkan melakukan enkripsi ke file-file atau folder yang berada di dalamnya. Enkripsi ini dilakukan dengan melakukan enkripsi nama dari file-file atau direktori nya dengan metode caesar cipher dengan key 
```9ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO```

Fungsi yang dipakai di No 1 antara lain:
- Fungsi enc1
Fungsi ini digunakan untuk enkripsi string sesuai jenis enkripsinya
- Fungsi dec1
Fungsi ini dipakai untuk melakukan dekripsi dari string yang telaj terenkripsi
- Fungsi logEncv
Fungsi ini digunakan untuk log pembuatan folder enkripsi
- Fungsi cut_ext
Fungsi untuk menghilangkan ekstensi dari filename
- Fungsi get_ext
Fungsi untuk mendapat ekstensi dari filename
- Fungsi loopDir1
Fungsi untuk melakukan enkripsi atau dekripsi secara rekursif
- Fungsi Substring
Fungsi untuk mendapat substring dari suatu string

Lalu beberapa fungsi fuse yang di modifikasi diantaranya :
- Fungsi xmp_getattr
Menambahkan rename file atau folder jika dibuat didalam folder enkripsi
- Fungsi xmp_rename
Menambahkan apabila membuat folder enkripsi dengan rename
- Fungsi xmp_mkdir
Melacak apabila directory dibuat dibawah directory enkripsi
- Fungsi xmp_mknod
Melacak apakah file dibuat di bawah directory enkripsi

# Soal 2
Diminta cara pendeteksian nama folder dan rekursi sama. Bedanya, untuk setiap file dilakukan `split` agar file terpecah-pecah menjadi ukuran 1024 byte masing-masingnya.
Agar file dapat terbaca dengan normal, pertama fungsi `readdir()` implementasi FUSE perlu diubah agar file-file dengan format nama terpecah tidak terbaca. Sebaliknya, fungsi tersebut akan mengembalikan hanya satu file saja.  

Kemudian, pada implementasi fungsi `read()` diubah sehingga setiap pembacaan file dilakukan looping untuk membaca semua file-file pecahannya, dan dioutputkan ke buffer sebagai satu file
# Soal 3
```awk
static int xmp_fsyncdir(const char *path, int isdatasync, struct fuse_file_info *fi) {
    
    printf("\n\nDEBUG sync\n\n");
    char fpath[1000], fileName1[100], fileName2[100];
    int n, m, res;

    sprintf(fpath, "%s%s", dirpath, path);
    sprintf(fileName1, "%s", path);
    sprintf(fileName2, "sync_%s", path);

    DIR *dp = opendir(fpath);
    DIR *d1 = opendir(fileName1);
    DIR *d2 = opendir(fileName2);
    struct tm *foo1, *foo2;
    struct stat attrib1, attrib2;
    struct dirent *de, *di1, *di2;
    struct dirent **namelist1, **namelist2;

    while((de = readdir(dp)) != NULL) {
        if(strcmp(fileName1, de->d_name) == 0 && strcmp(fileName2, de->d_name) == 0)   {
            if  (((di1 = readdir(d1)) != NULL) && ((di2 = readdir(d2)) != NULL))	    {
                n = scandir(fileName1, &namelist1, NULL, alphasort);
                m = scandir(fileName2, &namelist2, NULL, alphasort);
                if (n == m){
                    while (n--) {
                        if(strcmp(di1->d_name, di2->d_name) != 0) break; 
                        
                        stat(di1->d_name, &attrib1);
                        stat(di2->d_name, &attrib2);
                        foo1 = gmtime(&(attrib1.st_mtime));
                        foo2 = gmtime(&(attrib2.st_mtime));
                        res = foo1->tm_min - foo2->tm_min;
                        if(res > 0.1) break;

						FILE * fp = fopen(fileName1, "wb");
						write(fp, fileName1, strlen(fileName1));
						write(fp, "\n", 1);
						fsync(fp);

						FILE * fd = fopen(fileName2, "wb");
						write(fd, fileName2, strlen(fileName2));
						write(fd, "\n", 1);
						fsync(fd);

						fclose(fp);
						fclose(fd);
                    }				
                }
				closedir(d1);
				closedir(d2);
            }
        }
    }
    return 0;
} 
```
# Soal 4
```awk
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
    fp = fopen("/home/rofita/fs.log", "a");
    fprintf(fp, "%s\n", logLine);
    fclose(fp);
}

```
