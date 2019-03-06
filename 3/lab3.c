#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int countP = 0, countC = 0;

typedef struct {
    char *name;             // имя файла
    int fd;                 // дескриптор файла
    struct stat data;       // метаданные
} inodeTable;
inodeTable inTab[7];        // Таблица описателей файлов

typedef struct {
    char *name;             // имя файла
    int linksCount;         // Число ссылок
    int mode;               // режим чтения / записи
    int locPoint;           // расположение каретки в файле
} filesTable;
filesTable fTab[7];         // Таблица файлов

typedef struct {
    char *name;             // имя файла
    int fd;                 // дескриптор файла
} openedFilesTable;
openedFilesTable ofTabP[7], ofTabC[7];  // Таблица открытых файлов



/* вывод таблицы файлов и таблицы открытых файлов
 @function printTable
 @param isParent - выводить таблицы процесса-предка или процесса-потомка
 */
void printTables(){

    printf("Таблица файлов:\n");
    printf("name\t\tlinks\tmode\tlocP\tref\n");
    for(int i = 3; i < countP; ++i)
        printf("%s   \t%d\t%d\t%d\t%d\n", fTab[i].name, fTab[i].linksCount, fTab[i].mode, fTab[i].locPoint, &inTab[i]);

    printf("\nТаблица открытых файлов:\n");
    printf("name\t\tfd\tref\n");
    for(int i = 0; i < countP; ++i)
        printf("%s   \t%d\t%d\n", ofTabP[i].name, ofTabP[i].fd, &inTab[i]);
    for(int i = 0; i < countC; ++i)
        printf("%s   \t%d\t%d\n", ofTabC[i].name, ofTabC[i].fd, &inTab[i]);
    
    printf("\n\n\n");
}



/* наследование таблиц при вызове fork()
 происходит копирование элементов таблицы открытых файлов и
 увеличение числа ссылок на файлы на 1
 @function copyTables
 */
void copyTables(){
    for (int i = 0; i < countP; ++i){
        fTab[i].linksCount++;
        ofTabC[i] = ofTabP[i];
    }
    countC = countP;
}

/* закрытие файлов дочернего процесса
 @function closeChildProcessFiles
 */
void closeChildProcessFiles(){

    for (int i = countC - 1; i>=0; --i)
        fTab[i].linksCount--;
    
    countC = 0;
}


int main(int argc, char *argv[]) {

    char *names[] = {"stdin","stdout","stderr", "file1.txt", "file2.txt", "file3.txt", "file4.txt"};

    // Шаг 1
    printf("Открытие стандартных файлов ввода-ввывода и 4 пользовательских файлов родительским процессом\n");
    for(int i = 0; i < 3; ++i) {

        ofTabP[i].name = names[i];
        ofTabP[i].fd = i;
    }

    for(int i = 3; i < 7; ++i) {

        int fd = open(names[i], O_APPEND);  // дескриптор файла
        inTab[i].name = names[i];
        inTab[i].fd = fd;
        fstat(fd, &inTab[i].data);

        fTab[i].name = names[i];
        fTab[i].linksCount = 1;
        fTab[i].mode = O_APPEND;            // w+
        fTab[i].locPoint = 0;               // начало файла

        ofTabP[i].name = names[i];
        ofTabP[i].fd = fd;
    }
    countP = 7;
    printTables();


    // Шаг 2
    printf("\n\nЗакрытие 2 пользовательских файлов процессом-предком (file3.txt и file4.txt)\n");
    countP -= 2;
    printTables();


    // Шаг 3
    printf("\n\nПорождение процесса-потомка через fork()\n");
    copyTables();
    printTables();

    printf("Закрытие 1 пользовательского файла процессом-потомком (file2.txt)\n");
    countC--;
    fTab[4].linksCount--;
    printTables();

    // Шаг 4
    printf("\n\nЗавершается процесс-потомок. После каждого из этапов печатаются таблицы  файлов и  открытых  файлов для обоих процессов\n");
    closeChildProcessFiles();
    printTables();
}

