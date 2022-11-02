#include <ctype.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <semaphore.h>
#include "utils.h"
#include "process_list.h"
#include "mergesort.h"

//Iannone Bruno
void process_list()
{
    // Apertura semafori e shared memory
    sem_t *shm1_sem = sem_open(SHM1_SEM, O_CREAT, 0600);

    if (shm1_sem == SEM_FAILED)
    {
        handle_error("sem_open filled");
    }
    sem_t *shm2_sem = sem_open(SHM2_SEM, O_CREAT, 0600);

    if (shm2_sem == SEM_FAILED)
    {
        handle_error("sem_open filled");
    }
    if (sem_wait(shm2_sem) < 0)
    {
        handle_error("stat_manager.c: Errore nella post");
    }
    int shm_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0660);
    if (shm_fd < 0)
    {
        handle_error("process_list.c: Errore nella shm_open");
    }
    int *shm_ptr;
    if ((shm_ptr = (int *)mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0)) == MAP_FAILED)
    {
        exit(EXIT_FAILURE);
        handle_error("process_list.c: errore nella mmap ");
    }

    // Scansione processi
    int flag;
    printf("0: ordinare per CpuUsage, 1: ordinare per MemUsage?\nRisposta: ");
    scanf("%d", &flag);

    int gnome_pid = getppid();
    char *cgnome_pid = malloc(sizeof(int));
    if (string_to_int(cgnome_pid, gnome_pid) < 0)
    {
        handle_error("process_list.c: Errore nella conversione del pid");
    }
    printf("%s %d\n", cgnome_pid, gnome_pid);
    free(cgnome_pid);
    ListHead *head = List_init();

    FILE *fileMemInfo; // file per recuperare la memoria totale

    FILE *fileCpuInfo; // file per recuperare la CPU totale

    int memTot; // memoria totale (kb)

    float cpuTot; // totale tempo CPU

    struct dirent *pDs; // puntatore alla direct struct

    DIR *directory;
    int i = 0;
    while (1)
    {
        i++;
        directory = opendir("/proc/"); // entro in proc

        if (directory)
        {

            memTot = getTotalMemory(fileMemInfo);

            cpuTot = getTotalCpu(fileCpuInfo);

            // printf("Cpu used: %f", cpuTot);

            pDs = readdir(directory);

            while (pDs)
            {
                struct_process *s_process = struct_process_init(head);

                // escludo le cartelle "." e ".." e controllo che siano delle cartelle con nome un numero
                if (((strcmp(pDs->d_name, ".") == 0 || strcmp(pDs->d_name, "..") == 0)))
                {
                    // do nothing
                }
                else if ((pDs->d_type == DT_DIR) && (isNumber(pDs->d_name) == 1))
                {

                    if (getPidandName(pDs, s_process) == -1)
                    {
                        goto E; // Con la goto saltiamo direttamente alla procedura di cleanup
                    }
                    getUsedMemory(pDs, memTot, s_process);
                    getUsedCpu(pDs, cpuTot, s_process);

                    List_insert(s_process, &head);
                }

                pDs = readdir(directory);
            }

        E:
            closedir(directory);

            MergeSort(&head->first, flag); // Sorting dei risultati trovati
            List_print(head);              // Stampa della lista ordinata
            List_cleaner(head);            // Deallocazione lista

            // Controllo cicliclo della shared memory per terminare
            if (sem_wait(shm1_sem) < 0)
            {
                handle_error("stat_manager.c: Errore nella post");
            }
            if (shm_ptr[0] == 9)
            {
                free(head);
                shm_ptr[1] = gnome_pid;
                if (sem_post(shm2_sem) < 0)
                {
                    handle_error("stat_manager.c: Errore nella post");
                }

                printf("Exiting...\n");

                if (sem_post(shm1_sem) < 0)
                {
                    handle_error("stat_manager.c: Errore nella post");
                }
                //Cleanup
                if (sem_close(shm1_sem) < 0)
                {
                    handle_error("process_list.c: Errore nella close di shm1_sem");
                }
                if (sem_close(shm2_sem) < 0)
                {
                    handle_error("process_list.c: Errore nella close di shm2_sem");
                }
                return;
            }
            if (sem_post(shm1_sem) < 0)
            {
                handle_error("stat_manager.c: Errore nella post");
            }

            sleep(5); //Un po' di leggibilità...
        }
    }
}