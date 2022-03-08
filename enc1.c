#include "gfunctions.h"
#include "sh_mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEM_KEY_P1 (key_t) 2000
#define SHM_KEY_P1 (key_t) 2001

#define SEM_KEY_CHAN (key_t) 3000
#define SHM_KEY_CHAN (key_t) 3001
#define SIZE (int) 1024

int main(int argc,char* argv[]){
    pCom p1   = sh_mem_create(SHM_KEY_P1  , SEM_KEY_P1  ,SIZE);
    pCom chan = sh_mem_create_super(SHM_KEY_CHAN, SEM_KEY_CHAN,SIZE);

	char *args[]={"./chan",argv[1],NULL};
	make_process(args);

    int num_wrong_checksum = 0,num_sent_again = 0;
    char **str_chan,*str_p1   = sh_mem_read(p1);
    sh_mem_init(chan);
    while(p1->exit == F && chan->exit == F){
        char* checksum = get_checksum(str_p1);

        str_chan = sh_mem_wr1_checksum(chan,str_p1,checksum);
        free(checksum);
        if(str_chan == NULL){ break; }
        else if(strcmp(str_chan[0],"\1")==0){
            num_sent_again++;
            printf("enc1 : retransmit %s !\n",str_p1);
            free(str_chan[0]);
            free(str_chan[1]);
            free(str_chan);
            str_chan = sh_mem_wr1_checksum(chan,str_p1,NULL);
        }
        checksum = get_checksum(str_chan[0]);
        if(str_chan[1] != NULL){
            if(strcmp(checksum,str_chan[1]) != 0){
                num_wrong_checksum++;
                printf("enc1 : give me the message again ! \n");
                free(str_chan[0]);
                free(str_chan[1]);
                free(str_chan);
                str_chan = sh_mem_wr1_checksum(chan,"\1",NULL);
            }
        }
        free(str_p1);
        str_p1 = sh_mem_wr2(p1,str_chan[0]);

        free(str_chan[0]);
        free(str_chan[1]);
        free(str_chan);
    }
    if(str_p1 != NULL){
        sh_mem_wr1(chan,str_p1);
        free(str_p1);
        delete_sh_mem(p1);
    }
    else{
        delete_sh_mem(chan);
    }
    free(chan); 
    free(p1);
    wait_proc();
    printf("enc1 -> number of messages with different checksum : %d\n\
     -> number of retransmitted messages : %d\n",num_wrong_checksum,num_sent_again);
    printf("end of enc1\n");
    return 0; 
}