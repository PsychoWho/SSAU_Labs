#include <iostream>

using namespace std;

//(c/a - 2*b)/(b +4*a - 10*c);

double cpp_m(double a, double b, double c) {
    return (c / a - 2 * b) / (b + 4 * a - 10 * c);
}

void main()
{
    char s[20];
    double a, b, c;
    int error = 0;  // 0 - нет ошибок, 1 - деление на ноль, 2 - переполнение
    double answ = 0;  // Переменная для хранения результата
    cout << "Enter a > ";
    cin >> a;
    cout << "Enter b > ";
    cin >> b;
    cout << "Enter c > ";
    cin >> c;

    //константы для вычисления
    const double с2 = 2.0;
    const double с4 = 4.0;
    const double с10 = 10.0;

    _asm {
        finit                   ;инициализация процессора

        ; проверка, является ли а == 0, так как есть деление на а
        fldz                    ; загружаем 0
        fld qword ptr[a]        ; загружаем а
        fucomp                  ; сравниваем а и 0
        fnstsw ax               ; сохраняем статус
        sahf                    ; загружаем стутус в EFLAGS
        jz division_by_zero     ; если а == 0

        ; Вычисление(c / a - 2 * b)
        fld  qword ptr[c]       ; st(0) = c
        fdiv qword ptr[a]       ; st(0) = c / a
        fld  qword ptr[b]       ; st(0) = b, st(1) = c / a
        fmul qword ptr[с2]      ; st(0) = 2 * b
        fsubp st(1), st         ; st(0) = c / a - 2 * b

        ; Вычисление(b + 4 * a - 10 * c)
        fld  qword ptr[a]       ; st(0) = a, st(1) = (c / a - 2 * b)
        fmul qword ptr[с4]      ; st(0) = 4 * a
        fadd qword ptr[b]       ; st(0) = b + 4 * a
        fld  qword ptr[c]       ; st(0) = c, st(1) = b + 4 * a, st(2) = (c / a - 2 * b)
        fmul qword ptr[с10]     ; st(0) = 10 * c
        fsubp st(1), st         ; st(0) = b + 4 * a - 10 * c

        ; Проверка(знаменатель == 0)
        fldz                    ; st(0) = 0, st(1) = (b + 4 * a - 10 * c)
        fucomp                  ; Сравниваем знаменатель и 0
        fnstsw ax               ; Сохраняем статус
        sahf                    ; Загружаем статус в EFLAGS
        jz division_by_zero     ; Если знаменатель == 0, ошибка деления на ноль

        fdivp st(1), st         ; st(0) = (c / a - 2 * b) / (b + 4 * a - 10 * c)
        fst qword ptr[answ]     ; Сохранение результата
        jmp fin
        

    division_by_zero:
        mov error, 1
        jmp fin

    fin:
        
    }

    // Вывод результата или ошибки
    if (error == 1) {
        cout << "Error: division by zero!" << endl;
    }
    else {
        cout << "Result ASM: " << answ << endl;
        cout << "Result C++: " << cpp_m(a,b,c) << endl;
    }
}
