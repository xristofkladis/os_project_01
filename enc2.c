#include "gfunctions.h"
#include "sh_mem.h"
#include <stdio.h>
#include <stdlib.h>

#define SEM_KEY_CHAN (key_t) 4000
#define SHM_KEY_CHAN (key_t) 4001

#define SEM_KEY_P2 (key_t) 5000
#define SHM_KEY_P2 (key_t) 5001
#define SIZE (int) 1024

int main(void){
    pCom chan = sh_mem_create(SHM_KEY_CHAN , SEM_KEY_CHAN  ,SIZE);
    pCom p2   = sh_mem_create_super(SHM_KEY_P2   , SEM_KEY_P2    ,SIZE);

    int num_wrong_checksum = 0,num_sent_again = 0;
    char *str_p2,**str_chan = sh_mem_read_checksum(chan);
    sh_mem_init(p2);
    char* checksum = get_checksum(str_chan[0]);
    if(str_chan[1] != NULL){
        if(strcmp(checksum,str_chan[1]) != 0){
            num_wrong_checksum++;
            printf("enc2 : give me the message again ! \n");
            free(str_chan[0]);
            free(str_chan[1]);
            free(str_chan);
            str_chan = sh_mem_wr2_checksum(chan,"\1",NULL);
        }
    }
    free(checksum);
    while(chan->exit == F && p2->exit == F){
        str_p2 = sh_mem_wr1(p2,str_chan[0]);
        checksum = get_checksum(str_p2);
        free(str_chan[0]);
        free(str_chan[1]);
        free(str_chan);
        str_chan = NULL;
        str_chan = sh_mem_wr2_checksum(chan,str_p2,checksum);
        free(checksum);

        if(str_chan == NULL){break;}
        if(strcmp(str_chan[0],"\1")==0){
            num_sent_again++;
            printf("enc2 : retransmit %s !\n",str_p2);
            free(str_chan[0]);
            free(str_chan[1]);
            free(str_chan);
            str_chan = sh_mem_wr2_checksum(chan,str_p2,NULL);
        }
        checksum = get_checksum(str_chan[0]);
        if(str_chan[1] != NULL && strcmp(str_chan[0],"TEMP") != 0){
            if(strcmp(checksum,str_chan[1]) != 0){
                num_wrong_checksum++;
                printf("enc2 : give me the message again ! \n");
                free(str_chan[0]);
                free(str_chan[1]);
                free(str_chan);
                str_chan = sh_mem_wr2_checksum(chan,"\1",NULL);
            }
        }

        //printf("enc2_read_chan : %s\n",str_chan[0]);
        free(str_p2);
    }
    if(str_chan != NULL){
        sh_mem_wr1(p2,str_chan[0]);
        free(str_chan[0]);
        free(str_chan[1]);
        free(str_chan);
        delete_sh_mem(chan);
    }
    else{
        free(str_p2);
        delete_sh_mem(p2);
    }
    free(p2); 
    free(chan);
    printf("enc2 -> number of messages with different checksum : %d\n\
     -> number of retransmitted messages : %d\n",num_wrong_checksum,num_sent_again);
    printf("end of enc2\n");
    return 0; 
}