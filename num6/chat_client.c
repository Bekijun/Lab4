#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 3490
#define BUFFER_SIZE 256

int sock;

void *receive_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    int valread;

    while (1) {
        valread = read(sock, buffer, BUFFER_SIZE - 1);
        if (valread > 0) {
            buffer[valread] = '\0';
            printf("Message from server: %s", buffer);
        } else if (valread == 0) {
            printf("Server disconnected.\n");
            close(sock);
            exit(EXIT_SUCCESS);
        } else {
            perror("recv error");
        }
    }
}

int main() {
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    pthread_t recv_thread;

    // 소켓 생성
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        exit(EXIT_FAILURE);
    }

    // 서버에 연결
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the chat server.\n");

    // 메시지 수신용 쓰레드 생성
    pthread_create(&recv_thread, NULL, receive_messages, NULL);

    // 사용자 입력 처리
    while (1) {
        fgets(buffer, BUFFER_SIZE, stdin);
        send(sock, buffer, strlen(buffer), 0);
    }

    return 0;
}
