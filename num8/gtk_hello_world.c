#include <gtk/gtk.h>

// 버튼 클릭 시 호출되는 콜백 함수
void on_button_clicked(GtkWidget *widget, gpointer data) {
    g_print("Hello, World!\n");
}

int main(int argc, char *argv[]) {
    GtkWidget *window;    // 창 위젯
    GtkWidget *button;    // 버튼 위젯

    // GTK 초기화
    gtk_init(&argc, &argv);

    // 창 생성
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Hello World");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);

    // 창 닫기 버튼 이벤트 처리
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 버튼 생성
    button = gtk_button_new_with_label("Click Me!");

    // 버튼 클릭 이벤트 처리
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);

    // 버튼을 창에 추가
    gtk_container_add(GTK_CONTAINER(window), button);

    // 창 표시
    gtk_widget_show_all(window);

    // GTK 메인 루프 실행
    gtk_main();

    return 0;
}
