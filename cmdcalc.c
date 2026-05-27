#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "stack.h"
// 1. Определение приоритета операций
int get_precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}
// 2. Выполнение одной операции из стека
void process_op(Stack *s) {
    double v2 = pop_num(s); // Второе число
    double v1 = pop_num(s); // Первое число
    char op = pop_op(s);    // Оператор
    if (op == '+') push_num(s, v1 + v2);
    if (op == '-') push_num(s, v1 - v2);
    if (op == '*') push_num(s, v1 * v2);
    if (op == '/') push_num(s, v1 / v2);
}
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Ошибка: передайте выражение в кавычках\n");
        return 1;
    }
    char *expr = argv[1];
    Stack *s = create_stack(strlen(expr)); // Динамическое выделение памяти
    // 3. Главный цикл обработки строки
    for (int i = 0; expr[i] != '\0'; i++) {
        if (isspace(expr[i])) continue; // Игнорируем пробелы

        if (isdigit(expr[i]) || expr[i] == '.') {
            // Исправленное чтение числа 
            char *endptr;
            push_num(s, strtod(&expr[i], &endptr));
            i = endptr - expr - 1; 
        } else {
            // Если пришел оператор, проверяем приоритеты
            while (!is_empty_op(s) && get_precedence(peek_op(s)) >= get_precedence(expr[i])) {
                process_op(s);
            }
            push_op(s, expr[i]);
        }
    }
    // 4. Выполняем все оставшиеся операции
    while (!is_empty_op(s)) {
        process_op(s);
    }
    // Вывод итогового результата
    printf("Результат: %g\n", pop_num(s));
    free_stack(s); // Очистка памяти
    return 0;
}
