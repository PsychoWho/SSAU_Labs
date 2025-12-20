#include <iostream>

using namespace std;

struct Point {
    double x;
    double y;
};

Point operator*(const Point& p, double d) {
    return Point{ p.x * d, p.y * d };
}

Point operator+(const Point& p, const Point& g) {
    return Point{ p.x + g.x, p.y + g.y };
}

Point operator-(const Point& p, const Point& g) {
    return Point{ p.x - g.x, p.y - g.y };
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

double fun(const Point& p) {
    return pow(p.x + 2 * p.y - 7, 2) + pow(2 * p.x + p.y - 5, 2);
}


Point search(Point p, double step) {
    Point res = p;

    //x
    Point t = res;
    t.x += step;
    if (fun(t) < fun(res)) {
        res = t;
    }
    else {
        t = res;
        t.x -= step;
        if (fun(t) < fun(res)) {
            res = t;
        }
    }


    //y
    t = res;
    t.y += step;
    if (fun(t) < fun(res)) {
        res = t;
    }
    else {
        t = res;
        t.y -= step;
        if (fun(t) < fun(res)) {
            res = t;
        }
    }

    return res;
}


void hooke_jeeves(Point start, double e, int max_iter_hj, double start_step, double beta) {
    int iter = 0;
    double step = start_step;

    Point p = start;
    Point base;

    do
    {
        iter++;

        base = p;

        p = search(base, step);

        if (fun(p) < fun(base)) {

            Point test = p;
            test = test + (p - base);
            Point t = search(test, step);

            if (fun(t) < fun(p)) {
                base = t;
            }
            else
            {
                base = p;
            }
        }
        else {
            step *= beta;
        }

        if (iter >= max_iter_hj) {
            cout << "\nПревышен предел итераций метода! (" << max_iter_hj << ")";
            break;
        }

    } while (step > e);

    cout << "\n=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=\n";
    cout << "\nМ-д Хука-Дживса:   " << base;
    cout << "\nКол-во итераций метода: " << iter;
    cout << "\n\nF" << base << " = " << fun(base) << endl;
    cout << "\n=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=\n";
}


int main()
{
    setlocale(LC_ALL, "Russian");

    Point a = { 0,0 };
    
    hooke_jeeves(a, 1e-6, 1000, 10, 0.5);
}
