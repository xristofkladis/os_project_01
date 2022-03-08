//General functions

#include "gfunctions.h"
#include <openssl/md5.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int make_process(char** args){
	int id = fork();
	if(id != 0){ return id; }
	else{ execvp(args[0],args); return -1; }
}

void wait_proc(){
    int status, wpid;
    while ((wpid = wait(&status)) > 0);
}

char* get_checksum(const char* str){    // code from https://stackoverflow.com/questions/7627723/how-to-create-a-md5-hash-of-a-string-in-c
    if(str == NULL)               { return NULL; }
    else if(strcmp(str,"TERM")==0){ return NULL; }
    int length = strlen(str);
    int n;
    MD5_CTX c;
    unsigned char digest[16];
    char *out = (char*)malloc(33);

    MD5_Init(&c);

    while (length > 0) {
        if (length > 512)
            MD5_Update(&c, str, 512);
        else
            MD5_Update(&c, str, length);
        length -= 512;
        str += 512;
    }
    MD5_Final(digest, &c);

    for (n = 0; n < 16; ++n)
        snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)digest[n]);
    return out;
}

void random_change(char** pstr,double arg,int base){
    if(*pstr == NULL)               { return; }
    else if(strcmp(*pstr,"\1") == 0){ return; }
    char* str_old = *pstr;
    time_t t;
    int len = strlen(*pstr);
    char str[len+1];
    str[len]= '\0';
    srand((unsigned) time(&t));

    for(int i=0; i<len; i++){
        int r = rand()%base;
        if( r < arg ){ str[i] = str_old[i]; }
        else         { str[i] = 32 + rand()%55; }
    }
    free(*pstr);
    *pstr = malloc(sizeof(char)*(len+1));
    strcpy(*pstr,str);
}

char* scan_space(int *check){
    printf("Give me a string : ");
    unsigned int len = 20;
    unsigned int current_size = len;
    char c;

    c = getchar();
    if(c==EOF){ return NULL; }
    int i=0;
    while(c==' '){ c = getchar();}

    if(c == '\n'){ (*check)=0; return NULL;}

    char* str = malloc(len*sizeof(char));

    while(c!=EOF && c!='\n'){
        str[i] = c;
        i++;
        c = getchar();
        if(i==(current_size)){
            current_size += len;
            char * str_help = (char*)realloc(str,sizeof(char)*(current_size));
            if(str_help == NULL)
                printf("- error! Something wrong with realloc in scan function -> data.c!!\n");
            else
                str = str_help;
        }
    }
    char* new_str = malloc(sizeof(char)*(i+1));
    for(int k=0;k<i;k++)
        new_str[k] = str[k];
    new_str[i] = '\0';
    free(str);
    if(c == '\n')
        (*check)=0;
    if(c == EOF)
        (*check)=-1;
    return new_str;
}