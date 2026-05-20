#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "stack.h"

// Определение приоритета: умножение и деление (2) выше сложения и вычитания (1)
int get_precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

// Выполнение операции: берем два числа, применяем оператор и кладем результат обратно
void process_op(Stack *s) {
    double v2 = pop_num(s); // Извлекаем второе число (оно было введено позже)
    double v1 = pop_num(s); // Извлекаем первое число
    char op = pop_op(s);    // Извлекаем оператор
    switch(op) {
        case '+': push_num(s, v1 + v2); break;
        case '-': push_num(s, v1 - v2); break;
        case '*': push_num(s, v1 * v2); break;
        case '/': push_num(s, v1 / v2); break;
    }
}

int main(int argc, char *argv[]) {
    // Простейшая проверка аргументов
    if (argc < 2) {
        printf("Использование: %s \"выражение\"\n", argv[0]);
        return 1;
    }

    char *expr = argv[1];
    Stack *s = create_stack(strlen(expr));

    for (int i = 0; expr[i]; i++) {
        // Пропускаем пробелы в выражении
        if (isspace(expr[i])) continue;

        // Если это число (или дробное), считываем его
        if (isdigit(expr[i]) || expr[i] == '.') {
            // strtod считывает double и автоматически двигает указатель i
            push_num(s, strtod(&expr[i], &expr[i]));
            i--; // Корректируем цикл, так как strtod уже сдвинул i
        } else {
            // Если приоритет текущего оператора <= приоритета оператора в стеке,
            // сначала выполняем операцию из стека
            while (!is_empty_op(s) && get_precedence(peek_op(s)) >= get_precedence(expr[i])) {
                process_op(s);
            }
            push_op(s, expr[i]);
        }
    }

    // Выполняем оставшиеся операции в стеке
    while (!is_empty_op(s)) {
        process_op(s);
    }

    // Результат — единственное число, оставшееся в стеке
    printf("%g\n", pop_num(s));
    free_stack(s);
    return 0;
}
