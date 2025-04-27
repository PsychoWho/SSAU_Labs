#include <iostream>
#include <string>
#include <sstream>
#include <winsock2.h>
#include <vector>
#include <thread>

#define MAX_CLIENT 2
#define BUFSIZE 1024

#pragma comment(lib, "ws2_32.lib")

using namespace std;

HANDLE hSemaphore;

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

//проверка строки на число
bool isNumber(string str){
	int point = 0;
	if (str.empty()) return false;
	for (int i = 0; i < str.size(); i++) {
		if (!isdigit(str[i])) {
			if (point == 0 && i > 0 && str[i] == '.') point++;
			else return false;
		}
	}
	return true;
}

//вычисление части выражения 
double calc(char op, double a, double b) {
	switch (op) {
	case '+': return a + b;
	case '-': return a - b;
	case '*': return a * b;
	case '/': return a / b;
	default: return 0;
	}
}

//основаная чать программы
string prog(const string& exp) {
	istringstream iss(exp);
	char op;
	string num1, num2;
	iss >> op >> num1 >> num2;

	if (isOperation(op) && isNumber(num1) && isNumber(num2)) {
		double a = stod(num1);
		double b = stod(num2);
		if (op == '/' && b == 0) return "Error! Division by zero.";
		double res = calc(op, a, b);
		return to_string(res);
	}
	else return "Error! Incorrect input.";
}


void handleClient(SOCKET clientSocket, int client) {
	char buffer[BUFSIZE];
	int bytesReceived;

	while (true) {
		bytesReceived = recv(clientSocket, buffer, BUFSIZE, 0);
		if (bytesReceived <= 0) {
			cout << "Client #" << client << " disconnected." << endl;
			break;
		}

		buffer[bytesReceived] = '\0';
		string exp(buffer);
		string res = prog(exp);

		send(clientSocket, res.c_str(), res.size() + 1, 0);

		cout << "CReceived from client #" << client << " operation (" << exp << "). Result: " << res << endl;
	}

	closesocket(clientSocket);
	ReleaseSemaphore(hSemaphore, 1, NULL);
}

int main()
{
	WSADATA wsaData;
	WORD DLLVersion = MAKEWORD(2, 2);
	if(WSAStartup(DLLVersion, &wsaData) != 0) {
		cerr << "WSAStartup failed" << endl;
		return 1;
	}

	SOCKADDR_IN addr;
	static int sizeOfAddr = sizeof(addr);
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET) {
		cerr << "Socket creation error: " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	if (bind(serverSocket, (SOCKADDR*)&addr, sizeOfAddr)) {
		cerr << "Bind failed: " << WSAGetLastError() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
		cerr << "Listen failed: " << WSAGetLastError() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	hSemaphore = CreateSemaphore(NULL, MAX_CLIENT, MAX_CLIENT, L"semaphore");
	if (hSemaphore == NULL) {
		cerr << "CreateSemaphore error: " << GetLastError() << endl;
		WSACleanup();
		return 1;
	}

	cout << "WAITING for connections......" << endl;

	vector<thread> clientThreads;
	int clientCount = 0;

	while (true) {
		WaitForSingleObject(hSemaphore, INFINITE);

		SOCKET clientSocket = accept(serverSocket, (SOCKADDR*)&addr, &sizeOfAddr);
		if (clientSocket == INVALID_SOCKET) {
			cerr << "Accept failed: " << WSAGetLastError() << endl;
			ReleaseSemaphore(hSemaphore, 1, NULL);
			continue;
		}

		clientCount++;
		cout << "Client #" << clientCount << " connected." << endl;

		clientThreads.emplace_back(handleClient, clientSocket, clientCount);
	}

	for (auto& t : clientThreads) {
		if (t.joinable()) t.join();
	}

	closesocket(serverSocket);
	WSACleanup();
	CloseHandle(hSemaphore);

	return 0;
}
