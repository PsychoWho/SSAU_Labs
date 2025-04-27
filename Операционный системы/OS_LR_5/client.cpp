#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#define BUFSIZE 1024

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main()
{
    WSADATA wsaData;
    WORD DLLVersion = MAKEWORD(2, 2);
    if (WSAStartup(DLLVersion, &wsaData) != 0) {
        cerr << "WSAStartup failed" << endl;
        return 1;
    }

    SOCKADDR_IN addr;
    static int sizeOfAddr = sizeof(addr);
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Socket creation error: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    if (connect(clientSocket, (sockaddr*)&addr, sizeOfAddr) == SOCKET_ERROR) {
        cerr << "Connection failed: " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    cout << "<!>	<op> <num> <num>	| expression for calculation" << endl
         << "<!>	exit		    	| exiting the program" << endl;

    string input;
    char buffer[BUFSIZE];

    while (true) {
        cout << "> ";
        getline(cin, input);

        if (input == "exit") {
            break;
        }

        // Отправка данных серверу
        send(clientSocket, input.c_str(), input.size() + 1, 0);

        // Получение ответа от сервера
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            cerr << "Server disconnected" << endl;
            break;
        }

        buffer[bytesReceived] = '\0';
        cout << "Server response: " << buffer << endl;
    }

    // Закрытие соединения
    closesocket(clientSocket);
    WSACleanup();

    return 0;

}
