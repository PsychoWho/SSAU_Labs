#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#define BUFSIZE 1024

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "sfml-graphics.lib")
#pragma comment(lib, "sfml-window.lib")
#pragma comment(lib, "sfml-system.lib")

using namespace std;

int main() {
    // Инициализация Winsock
    WSADATA wsaData;
    WORD DLLVersion = MAKEWORD(2, 2);
    if (WSAStartup(DLLVersion, &wsaData) != 0) {
        cerr << "WSAStartup failed" << endl;
        return 1;
    }

    // Настройка соединения
    SOCKADDR_IN addr;
    static int sizeOfAddr = sizeof(addr);
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Socket creation error" << endl;
        WSACleanup();
        return 1;
    }

    if (connect(clientSocket, (sockaddr*)&addr, sizeOfAddr) == SOCKET_ERROR) {
        cerr << "Connection failed" << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Создание окна SFML
    sf::RenderWindow window(sf::VideoMode(600, 400), "Client GUI");
    window.setFramerateLimit(30);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        font.loadFromFile("C:/Windows/Fonts/Arial.ttf");
    }

    // Элементы интерфейса
    vector<string> serverResponses;
    
    // Поле для вывода ответов сервера
    sf::Text responsesText("", font, 16);
    responsesText.setPosition(10, 10);
    responsesText.setFillColor(sf::Color::White);
    
    // Поле ввода
    sf::RectangleShape inputBox(sf::Vector2f(580, 30));
    inputBox.setPosition(10, 350);
    inputBox.setFillColor(sf::Color::White);
    inputBox.setOutlineThickness(2);
    inputBox.setOutlineColor(sf::Color::Black);
    
    sf::Text inputText("", font, 16);
    inputText.setPosition(15, 355);
    inputText.setFillColor(sf::Color::Black);
    
    string currentInput;
    bool hasFocus = false;

    // Подсказка
    sf::Text hintText("Enter expression in format: <op> <num> <num>", font, 14);
    hintText.setPosition(10, 320);
    hintText.setFillColor(sf::Color::Yellow);

    // Основной цикл
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            // Обработка клика по полю ввода
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos = window.mapPixelToCoords(
                    sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                
                if (inputBox.getGlobalBounds().contains(mousePos)) {
                    hasFocus = true;
                    inputBox.setOutlineColor(sf::Color::Blue);
                } else {
                    hasFocus = false;
                    inputBox.setOutlineColor(sf::Color::Black);
                }
            }
            
            // Обработка ввода текста
            if (event.type == sf::Event::TextEntered && hasFocus) {
                if (event.text.unicode == '\b') { // Backspace
                    if (!currentInput.empty()) {
                        currentInput.pop_back();
                    }
                }
                else if (event.text.unicode == '\r') { // Enter
                    if (!currentInput.empty()) {
                        // Отправка на сервер
                        send(clientSocket, currentInput.c_str(), currentInput.size() + 1, 0);
                        
                        // Получение ответа
                        char buffer[BUFSIZE];
                        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
                        if (bytesReceived <= 0) {
                            serverResponses.push_back("Server disconnected");
                            window.close();
                        } else {
                            buffer[bytesReceived] = '\0';
                            serverResponses.push_back("> " + currentInput);
                            serverResponses.push_back("Server: " + string(buffer));
                        }
                        
                        currentInput.clear();
                    }
                }
                else if (event.text.unicode < 128) {
                    currentInput += static_cast<char>(event.text.unicode);
                }
                
                inputText.setString(currentInput);
            }
        }

        // Обновление текста ответов
        string responsesContent;
        for (const auto& response : serverResponses) {
            responsesContent += response + "\n";
        }
        responsesText.setString(responsesContent);

        // Отрисовка
        window.clear(sf::Color(50, 50, 50));
        
        // Отрисовка области ответов
        sf::RectangleShape responsesBackground(sf::Vector2f(580, 300));
        responsesBackground.setPosition(10, 10);
        responsesBackground.setFillColor(sf::Color(30, 30, 30));
        window.draw(responsesBackground);
        
        // Настройка view для прокрутки
        sf::View responsesView(sf::FloatRect(0, 0, 580, 300));
        responsesView.setViewport(sf::FloatRect(0.0167f, 0.025f, 0.9667f, 0.75f));
        window.setView(responsesView);
        
        // Позиционирование текста для прокрутки
        if (!serverResponses.empty()) {
            float textHeight = serverResponses.size() * 20.0f;
            float visibleHeight = 300.0f;
            float scrollPos = max(0.0f, textHeight - visibleHeight);
            responsesText.setPosition(10, -scrollPos);
        }
        
        window.draw(responsesText);
        
        // Восстановление стандартного view
        window.setView(window.getDefaultView());
        
        // Отрисовка поля ввода и подсказки
        window.draw(inputBox);
        window.draw(inputText);
        window.draw(hintText);
        
        window.display();
    }

    // Закрытие соединения
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
