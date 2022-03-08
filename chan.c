#include "gfunctions.h"
#include "sh_mem.h"
#include <stdio.h>
#include <stdlib.h>

#define SEM_KEY_ENC1 (key_t) 3000
#define SHM_KEY_ENC1 (key_t) 3001

#define SEM_KEY_ENC2 (key_t) 4000
#define SHM_KEY_ENC2 (key_t) 4001
#define SIZE (int) 1024

int main(int argc, char* argv[]){
    double arg = atof(argv[1]);
	char *args[]={"./enc2",NULL};
    make_process(args);

    pCom enc1 = sh_mem_create(SHM_KEY_ENC1, SEM_KEY_ENC1 ,SIZE);
    pCom enc2 = sh_mem_create_super(SHM_KEY_ENC2, SEM_KEY_ENC2 ,SIZE);

    char **str_enc2,**str_enc1   = sh_mem_read_checksum(enc1);
    sh_mem_init(enc2);
    if(str_enc1[1] != NULL){ random_change(&(str_enc1[0]),arg,100);}
    //printf("chan_read_enc1 : %s\n",str_enc1[0]);
    while(enc1->exit == F && enc2->exit == F){
        str_enc2 = sh_mem_wr1_checksum(enc2,str_enc1[0],str_enc1[1]);

        if(str_enc2 == NULL){break;}
        else if(str_enc2[1] != NULL){ random_change(&(str_enc2[0]),arg,100);}
        //printf("chan_read_enc2 : %s\n",str_enc2[0]);
        free(str_enc1[0]);
        free(str_enc1[1]);
        free(str_enc1);

        str_enc1 = sh_mem_wr2_checksum(enc1,str_enc2[0],str_enc2[1]);
        if(str_enc1 == NULL){break;}
        else if(str_enc1[1] != NULL){ random_change(&(str_enc1[0]),arg,100);}
        free(str_enc2[0]);
        free(str_enc2[1]);
        free(str_enc2);
    }
    if(str_enc1 != NULL){
        sh_mem_wr1_checksum(enc2,str_enc1[0],NULL);
        free(str_enc1[0]);
        free(str_enc1[1]);
        free(str_enc1);
        delete_sh_mem(enc1);
    }
    else{
        free(str_enc2[0]);
        free(str_enc2[1]);
        free(str_enc2);
        delete_sh_mem(enc2);
    }
    free(enc2); 
    free(enc1);
    
    wait_proc();
    printf("end of chan\n");
    return 0; 
}