#include "gfunctions.h"
#include "sh_mem.h"
#include <stdio.h>
#include <stdlib.h>

#define SEM_KEY_ENC2 (key_t) 5000
#define SHM_KEY_ENC2 (key_t) 5001
#define SIZE (int) 1024

int main(){
    pCom enc2 = sh_mem_create(SHM_KEY_ENC2, SEM_KEY_ENC2 ,SIZE);
    int i,num_mes=1;
    char *str,*str_enc2   = sh_mem_read(enc2);
    while(enc2->exit == F){
        printf("Message from p2  : %s\n",str_enc2);
        free(str_enc2);
        str = scan_space(&i);
        str_enc2 = sh_mem_wr2(enc2,str);
        num_mes++;
        free(str);
    } 
    free(str_enc2);
    free(enc2);

    printf("p2 : Num of sented messages = %d \n",num_mes);
	return 0;
}