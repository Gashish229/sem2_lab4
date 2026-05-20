#include "stack.h"
#include <stdlib.h>

// Создание стека: выделяем память под массивы для чисел и операторов
Stack* create_stack(int size) {
    Stack *s = (Stack*)malloc(sizeof(Stack));
    s->nums = (double*)malloc(size * sizeof(double));
    s->ops = (char*)malloc(size * sizeof(char));
    s->top_num = -1; // -1 значит, что стек пуст
    s->top_op = -1;
    s->size = size;
    return s;
}

// Добавление числа (Push)
void push_num(Stack *s, double val) { s->nums[++s->top_num] = val; }

// Извлечение числа (Pop)
double pop_num(Stack *s) { return s->nums[s->top_num--]; }

// Добавление оператора (Push)
void push_op(Stack *s, char op) { s->ops[++s->top_op] = op; }

// Извлечение оператора (Pop)
char pop_op(Stack *s) { return s->ops[s->top_op--]; }

// Просмотр верхнего оператора (Peek)
char peek_op(Stack *s) { return s->ops[s->top_op]; }

// Проверка, есть ли операторы в стеке
int is_empty_op(Stack *s) { return s->top_op == -1; }

// Освобождение динамической памяти
void free_stack(Stack *s) { 
    free(s->nums); 
    free(s->ops); 
    free(s); 
}
