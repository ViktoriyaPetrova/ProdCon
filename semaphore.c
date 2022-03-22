//Viktoriya Petrova
//CS333
//Lab 6 Part A3
//2022-03-08

//A solution to the producer/consumer problem using semaphores and pthreads.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

sem_t empty;
sem_t full;
#define BUFF_SIZE 257

typedef struct{
    char buffer[BUFF_SIZE];
    int flag;
    int num_files;
}myarg_t;

void put(int i, char file_content[257]){
    if(i > 9){
        printf("ERROR: Invalid text file.\n");
        exit(1);
    }

    FILE *fp;
    long size;
    char file_name[8];

    sprintf(file_name,"in%d.txt", i);

    fp = fopen(file_name, "rb");
    if(!fp){
        printf("ERROR: Could not open file.\n");
        exit (1);
    }
    if(fseek(fp, 0, SEEK_END) != 0){
        printf("ERROR: fseek()\n");
        exit(1);
    }
    size = ftell(fp);
    rewind(fp);

    if(size < 257){
        if(fread(file_content, 1, size, fp) < size){
            printf("ERROR: fread()\n");
            exit(1);
        }
    }
    else{
        fread(file_content, 1, 256, fp);
        size = 256;
    }
    fclose(fp);
    file_content[size] = '\0';
}

void *producer(void *arg){
    myarg_t *args = (myarg_t *) arg;
    char file_content[257];
    for(int i = 0; i < args -> num_files; i++){
        sem_wait(&empty);
        put(i, file_content);
        strcpy(args -> buffer,file_content);
        sem_post(&full);
    }
    return NULL;
}

void *consumer(void *arg){
    myarg_t *args = (myarg_t *) arg;
    for(int i = 0; i < args -> num_files; i++){
        sem_wait(&full);
        printf("%s\n", args -> buffer);
        sem_post(&empty);
    }
    return NULL;

}

int main(int argc, char *argv[]){
    pthread_t p,c;
    int rc = 0;
    myarg_t args = {{0}, 0, 10};
    sem_init(&empty, 0, 1);
    sem_init(&full, 0, 0);

    rc = pthread_create(&p, NULL, producer, &args);
    if(rc){
        printf("ERROR:pthread_create(%d)\n", rc);
        exit(1);
    }
    rc = pthread_create(&c, NULL, consumer, &args);
    if(rc){
        printf("ERROR:pthread_create(%d)\n", rc);
        exit(1);
    }
    rc = pthread_join(p,NULL);
    if(rc){
        printf("ERROR: pthread_join(%d)\n", rc);
        exit(1);
    }
    rc = pthread_join(c,NULL);
    if(rc){
        printf("ERROR: pthread_join(%d)\n", rc);
        exit(1);
    }
    sem_destroy(&empty);
    sem_destroy(&full);

    exit(0);
}
