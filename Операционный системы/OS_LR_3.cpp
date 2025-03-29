#include <iostream>
#include <string>
#include <unistd.h>
#include <cmath>

using namespace std;

const char SYMBOLS[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

struct ConversionData {
    char command;
    string number_is_base;
    int number_is_dec;
    int base;
};

int pipe_in[2];
int pipe_out[2];
pid_t pid;

/*------работа с массивом------*/
bool is_symbol(char sym, int base){
    for(int i = 0; i < base; i++){
        if(toupper(sym) == SYMBOLS[i]) return true;
    }
    return false;
}

bool is_number(string num, int base){
    for(int i = 0; i < num.length(); i++){
        if(!is_symbol(num[i], base)) return false;
    }
    return true;
}

int in_dec(char sym, int base){
    for(int i = 0; i < base; i++){
        if(toupper(sym) == SYMBOLS[i]) return i;
    }
    return -1;
}

/*------перевод в десятичную------*/
int to_dec(string num, int base){
    int dec = 0;
    int j = 0;
    for(int i = num.length()-1; i >= 0; i--){
        dec += in_dec(num[i], base) * pow(base, j);
        j++;
    }
    return dec;
}

/*------перевод в свою------*/
string to_base(int dec, int base){
    string bs = "";
    if(dec == 0) return "0";
    while (dec){
        bs = SYMBOLS[dec % base] + bs;
        dec /= base;
    }
    return bs;
}

/*------help------*/
void help(){
    cout << "Доступные опции:\n"
         << ">   --help     |  Показать список доступных опциий\n"
         << ">   --dec      |  Переводит число в десятичную систему\n"
         << ">   --base     |  Переводит десятичное число в новою систему\n"
         << ">   --exit     |  Выход из программы\n\n";
}

/*------чтение данных------*/
int read_int(const string& msg){
    int res;
    bool flag = true;
    while (flag)
    {
        cout << msg;
        cin >> res;
        if((res <=0) || (cin.fail() || (cin.peek() != '\n'))){
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Ошибка! Введите положительное число\n";
        }else{
            flag = false;
        }
    }
    return res;
}

int read_base(const string& msg){
    int res;
    bool flag = true;
    while(flag){
        cout << msg;
        cin >> res;
        if(((res < 2) || (res > 36)) || ((cin.fail()) || (cin.peek() != '\n'))){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Ошибка! Число в не отрезка [2,36]\n";
        }
        else flag = false;
    }
    return res;
}

string read_string(const string& msg, int base){
    string res;
    bool flag = true;
    while(flag){
        cout << msg;
        cin >> res;
        if(is_number(res,base)){
            flag = false;
        }
        else {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Ошибка! Не является числом в системе (" <<  base << ")\n";
        }
    }
    return res;
}

/*------клиентская часть------*/
void frontend() {
    close(pipe_in[0]);
    close(pipe_out[1]);

    ConversionData data;
    string command;
    bool error = true;
    while(error) {
        error = false;
        cout << "Введите команду > ";
        cin >> command;
        if (command == "--help") {
            help();
            error = true;
        }
        else if (command == "--dec") {
            cout << ">!> основание должно принадлежать отрезку [2, 36]\n"
                 << ">!> число может состоять из цифр 0...9 и букв A...Z в зависимости от основания\n"
                 << "Введите основание системы исчисления числа и число для перевода десятичную:\n";
            data.base = read_base("| система исчисления > ");
            data.number_is_base = read_string("| число > ", data.base);
            data.command = 'd';
            write(pipe_in[1], &data, sizeof(data));
            int res;
            read(pipe_out[0], &res, sizeof(int));
            cout << data.number_is_base << " (" << data.base << ") => " << res << " (10)\n";
        }
        else if (command == "--base") {
            cout << ">!> основание должно принадлежать отрезку [2, 36]\n"
                 << ">!> число может состоять из цифр 0...9\n"
                 << "Введите десятичное число и основание системы исчиления числа для перевода:\n";
            data.base = read_base("| система исчисления > ");
            data.number_is_dec = read_int("| число > ");
            data.command = 'b';
            write(pipe_in[1], &data, sizeof(data));
            string res;
            read(pipe_out[0], &res, sizeof(string));
            cout << data.number_is_dec << " (10) => " << res << " (" << data.base << ")\n";
        }
        else if (command == "--exit") exit(0);
        else {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Неверная команда. Попробуйте --help\n";
            error = true;
        }
    }

    close(pipe_in[1]);
    close(pipe_out[0]);
    exit(0);
}

/*------серверная часть------*/
void backend(){
    close(pipe_in[1]);
    close(pipe_out[0]);

    ConversionData data;
    read(pipe_in[0], &data, sizeof(ConversionData));
    if(data.command == 'd'){
        int res_d = to_dec(data.number_is_base, data.base);
        write(pipe_out[1], &res_d, sizeof(int));
    }
    if(data.command == 'b'){
        string res_b = to_base(data.number_is_dec, data.base);
        write(pipe_out[1], &res_b, sizeof(string));
    }

    close(pipe_in[0]);
    close(pipe_out[1]);
    exit(0);
}

int main() {
    help();

    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) {
        cerr << "Ошибка! Канал не был создан!\n";
        return 1;
    }

    pid = fork();
    if (pid < 0) {
        cerr << "Ошибка создания процесса.\n";
        return 1;
    } else if (pid > 0) {
        frontend();
    } else {
        backend();
    }

    for (int i = 0; i < 2; ++i) {
        close(pipe_in[i]);
        close(pipe_out[i]);
    }
}
