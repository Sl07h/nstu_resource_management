#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>

const int BUF_SIZE = 128;
const int N_SECONDS = 1;
const int MESSAGES_COUNT = 4;


// Структура подготавливаемых данных
struct {
	int number;
	char message[40];
} data;



int main()
{
	pid_t pidP0 = getpid();
	pid_t pidP1 = fork();
	pid_t pidP2 = 0;
	pid_t pidP;
	printf("P0 (%d):\tСоздание каналов чтения и записи\n", pidP0);
	int returnStatus, count;

	if (pidP1 == 0) {
		pidP = 1;
		pidP1 = getpid();
		pidP2 = fork();
		if (pidP2 == 0) {
			pidP = 2;
			pidP2 = getpid();
		}
	}
	else
		pidP = 0;


	switch (pidP)
	{
		// Процесс P0
		case 0: {
			printf("P0 (%d):\tСоздание P1\n", pidP0);
			//sigpause(SIGUSR1);
			printf("P0 (%d):\tОбработка данных из канала\n", pidP0);
			printf("P0 (%d):\tОжидание завершения процессов\n", pidP0);
			wait(&returnStatus);
			printf("P0 (%d):\tПроцесс P1 завершился со статусом %d\n", pidP0, returnStatus);
		}break;

		// Процесс P1
		case 1: {
			printf("P1 (%d):\tСоздание P2\n", pidP1);
			sleep(N_SECONDS);
			// Прерываем процесс P2
			kill(pidP2, SIGINT);
			wait(&returnStatus);
			printf("P1 (%d):\tПроцесс P2 завершился со статусом %d \n", pidP1, returnStatus);

		}break;

		// Процесс P2
		case 2: {

			printf("P2 (%d):\tОткрытие большого файла\n", pidP2);
			FILE *file_in = fopen("input.txt", "r");
			if (file_in == NULL)
			{
				printf("P2 (%d):\tОшибка открытия файла\n", pidP2);
				return -1;
			}
			else
			{
				printf("P2 (%d):\tЧтение большого файла\n", pidP2);
				char *string = NULL;
				while (fgets(string, BUF_SIZE, file_in) != -1);
				return 0;
			}

		}break;

		default:
		printf("Ошибка процесса\n");
		break;
	}
}