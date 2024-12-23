#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 3490
#define MAX_CLIENTS 10
#define BUFFER_SIZE 256

int main() {
    int server_sock, client_sock, max_fd, activity, valread;
    int client_socks[MAX_CLIENTS] = {0};
    struct sockaddr_in server_addr, client_addr;
    fd_set readfds;
    char buffer[BUFFER_SIZE];
    socklen_t addrlen = sizeof(client_addr);

    // 서버 소켓 생성
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 소켓과 주소 바인딩
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // 연결 대기
    if (listen(server_sock, MAX_CLIENTS) < 0) {
        perror("listen failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Chat server is running on port %d\n", PORT);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_sock, &readfds);
        max_fd = server_sock;

        // 클라이언트 소켓 추가
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_socks[i] > 0) {
                FD_SET(client_socks[i], &readfds);
            }
            if (client_socks[i] > max_fd) {
                max_fd = client_socks[i];
            }
        }

        // 활동 감지
        activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("select error");
            exit(EXIT_FAILURE);
        }

        // 새로운 클라이언트 연결 처리
        if (FD_ISSET(server_sock, &readfds)) {
            if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addrlen)) < 0) {
                perror("accept error");
                exit(EXIT_FAILURE);
            }

            printf("New connection, socket fd: %d, IP: %s, port: %d\n",
                   client_sock, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            // 클라이언트 소켓 배열에 추가
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_socks[i] == 0) {
                    client_socks[i] = client_sock;
                    break;
                }
            }
        }

        // 기존 클라이언트로부터 메시지 처리
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_socks[i] > 0 && FD_ISSET(client_socks[i], &readfds)) {
                valread = read(client_socks[i], buffer, BUFFER_SIZE);
                if (valread == 0) {
                    // 클라이언트 연결 종료
                    printf("Client disconnected, socket fd: %d\n", client_socks[i]);
                    close(client_socks[i]);
                    client_socks[i] = 0;
                } else {
                    buffer[valread] = '\0';
                    printf("Client %d sent: %s", client_socks[i], buffer);

                    // 메시지 방송
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (client_socks[j] > 0 && client_socks[j] != client_socks[i]) {
                            send(client_socks[j], buffer, strlen(buffer), 0);
                        }
                    }
                }
            }
        }
    }

    return 0;
}
