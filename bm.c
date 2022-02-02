#include "color.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define IS_EXT(name, ext) strcmp(&name[strlen(name) - strlen(ext)], ext)

int b[256] = {0};
int *g = NULL;
int size = 0;

void buildBads(char *t, int tLength, int bads[256]) {
    for (int i = 0; i < 256; ++i)
        bads[i] = tLength;
    for (int i = 0; i < tLength; ++i)
        bads[t[i]] = tLength - 1 - i;
}

void suffixes(char *t, int tLength, int *suff) {
    suff[tLength - 1] = tLength;
    for (int i = tLength - 2; i >= 0; --i) {
        int j = i;
        while (j >= 0) {
            if (t[j] != t[tLength - 1 - i + j])
                break;
            j--;
        }
        suff[i] = i - j;
    }
}

void buildGoods(char *t, int tLength, int *goods) {
    int *suff = malloc(sizeof(int) * tLength);
    suffixes(t, tLength, goods);
    for (int i = 0; i < tLength; ++i) {
        goods[i] = 2 * tLength - 1 - i;
    }
    int j = 0;
    for (int i = tLength - 1; i >= 0; --i) {
        if (suff[i] == i + 1) {
            for (; j < tLength - 1 - i; ++j) {
                if (goods[j] == 2 * tLength - 1 - j)
                    goods[j] = 2 * tLength - 2 - i - j;
            }
        }
    }
    for (int i = 0; i < tLength - 1; ++i) {
        goods[tLength - 1 - suff[i]] = tLength - 1 - i + suff[i];
    }
    free(suff);
}

int indexBM(char *s, int sLength, char *t, int tLength, int pos) {
    if (sLength < tLength)
        return -1;
    if (!g) {
        buildBads(t, tLength, b);
        g = malloc(sizeof(int) * tLength);
        size = tLength;
        buildGoods(t, tLength, g);
    }
    if (size < tLength) {
        buildBads(t, tLength, b);
        free(g);
        g = malloc(sizeof(int) * tLength);
        size = tLength;
        buildGoods(t, tLength, g);
    }
    int i = tLength - 1;
    int j = pos + i;
    while (i >= 0 && j < sLength) {
        if (t[i] == s[j]) {
            i--;
            j--;
        } else {
            j += g[i] > b[s[j]] ? g[i] : b[s[j]];
            i = tLength - 1;
        }
    }
    if (i < 0)
        return j + 1;
    else
        return -1;
}

int searchFile(char *path, char *t) {
    FILE *fp = fopen(path, "r");
    if (!fp)
        return 0;
    char s[1024] = {0};
    int line = 0;
    int line_p = 0;
    while (fgets(s, 1024, fp) != NULL) {
        line++;
        int c = 0;
        int sLength = strlen(s);
        int tLength = strlen(t);
        char r[2048] = {0};
        int i = 0;
        while (i < sLength) {
            int index = indexBM(s, sLength, t, tLength, i);
            if (index == -1) {
                if (c > 0)
                    strcat(r, &s[i]);
                break;
            }
            strncat(r, &s[i], index - i);
            strcat(r, L_GREEN);
            strcat(r, t);
            strcat(r, NONE);
            i = index + tLength;
            c++;
            line_p++;
        }
        if (c > 0 && line_p == 1)
            printf(RED "\n%s:\n" NONE, path);
        if (c > 0)
            printf(YELLOW "%d:" NONE "%s", line, r);
    }
    fclose(fp);
    return 1;
}

int searchDir(char *pathname, char *t) {
    DIR *path = NULL;
    path = opendir(pathname);
    if (path == NULL) {
        return 0;
    }
    struct dirent *ptr;
    char buf[1024];
    while ((ptr = readdir(path)) != NULL) {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
            continue;
        if (ptr->d_type == DT_DIR) {
            sprintf(buf, "%s/%s", pathname, ptr->d_name);
            searchDir(buf, t);
        } else if (ptr->d_type == DT_REG) {
            sprintf(buf, "%s/%s", pathname, ptr->d_name);
            if (IS_EXT(buf, ".c") == 0 || IS_EXT(buf, ".h") == 0 ||
                IS_EXT(buf, ".cpp") == 0 || IS_EXT(buf, ".hpp") == 0 ||
                IS_EXT(buf, ".cc") == 0 || IS_EXT(buf, ".s") == 0 ||
                IS_EXT(buf, ".S") == 0 || IS_EXT(buf, ".asm") == 0)
                searchFile(buf, t);
        }
    }
    closedir(path);
    return 1;
}

int main(int argc, char *argv[]) {
    /*
        char s[1024] = {0};
        char t[128] = {0};
        printf("Please enter the main string:\n");
        scanf("%s",s);
        printf("Please enter the pattern string:\n");
        scanf("%s",t);
        int sLength = strlen(s);
        int tLength = strlen(t);
        char r[1024] = {0};
        int i = 0;
        while(i<sLength){
            int index = indexBM(s,sLength,t,tLength,i);
            if(index==-1){
                strcat(r,&s[i]);
                break;
            }
            strncat(r,&s[i],index-i);
            strcat(r," [");
            strcat(r,t);
            strcat(r,"] ");
            i = index + tLength;
        }
        printf("%s\n",r);
    */

    if (argc < 2)
        return 0;
    struct stat statbuf;
    if (lstat(argv[2], &statbuf) != 0)
        return 0;
    if (S_ISDIR(statbuf.st_mode))
        searchDir(argv[2], argv[1]);
    else
        searchFile(argv[2], argv[1]);
    if (g) {
        free(g);
        g = NULL;
        size = 0;
    }
    return 0;
}
