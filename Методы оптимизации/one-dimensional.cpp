#include <iostream>
#include <math.h>

using namespace std;

int iter;


double f(double x) {
    iter++;
    return 0.1*exp(pow(x+1, 2));
}


double half(double a, double b, double e) {
    iter = 0;
    while (abs(b - a) > 2*e) {
        double c = a + (b - a) / 2;

        if (f(c - e) < f(c + e)) {
            b = c;
        }
        else {
            a = c;
        }
    }
    return a + (b - a) / 2;
}

double golden(double a, double b, double e) {
    iter = 0;
    double gold = 0.382;
    double xleft = a + gold * (b - a);
    double xright = b - gold * (b - a);
    double fl = f(xleft);
    double fr = f(xright);

    while (abs(b - a) > e) {
        if (fl < fr) {
            b = xright;
            xright = xleft;
            fr = fl;
            xleft = a + gold * (b - a);
            fl = f(xleft);
        }
        else
        {
            a = xleft;
            xleft = xright;
            fl = fr;
            xright = b - gold * (b - a);
            fr = f(xright);
        }
    }
    return (a + b) / 2;
}

double fib(int n) {
    int a = 1;
    int b = 1;
    int c;

    for (int i = 2; i <= n; i++) {
        c = a + b;
        a = b;
        b = c;
    }
    return b;
}

void fibonacci(double a, double b, int n) {
    iter = 0;

    double* L = new double[n + 1];
        
    L[1] = b - a;

    double e = L[1] / fib(n);

    L[n] = L[1] / fib(n) + fib(n - 2) / fib(n) * e;

    for (int i = 1; i < n-1; i++) {
        L[n - i] = fib(i + 1) * L[n] - fib(i - 1) * e;
    }


    double x1 = b - L[2];
    double x2 = a + L[2];
    double f1 = f(x1);
    double f2 = f(x2);

    for (int k = 3; k <= n; k++) {
        if (f1 < f2) {
            b = x2;
            x2 = x1;
            f2 = f1;
            x1 = b - L[k];
            f1 = f(x1);
        }
        else
        {
            a = x1;
            x1 = x2;
            f1 = f2;
            x2 = a + L[k];
            f2 = f(x2);
        }
    }
    if (f1 < f2) {
        b = x2;
    }
    else
    {
        a = x1;
    }
    
    cout << endl << "Метод Фибоначчи: " << (a + b) / 2 <<
        endl << "Кол-во вызовов функции: " << iter <<
        endl << "e:         " << e <<
        endl << "x2-x1:     " << x2 - x1 <<
        endl << "(b-a)/2:   " << (b - a) / 2 <<
        endl << "b-a:       " << b - a <<
        endl << "L[" << n << "]:     " << L[n] << endl;

    delete[] L;
}

int main() {
    setlocale(LC_ALL, "Russian");
    
    double a = -2.5;
    double b = 4;
    double e = 1e-5;
    int fib_n = 21;

    cout << "Точность: " << e << endl;
    cout << endl << "Метод дихотомии: " << half(a, b, e) << endl << "Кол-во вызовов функции: " << iter << endl;
    cout << endl << "Метод золотого сечения: " << golden(a, b, e) << endl << "Кол-во вызовов функции: " << iter << endl;
    fibonacci(a, b, iter);
}
