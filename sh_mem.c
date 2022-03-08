#include "sh_mem.h"

#define PERMS 0600

struct sembuf lock1   = {0, -1, SEM_UNDO };
struct sembuf unlock1 = {0,  1, SEM_UNDO };
struct sembuf lock2   = {1, -1, SEM_UNDO };
struct sembuf unlock2 = {1,  1, SEM_UNDO };

void sem1_lock  (pCom p0){semop(p0->semid,&lock1  ,1);}
void sem1_unlock(pCom p0){semop(p0->semid,&unlock1,1);}

void sem2_lock  (pCom p0){semop(p0->semid,&lock2  ,1);}
void sem2_unlock(pCom p0){semop(p0->semid,&unlock2,1);}

pCom sh_mem_create(key_t shm_key,key_t sem_key,int size){
    pCom p0 = malloc(sizeof(com));
    p0->shmid = shmget(shm_key, size, 0644|IPC_CREAT);
    p0->semid = semget(sem_key, 2, PERMS|IPC_CREAT);
    p0->exit = F; // p0->exit will be 1 if we give "TERM" as message
    if (p0->shmid < 0) {
        free(p0);
        perror("Shared memory");
        return NULL;
    }
    if (p0->semid < 0){
        free(p0);
        perror("semget: semget failed");
        return NULL;
    }
    return p0;
}

pCom sh_mem_create_super(key_t shm_key,key_t sem_key,int size){
    pCom p0 = malloc(sizeof(com));
    p0->shmid = shmget(shm_key, size, 0644|IPC_CREAT);
    p0->semid = semget(sem_key, 2, PERMS|IPC_CREAT);
    delete_sh_mem(p0);
    p0->shmid = shmget(shm_key, size, 0644|IPC_CREAT);
    p0->semid = semget(sem_key, 2, PERMS|IPC_CREAT);

    p0->exit = F; // p0->exit will be 1 if we give "TERM" as message

    if (p0->shmid < 0) {
        free(p0);
        perror("Shared memory");
        return NULL;
    }
    if (p0->semid < 0){
        free(p0);
        perror("semget: semget failed");
        return NULL;
    }
    return p0;
}

int sh_mem_init(pCom p0){
    if(p0 == NULL){
        perror("pCom");
        return -1;
    }
    union semun arg;
    // init semaphore
    semctl(p0->semid,0,SETVAL,arg);
    semctl(p0->semid,1,SETVAL,arg);

    return 0;
}

//--------------------------------------------
char* sh_mem_read(pCom p0){
    sem1_lock(p0);
    char* str = shm_read(p0);
    if(strcmp(str,"TERM")==0){ p0->exit = T; }
    return str;
}

char** sh_mem_read_checksum(pCom p0){
    sem1_lock(p0);
    char** str = shm_read_checksum(p0);
    if(strcmp(str[0],"TERM")==0){ p0->exit = T; }
    return str;
}
//--------------------------------------------

/* read and write in sh_mem shared memory*/

void shm_write_checksum(pCom p0,const char* mes,char* checksum){
    if(p0 == NULL){ perror("pCom"); return; }
    pSh_mem mem = shmat(p0->shmid,(void*)0,0);
    strcpy(mem->str,mes);
    if(checksum != NULL){ strcpy(mem->checksum,checksum); }
    else { strcpy(mem->checksum,"\0"); }    // des edw
    //detach from shared memory 
    shmdt(mem);
}

char** shm_read_checksum(pCom p0){
    if (p0 == NULL) { perror("pCom"); return NULL;}
    pSh_mem mem = shmat(p0->shmid,(void*)0,0);
    char** str = malloc(sizeof(char*)*2);
    str[0] = malloc(sizeof(char)*(strlen(mem->str)+1));
    strcpy(str[0],mem->str);

    if(strcmp(mem->checksum,"\0")!=0){
        str[1] = malloc(sizeof(char)*(strlen(mem->checksum)+1));
        strcpy(str[1],mem->checksum);
    }
    else{ str[1] = NULL;}
    shmdt(mem);             //detach from shared memory
    return str;
}

//-------------------------------------------------------

char** sh_mem_wr1_checksum(pCom p0,char* mes,char* checksum){
    if(mes == NULL){ return NULL; }
    shm_write_checksum(p0,mes,checksum);
    sem1_unlock(p0);                //unlock semaphore 1
    if(strcmp(mes,"TERM")==0)  {   p0->exit = T; return NULL; }
    sem2_lock(p0);                  //lock   semaphore 2
    char** str = shm_read_checksum(p0);
    if(strcmp(str[0],"TERM")==0) { p0->exit = T; }
    return str;
}

char** sh_mem_wr2_checksum(pCom p0,char* mes,char* checksum){
    if(mes == NULL){ return NULL; }
    shm_write_checksum(p0,mes,checksum);
    sem2_unlock(p0);                //unlock semaphore 1
    if(strcmp(mes,"TERM")==0)  {   p0->exit = T; return NULL; }
    sem1_lock(p0);                  //lock   semaphore 2
    char** str = shm_read_checksum(p0);
    if(strcmp(str[0],"TERM")==0) { p0->exit = T; }
    return str;
}

/* read and write in simple string shared memory*/
void shm_write(pCom p0,const char* str){
    if(p0 == NULL){ perror("pCom"); return; }
    char* messsage = shmat(p0->shmid,(void*)0,0);
    strcpy(messsage,str);
    shmdt(messsage);        //detach from shared memory 
}

char* shm_read(pCom p0){
    if (p0 == NULL) { perror("pCom"); return NULL;}
    char* message = shmat(p0->shmid,(void*)0,0);
    char* res = malloc(sizeof(char)*(strlen(message)+1));
    strcpy(res,message);
    shmdt(message);         //detach from shared memory 
    return res;
}
/*----------------------------------------------*/

char* sh_mem_wr1(pCom p0,char* mes){
    if(mes == NULL){ return NULL; }     // if mem = NULL return NULL
    shm_write(p0,mes);
    sem1_unlock(p0);                //unlock semaphore 1
    if(strcmp(mes,"TERM")==0){ p0->exit = T; return NULL; }
    sem2_lock(p0);                  //lock   semaphore 2
    char* str = shm_read(p0);
    if(strcmp(str,"TERM")==0)   { p0->exit = T; }
    return str;
}

char* sh_mem_wr2(pCom p0,char* mes){
    if(mes == NULL){ return NULL; }     // if mem = NULL return NULL
    shm_write(p0,mes);
    sem2_unlock(p0);                //unlock semaphore 2
    if(strcmp(mes,"TERM")==0){ p0->exit = T; return NULL; }
    sem1_lock(p0);                  //lock   semaphore 1
    char* str = shm_read(p0);
    if(strcmp(str,"TERM")==0)   { p0->exit = T; }
    return str;
}

void delete_sh_mem(pCom p0){
    if (p0 == NULL) {
        perror("pCom");
        return ;
    }
    shmctl(p0->shmid,IPC_RMID,NULL);
    semctl(p0->semid,0,IPC_RMID,0);
    semctl(p0->semid,1,IPC_RMID,0);
}

void delete_str(char*** pstr){
    if(*pstr == NULL)
        return;
    free(*pstr[0]);
    free(*pstr[1]);
    free(*pstr);
    *pstr = NULL;
}

void print_tt(char** t){
    if(t == NULL)
        printf("t is NULL\n");
    else{
        printf("\nt[0] = %s",t[0]);
        printf("\nt[1] = %s\n",t[1]);
    }
}