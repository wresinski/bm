#include "color.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define IS_EXT(name, ext) strcmp(&name[strlen(name) - strlen(ext)], ext)

int *next = NULL;
int size = 0;

void getNext(char *t, int tLength, int *next) {
    next[0] = -1;
    next[1] = 0;
    int i = 1;
    int j = next[i];
    i++;
    while (i < tLength) {
	if (t[i - 1] == t[j]) {
	    next[i] = j + 1;
	    j++;
	    i++;
	} else if (j == 0) {
	    next[i] = j;
	    i++;
	} else
	    j = next[j];
    }
}

void getNextVal(char *t, int tLength, int *next) {
    next[0] = -1;
    next[1] = 0;
    int i = 1;
    int j = next[j];
    i++;
    while (i < tLength) {
	if (t[i - 1] == t[j]) {
	    if (t[i] != t[j + 1])
		next[i] = j + 1;
	    else
		next[i] = next[j + 1];
	    j++;
	    i++;
	} else if (j == 0) {
	    next[i] = j;
	    i++;
	} else
	    j = next[j];
    }
}

int indexKMP(char *s, int sLength, char *t, int tLength, int pos) {
    if (!next) {
	next = malloc(sizeof(int) * tLength);
	size = tLength;
	getNextVal(t, tLength, next);
    }
    if (size < tLength) {
	free(next);
	next = malloc(sizeof(int) * tLength);
	size = tLength;
	getNextVal(t, tLength, next);
    }
    // getNext(t,tLength,next);
    // getNextVal(t,tLength,next);
    int i = pos;
    int j = 0;
    while (i < sLength && j < tLength) {
	if (j == -1 || s[i] == t[j]) {
	    i++;
	    j++;
	} else
	    j = next[j];
    }
    if (j >= tLength)
	return i - tLength;
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
	    int index = indexKMP(s, sLength, t, tLength, i);
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
	    int index = indexKMP(s,sLength,t,tLength,i);
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
    if (next) {
        free(next);
        next = NULL;
        size = 0;
    }
    return 0;
}
