#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <iostream>

using namespace std;

//(c/a - 2*b)/(b +4*a - 10*c);

double cpp_m(int a, int b, int c) {
    return (c / a - 2 * b) / (b + 4 * a - 10 * c);
}


void main()
{
    char s[20];
    int a, b, c;
    int error = 0;  // 0 - нет ошибок, 1 - деление на ноль, 2 - переполнение
    int answ = 0;  // Переменная для хранения результата
    CharToOem(_T("Enter "), s);
    printf("%s a: ", s);
    scanf_s("%d", &a);
    printf("%s b: ", s);
    scanf_s("%d", &b);
    printf("%s c: ", s);
    scanf_s("%d", &c);
    _asm {
        mov eax, a;     EAX = a
        mov ebx, b;     EBX = b
        mov ecx, c;     ECX = c

        ; Вычисляем знаменатель : (b + 4 * a - 10 * c)
        ; Вычисляем 4 * a
        mov edx, 4;      EDX = 4
        imul edx, eax;   EDX = 4 * a
        jo overflow;     Проверка на переполнение

        ; Вычисляем b + 4 * a
        add ebx, edx;       EBX = b + 4 * a
        jo overflow;        Проверка на переполнение

        ; Вычисляем 10 * c
        mov edx, 10;    EDX = 10
        imul edx, ecx;  EDX = 10 * c
        jo overflow;    Проверка на переполнение

        ; Вычисляем знаменатель : (b + 4 * a - 10 * c)
        sub ebx, edx;   EBX = (b + 4 * a - 10 * c)
        jo overflow;    Проверка на переполнение

        ; Проверка деления на ноль
        cmp ebx, 0
        je division_by_zero

        ; Вычисляем числитель : (c / a - 2 * b)
        ; Вычисляем c / a
        mov eax, c;     EAX = c
        mov ecx, a;     ECX = a
        cdq;            Расширяем EAX в EDX : EAX для деления
        idiv ecx;       EAX = c / a
        mov ecx, eax;   ECX = c / a

        ; Вычисляем 2 * b
        mov eax, b;     EAX = b
        mov edx, 2;     EDX = 2
        imul eax, edx;  EAX = 2 * b
        jo overflow;    Проверка на переполнение

        ; Вычисляем числитель : (c / a - 2 * b)
        sub ecx, eax;   ECX = (c / a - 2 * b)
        jo overflow;    Проверка на переполнение

        ; Вычисляем результат : (c / a - 2 * b) / (b + 4 * a - 10 * c)
        mov eax, ecx;   EAX = (c / a - 2 * b)
        cdq;            Расширяем EAX в EDX : EAX для деления
        idiv ebx;       EAX = (c / a - 2 * b) / (b + 4 * a - 10 * c)
        mov answ, eax;  Сохраняем результат в answ
        jmp fin;        Переход к завершению

        division_by_zero :
        mov error, 1; Устанавливаем флаг ошибки "division by zero"
            jmp fin

            overflow :
        mov error, 2; Устанавливаем флаг ошибки "overflow"
            jmp fin

            fin :
    }
    // Вывод результата или ошибки
    if (error == 1) {
        cout << "Error: division by zero!" << endl;
    }
    else if (error == 2) {
        cout << "Error: overflow!" << endl;
    }
    else {
        cout << "Result ASM: " << answ << endl;
        cout << "Result C++: " << cpp_m(a,b,c) << endl;
    }
    cout << endl;
}