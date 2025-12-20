#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

using namespace std;

struct Point {
    double x;
    double y;
    double fitness;
};

ostream& operator<<(ostream& out, const Point& p) {
    out << "(" << p.x << ", " << p.y << ")";
    return out;
}

double fun(const Point& p) {
    return pow(p.x + 2 * p.y - 7, 2) + pow(2 * p.x + p.y - 5, 2);
}

double randomDouble(double min, double max) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

vector<Point> initializePopulation(int popSize, double minVal, double maxVal) {
    vector<Point> population(popSize);

    for (int i = 0; i < popSize; i++) {
        population[i].x = randomDouble(minVal, maxVal);
        population[i].y = randomDouble(minVal, maxVal);
        population[i].fitness = fun(population[i]);
    }

    return population;
}

//селекция
Point selection(const vector<Point>& population, int tournamentSize) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> dis(0, population.size() - 1);

    Point best = population[dis(gen)];

    for (int i = 1; i < tournamentSize; i++) {
        Point candidate = population[dis(gen)];
        if (candidate.fitness < best.fitness) {
            best = candidate;
        }
    }

    return best;
}

// Кроссовер (среднее арифметическое)
Point crossover(const Point& parent1, const Point& parent2, double crossoverRate) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);

    Point child;

    if (dis(gen) < crossoverRate) {
        double alpha = dis(gen); 
        child.x = alpha * parent1.x + (1 - alpha) * parent2.x;
        child.y = alpha * parent1.y + (1 - alpha) * parent2.y;
    }
    else {
        child = parent1;
    }

    child.fitness = fun(child);
    return child;
}

// Мутация
void mutate(Point& individual, double mutationRate, double minVal, double maxVal) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);

    // Мутируем x
    if (dis(gen) < mutationRate) {
        individual.x += randomDouble(-1.0, 1.0) * 0.1 * (maxVal - minVal);
        if (individual.x < minVal) individual.x = minVal;
        if (individual.x > maxVal) individual.x = maxVal;
    }

    // Мутируем y
    if (dis(gen) < mutationRate) {
        individual.y += randomDouble(-1.0, 1.0) * 0.1 * (maxVal - minVal);
        if (individual.y < minVal) individual.y = minVal;
        if (individual.y > maxVal) individual.y = maxVal;
    }

    individual.fitness = fun(individual);
}

Point geneticAlgorithm(int popSize, int generations,
    double crossoverRate, double mutationRate,
    double minVal, double maxVal) {

    vector<Point> population = initializePopulation(popSize, minVal, maxVal);

    Point bestSolution = population[0];
    for (const auto& p : population) {
        if (p.fitness < bestSolution.fitness) {
            bestSolution = p;
        }
    }

    for (int gen = 0; gen < generations; gen++) {
        vector<Point> newPopulation;

        while (newPopulation.size() < popSize) {
            Point parent1 = selection(population, 3);
            Point parent2 = selection(population, 3);

            Point child = crossover(parent1, parent2, crossoverRate);

            mutate(child, mutationRate, minVal, maxVal);

            newPopulation.push_back(child);

            if (child.fitness < bestSolution.fitness) {
                bestSolution = child;
            }
        }

        population = newPopulation;

        
        if (gen % 2 == 0) {
            cout << "Поколение " << gen << ": f("
                << bestSolution.x << ", " << bestSolution.y
                << ") = " << bestSolution.fitness << endl;
        }
    }

    return bestSolution;
}

int main() {

    setlocale(LC_ALL, "Russian");

    int popSize = 100;           
    int generations = 20;      
    double crossoverRate = 0.8; 
    double mutationRate = 0.1;  
    double minVal = -35.0;      
    double maxVal = 70.0;       

    cout << "Параметры:" << endl;
    cout << "- Размер популяции:       " << popSize << endl;
    cout << "- Поколений:              " << generations << endl;
    cout << "- Вероятность кроссовера: " << crossoverRate << endl;
    cout << "- Вероятность мутации:    " << mutationRate << endl;
    cout << "- Диапазон поиска:  [" << minVal << ", " << maxVal << "]" << endl;

    cout << "\n=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=\n";
    cout << "генетический метод\n" << endl;
    
    Point result = geneticAlgorithm(popSize, generations, crossoverRate,
        mutationRate, minVal, maxVal);

    cout << "\nНайденная точка: " << result << endl;
    cout << "Значение функции: " << result.fitness << endl;
    
    cout << "\n=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=\n";
}
