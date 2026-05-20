#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>

// Структура стека для хранения операндов (чисел) и операторов
typedef struct {
    double *nums;  // Динамический массив для чисел (double)
    char *ops;     // Динамический массив для знаков операций (+, -, *, /)
    int top_num;   // Индекс вершины стека чисел
    int top_op;    // Индекс вершины стека операторов
    int size;      // Максимальный размер стека
} Stack;

// Прототипы функций для работы со стеком
Stack* create_stack(int size);
void push_num(Stack *s, double val);
double pop_num(Stack *s);
void push_op(Stack *s, char op);
char pop_op(Stack *s);
int is_empty_op(Stack *s); // Проверка стека операторов на пустоту
char peek_op(Stack *s);    // Посмотреть верхний оператор без удаления
void free_stack(Stack *s); // Очистка памяти

#endif
