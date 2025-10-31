#include <iostream>
#include <math.h>

using namespace std;

int iter;

//точка
struct Point {
    double x;
    double y;

    double norm() { return sqrt(x * x + y * y); }
};

Point operator+(const Point& p, double d) {
    return Point{ p.x + d, p.y + d };
}

Point operator+(const Point& p, const Point& g) {
    return Point{ p.x + g.x, p.y + g.y };
}

Point operator-(const Point& p, double d) {
    return Point{ p.x - d, p.y - d };
}

Point operator-(const Point& p, const Point& g) {
    return Point{ p.x - g.x, p.y - g.y };
}

Point operator/(const Point& p, double d) {
    return Point{ p.x / d, p.y / d };
}

Point operator*(const Point& p, double d) {
    return Point{ p.x * d, p.y * d };
}

Point operator*(const Point& p, const Point& d) {
    return Point{ p.x * d.x, p.y * d.y };
}

ostream& operator<<(ostream& out, const Point& p) {
    out << "(" << p.x << ", " << p.y << ")";
    return out;
}

double norm(Point& a, Point& b) {
    double x = abs(a.x - b.x);
    double y = abs(a.y - b.y);
    return sqrt(x * x + y * y);
}


//функция (с минимумом в (1,3))
double fun(Point p) {
    iter++;
    return pow(p.x + 2 * p.y - 7, 2) + pow(2 * p.x + p.y - 5, 2);
}


//методы
Point half2D(Point A, Point B, double e) {
    iter = 0;

    Point d = {
        e * (B.y - A.y) / norm(A,B),
        e * (B.x - A.x) / norm(A,B)
    };

    while (norm(A,B) > 2 * d.norm()) {
        Point C = (A + B) / 2;

        if (fun(C - d) < fun(C + d)) {
            B = C;
        }
        else {
            A = C;
        }
    }
    return (A + B) / 2;
}


Point golden2D(Point A, Point B, double e) {
    iter = 0;

    Point d = {
        e * (B.y - A.y) / norm(A,B),
        e * (B.x - A.x) / norm(A,B)
    };

    double gold = 0.382;

    Point Xl = A + (B - A) * gold;
    Point Xr = B - (B - A) * gold;
    double fl = fun(Xl);
    double fr = fun(Xr);
    
    while (norm(A, B) > d.norm()) {
        if (fl < fr) {
            B = Xr;
            Xr = Xl;
            fr = fl;
            Xl = A + (B - A) * gold;
            fl = fun(Xl);
        }
        else {
            A = Xl;
            Xl = Xr;
            fl = fr;
            Xr = B - (B - A) * gold;
            fr = fun(Xr);
        }
    }

    return (A + B) / 2;
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

void fibonacci2D(Point A, Point B, int n) {
    iter = 0;

    Point* L = new Point[n + 1];

    L[1] = B - A;

    Point e = L[1] / fib(n);

    L[n] = L[1] / fib(n) + e * fib(n - 2) / fib(n);

    for (int i = 1; i < n - 1; i++) {
        L[n - i] = L[n] * fib(i + 1) - e * fib(i - 1);
    }

    Point X1 = B - L[2];
    Point X2 = A + L[2];
    double f1 = fun(X1);
    double f2 = fun(X2);

    for (int k = 3; k <= n; k++) {
        if (f1 < f2) {
            B = X2;
            X2 = X1;
            f2 = f1;
            X1 = B - L[k];
            f1 = fun(X1);
        }
        else {
            A = X1;
            X1 = X2;
            f1 = f2;
            X2 = A + L[k];
            f2 = fun(X2);
        }
    }
    if (f1 < f2) {
        B = X2;
    }
    else {
        A = X1;
    }

    Point D = (A + B) / 2;

    cout << "\nМетод фибоначи: " << D;
    cout << "\nКол-во вызовов ф-и:     " << iter;
    cout << "\nF" << D << " = " << fun(D);
    cout << "\ne:     " << e;
    cout << "\nx2-x1: " << X2 - X1;
    cout << "\nb-a:   " << norm(A,B);
    cout << "\nL[" << n << "]: " << L[n].norm() << endl;
}



int main()
{
    setlocale(LC_ALL, "Russian");

    Point A{ 0,0};
    Point B{ 3,9 };
    double e = 1e-5;

    cout << "Точность: " << e << endl;
    
    Point half_answ = half2D(A, B, e);
    cout << "\nМетод дихотомии:    " << half_answ;
    cout << "\nКол-во вызовов ф-и: " << iter;
    cout << "\nF" << half_answ << " = " << fun(half_answ) << endl;
    
    Point gold_answ = golden2D(A, B, e);
    cout << "\nМетод золотого сечения: " << gold_answ;
    cout << "\nКол-во вызовов ф-и:     " << iter;
    cout << "\nF" << gold_answ << " = " << fun(gold_answ) << endl;

    fibonacci2D(A,B,iter-1);
}
