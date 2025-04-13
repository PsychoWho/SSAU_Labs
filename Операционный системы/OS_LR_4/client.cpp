#include <Windows.h>
#include <iostream>
#include <string>

#define BUFSIZE 1024

using namespace std;

void option() {
	cout << ">	<знак операции><число>	|	Добавление в выражение" << endl
		 << ">	calc			|	Вычисление выражения" << endl
		 << ">	clear			|	Очищение выражения" << endl
		 << ">	valid			|	Проверка выражения" << endl
		 << ">	exit			|	Закрытие программы" << endl;
}

int main() {
	setlocale(LC_ALL, "Russian");

	HANDLE hPipe;
	bool fSuccess = false;
	DWORD dwRead;
	char buffer[BUFSIZE];

	while (true) {
		hPipe = CreateFile(TEXT("\\\\.\\pipe\\mynamedpipe"),
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);

		if (hPipe != INVALID_HANDLE_VALUE) break;

		if (GetLastError() != ERROR_PIPE_BUSY) {
			cerr << "ОШИБКА. Не удалось подключиться к каналу. GLE=" << GetLastError() << endl;
			return -1;
		}

		if (!WaitNamedPipe(TEXT("\\\\.\\pipe\\mynamedpipe"), 20000)) {
			cerr << "ОШИБКА. Не удалось дождаться освобождения канала" << endl;
			return -1;
		}
	}

	cout << ">>>	подключёно к серверу	<<<" << endl;
	option();
	cout << "Вводие команды: " << endl;

	string input;
	while (true){
		cout << "> ";
		getline(cin, input);
		if (input.empty()) continue;

		if (input == "exit") break;

		//данные отправляются на сервер
		fSuccess = WriteFile(
			hPipe,
			input.c_str(),
			input.size(),
			&dwRead,
			NULL);

		if (!fSuccess) {
			cout << "ОШИБКА записи. GLE=" << GetLastError() << endl;
		}

		//читаем данные с сервера
		fSuccess = ReadFile(
			hPipe,
			buffer,
			BUFSIZE,
			&dwRead,
			NULL);

		if (!fSuccess || dwRead == 0) {
			if (GetLastError() == ERROR_BROKEN_PIPE) {
				cout << "ОШИБКА. сервер отключился." << endl;
			}
			else {
				cout << "ОШИБКА чтения. GLE=" << GetLastError() << endl;
			}
			break;
		}
		
		buffer[dwRead] = '\0';
		cout << "сервер > " << buffer << endl;
	}

	CloseHandle(hPipe);
	return 0;
}
