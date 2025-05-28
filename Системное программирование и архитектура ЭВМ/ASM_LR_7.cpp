#include <iostream>
#include <cmath>

using namespace std;

//exp(sin(ctg(x+5pi))^2))

const double PI = acos(-1.0);

double ctg(double x) {
    return cos(x) / sin(x);
}

double fun(double x) {
    return exp(pow(sin(ctg(x+5*PI)), 2));
}

int main()
{
    double x;
    double answ = 0; // Переменная для хранения результата

    cout << "Enter x > ";
    cin >> x;
    while (x == 0) {
        cout << "Error! x = 0" << endl << "Enter x!=0 > ";
        cin >> x;
    }

    const double c5 = 5.0;

    _asm {
        finit               ;инициализация процессора                   
        
        fld qword ptr[c5]   ;st(0) = 5
        fldpi               ;st(0) = pi,    st(1) = 5 
        fmul                ;st(0) = 5*pi
        fadd qword ptr[x]   ;st(0) = x+5*pi

        fptan               ;st(0) = 1.0, st(1) = tg(x + 5pi)
        fdiv st, st(1)      ;st(0) = ctg(x+5pi)

        fsin                ;st(0) = sin(ctg(x+5pi))
        fmul st, st         ;st(0) = sin(ctg(x+5pi))^2

        fldl2e              ;st(0) = log2(e),   st(1) = sin(ctg(x+5pi))^2
                            ;пусть sin(ctg(x+5pi))^2 = y
        fmul                ;st(0) = y * log2(e)
                            ;пусть y*log2(e) = L

        fld st(0)           ;st(0) = L,     st(1) = L
        frndint             ;st(0) = int(L),    st(1) = L
        fsub st(1), st      ;st(0) = int(L),    st(1) = frac(L)
        fxch st(1)          ;st(0) = frac(L),   st(1) = int(L)
        f2xm1               ;st(0) = 2^(frac(L))-1,     st(1) = int(L)
        fld1                ;st(0) = 1, st(1) = 2^(frac(L))-1, st(2) = int(L)
        fadd                ;st(0) = 2^(frac(L)), st(1) = int(L)
        fscale              ;st(0) = 2^(frac(L)) * 2^int(L), st(1) = int(L)


        ;st(0) = 2^(frac(L)) * 2^int(L) = 2^L = 2^(y*log2(e)) = e^y = e^(sin(ctg(x + 5pi))^2)

        fst qword ptr[answ] ;сохранение результата
    }

    // Вывод результата
    cout << "ASM: " << answ << endl;
    cout << "c++: " << fun(x) << endl;
}
