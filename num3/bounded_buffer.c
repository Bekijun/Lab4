#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5 // 제한 버퍼 크기

int buffer[BUFFER_SIZE];
int count = 0; // 현재 버퍼에 저장된 항목 개수
int in = 0;    // 다음에 삽입할 위치
int out = 0;   // 다음에 제거할 위치

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;

// 생산자 함수
void* producer(void* arg) {
    int id = *(int*)arg;
    while (1) {
        int item = rand() % 100; // 생산 항목 생성
        pthread_mutex_lock(&mutex);

        while (count == BUFFER_SIZE) {
            // 버퍼가 가득 찼을 때 대기
            pthread_cond_wait(&not_full, &mutex);
        }

        // 항목 삽입
        buffer[in] = item;
        printf("Producer %d: Produced %d at %d\n", id, item, in);
        in = (in + 1) % BUFFER_SIZE;
        count++;

        // 소비자를 깨움
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);

        sleep(rand() % 2); // 생산자 대기 (시뮬레이션)
    }
    return NULL;
}

// 소비자 함수
void* consumer(void* arg) {
    int id = *(int*)arg;
    while (1) {
        pthread_mutex_lock(&mutex);

        while (count == 0) {
            // 버퍼가 비었을 때 대기
            pthread_cond_wait(&not_empty, &mutex);
        }

        // 항목 소비
        int item = buffer[out];
        printf("Consumer %d: Consumed %d from %d\n", id, item, out);
        out = (out + 1) % BUFFER_SIZE;
        count--;

        // 생산자를 깨움
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);

        sleep(rand() % 3); // 소비자 대기 (시뮬레이션)
    }
    return NULL;
}

int main() {
    pthread_t producers[2], consumers[2];
    int producer_ids[2] = {1, 2};
    int consumer_ids[2] = {1, 2};

    // 생산자 쓰레드 생성
    for (int i = 0; i < 2; i++) {
        pthread_create(&producers[i], NULL, producer, &producer_ids[i]);
    }

    // 소비자 쓰레드 생성
    for (int i = 0; i < 2; i++) {
        pthread_create(&consumers[i], NULL, consumer, &consumer_ids[i]);
    }

    // 쓰레드 조인 (사실 무한 루프이므로 종료되지 않음)
    for (int i = 0; i < 2; i++) {
        pthread_join(producers[i], NULL);
        pthread_join(consumers[i], NULL);
    }

    return 0;
}