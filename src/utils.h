#pragma once
#include <stdio.h>
#include <dirent.h>
#include "linked_list.h"

//Define per nomi della shared memory e i semafori
#define SHM_NAME  "SHM"
#define SHM_SIZE  sizeof(int)
#define SHM1_SEM "SEM1"
#define SHM2_SEM "SEM2"

int isNumber(char *str);
int getTotalMemory(FILE *fileMemInfo);
void getUsedMemory(struct dirent *pDsCopy, int totMem, struct_process* s_process);
float getTotalCpu(FILE *fileCpuInfo);
void getUsedCpu(struct dirent *pDsCopy, int cpuTot, struct_process* s_process);
int getPidandName(struct dirent *pDsCopy, struct_process* s_process);
void handle_error(char* msg);
int string_to_int(char* dest, int src);
