//Viktoriya Petrova
//CS333
//Lab 6 Part A2
//2022-03-08

//A solution to the producer/consumer problem using mutex, conditional statements, and pthreads.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty, fill = PTHREAD_COND_INITIALIZER;
#define BUFF_SIZE 257

typedef struct{
    char buffer[BUFF_SIZE];
    int flag;
    int num_files;
}myarg_t;

//Retrieve the data that needs to be loaded into the buffer from the external files.
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

//Producer thread function. Calls on put() and loads the buffer.
void *producer(void *arg){
    myarg_t *args = (myarg_t *) arg;
    char file_content[257];
    for(int i = 0; i < args -> num_files; i++){
        pthread_mutex_lock(&lock);
        while(args -> flag == 1){
            pthread_cond_wait(&empty, &lock);
        }
        put(i, file_content);
        strcpy(args -> buffer,file_content);
        args -> flag = 1;
        pthread_cond_signal(&fill);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

//Consumer thread function. Prints out the buffer content.
void *consumer(void *arg){
    myarg_t *args = (myarg_t *) arg;
    for(int i = 0; i < args -> num_files; i++){
        pthread_mutex_lock(&lock);
        while(args -> flag == 0){
            pthread_cond_wait(&fill, &lock);
        }
        printf("%s\n", args -> buffer);
        args -> flag = 0;
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&lock);
    }
    return NULL;

}

int main(int argc, char *argv[]){
    pthread_t p,c;
    int rc = 0;
    myarg_t args = {{0}, 0, 10};

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
        printf("ERROR: pthred_join(%d)\n", rc);
        exit(1);
    }
    rc = pthread_join(c,NULL);
    if(rc){
        printf("ERROR: pthred_join(%d)\n", rc);
        exit(1);
    }
    exit(0);
}
