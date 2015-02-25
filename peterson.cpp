#include <pthread.h>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

const int BUFFER_SIZE = 25;

string buffer[BUFFER_SIZE];
int in = 0, out = 0, count = 0;
int run_length;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c_cons = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_prod = PTHREAD_COND_INITIALIZER;

void *producer(void *param);
void *consumer(void *param);

main(int argc, char *argv[]){

        pthread_t tid1, tid2;
        int i;

        if(pthread_create(&tid1,NULL,producer,NULL) != 0){
                cout << "Unable to create producer.\n";
                exit(1);
        }
        if(pthread_create(&tid2,NULL,consumer,NULL) != 0){
                cout << "Unable to create consumer thread.\n";
                exit(1);
        }

        pthread_join(tid1,NULL);
        pthread_join(tid2,NULL);
        cout << "Parent quitting.\n";
}

void *producer(void *param){

    int index;
    int rNum = rand() % 10+1;
    
    while(1){
        pthread_mutex_lock(&m);
	
		if(count > BUFFER_SIZE)
            exit(1);

        while(count == BUFFER_SIZE)
          	pthread_cond_wait(&c_prod, &m);

        in = (in+1) % BUFFER_SIZE;
        count++;
        pthread_mutex_unlock(&m);
        pthread_cond_signal(&c_cons);

        printf("P: %d\n", index);
        index = out;

        for(int i=0; i < count; i++){
            printf("%s", buffer[index].c_str());
            index = (index+1) % BUFFER_SIZE;
        }
    	sleep(rNum);
	}
    cout << "Producer quitting.\n";
}

void *consumer(void *param){

    int index;
    int rNum = rand() % 10+1;
    
    while(1){
        pthread_mutex_lock(&m);
        if(count < 0)
            exit(1);

        while(count == 0)
            pthread_cond_wait(&c_cons, &m);

        out = (out + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&m);
        count--;
        pthread_cond_signal(&c_prod);

        printf("C: %d\n", index);
        index = out;
    
        for(int i=0; i < count; i++){
            printf("%s", buffer[index].c_str());
            index = (index+1) % BUFFER_SIZE;
        }
 		sleep(rNum);
    }
    cout << "Consumer quitting.\n";
}
