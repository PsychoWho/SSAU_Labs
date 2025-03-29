#include <iostream>

using namespace std;

/*Найти минимальный элемент в массиве A={a[i]}
и подсчитать количество его вхождений.*/

void fun(int size, int* array) {
    int min = array[0];
    int count = 1;

    for (int i = 1; i < size; i++) {
        if (array[i] < min) {
            min = array[i];
            count = 0;
        }
        if (array[i] == min) count++;
    }   

    cout << "min: " << min << " | count: " << count << endl;
}

void main()
{
    int s = 0; 
    int minimum = 0;
    int count = 0;
    do {
        cout << "Enter size > ";
        cin >> s;
    } while (s <= 0);
    int* array = new int[s];
    for (int i = 0; i < s; i++) {
        cout << "Enter array[" << i << "] > ";
        cin >> array[i];
    }

    _asm {
        xor esi, esi;       подготовим регистр индекса в массиве
        xor edi, edi;       счётчик количества элементов
        mov ebx, array;     ebx указывает на начало массива
        mov ecx, s;         счётчик цикла по всем элементам массива

        jecxz fin;          завершить если длина массива 0

        mov edx, [ebx + esi * 4];   определяем элемент для сравнения(1 элемент массива)
        inc esi;                    индекс на 1
        inc edi;                    кол - во вхождений данного минимума
        dec ecx;                    уменьшаем счётчик, так как первый элемент уже обработан

    
    begin_loop:
        mov eax, [ebx + esi * 4];   определяем текущий эл-т
        cmp eax, edx;               сравниваем новое значение с текущим минимумом
        jl new_min;                 если новое значение меньше текущего минимума, то переходим для обновления минимума
        je repeat_min;              если новое значение совподщает с минимумом, то переходим для увеличения счётчика
        jmp next_index;             переход для увеличения индекса 
                
    new_min:
        mov edx, eax;   обнавляем минимум
        mov edi, 1;     сбрасываем счётчик
        jmp next_index; переход для увеличения индекса
        
    repeat_min:
        inc edi;        прибавляем 1 к счётчику
        jmp next_index; переход для увеличения индекса

    next_index:
        inc esi;            прибавляем 1 к индексу
        loop begin_loop;    переход в цикл
        
    fin:
        mov minimum, edx;   записываем минимум в ответ
        mov count, edi;     записываем кол-во вхождений в ответ
    }

    cout << "asm >> " << "min: " << minimum << " | count: " << count << endl;
    cout << "c++ >> ";
    fun(s, array);
    delete[] array;
}