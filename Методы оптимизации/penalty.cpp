#include <iostream>
#include <math.h>
#include <functional>

using namespace std;

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

double fun(Point p) {
    //функция (с минимумом в (1,3))
    return pow(p.x + 2 * p.y - 7, 2) + pow(2 * p.x + p.y - 5, 2);
}

//градиет функции со штрафом 
Point grad(Point p, double r, double h, function<double(Point, double)> func) {
    double dx = (func(Point{ p.x + h,p.y }, r) - func(Point{ p.x - h,p.y }, r)) / (2 * h);
    double dy = (func(Point{ p.x,p.y + h }, r) - func(Point{ p.x,p.y - h }, r)) / (2 * h);
    return Point{ dx,dy };
}

Point gradientDescent(Point start, double e, int max_iter_gd, double r, function<double(Point, double)> fun) {
    int iter_gd = 0;

    Point p = start;
    Point old;
    Point gr;

    do
    {
        iter_gd++;

        old = p;

        gr = grad(p, r, 1e-5, fun);
        p = old - gr * 0.01;

        if (iter_gd >= max_iter_gd) {
            break;
        }

    } while (norm(old, p) > e);

    return p;
}

// ------ Метод внешнего штрафа ------

//штрафная функция
double barrierFunction(Point p, double r) {
    /*
    Пусть ограничения будут такие:
        1.5 <= x <= 2
    */
    double bar = 0.0;

    if (p.x < 1.5) bar += pow(1.5 - p.x, 2);
    if (p.x > 2) bar += pow(p.x - 2, 2);

    return r * bar;

}

//целевая со штрафом
double barFun(Point p, double r) {
    return fun(p) + barrierFunction(p, r);
}

//сам метод (с градиентным спуском)
void barrierMethod(Point start, double e, int max_iter, double start_r, double beta) {
    int iter = 0;
    double r = start_r;
    double old_r;
    Point p = start;
    Point old;

    bool constraints_violated;

    do
    {
        iter++;

        old_r = r;
        old = p;

        p = gradientDescent(old, e, 100, old_r, barFun);

        r = beta * old_r;

        if (iter >= max_iter) {
            cout << "\nПревышен предел итераций метода! (" << max_iter << ")";
            break;
        }

    } while (barrierFunction(p, old_r) > e);

    cout << "\n=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=\n";
    cout << "\nНачальная точка: " << start;
    cout << "\n\nОграничения:" << "\n 1.5 <= x <= 2";
    cout << "\n\nМ-д внешнего штрафа:  " << p;
    cout << "\nКол-во итераций м-да: " << iter;
    cout << "\n\nF" << p << " = " << fun(p) << endl;
}


// ------ Метод внутреннего штрафа ------

//штрафная функция
double penaltyFunction(Point p, double r) {
    /*
    Пусть ограничения будут такие:
        1.5 <= x <= 2
    */
    double pen = 0.0;

    if (p.x < 1.5 || p.x > 2.0) {
        return 1e100;
    }

    pen += -log(p.x - 1.5);
    pen += -log(2.0 - p.x);

    return r * pen;

}

//целевая со штрафом
double penFun(Point p, double r) {
    return fun(p) + penaltyFunction(p, r);
}

//сам метод (с градиентным спуском)
void penaltyMethod(Point start, double e, int max_iter, double start_r, double beta) {
    int iter = 0;
    double r = start_r;
    double old_r;
    Point p = start;
    Point old;

    bool constraints_violated;

    do
    {
        iter++;

        old_r = r;
        old = p;

        p = gradientDescent(old, e, 100, old_r, penFun);

        r = beta * old_r;

        if (iter >= max_iter) {
            cout << "\nПревышен предел итераций метода! (" << max_iter << ")";
            break;
        }

    } while (norm(old, p) > e);

    cout << "\n=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=\n";
    cout << "\nНачальная точка: " << start;
    cout << "\n\nОграничения:" << "\n 1.5 <= x <= 2";
    cout << "\n\nМ-д внутреннего штрафа:  " << p;
    cout << "\nКол-во итераций м-да:  " << iter;
    cout << "\n\nF" << p << " = " << fun(p) << endl;
}



int main()
{
    setlocale(LC_ALL, "Russian");

    double e = 1e-6;
    int max_iter = 10000;

    cout << "Точность: " << e << endl;

    Point A = { 30 , 20 };
    barrierMethod(A, e, max_iter, 1, 3);

    Point B = { 1.8 , 4 };
    penaltyMethod(B, e, max_iter, 1, 0.5);

    cout << "\n=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=\n";
}
