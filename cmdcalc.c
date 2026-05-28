#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "stack.h"

// 1. Определение приоритета операций (для оценки "4")
int get_precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

// 2. Выполнение одной операции из стека
void process_op(Stack *s) {
    double v2 = pop_num(s); // Второе число (правое)
    double v1 = pop_num(s); // Первое число (левое)
    char op = pop_op(s);    // Оператор

    if (op == '+') push_num(s, v1 + v2);
    if (op == '-') push_num(s, v1 - v2);
    if (op == '*') push_num(s, v1 * v2);
    if (op == '/') push_num(s, v1 / v2);
}

int main(int argc, char *argv[]) {
    // Защита от дурака (запуск без аргументов)
    if (argc < 2) {
        printf("Ошибка: передайте выражение в кавычках\n");
        return 1;
    }

    char *expr = argv[1];
    Stack *s = create_stack(strlen(expr)); // Динамическое выделение памяти

    // Флаг состояния: ожидаем ли мы сейчас знак числа (унарный минус)? (1 - да)
    int expect_unary = 1; 

    // 3. Главный цикл обработки строки
    for (int i = 0; expr[i] != '\0'; i++) {
        if (isspace(expr[i])) continue; // Игнорируем пробелы

        // Если это цифра, точка ИЛИ минус, который является знаком числа
        if (isdigit(expr[i]) || expr[i] == '.' || (expr[i] == '-' && expect_unary)) {
            
            char *endptr;
            // strtod считывает число целиком (вместе с минусом, если он есть)
            push_num(s, strtod(&expr[i], &endptr)); 
            
            i = endptr - expr - 1; // Корректно сдвигаем индекс цикла на конец числа
            
            expect_unary = 0; // Число считано, дальше ждем знак математической операции
        } 
        else {
            // Если пришел оператор, проверяем приоритеты
            while (!is_empty_op(s) && get_precedence(peek_op(s)) >= get_precedence(expr[i])) {
                process_op(s);
            }
            push_op(s, expr[i]);
            
            expect_unary = 1; // После знака операции снова может идти унарный минус (напр. 2 * -3)
        }
    }

    // 4. Выполняем все оставшиеся в стеке операции
    while (!is_empty_op(s)) {
        process_op(s);
    }

    // Вывод итогового результата
    printf("Результат: %g\n", pop_num(s));
    
    free_stack(s); // Очистка выделенной памяти
    return 0;
}
