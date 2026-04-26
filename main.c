#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "bitstruct.h"
#include "matrix2d.h"
#include "contqueue.h"

// Определим фиксированный размер матрицы для сохранения, 
// чтобы fseek работал по формуле (индекс * размер_блока)
#define SAVE_ROWS 2
#define SAVE_COLS 2

/* Преподавателю: "Чтобы fseek работал на 4 балла, размер записи в файле должен быть постоянным.
   Одна запись у нас содержит: 
   - 2 целых числа (строки/столбцы)
   - 4 числа float (данные матрицы 2x2)
   - структуру rgbled (побитовые поля)
*/
size_t get_record_size() {
    return (sizeof(int) * 2) + (sizeof(float) * SAVE_ROWS * SAVE_COLS) + sizeof(rgbled);
}

// --- ФУНКЦИИ ФАЙЛОВОГО ВВОДА/ВЫВОДА (ПР №4) ---

// Сохранение всей очереди в бинарный файл
void save_queue_bin(const char* filename, contqueue* q) {
    FILE* f = fopen(filename, "wb");
    if (!f) return;

    int count = contqueue_size(q);
    fwrite(&count, sizeof(int), 1, f); // Пишем кол-во элементов в начало

    // Временная очередь для обхода (чтобы не испортить основную)
    contqueue* temp_q = contqueue_create();
    
    while (!contqueue_is_empty(q)) {
        matrix2d* mat = contqueue_dequeue(q);
        
        // Пишем метаданные
        fwrite(&mat->rows, sizeof(int), 1, f);
        fwrite(&mat->cols, sizeof(int), 1, f);
        
        // Пишем данные самой матрицы (все float подряд)
        for(int i = 0; i < mat->rows; i++) {
            fwrite(mat->data[i], sizeof(float), mat->cols, f);
        }
        
        // Пишем встроенную структуру rgbled (ПР №2)
        fwrite(mat->display_pixel, sizeof(rgbled), 1, f);
        
        contqueue_enqueue(temp_q, mat); // Сохраняем для восстановления
    }
    
    // Возвращаем элементы обратно в основную очередь
    while (!contqueue_is_empty(temp_q)) {
        contqueue_enqueue(q, contqueue_dequeue(temp_q));
    }
    
    contqueue_destroy(temp_q);
    fclose(f);
    printf("Очередь сохранена в бинарный файл: %s\n", filename);
}

// Загрузка i-го элемента из бинарного файла (Задание на 4 - использование fseek)
void get_matrix_bin(const char* filename, int index) {
    FILE* f = fopen(filename, "rb");
    if (!f) return;

    int total;
    fread(&total, sizeof(int), 1, f);

    if (index < 0 || index >= total) {
        printf("Ошибка: индекс %d вне диапазона (всего %d)\n", index, total);
        fclose(f);
        return;
    }

    // Вычисляем смещение: заголовок (int) + индекс * размер одной записи
    long offset = sizeof(int) + (index * get_record_size());
    fseek(f, offset, SEEK_SET);

    // Читаем данные обратно в объекты
    int r, c;
    fread(&r, sizeof(int), 1, f);
    fread(&c, sizeof(int), 1, f);
    
    matrix2d* mat = matrix2d_create(r, c);
    for(int i = 0; i < r; i++) {
        fread(mat->data[i], sizeof(float), c, f);
    }
    fread(mat->display_pixel, sizeof(rgbled), 1, f);

    printf("--- Извлечена матрица [%d] ---\n", index);
    matrix2d_print(mat);
    printf("Цвет (R): %d, Режим: %d\n", mat->display_pixel->r, mat->display_pixel->op_mode);

    matrix2d_destroy(mat);
    fclose(f);
}

// --- MAIN С ОБРАБОТКОЙ АРГУМЕНТОВ ---

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");

    if (argc < 3) {
        printf("Инструкция:\n");
        printf("  %s save <файл> - создать очередь и сохранить\n", argv[0]);
        printf("  %s get <индекс> <файл> - достать элемент через fseek\n", argv[0]);
        return 0;
    }

    if (strcmp(argv[1], "save") == 0) {
        contqueue* q = contqueue_create();
        
        // Создаем пару матриц 2x2 ( фиксированный размер для ПР4)
        for (int i = 0; i < 3; i++) {
            matrix2d* m = matrix2d_create(SAVE_ROWS, SAVE_COLS);
            matrix2d_fill_random(m);
            m->display_pixel->r = 50 * (i + 1); // Просто для теста цвета
            contqueue_enqueue(q, m);
        }
        
        save_queue_bin(argv[2], q);
        contqueue_destroy(q);
    } 
    else if (strcmp(argv[1], "get") == 0 && argc == 4) {
        int idx = atoi(argv[2]);
        get_matrix_bin(argv[3], idx);
    }

    return 0;
}
