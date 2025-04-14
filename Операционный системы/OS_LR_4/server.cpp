#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>

#define BUFSIZE 1024
#define INSTANCES 1
#define PIPE_TIMEOUT 5000

using namespace std;

//проверка выражения на деление на 0
bool isValid(const string& str) {
	for (int i = 0; i < str.size()-1; i++) {
		if (str[i] == '/' && str[i + 1] == '0') return false;
	}
	return true;
}

//проверка символа на операцию
bool isOperation(char sym) {
	switch (sym)
	{
	case '+':
	case '-':
	case '/':
	case '*':
		return true;
	default:
		return false;
	}
}

//проверка на корректный ввод в выражение
bool isCorrect(const string& str) {
	int point = 0;
	if (!isOperation(str[0])) return false;
	for (int i = 1; i < str.size(); i++) {
		if (!isdigit(str[i])) {
			if (point == 0 && i > 1 && str[i] == ',') point++;
			else return false;		
		}	
						
	}
	return true;
}

//вычисление части выражения 
double calc(double a, double b, char op) {
	switch (op) {
	case '+': return a + b;
	case '-': return a - b;
	case '*': return a * b;
	case '/': return a / b;
	default: return 0;
	}
}

//разделение строки выражения на последовательнось операций и чисел
vector<string> tokenize(const string& expr) {
	vector<string> tokens;
	string current;
	for (char c : expr) {
		if (isOperation(c)) {
			if (!current.empty()) {
				tokens.push_back(current);
				current.clear();
			}
			tokens.push_back(string(1, c));
		}
		else {
			current += c;
		}
	}
	if (!current.empty()) {
		tokens.push_back(current);
	}
	return tokens;
}

//вычисление выражения
string evaluateExpression(const string& expr) {
	vector<string> tokens = tokenize(expr);
	if (tokens.empty()) return "0";

	vector<double> numbers;
	vector<char> ops;

	numbers.push_back(0);

	// Разделение чисел и операций
	for (int i = 0; i < tokens.size(); ) {
		char op = tokens[i][0];
		double num = stod(tokens[i + 1]);
		ops.push_back(op);
		numbers.push_back(num);
		i += 2;
	}

	// Обработка умножения и деления
	for (int j = 0; j < ops.size(); ) {
		char op = ops[j];
		if (op == '*' || op == '/') {
			double a = numbers[j];
			double b = numbers[j + 1];
			double res = calc(a, b, op);
			numbers[j] = res;
			numbers.erase(numbers.begin() + j + 1);
			ops.erase(ops.begin() + j);
		}
		else {
			j++;
		}
	}

	// Обработка сложения и вычитания
	double result = numbers[0];
	for (int j = 0; j < ops.size(); j++) {
		result = calc(result, numbers[j + 1], ops[j]);
	}

	return to_string(result);
}

int main() {
	setlocale(LC_ALL, "Russian");

	HANDLE hPipe;
	char buffer[BUFSIZE];
	DWORD dwRead, dwWritten;
	bool fSuccess;
	bool connect = false;
	string expression;

	hPipe = CreateNamedPipe(
		TEXT("\\\\.\\pipe\\mynamedpipe"),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		INSTANCES,
		BUFSIZE * 4,
		BUFSIZE * 4,
		PIPE_TIMEOUT,
		NULL);

	if (hPipe == INVALID_HANDLE_VALUE) {
		cout << "ОШИБКА. именованный канал не создан. GLE=" << GetLastError() << endl;
		return -1;
	}

	cout << "Ожидаем клиента..." << endl;

	connect = ConnectNamedPipe(hPipe, NULL) ? true : (GetLastError() == ERROR_PIPE_CONNECTED);

	if (!connect) {
		cout << "ОШИБКА. клиент не подключён. GLE=" << GetLastError() << endl;
		CloseHandle(hPipe);
		return -1;
	}

	cout << "Клиент подключён" << endl;

	while (true) {
		fSuccess = ReadFile(
			hPipe,
			buffer,
			BUFSIZE,
			&dwRead,
			NULL);

		if (!fSuccess || dwRead == 0) {
			if (GetLastError() == ERROR_BROKEN_PIPE) {
				cout << "Клиент отключился." << endl;
			}
			else {
				cout << "ОШИБКА чтения. GLE=" << GetLastError() << endl;
			}
			break;
		}

		buffer[dwRead] = '\0';
		string input(buffer);

		cout << "Получено от клиента: " << input << endl;

		if (input == "calc") {
			string answ = isValid(expression) ? evaluateExpression(expression) : "!есть деление на 0!";

			fSuccess = WriteFile(
				hPipe,
				answ.c_str(),
				answ.size(),
				&dwWritten,
				NULL);
		}
		else if (input == "clear") {
			expression.clear();
			string answ = "Выражение очищено";
			fSuccess = WriteFile(
				hPipe,
				answ.c_str(),
				answ.size(),
				&dwWritten,
				NULL);
		}
		else if (input == "valid") {
			string answ = isValid(expression) ? "Выражение корректно" : "Выражение некорректно";
			fSuccess = WriteFile(
				hPipe,
				answ.c_str(),
				answ.size(),
				&dwWritten,
				NULL);
		}
		else {
			if (isCorrect(input)) {
				expression += input;
				fSuccess = WriteFile(
					hPipe,
					expression.c_str(),
					expression.size(),
					&dwWritten,
					NULL);				
			}
			else {
				string answ = "Выражение некорректно";
				fSuccess = WriteFile(
					hPipe,
					answ.c_str(),
					answ.size(),
					&dwWritten,
					NULL);
			}	
		}

		if (!fSuccess) {
			cout << "ОШИБКА записи. GLE=" << GetLastError() << endl;
		}
	}

	CloseHandle(hPipe);
	return 0;
}
