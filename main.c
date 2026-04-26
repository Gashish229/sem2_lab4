#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- НАША СТРУКТУРА ДАННЫХ (Вариант 2 - Пиксель экрана) ---
// #pragma pack(push, 1) отключает выравнивание памяти. 
// Преподавателю: "Я использовал это, чтобы в бинарном файле структура занимала ровно 6 байт без пустых мест (padding)".
#pragma pack(push, 1)
typedef struct {
    unsigned char r;          // Красный цвет (0-255)
    unsigned char g;          // Зеленый цвет (0-255)
    unsigned char b;          // Синий цвет (0-255)
    unsigned char brightness; // Яркость
    unsigned short temp;      // Температура цвета
} rgbled;
#pragma pack(pop)

// --- ФУНКЦИИ ДЛЯ РАБОТЫ С ТЕКСТОВЫМ ФАЙЛОМ (на оценку "3") ---

// Сохранение в текстовый файл
void save_text(const char* filename, int count) {
    // Открываем файл для записи текста ("w" - write)
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Ошибка: не удалось открыть файл для записи!\n");
        return;
    }

    // Пишем сначала количество элементов (чтобы при чтении знать, сколько их)
    fprintf(file, "%d\n", count);

    for (int i = 0; i < count; i++) {
        // Генерируем случайный пиксель
        rgbled led;
        led.r = rand() % 256;
        led.g = rand() % 256;
        led.b = rand() % 256;
        led.brightness = rand() % 100;
        led.temp = 3000 + (rand() % 4000); // от 3000 до 7000 Кельвинов

        // Записываем данные в строчку через пробел
        fprintf(file, "%d %d %d %d %d\n", led.r, led.g, led.b, led.brightness, led.temp);
    }
    fclose(file); // Всегда закрываем файл!
    printf("Успешно сохранено %d элементов в текстовый файл %s\n", count, filename);
}

// Извлечение конкретного элемента из текстового файла
// Преподавателю: "В текстовом файле строки могут быть разной длины, поэтому fseek тут не поможет. 
// Приходится читать файл построчно с самого начала до нужного индекса."
rgbled* get_element_from_text_file(const char* filename, int index) {
    FILE *file = fopen(filename, "r"); // "r" - чтение
    if (!file) return NULL;

    int total_count;
    fscanf(file, "%d", &total_count); // Читаем общее количество

    // Если индекс больше, чем есть элементов
    if (index < 0 || index >= total_count) {
        fclose(file);
        return NULL; 
    }

    // Выделяем память под результат
    rgbled* result = (rgbled*)malloc(sizeof(rgbled));

    // Идем по файлу, пока не дойдем до нужного индекса
    for (int i = 0; i <= index; i++) {
        int r, g, b, bright, temp;
        // Читаем данные элемента
        fscanf(file, "%d %d %d %d %d", &r, &g, &b, &bright, &temp);
        
        // Если это нужный нам индекс, сохраняем в структуру
        if (i == index) {
            result->r = r; result->g = g; result->b = b;
            result->brightness = bright; result->temp = temp;
        }
    }
    fclose(file);
    return result;
}

// Вывод всего текстового файла (команда list)
void list_text(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Ошибка чтения файла!\n");
        return;
    }

    int count;
    fscanf(file, "%d", &count);
    printf("В файле хранится элементов: %d\n", count);

    for (int i = 0; i < count; i++) {
        int r, g, b, bright, temp;
        fscanf(file, "%d %d %d %d %d", &r, &g, &b, &bright, &temp);
        printf("[%d] RGB(%d, %d, %d), Яркость: %d, Темп: %dK\n", i, r, g, b, bright, temp);
    }
    fclose(file);
}

// --- ФУНКЦИИ ДЛЯ РАБОТЫ С БИНАРНЫМ ФАЙЛОМ (на оценку "4") ---

// Сохранение в бинарный файл
// Преподавателю: "Здесь используется 'wb' (write binary). Я сохраняю элементы сразу 
// блоком памяти через fwrite, это намного быстрее и эффективнее, чем текст."
void save_binary(const char* filename, int count) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Ошибка создания бинарного файла!\n");
        return;
    }

    // Сначала записываем количество элементов (1 штука размером с int)
    fwrite(&count, sizeof(int), 1, file);

    for (int i = 0; i < count; i++) {
        rgbled led;
        led.r = rand() % 256; led.g = rand() % 256; led.b = rand() % 256;
        led.brightness = rand() % 100; led.temp = 3000 + (rand() % 4000);
        
        // Записываем структуру целиком в двоичном виде
        fwrite(&led, sizeof(rgbled), 1, file);
    }
    fclose(file);
    printf("Успешно сохранено %d элементов в БИНАРНЫЙ файл %s\n", count, filename);
}

// Специфическая функция из задания на 4
// Преподавателю: "Это самая важная функция для 4 баллов. Мы используем fseek.
// Сначала пропускаем sizeof(int) (там хранится количество), а затем прыгаем 
// сразу на нужный элемент: index * element_size. Так работает произвольный доступ!"
void get_element_from_binary_file(const char* filename, int index, size_t element_size, void* result) {
    FILE *file = fopen(filename, "rb"); // "rb" - read binary
    if (!file) {
        printf("Ошибка открытия файла!\n");
        return;
    }

    int total_count;
    fread(&total_count, sizeof(int), 1, file); // Читаем сколько всего штук

    if (index < 0 || index >= total_count) {
        printf("Ошибка: Индекс выходит за границы файла (всего %d)\n", total_count);
        fclose(file);
        return;
    }

    // Вычисляем позицию: пропускаем 1 int (счетчик) + пропускаем (index) элементов
    long offset = sizeof(int) + (index * element_size);
    
    // Сдвигаем курсор файла на вычисленную позицию (SEEK_SET значит от начала файла)
    fseek(file, offset, SEEK_SET);

    // Считываем один элемент прямо в указатель result
    fread(result, element_size, 1, file);
    
    fclose(file);
}

// --- ГЛАВНАЯ ФУНКЦИЯ И ОБРАБОТКА КОМАНДНОЙ СТРОКИ ---
int main(int argc, char *argv[]) {
    srand(time(NULL)); // Инициализация генератора случайностей

    // Проверяем, что передано хотя бы 3 аргумента (например: ./main save data.txt)
    if (argc < 3) {
        printf("Использование:\n");
        printf("  ./main save <filename>      - сгенерировать и сохранить в текст\n");
        printf("  ./main list <filename>      - вывести текстовый файл\n");
        printf("  ./main get <index> <file>   - получить элемент из текста\n");
        printf("  ./main savebin <filename>   - сохранить в бинарный файл\n");
        printf("  ./main getbin <index> <file>- получить из бинарного файла\n");
        return 1;
    }

    char *command = argv[1];

    // Обработка команды "save" (Текст)
    if (strcmp(command, "save") == 0) {
        char *filename = argv[2];
        save_text(filename, 5); // Для примера генерируем 5 элементов
    } 
    // Обработка команды "list" (Текст)
    else if (strcmp(command, "list") == 0) {
        char *filename = argv[2];
        list_text(filename);
    }
    // Обработка команды "get" (Текст)
    else if (strcmp(command, "get") == 0) {
        if (argc < 4) { printf("Укажите индекс!\n"); return 1; }
        int index = atoi(argv[2]); // Переводим текст в число
        char *filename = argv[3];
        
        rgbled* led = get_element_from_text_file(filename, index);
        if (led) {
            printf("Текстовый элемент %d: RGB(%d,%d,%d) Яркость:%d Темп:%d\n", 
                   index, led->r, led->g, led->b, led->brightness, led->temp);
            free(led); // Очищаем память
        } else {
            printf("Элемент не найден.\n");
        }
    }
    // Обработка команды "savebin" (Бинарный)
    else if (strcmp(command, "savebin") == 0) {
        char *filename = argv[2];
        save_binary(filename, 5);
    }
    // Обработка команды "getbin" (Бинарный)
    else if (strcmp(command, "getbin") == 0) {
        if (argc < 4) { printf("Укажите индекс!\n"); return 1; }
        int index = atoi(argv[2]);
        char *filename = argv[3];

        rgbled led; // Сюда запишем результат
        // Вызываем функцию строго по условию из "Задания на 4"
        get_element_from_binary_file(filename, index, sizeof(rgbled), &led);
        
        printf("Бинарный элемент %d: RGB(%d,%d,%d) Яркость:%d Темп:%d\n", 
               index, led.r, led.g, led.b, led.brightness, led.temp);
    }
    else {
        printf("Неизвестная команда!\n");
    }

    return 0;
}
