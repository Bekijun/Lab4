#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_get(int client_sock, char *resource) {
    FILE *file = fopen(resource, "r");
    if (file == NULL) {
        // 404 Not Found 응답
        const char *not_found_response = 
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 23\r\n"
            "\r\n"
            "<h1>404 Not Found</h1>";
        send(client_sock, not_found_response, strlen(not_found_response), 0);
        return;
    }

    // 200 OK 응답
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response), 
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n\r\n");
    send(client_sock, response, strlen(response), 0);

    // 파일 내용 전송
    char file_buffer[BUFFER_SIZE];
    while (fgets(file_buffer, sizeof(file_buffer), file)) {
        send(client_sock, file_buffer, strlen(file_buffer), 0);
    }
    fclose(file);
}

void handle_post(int client_sock, char *body) {
    char response[BUFFER_SIZE];
    char output[BUFFER_SIZE];
    FILE *pipe;

    // CGI 프로그램 실행
    pipe = popen("./cgi_program", "w+");
    if (pipe == NULL) {
        perror("popen");
        const char *error_response = 
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 27\r\n"
            "\r\n"
            "<h1>500 Internal Error</h1>";
        send(client_sock, error_response, strlen(error_response), 0);
        return;
    }

    // POST 데이터 CGI 프로그램에 전달
    fprintf(pipe, "%s", body);
    fflush(pipe);

    // CGI 프로그램 출력 읽기
    size_t output_len = fread(output, 1, sizeof(output) - 1, pipe);
    output[output_len] = '\0';
    pclose(pipe);

    // CGI 프로그램 출력 클라이언트에 전송
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "Content-Length: %zu\r\n"
             "\r\n"
             "%s",
             strlen(output), output);

    send(client_sock, response, strlen(response), 0);
}


void handle_client(int client_sock) {
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        close(client_sock);
        return;
    }

    buffer[bytes_received] = '\0';
    printf("Request:\n%s\n", buffer);

    char method[8], resource[64], protocol[16];
    sscanf(buffer, "%s %s %s", method, resource, protocol);

    // GET 요청 처리
    if (strcmp(method, "GET") == 0) {
        if (strcmp(resource, "/") == 0) {
            strcpy(resource, "index.html");
        } else {
            memmove(resource, resource + 1, strlen(resource)); // 앞의 '/' 제거
        }
        handle_get(client_sock, resource);
    }
    // POST 요청 처리
    else if (strcmp(method, "POST") == 0) {
        char *body = strstr(buffer, "\r\n\r\n");
        if (body != NULL) {
            body += 4; // 헤더와 본문 분리
            handle_post(client_sock, body);
        }
    }

    close(client_sock);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // 서버 소켓 생성
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // 주소와 포트 바인딩
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // 연결 대기
    if (listen(server_sock, 10) == -1) {
        perror("listen");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d...\n", PORT);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_sock == -1) {
            perror("accept");
            continue;
        }

        handle_client(client_sock);
    }

    close(server_sock);
    return 0;
}
