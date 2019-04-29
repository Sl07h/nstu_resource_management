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

int pipeR, pipeW;

// Структура подготавливаемых данных
struct {
	int number;
	char message[128];
} data;

// Отправка данных основному процессу P0
void sendToPipe(int number, char *message) {
	if (strlen(message)+1>128)
	{
		printf("ОШИБКА. Сообщение больше 128 байт");
		exit(-1);
	}
	data.number = number;
	strcpy(data.message, message);
	write(pipeW, &data, sizeof(data));
}


int main()
{
	char string[128];
	int fd[2];
	if (pipe(fd) == -1)
	{
		printf("Ошибка создания канала\n");
		exit(-1);
	}

	pipeW = fd[1];
	pipeR = fd[0];
	pid_t pidP0 = getpid();
	pid_t pidP1 = fork();
	pid_t pidP2 = 0;
	pid_t pidP;
	printf("P0 (%d):\tСоздание каналов чтения и записи\n", pidP0);
	int returnStatus;

	if (pidP1 == 0) {
		close(pipeR);		
		pidP = 1;
		pidP1 = getpid();
		pidP2 = fork();
		if (pidP2 == 0) {
			pidP = 2;
			pidP2 = getpid();
		}
	}
	else{
		close(pipeW);
		pidP = 0;
	}


	switch (pidP)
	{
		// Процесс P0
		case 0: {
			printf("P0 (%d):\tСоздание P1\n", pidP0);
			printf("P0 (%d):\tОбработка данных из канала\n", pidP0);
			for (int i = 0; i < MESSAGES_COUNT; i++)
			{
				read(pipeR, &data, sizeof(data));
				printf("P0: %s\n", data.message);
			}
			printf("P0 (%d):\tОжидание завершения процессов\n", pidP0);
			wait(&returnStatus);
			printf("P0 (%d):\tПроцесс P1 завершился со статусом %d\n", pidP0, returnStatus);
		}break;


		// Процесс P1
		case 1: {
			close(pipeR);
			sprintf(string,"P1 (%d):\tСоздание P2", pidP1);
			sendToPipe(1,string);
			sleep(N_SECONDS);
			// Прерываем процесс P2
			kill(pidP2, SIGINT);
			wait(&returnStatus);
			sprintf(string,"P1 (%d):\tПроцесс P2 завершился со статусом %d", pidP1, returnStatus);
			sendToPipe(1,string);
		}break;


		// Процесс P2
		case 2: {
			sprintf(string, "P2 (%d):\tОткрытие большого файла", pidP2);
			sendToPipe(1,string);
			FILE *file_in = fopen("input.txt", "r");
			if (file_in == NULL)
			{
				sprintf(string,"P2 (%d):\tОшибка открытия файла", pidP2);
				sendToPipe(1,string);
				return -1;
			}
			else
			{
				sprintf(string, "P2 (%d):\tЧтение большого файла", pidP2);
				sendToPipe(1,string);
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