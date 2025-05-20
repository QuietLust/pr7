#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

void search_in_files (const char *file_name, const char *word, const int ignore_case) {
    //Открытие необходимого файла и проверка на открытие
    FILE *file = fopen(file_name, "r");
    if (!file){
        perror("Ошибка открытия файла");
        return;
    }

    //Переменные для строки со словом (буфер и счетчик)
    char line[1024];
    int line_count = 0;

    //Буферы для строки и слова нижнего регистра
    char lower_line[1024];
    char lower_word[1024];

    //Перевод искомого слова в нижний регистр
    if (ignore_case) {
        strcpy(lower_word, word);
        for (int i = 0; lower_word[i]; i++) {
            lower_word[i] = tolower(lower_word[i]);
        }
    }

    while (fgets(line, sizeof(line), file)) {
        line_count++;
        int found = 0;

        if (ignore_case){
            //Создаём копию строки, но в нижнем регистре
            strcpy(lower_line, line);
            for (int i = 0; lower_line[i]; i++) {
                lower_line[i] = tolower(lower_line[i]);
            }
            found = (strstr(lower_line, lower_word) != NULL);
        } else {
            found = (strstr(line, word) != NULL);
        }

        //Вывод результата
        if (found) {
            printf("%s:%d:%s", file_name, line_count, line);
        }
    }

    fclose(file);
}

void search_in_dir (const char *dir_path, const char *word, const int ignore_case) {
    //Открытие заданной директории
    DIR *dir = opendir(dir_path);
    if (!dir) {
        perror("Ошибка открытия директории");
        return;
    }

    struct dirent *entry;
    while ((entry=readdir(dir)) != NULL) {

        //Проверка на текущую и родительскую папку
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        //Сохраняем полный путь к файлу/директории
        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);

        //Рекурсивный вызов, для проверки поддиректорий
        if (entry->d_type == DT_DIR) {
            search_in_dir(path, word, ignore_case);
        }//Обработка файлов
        else if (entry->d_type == DT_REG) {
            search_in_files(path, word, ignore_case);
        }

    }

    closedir(dir);
}

void help (char *prog_name) {
    printf("Использование: %s [ОПЦИИ...] [ДИРЕКТОРИЯ] ИСКОМОЕ_СЛОВО\n", prog_name);
    printf("Опции:\n");
    printf("  -h, --help     Показать справку и выйти\n");
    printf("  -i             Игнорировать регистр символов\n");
    printf("Описание:\n");
    printf("  Ищет СЛОВО во всех текстовых файлах в указанной ДИРЕКТОРИИ и её поддиректориях.\n");
    printf("  Если ДИРЕКТОРИЯ не указана, используется ~/files.\n");
}

int main(int argc, char *argv[]) {
    int ignore_case = 0;
    //Массив для директории и искомого слова
    char *args[2] = {NULL};
    // Получение домашней директории и проверка
    const char *home_dir = getenv("HOME");
    if (!home_dir) {
        fprintf(stderr, "Ошибка: не удалось определить домашнюю директорию\n");
        return EXIT_FAILURE;
    }

    //Обработка аргументов
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-'){
            if ((strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "-h") == 0)) {
                help(argv[0]);
                return EXIT_SUCCESS;
            }
            else if (strcmp(argv[i], "-i") == 0) {
                ignore_case = 1;
            }
            else {
                fprintf(stderr, "Ошибка: неизвестная опция '%s'\n", argv[i]);
                return EXIT_FAILURE;
            }
        } else {
            if (args[1] != NULL) {
                fprintf(stderr, "Ошибка, слишком много аргументов!\n");
                help(argv[0]);
                return EXIT_FAILURE;
            }

            if (args[0] == NULL) {
                args[0] = argv[i];
            } else {
                args[1] = argv[i];
            }
        }
    }

    //Проверка обязательного аргумента
    if(args[0] == NULL && args[1] == NULL) {
        fprintf(stderr, "Ошибка: не указано искомое слово!\n");
        help(argv[0]);
        return EXIT_FAILURE;
    }

    //Формирование директории по умолчанию
    char default_dir[PATH_MAX];
    snprintf(default_dir, sizeof(default_dir), "%s/files", home_dir);

    //Обработка аргументов (путь к файлам и искомое слово)
    const char *dir_path = (args[1] != NULL) ? args[0] : default_dir;
    const char *word = (args[1] != NULL) ? args[1] : args[0];

    //printf("\n\t%s\t%s\t%d\n\n", dir_path, word, ignore_case);

    //Проверка, указано ли искомое число
    if (!word) {
        fprintf(stderr, "Не указано искомое слово!\n");
        help(argv[0]);
        return EXIT_FAILURE;
    }

    search_in_dir(dir_path, word, ignore_case);
    return EXIT_SUCCESS;
}
