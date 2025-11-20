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


double fun(Point p) {
    iter++;
    //функция (с минимумом в (1,3))
    return pow(p.x + 2 * p.y - 7, 2) + pow(2 * p.x + p.y - 5, 2);
    
    // Минимум в (2, -1), значение 0
    //return pow(p.x - 2, 2) + pow(p.y + 1, 2);
}

Point grad(Point p, double h) {
    double dx = (fun(Point{ p.x + h ,p.y }) - fun(Point{ p.x - h ,p.y })) / (2 * h);
    double dy = (fun(Point{ p.x ,p.y + h }) - fun(Point{ p.x ,p.y - h })) / (2 * h);
    return Point{ dx,dy };
}

Point grad(Point p) {
    double dx = 2 * (p.x + 2 * p.y - 7) + 4 * (2 * p.x + p.y - 5);
    double dy = 4 * (p.x + 2 * p.y - 7) + 2 * (2 * p.x + p.y - 5);
    return Point{ dx,dy };
}

//двумерный метод золотого сечения
Point golden2D(Point A, Point B, double e) {

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

void coordinatDescent2D(Point start, double e, double r, int max_iter_cd) {
    int iter_cd = 0;
    iter = 0;

    Point p = start;
    Point old;

    Point ex = { e, 0 };
    Point ey = { 0, e };

    Point rx = { r, 0 };
    Point ry = { 0, r };

    do
    {
        old = p;

        //x
        Point x;
        if (fun(p - ex) < fun(p + ex)) {
            x = golden2D(p - rx, p, e);
        }
        else {
            x = golden2D(p, p + rx, e);
        }
        p.x = x.x;

        //y
        Point y;
        if (fun(p - ey) < fun(p + ey)) {
            y = golden2D(p - ry, p, e);
        }
        else {
            y = golden2D(p, p + ry, e);
        }
        p.y = y.y;

        iter_cd++;

        if (iter_cd >= max_iter_cd) {
            cout << "\nПревышен предел итераций метода! (" << max_iter_cd << ")";
            break;
        }

    } while (norm(old, p) > e);

    cout << "\n=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=\n";
    cout << "\nПокоординатный спуск:   " << p;
    cout << "\nКол-во вызовов функции: " << iter;
    cout << "\nКол-во итераций метода: " << iter_cd;
    cout << "\n\nF" << p << " = " << fun(p) << endl;
}

void gradientDescent2D(Point start, double e, int max_iter_gd) {
    int iter_gd = 0;

    Point p = start;
    Point old;

    do
    {
        iter_gd++;

        old = p;

        Point gr = grad(p, 0.1);
        p = old - gr*0.01;

        if (iter_gd >= max_iter_gd) {
            cout << "\nПревышен предел итераций метода! (" << max_iter_gd << ")";
            break;
        }

    } while (norm(old, p) > e);

    cout << "\n=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=\n";
    cout << "\nГрадиентный спуск:    " << p;
    cout << "\nКол-во итераций м-да: " << iter_gd;
    cout << "\n\nF" << p << " = " << fun(p) << endl;
}

void conjugateGradient2D(Point start, double e, int max_iter_сg) {
    int iter_cg = 1;

    Point p = start;
    Point old;
    Point old_gr;
    
    Point gr = grad(p, 0.1);
    Point s = gr * (-1.0);

    do
    {
        iter_cg++;

        old = p;
        old_gr = gr;

        p = old + s * 0.01;

        gr = grad(p, 0.1);

        double w = pow(gr.norm(), 2) / pow(old_gr.norm(), 2);
        
        s = gr * (-0.1) + s * w;

        if (iter_cg >= max_iter_сg) {
            cout << "\nПревышен предел итераций метода! (" << max_iter_сg << ")";
            break;
        }

    } while (norm(old, p) > e);

    cout << "\n=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=\n";
    cout << "\nСопряжённый градиент: " << p;
    cout << "\nКол-во итераций м-да: " << iter_cg;
    cout << "\n\nF" << p << " = " << fun(p) << endl;
}


int main()
{
    setlocale(LC_ALL, "Russian");

    Point A = { 2, 4};
    double e = 1e-6;
    int max_iter = 100000;
    double r = 2;

    cout << "Точность:        " << e << endl;
    cout << "Начальная точка: " << A << endl;
    coordinatDescent2D(A, e, r, max_iter);
    gradientDescent2D(A, e, max_iter);
    conjugateGradient2D(A, e, max_iter);
    cout << "\n=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=\n";

}
