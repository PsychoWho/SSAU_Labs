#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <winsock2.h>
#include <vector>
#include <thread>

#define MAX_CLIENT 2
#define BUFSIZE 1024

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "sfml-graphics.lib")
#pragma comment(lib, "sfml-window.lib")
#pragma comment(lib, "sfml-system.lib")

using namespace std;

HANDLE hSemaphore;
sf::Font font;
vector<string> logMessages;
bool serverRunning = true;

// Функция для добавления сообщения в лог
void logEvent(const string& message) {
    logMessages.push_back(message);
}

bool isOperation(char sym) {
    switch (sym) {
    case '+':
    case '-':
    case '/':
    case '*':
        return true;
    default:
        return false;
    }
}

bool isNumber(string str) {
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

double calc(char op, double a, double b) {
    switch (op) {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/': return a / b;
    default: return 0;
    }
}

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
            logEvent("Client #" + to_string(client) + " disconnected.");
            break;
        }

        buffer[bytesReceived] = '\0';
        string exp(buffer);
        string res = prog(exp);

        send(clientSocket, res.c_str(), res.size() + 1, 0);

        logEvent("Received from client #" + to_string(client) + ": " + exp + " = " + res);
    }

    closesocket(clientSocket);
    ReleaseSemaphore(hSemaphore, 1, NULL);
}

void serverThreadFunction(SOCKET serverSocket) {
    SOCKADDR_IN addr;
    static int sizeOfAddr = sizeof(addr);
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;

    if (bind(serverSocket, (SOCKADDR*)&addr, sizeOfAddr)) {
        logEvent("Bind failed");
        closesocket(serverSocket);
        WSACleanup();
        serverRunning = false;
        return;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        logEvent("Listen failed");
        closesocket(serverSocket);
        WSACleanup();
        serverRunning = false;
        return;
    }

    hSemaphore = CreateSemaphore(NULL, MAX_CLIENT, MAX_CLIENT, L"semaphore");
    if (hSemaphore == NULL) {
        logEvent("CreateSemaphore error");
        WSACleanup();
        serverRunning = false;
        return;
    }

    logEvent("Server started. Waiting for connections...");

    vector<thread> clientThreads;
    int clientCount = 0;

    while (serverRunning) {
        WaitForSingleObject(hSemaphore, INFINITE);

        SOCKET clientSocket = accept(serverSocket, (SOCKADDR*)&addr, &sizeOfAddr);
        if (clientSocket == INVALID_SOCKET) {
            logEvent("Accept failed");
            ReleaseSemaphore(hSemaphore, 1, NULL);
            continue;
        }

        clientCount++;
        logEvent("Client #" + to_string(clientCount) + " connected.");

        clientThreads.emplace_back(handleClient, clientSocket, clientCount);
    }

    for (auto& t : clientThreads) {
        if (t.joinable()) t.join();
    }

    closesocket(serverSocket);
    WSACleanup();
    CloseHandle(hSemaphore);
}

int main() {
    // Инициализация Winsock
    WSADATA wsaData;
    WORD DLLVersion = MAKEWORD(2, 2);
    if (WSAStartup(DLLVersion, &wsaData) != 0) {
        cerr << "WSAStartup failed" << endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Socket creation error" << endl;
        WSACleanup();
        return 1;
    }

    // Создание окна SFML
    sf::RenderWindow window(sf::VideoMode(800, 600), "Server GUI");
    window.setFramerateLimit(30);

    if (!font.loadFromFile("arial.ttf")) {
        // Если шрифт не загружен, используем стандартный
        font.loadFromFile("C:/Windows/Fonts/Arial.ttf");
    }

    // Создание элементов интерфейса
    sf::Text logText("", font, 16);
    logText.setPosition(10, 10);
    logText.setFillColor(sf::Color::White);

    sf::RectangleShape exitButton(sf::Vector2f(150, 40));
    exitButton.setPosition(325, 540);
    exitButton.setFillColor(sf::Color::Red);

    sf::Text exitButtonText("Exit", font, 20);
    exitButtonText.setPosition(375, 545);
    exitButtonText.setFillColor(sf::Color::White);

    // Запуск сервера в отдельном потоке
    thread serverThread(serverThreadFunction, serverSocket);

    // Основной цикл GUI
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                serverRunning = false;
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(
                        event.mouseButton.x, event.mouseButton.y));

                    if (exitButton.getGlobalBounds().contains(mousePos)) {
                        serverRunning = false;
                        window.close();
                    }
                }
            }
        }

        // Обновление текста лога
        string logContent;
        for (const auto& message : logMessages) {
            logContent += message + "\n";
        }
        logText.setString(logContent);

        // Отрисовка
        window.clear(sf::Color(50, 50, 50));

        // Отрисовка лога
        sf::RectangleShape logBackground(sf::Vector2f(780, 500));
        logBackground.setPosition(10, 10);
        logBackground.setFillColor(sf::Color(30, 30, 30));
        window.draw(logBackground);

        // Создание view для прокрутки лога
        sf::View logView(sf::FloatRect(0, 0, 780, 500));
        logView.setViewport(sf::FloatRect(0.0125f, 0.0167f, 0.975f, 0.833f));
        window.setView(logView);

        // Установка позиции текста для прокрутки
        if (logMessages.size() > 0) {
            float textHeight = logMessages.size() * 20.0f;
            float visibleHeight = 500.0f;
            float scrollPos = max(0.0f, textHeight - visibleHeight);
            logText.setPosition(10, -scrollPos);
        }

        window.draw(logText);

        // Восстановление стандартного view
        window.setView(window.getDefaultView());

        // Отрисовка кнопки выхода
        window.draw(exitButton);
        window.draw(exitButtonText);

        window.display();
    }

    // Ожидание завершения серверного потока
    if (serverThread.joinable()) {
        serverThread.join();
    }

    return 0;
}
