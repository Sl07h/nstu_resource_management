#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
const float pi = 3.14159265358979323846;


/**
 @function f
 @brief вычисляет sin(x) через ряд Тейлора (-1)^(k+1) * x^(2k-1) / (2k-1)!
 @param x - число, число членов ряда
 @return вещественный элемент
 */
float f(float x, int N) {

    float result = 0.0;
    float tmp = 0.0;
    for (int k = 1; k < N; ++k) {

        tmp = pow(x, (2 * k - 1));
        for (int i = 1; i <= 2 * k - 1; ++i)
            tmp /= i;

        if ((k + 1) % 2 == 0)
            result += tmp;
        else
            result -= tmp;
    }
    return result;
}



int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        printf("Инструкция:\n");
        printf("Запуск программы происходит командой ./lab41 K, N, где K - число членов ряда Тейлора, а N - количество интервалов от 0 до PI\n");
        printf("Если введено неверное число параметров (не равно 2), то выводится сообщение об ошибке.\n");
        return 0;
    }
    else if (argc > 3) {
        printf("Ошибка: слишком длинный список параметров. Должно быть два параметра.\n");
        return -1;
    }
    else if (argc < 3) {
        printf("Ошибка: слишком короткий список параметров. Должно быть два параметра.\n");
        return -1;
    }


    char *end;
    // K - число членов ряда Тейлора, а N - количество интервалов от 0 до PI
    int K = strtol(argv[1], &end, 10), N = strtol(argv[2], &end, 10);

    // Массив вычисленных значений синуса
    float array_f[N + 1];

    // Вычисляем sin(x), x = 0,PI
    for (int i = 0; i <= N; ++i) {
        float x = pi * i / N;
        array_f[i] = f(x,K);
    }


    if (fork() == 0) {
        // Порождение процесса-потомка
        if (execv("outputRes", argv) == -1){
            printf("Ошибка создания процесса потомка.\n");
            return -1;
        }
    }

    // Создаём именованный канал
    char * myfifo = "/tmp/myfifo"; 
    mkfifo(myfifo, O_WRONLY);
    int fd = open(myfifo, O_WRONLY);

    // и передаём по каналу дочернему процессу
    for (int i = 0; i <= N; ++i)
        write(fd, &array_f[i], sizeof(array_f[i]));

    close(fd);
    unlink(myfifo);

    return 0;
}
