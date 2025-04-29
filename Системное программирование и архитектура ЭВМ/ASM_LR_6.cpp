#include <iostream>

using namespace std;

double fun(double a, double b) {
    if (a > b) return (3 * a - b) / (5 * b);
    if (a < b) return ((a * b - 10) / 5);
    return -a;
}

int main()
{
    double a, b;
    int error; // 0 - нет ошибок, 1 - деление на ноль
    double answ = 0; // Переменная для хранения результата

    cout << "Enter a > ";
    cin >> a;
    cout << "Entre b > ";
    cin >> b;

    //константы для вычисления
    const double c3 = 3.0;
    const double c5 = 5.0;
    const double c10 = 10.0;
    int status;

    _asm {
        ;                               |      st1     |       st2     |       st3      |
        finit                           ;инициализация процессора

        fld qword ptr[b];               |       b      |
        fld qword ptr[a];               |       a      |        b      |
        fcom st(1)                      ;сравниваем a и b

        fstsw status                    ;сохраняем регистр флагов сопроцессора
        mov ah, byte ptr[status + 1]
        sahf                            ;записываем в регистр флагов сопроцессора

        ja A_big                        ;переход если а больше
        jb B_big                        ;переход если b больше

        fchs;                           |      -a      |        b      |
        
        fst qword ptr[answ]             ;сохранение результата
        jmp fin                         ;переход в конец программы

        
    A_big:            
        ;считаем(3 * a - b) / (5 * b)

        ;считаем (3 * a - b)
        fmul qword ptr[c3];             |      3a      |        b      |
        fxch;                           |       b      |       3a      |
        fsubp st(1), st;                |     3a-b     |        

        ;считаем (5 * b)
        fld qword ptr[b];               |       b      |      3a-b     |
        fmul qword ptr[c5];             |      5b      |      3a-b     |

        ;проверка(знаменатель == 0)
        fldz;                           |       0      |       5b      |      3a-b      |
        fucomp                          ;сравниваем знаменатель и 0
        fnstsw ax                       ;сохраняем статус
        sahf                            ;загружаем статус в EFLAGS
        jz division_by_zero             ;если знаменатель == 0, ошибка деления на ноль

        fdivp st(1), st                 ;считаем (3 * a - b) / (5 * b)
        fst qword ptr[answ]             ;сохранение результата
        jmp fin                         ;переход в конец программы

    B_big:
        ;считаем ((a * b - 10) / 5)
        fld  qword ptr[a];              |       a      |
        fmul qword ptr[b];              |      ab      |
        fsub qword ptr[c10];            |     ab-10    |
        fdiv qword ptr[c5];             |   (ab-10)/5  |
        
        fst qword ptr[answ]             ;сохранение результата
        jmp fin                         ;переход в конец программы

    division_by_zero:
        mov error, 1                    ;присовоение ошибки 1 - деление на 0

    fin:

    }

    // Вывод результата или ошибки
    if (error == 1) {
        cout << "Error: division by zero!" << endl;
    }
    else {
        cout << "Result: " << answ << endl;
        cout << "c++: " << fun(a, b) << endl;
    }
}
