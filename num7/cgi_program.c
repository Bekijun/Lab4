#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char input[256];
    char name[128] = {0};
    char age[128] = {0};

    // 표준 입력으로부터 데이터 읽기
    if (fgets(input, sizeof(input), stdin) != NULL) {
        // POST 데이터 파싱
        char *name_ptr = strstr(input, "name=");
        char *age_ptr = strstr(input, "age=");

        if (name_ptr) {
            name_ptr += 5; // "name=" 이후의 값
            char *end = strchr(name_ptr, '&');
            if (end) *end = '\0'; // '&'로 끝나는 경우 자르기
            strncpy(name, name_ptr, sizeof(name) - 1);
        }

        if (age_ptr) {
            age_ptr += 4; // "age=" 이후의 값
            strncpy(age, age_ptr, sizeof(age) - 1);
        }
    }

    // HTML 응답 생성
    printf("Content-Type: text/html\r\n\r\n");
    printf("<html><body>");
    printf("<h1>CGI Program Output</h1>");
    if (strlen(name) > 0 && strlen(age) > 0) {
        printf("<p>Name: %s</p>", name);
        printf("<p>Age: %s</p>", age);
    } else {
        printf("<p>Error: Missing or invalid input data</p>");
    }
    printf("</body></html>");

    return 0;
}
