 // Christoforos Kladis 1115201600068
//General functions

#ifndef gfunctions_H
#define gfunctions_H


int make_process(char** args);

void wait_proc();

char* get_checksum(const char* str);

void random_change(char** pstr,double arg,int base);

char* scan_space(int *check);

#endif
