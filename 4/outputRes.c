#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
const float pi = 3.14159265358979323846;

int main(int argc, char *argv[]) {

    char *end;
    int K = strtol(argv[1], &end, 10), N = strtol(argv[2], &end, 10);

    // Открываем именованный канал
    char * myfifo = "/tmp/myfifo";
    int fd = open(myfifo, O_RDONLY);
    float number, x;

    printf("\ni\tx\t\tf(x)\t\tsin(x)\n");
    // Получаем значения от процесса-предка по каналу
    for (int i = 0; i <= N; ++i) {
        read(fd, &number, sizeof(float));
        x = pi * i / N;
        printf("%d\t%f\t%f\t%f\n", i, x, number, sin(x));
    }

    close(fd);
    return 0;
}