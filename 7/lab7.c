// Вариант 5:
// Программа моделирует работу примитивной СУБД, хранящей единственную таблицу в оперативной памяти.
// Выполняя некоторые циклы работ, K порожденных процессов посредством очереди сообщений передают 
// процессу номер строки и её содержимое, на которое нужно изменить хранящиеся в ней данные.
// Родительский процесс выполняет указанную операцию и возвращает старое содержимое изменённой строки. 

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
#define LENGTH 1024
#define K 5

struct {
	int msg_type;
	char msg_text[1024];
} message;

char table[K][LENGTH];

/*
 *
 *
 */
// int replaceLine(int i, char* s) {

// 	if (0 <= i && i < LENGTH) {
// 		char* result;
// 		strcpy(result, table[i]);
// 		if (strlen(s) + 1 <= LENGTH)
// 		{
// 			strcpy(table[i], s);
// 			return 0;
// 		}
// 		else {
// 			printf("Слишком длинная строка\n");
// 			return -1;
// 		}
// 	}
// 	else {
// 		printf("Неправильный индекс таблицы\n");
// 		return -2;
// 	}
// }


char* replaceLine(int i, char* s) {
	char result = (char*)malloc(LENGTH);
	strcpy(result, table[i]);
	strcpy(table[i], s);
	return result;
}


/*
 *
 * 
 */
void printTable(){
    printf("Таблица:\n");
    for (size_t i = 0; i < K; i++)
        printf("%d:\t%s\n",i,table[i]);    
}



int main()
{
	// Создаём очередб сообщений
	int msgQueueID;
	key_t key = ftok("lab7", 67);
	if (msgQueueID = msgget(key, 0666| IPC_CREAT) == -1){
		if (errno == EEXIST) 
		{
			//if queue create a long long time ago
			msgQueueID = msgget(IPC_PRIVATE, 0); 
		}
		else 
		{
			printf("ERROR! can't create message queue. \n");
			exit(1);
		}
	}
	

	// Инициализируем таблину начальными значениями
	for (size_t i = 0; i < K; i++)
		sprintf(table[i], "Строка номер %d", i);

    // Выводим исходную таблицу
    printTable();

	// Изменяем содержимое таблицы в процессах-потомках
	for (size_t i = 0; i < K; i++)
	{
		if (fork() == 0) {
			printf("Процесс %d(%d) изменяет строку %d\n", i, getpid(), i);
			message.msg_type = 1;
			sprintf(message.msg_text, "Новая строка номер %d", i);
			printf("Before: %s\n", message.msg_text);
			msgsnd(msgQueueID, &message, sizeof(message), 0);
			exit(0);
		}
	}

	sleep(5);
	// Ожидаем завершение K процессов-потомков
	int status,wpid;
	while ((wpid = wait(&status)) > 0);
    
	for (size_t i = 0; i < K; i++)
	{
		int len = msgrcv(msgQueueID, &message, sizeof(message), 1, 0);
		//printf("After: %s\n", message.msg_text);
		
		 printf("Строка %d до изменения: \'%s\'. Её длина %d\n", i, message.msg_text, len);
        // replaceLine(i, message.msg_text);
		// char oldLine = (char*)malloc(LENGTH);
        // strcpy(oldLine, replaceLine(i, message.msg_text));
        // printf("Строка до изменения: %s", oldLine);
	}

    // Выводим модифицированную таблицу
    printTable();

	// Уничтожаем очередь сообщений
	msgctl(msgQueueID, IPC_RMID, NULL);

	return 0;
}
