#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define MAX_CLIENTS 5
#define MESSAGE_SIZE 256

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t message_ready = PTHREAD_COND_INITIALIZER;

char message[MESSAGE_SIZE]; // 공유 메시지 버퍼
int new_message = 0;        // 메시지 상태 플래그
int client_count = 0;       // 등록된 클라이언트 쓰레드 수

void* server_thread(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        // 메시지가 준비될 때까지 대기
        while (new_message == 0) {
            pthread_cond_wait(&message_ready, &mutex);
        }

        // 메시지를 방송
        printf("[SERVER] Broadcasting message: %s\n", message);
        new_message = 0; // 메시지 처리 완료 상태로 변경

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* client_thread(void* arg) {
    int id = *(int*)arg;

    while (1) {
        char client_message[MESSAGE_SIZE];
        snprintf(client_message, MESSAGE_SIZE, "Hello from client %d", id);

        // 메시지 전송
        pthread_mutex_lock(&mutex);

        // 메시지를 공유 버퍼에 작성
        strncpy(message, client_message, MESSAGE_SIZE);
        new_message = 1; // 메시지가 준비되었음을 알림
        printf("[CLIENT %d] Sent message: %s\n", id, client_message);

        pthread_cond_signal(&message_ready); // 서버에게 알림
        pthread_mutex_unlock(&mutex);

        sleep(rand() % 3 + 1); // 대기 시간
    }
    return NULL;
}

int main() {
    pthread_t server;
    pthread_t clients[MAX_CLIENTS];
    int client_ids[MAX_CLIENTS];

    // 서버 쓰레드 생성
    pthread_create(&server, NULL, server_thread, NULL);

    // 클라이언트 쓰레드 생성
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_ids[i] = i + 1;
        pthread_create(&clients[i], NULL, client_thread, &client_ids[i]);
        client_count++;
    }

    // 클라이언트 쓰레드 대기
    for (int i = 0; i < MAX_CLIENTS; i++) {
        pthread_join(clients[i], NULL);
    }

    // 서버 쓰레드 대기 (종료되지 않는 무한 루프이므로 실행 중단 시까지 동작)
    pthread_join(server, NULL);

    return 0;
}
