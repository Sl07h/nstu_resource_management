// Составить программу, моделирующую работу Shell-интерпретатора при обработке командной строки,
//  указанной в варианте. При реализации программы путём выдачи сообщений информировать обо всех
// этапах ее работы (создан процесс, выполнение команды закончено и т.д.). Вариант 6:
// ls -lisa | sort | wc -l > a.txt

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>



int main()
{
    int fd1[2], fd2[2];
    if (pipe(fd1) == -1)
    {
        printf("Ошибка создания канала №1\n");
        exit(-1);
    }
    if (pipe(fd2) == -1)
    {
        printf("Ошибка создания канала №2\n");
        exit(-1);
    }

    pid_t pidP0 = getpid();
    printf("P0 (%d):\tСоздание каналов №1 и №2\n", pidP0);
    pid_t pidP1 = fork();
    pid_t pidP2 = 0;
    pid_t pidP;
    int returnStatus;

    if (pidP1 == 0) {
        pidP = 1;
        pidP1 = getpid();
        pidP2 = fork();
        if (pidP2 == 0) {
            pidP = 2;
            pidP2 = getpid();
        }
    }
    else {
        pidP = 0;
    }


    switch (pidP)
    {
        // Процесс P0
        case 0: {
            printf("P0 (%d):\tНачало процесса\n", pidP0);
            printf("P0 (%d):\tПроцесс завершён\n", pidP0);

            close(fd1[0]);
            close(fd2[0]);
            close(fd2[1]);
            close(1);
            dup(fd1[1]);
            close(fd1[1]);

            if (execl("/bin/ls", "ls", "-lisa", 0) < 0)
                exit(-1);
            else
                exit(0);            
            
        }break;


        // Процесс P1
        case 1: {
            printf("P1 (%d):\tНачало процесса\n", pidP1);
            printf("P1 (%d):\tЖду завершения процесса P0\n", pidP1);
            int status;
            waitpid(pidP0, &status, 0);
            printf("P1 (%d):\tДождался завершения процесса P0\n", pidP1);
            printf("P1 (%d):\tПроцесс завершён\n", pidP1);

            close(fd1[1]);
            close(0);
            dup(fd1[0]);
            close(fd1[0]);

            close(fd2[0]);
            close(1);
            dup(fd2[1]);
            close(fd2[1]);

            if( execl("/bin/sort", "sort", 0) < 0)
                exit(-1);
            else
                exit(0);
        }break;


        // Процесс P2
        case 2: {
            printf("P2 (%d):\tНачало процесса\n", pidP2);
            printf("P2 (%d):\tЖду завершения процесса P2\n", pidP2);
            int status;
            waitpid(pidP1, &status, 0);
            printf("P2 (%d):\tДождался завершения процесса P1\n", pidP2);
            printf("P2 (%d):\tПроцесс завершён\n", pidP2);

            close(fd1[0]);
            close(fd1[1]);

            close(fd2[1]);
            close(0);
            dup(fd2[0]);
            close(fd2[0]);

            int fd = open("a.txt",  O_CREAT|O_RDWR|O_TRUNC, 0644);
            close(1);
            dup(fd);
            close(fd);

            if (execl("/bin/wc", "wc", "-l", 0) < 0)
                exit(-1);
            else
                exit(0);

        }break;
    }
    return 0;
}