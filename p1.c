#include "gfunctions.h"
#include "sh_mem.h"
#include <stdio.h>
#include <stdlib.h>

#define SEM_KEY (key_t) 2000
#define SHM_KEY (key_t) 2001
#define SIZE (int) 1024

int main(int argc, char* argv[]){
    if(argc != 2){
        printf("./p1 must have possibility parameter\n");
        return 1;
    }
    double p = atof(argv[1]);
    if(p > 100){ printf("possibility must be 0 <= p <= 100\n"); return -1;}
    pCom p0 = sh_mem_create_super(SHM_KEY, SEM_KEY,SIZE);
    printf("p0->shmid is : %d\n",p0->shmid);

	char *args[]={"./enc1",argv[1],NULL};
	make_process(args);
    int i;
    sh_mem_init(p0);
    char *str,*str2;
    int num_mes = 0;
    while(p0->exit == F){
        str = scan_space(&i);
        str2 = sh_mem_wr1(p0,str);
        num_mes++;
        free(str);
        if(str2 != NULL)
            printf("Message from p2  : %s\n",str2);
        free(str2);
    }
    free(p0);

    wait_proc();
    printf("end of p1\n");
    printf("p1 : Num of sented messages = %d \n",num_mes);
	return 0;
}