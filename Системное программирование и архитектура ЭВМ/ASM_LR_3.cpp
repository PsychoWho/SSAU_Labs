#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <iostream>

using namespace std;

int fun(int a, int b) {
    if (a > b) return (3 * a - b) / (5 * b);
    if (a < b) return ((a * b - 10) / 5);
    return -a;
}

void main()
{
    char s[20];
    int a, b;
    int error = 0;  // 0 - нет ошибок, 1 - деление на ноль, 2 - переполнение
    int answ = 0;  // Переменная для хранения результата
    CharToOem(_T("Enter "), s);
    printf("%s a: ", s);
    scanf_s("%d", &a);
    printf("%s b: ", s);
    scanf_s("%d", &b);
    _asm {
        mov eax, a;     EAX = a
        mov ebx, b;     EBX = b

        cmp eax, ebx;   сравнение a и b
        jg l_big;       переход если a > b
        jl l_sml;       переход если a < b

        neg eax;        изменение знака
        mov answ, eax;  сохранение результата
        jmp fin;        переход на конец программы


    ;если a > b вычисляем (3a - b) / 5b
    l_big:
        ; проверка на деление на ноль
        cmp ebx, 0
        je division_by_zero

        ;вычисляем 3*а
        mov edx, 3;     EDX = 3
        imul edx, eax;  EDX = 3 * a
        jo overflow;    проверка на переполнение

        ;вычисляем 3a-b
        sub edx, ebx;   EDX = 3a-b
        jo overflow;    проверка на переполнение

        ;вычисляем 5b
        mov ebx, b;     EBX = b
        mov eax, 5;     EAX = 5
        imul ebx, eax;  EBX = 5 * b
        jo overflow;    проверка на переполнение
        
        ;вычисляем (3a - b) / 5b
        mov eax, edx;   EAX = 3a - b
        cdq;            расширяем EAX в EDX : EAX для деления
        idiv ebx;       EAX = (3a - b) / 5b
        mov answ, eax;  сохранение результата

        jmp fin;        переход на конец программы
    
   
    ;если a < b вычисляем (ab - 10)/5
    l_sml:
        ;вычисляем a*b
        imul ebx, eax;  EBX = a*b
        jo overflow;    проверка на переполнение

        ;вычисляем ab - 10
        mov edx, 10;    EDX = 10
        sub ebx, edx;   EBX = ab - 10
        jo overflow;    проверка на переполнение

        ;вычисляем (ab - 10) / 5
        mov ecx, 5;     ECX = 5
        mov eax, ebx;   EAX = ab - 10
        cdq;            расширяем EAX в EDX : EAX для деления
        idiv ecx;       EAX = (ab - 10) / 5
        mov answ, eax;  сохранение результата

        jmp fin


    division_by_zero:
        mov error, 1; Устанавливаем флаг ошибки "division by zero"
        jmp fin

    overflow:
        mov error, 2; Устанавливаем флаг ошибки "overflow"
        jmp fin

    fin:


    }
    // Вывод результата или ошибки
    if (error == 1) {
        cout << "Error: division by zero!" << endl;
    }
    else if (error == 2) {
        cout << "Error: overflow!" << endl;
    }
    else {
        cout << "Result: " << answ << endl;
        cout << "c++: " << fun(a,b) << endl;
    }
}