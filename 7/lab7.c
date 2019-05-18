#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <errno.h>
#define LENGTH 64
#define K 5

struct msg {
	long mtype;
	char mtext[LENGTH];
};


char table[K][LENGTH];


// Вывод таблицы на экран
void printTable()
{
	printf("Таблица:\n");
	for (size_t i = 0; i < K; i++)
		printf("%d:\t%s\n", i, table[i]);
}


// Замена строки по команде от процесса-потомка
void replaceTableLine(char *s){

	int i =0;
	int number = 0;
	int len = strlen(s);
	while (isdigit(s[i]) && i<len)
	{
		number*=10;
		number+=s[i] - '0';
		i++;
	}
	strncpy(table[number], s+i, len - i);
}


int main()
{
	struct msg message;
	// Создаём очередь сообщений
	int msgQueueID;
	key_t key;
	if (key = ftok("lab7", "a") == -1)
	{
		printf("ftok error\n");
		exit(-1);
	}

	if ((msgQueueID = msgget(key, IPC_CREAT | 0666)) == -1)
	{
		printf("Ошибка создания очереди\n");
		exit(-1);
	}

	// Инициализируем таблицу начальными значениями
	for (size_t i = 0; i < K; i++)
		sprintf(table[i], "Строка номер %d", i);

	// Выводим исходную таблицу
	printTable();

	// Изменяем содержимое таблицы в процессах-потомках
	for (size_t i = 0; i < K; i++)
	{
		if (fork() == 0)
		{
			printf("Процесс %d(%d) изменяет строку %d\n", i, getpid(), i);
			message.mtype = 1;
			sprintf(message.mtext, "%d Новая строка номер %d", i, i);
			// Отправляем через очередь сообщений
			if (msgsnd(msgQueueID, &message, sizeof(message.mtext), IPC_NOWAIT) < 0)
			{
				printf("Send error\n");
				exit(-1);
			}
			exit(0);
		}
	}

	// Ожидаем завершение K процессов-потомков
	int status, wpid;
	while ((wpid = wait(&status)) > 0);

	for (size_t i = 0; i < K; i++)
	{
		// Получем сообщение из очереди
		msgrcv(msgQueueID, &message, sizeof(message.mtext), 1, IPC_NOWAIT);
		// Заменяем строку таблицы
		replaceTableLine(message.mtext);
		// Выводим модифицированную таблицу
		printTable();
	}

	// Уничтожаем очередь сообщений
	msgctl(msgQueueID, IPC_RMID, NULL);

	return 0;
}