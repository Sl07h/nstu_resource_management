#include <dirent.h>	// Структура папки
#include <stdio.h>	// Стандартный ввод/вывод
#include <stdlib.h>	// Стандартная библиотека
#include <string.h>	// Строки
#include <errno.h>	// Проверка существования папки
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


struct pair {
        int foldersCount;       // Число папок
        int filesCount;         // Число файлов
};


struct directory {
        int foldersCount;       // Число папок
        int filesCount;         // Число файлов
        char *name;             // Название поддиректории
};



/**
 @function sum
 * Функция, находящая суммарное число файлов и папок
 @param a,b
 @return Суммарное число файлов и папок
 */
struct pair sum(struct pair a, struct pair b) {
        struct pair result;
        result.foldersCount = a.foldersCount + b.foldersCount;
        result.filesCount = a.filesCount + b.filesCount;
        return result;
}



/**
 @function compare
 * Функция сравнения двух поддиректорий по количеству файлов и папок
 @param a,b - элементы массива, которые будут сравниваться внутри qsort
 @return позитивный результат, если в первой папке хранится больше файлов
 * и папок, чем во второй, а иначе наоборот
 */
int compare(const void *a, const void *b) {
        int l = ((struct directory *)a)->foldersCount + ((struct directory *)a)->filesCount;
        int r = ((struct directory *)b)->foldersCount + ((struct directory *)b)->filesCount;
        return (r-l);
}



/**
 @function qetQuantityOfFoldersAndFiles
 * Функция, которая определяет число файлов и папок внутри директории
 @param path - текущая директория
 @return пара: число папок, число файлов
 */
struct pair getQuantityOfFoldersAndFiles(const char *path) {

        // Начальное число папок равно нулю
        struct pair quantity = {0, 0};

        
        DIR *dir;
        struct dirent *entry;
        struct stat buf;

        // Открываем директорию
        dir = opendir(path);
        if (!dir) {
                printf("Can't open directory %s\n", path);
                return quantity;
        }

        // Выводит все файлы и поддерректории внутри папки
        while ( (entry = readdir(dir)) != NULL) {

		char *newPath = (char*) calloc ((strlen(path) + 1 + strlen(entry->d_name) + 1), sizeof(char));
                strcpy(newPath, path);
                strcat(newPath, "/");
                strcat(newPath, entry->d_name);
                
                if (stat(newPath, &buf) != 0) {
                        printf("Error in stat.\n");
                        printf("%s\n", newPath);
                        return quantity;
                }
		
		if (S_ISREG(buf.st_mode)) { // Файл
                        quantity.filesCount++;

                } else if ( S_ISDIR(buf.st_mode) && strcmp(entry->d_name, ".") != 0 &&
                 strcmp(entry->d_name, "..") !=0 ) { // Папка, но не "." или ".."

                        quantity.foldersCount++;
                        quantity = sum(quantity, getQuantityOfFoldersAndFiles(newPath));
                }

                free(newPath);
        }
        closedir(dir);

        return quantity;
}



int main(int argc, char *argv[]) {

	if (argc > 2) {
        	printf("Ошибка: слишком длинный список параметров. Параметр должен быть один\n");
        	return -1;
	
        } else if (argc < 2) {
        	printf("Ошибка: слишком короткий список параметров. Параметр должен быть один.\n");
		return -1;

        } else if (strcmp(argv[1], "--help") == 0) {
        	printf("Инструкция:\n");
		printf("Введите название каталога, для которого вы хотите вывести имена находящихся в нём каталогов, упорядочив их по числу файлов и каталогов, содержащихся в отображаемом каталоге\n");
        	printf("Если введённый вами каталог не существует, то будет выведено сообщение о соответсвующей ошибке.\n");
        	printf("Если введено неверное число параметров (не равно 1), то выводится сообщение об ошибке.\n");
		return 0;
	}        

        char *path = argv[1];
        int array_len = 0;

        struct dirent *entry;
        struct directory *array = (struct directory*) malloc ( 0 * sizeof(struct directory));
        struct stat buf;

        // Открываем директорию
        DIR *dir = opendir(path);
	if (ENOENT == errno) {
		printf("Directory does not exist.\n");
		return -1;
	} else if (!dir) {
                printf("Can't open directory %s\n", path);
                return -1;
        }

        // Выводит все файлы и поддерректории внутри папки
        while ( (entry = readdir(dir)) != NULL) {

		char *newPath = (char*) calloc ((strlen(path) + 1 + strlen(entry->d_name) + 1), sizeof(char));
                strcpy(newPath, path);
                strcat(newPath, "/");
                strcat(newPath, entry->d_name);
                
                if (stat(newPath, &buf) != 0) {
                        printf("Error in stat.\n");
                        printf("%s\n", newPath);
                        return -1;
                }

		if ( S_ISDIR(buf.st_mode) && strcmp(entry->d_name, ".") != 0 &&
                 strcmp(entry->d_name, "..") !=0 ) { // Папка

                        struct pair p =  getQuantityOfFoldersAndFiles(newPath);
                        array_len++;
                        array = (struct directory*)realloc(array, array_len * sizeof(struct directory));
                        array[array_len-1].filesCount = p.filesCount;
                        array[array_len-1].foldersCount = p.foldersCount;
                        array[array_len-1].name = entry->d_name;
                }

                free(newPath);
        }
        closedir(dir);
       
	// Сортируем по убыванию
        qsort(array, array_len, sizeof(struct directory),compare);

	// Выводим подкатологи и число файлов и папок содержащихся в них
        printf("DIR\tf\td\n");
        for (int i = 0; i<array_len; ++i){
                printf("%s\t%d\t%d\n", array[i].name, array[i].filesCount, array[i].foldersCount);                
        }

        // Освобождаем память
        free(array);

        return 0;
}

