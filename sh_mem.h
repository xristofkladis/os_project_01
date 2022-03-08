// Christoforos Kladis 1115201600068

#ifndef shmem_h
#define shmem_h

#include <errno.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE      1024
#define CHECKSUM_SIZE 32

typedef enum { F, T } boolean;

union semun {
    int val; // val for SETVAL
    struct semid_ds *buff; // Buffer for IPC_STAT and IPC_SET
    short *array; // Buffer for GETALL and SETALL
};

typedef struct sh_mem * pSh_mem;

typedef struct sh_mem {
    char str[BUF_SIZE];
    char checksum[CHECKSUM_SIZE];
}sh_mem;

typedef struct com * pCom;

typedef struct com{
	int shmid;
	int semid;
	boolean exit;
}com;

pCom sh_mem_create(key_t shm_key,key_t sem_key,int size);

pCom sh_mem_create_super(key_t shm_key,key_t sem_key,int size);

int sh_mem_init(pCom p0);

void shm_write_checksum(pCom p0,const char* str,char* checksum);
char** shm_read_checksum(pCom p0);

void shm_write(pCom p0,const char* str);
char* shm_read(pCom p0);

char* sh_mem_read(pCom p0);

char** sh_mem_read_checksum(pCom p0);

char** sh_mem_wr1_checksum(pCom p0,char* mes,char* checksum);
char** sh_mem_wr2_checksum(pCom p0,char* mes,char* checksum);

char* sh_mem_wr1(pCom p0,char* mes);
char* sh_mem_wr2(pCom p0,char* mes);

void delete_sh_mem(pCom p0);

void delete_str(char*** pstr);

void print_tt(char** t);

#endif