#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <ctime>

using namespace std;

const char *MOD[] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"};

/*------help------*/
void help(){
    cout << "Доступные опции:\n"
    << ">   --help                            |  Показать список доступных опциий\n"
    << ">   --copy  <источник> <назначение>   |  Скопировать файл из источника в назначение\n"
    << ">   --move  <источник> <назначение>   |  Переместить файл из источника в назначение\n"
    << ">   --info  <файл>                    |  Показать информацию о файле\n"
    << ">   --chmod <файл> <режим>            |  Изменить права файла (<режим> из 3-х цифр)\n";
}

/*------info------*/
void infoPermission(mode_t mod){
    int owner = ((mod >> 6) & 0x7);
    int group = ((mod >> 3) & 0x7);
    int other = (mod & 0x7);

    cout << MOD[owner] << MOD[group] << MOD[other] << " или " << owner << group << other;
}

void info(const string &file){
    struct stat file_info;

    if(stat(file.c_str(), &file_info) != 0) {
        cerr << "Не удалось получить информацию о файле '" << file << "'.\n";
    }

    cout << "Полученная информация (--info):"
            << "\n>  Фaйл                |  " << "'" << file << "'"
            << "\n>  Размер              |  " << file_info.st_size
            << "\n>  Права доступа       |  ";
    infoPermission(file_info.st_mode);
    cout    << "\n>  Последнее изменение |  " << ctime(&file_info.st_mtime);
}

/*------move------*/
void move(const string &fileA, const string &fileB){
    cout << "Перемещение файла (--move).  Файл А => Файл В:\n";
    if(rename(fileA.c_str(), fileB.c_str()) == 0){
        cout << "Файл А '" << fileA << "' был успешно перемещён в файл В '" << fileB << "'.\n";
    }
    else cerr << "Ошибка перемещения! Файл А '" << fileA << "' не был перемещён в файл В '" << fileB << "'.\n";

}

/*------chmod------*/
bool is_oct_number(const string &str){
    for(int i = 0; i < str.length(); i++){
        if(!isdigit(str[i]) || str[i] == '8' || str[i] == '9') return false;
    }
    return true;
}

void chmod_per(const string &file, const string &mod){
    cout << "Изменение прав доступа (--chmod):\n";
    if (is_oct_number(mod)) {
        mode_t new_mod = stoi(mod, nullptr, 8);
        if (chmod(file.c_str(), new_mod) == 0) {
            cout << "Права доступа для файла '" << file << "' успешно изменены!\nНовые права: ";
            infoPermission(new_mod);
            cout << endl;
        } else cerr << "Ошибка! Права доступа для файла '" << file << "' не были изменены\n";
    }
    else {
        cerr << "Ошибка! <режим> '" << mod << "' не является восмеричным числом!\n"
        << "Права доступа для файла '" << file << "' не были изменены\n";
    }
}

/*------copy------*/
void copy(const string &fileA, const string &fileB){
    cout << "Копирование файла (--copy).  Файл А >>> Файл В:\n";
    if(fileA == fileB) {cerr << "Ошибка! Имена файлов совпадают! Копирование не было совершено!\n"; return;}

    ifstream fin;
    fin.open(fileA, ios::binary);
    if(!fin.is_open()){
        cerr << "Ошибка! Исходный файл А '" << fileA << "' не был открыт! Копирование не было совершено!\n";
        fin.close();
        return;
    }

    ofstream fout;
    fout.open(fileB, ios::binary);
    if(!fout.is_open()){
        cerr << "Ошибка! Файл назначения В '" << fileB << "' не был открыт! Копирование не было совершено!\n";
        fout.close();
        return;
    }

    const size_t bufSize = 32;
    char buffer[bufSize];
    while(!fin.eof()){
        fin.read(buffer, bufSize);
        fout.write(buffer, fin.gcount());
    }

    fin.close();
    fout.close();

    cout << "Исходный файл А '" << fileA << "' успешно скопирован в файл назначения В '" << fileB << "'\n";
}

/*------main------*/
int main(int argc, char* argv[]) {
    if(argc<2){cerr << "Нет аргументов! Попробуйте --help\n"; return 1;}

    string opt = argv[1];

    if(opt == "--help" && argc == 2) help();
    else if(opt == "--copy" && argc == 4) copy(argv[2], argv[3]);
    else if(opt == "--move" && argc == 4) move(argv[2], argv[3]);
    else if(opt == "--info" && argc == 3) info(argv[2]);
    else if(opt == "--chmod" && argc == 4) chmod_per(argv[2], argv[3]);
    else {cerr << "Ошибка аргументов! Попробуйте --help\n"; return 1;}
}