#include <iostream>
#include <string>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "HashTable.h"
#include "DynamicArray.h"
#include "data.h"
#include "queries.h"
#include "interface.h"

#define DEFAULT_PORT 7432
#define CLIENT_CLOSE_CONNECTION_SYMBOL '~'
#define BUFFER_SIZE 1024

using namespace std;

// Обработчик клиента
void handle_client(int client_socket, 
                   HashTable<string, DynamicArray<string>*>& jsonStructure, 
                   HashTable<string, int>& numb, 
                   const string& schemaName, 
                   int tuplesLimit, 
                   DynamicArray<string>& keys) {
    try {
        // Отправляем подтверждение клиенту
        string welcome_message = "Connection established!";
        send(client_socket, welcome_message.c_str(), welcome_message.size(), 0);

        char buffer[BUFFER_SIZE];

        while (true) {
            // Чтение данных от клиента
            ssize_t bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
            if (bytes_read <= 0) {
                cerr << "Error reading from socket or client disconnected." << endl;
                break;
            }

            buffer[bytes_read] = '\0';  // Завершаем строку
            string client_message(buffer);

            // Проверяем, хочет ли клиент закрыть соединение
            if (client_message[0] == CLIENT_CLOSE_CONNECTION_SYMBOL) {
                cout << "Client disconnected." << endl;
                break;
            }

            cout << "Received: " << client_message << endl;

            // Обработка SQL-запроса
            string response;
            code(response, client_message, jsonStructure, numb, schemaName, tuplesLimit, keys);

            // Отправка ответа клиенту
            send(client_socket, response.c_str(), response.size(), 0);
        }
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    // Закрытие соединения
    close(client_socket);
}

int main(int argc, char* argv[]) {
    try {
        // Инициализация схемы
        string fileName = "../schema.json";
        string schemaName;
        int tuplesLimit = 0;
        auto* jsonStructure = createHashTable<string, DynamicArray<string>*>(10, 75);
        auto* keys = createArray<string>(10, 75);
        auto* numb = createHashTable<string, int>(10, 75);

        ReadJsonFile(fileName, schemaName, tuplesLimit, *jsonStructure, *keys);
        
        // Создание сокета
        int server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == -1) {
            cerr << "Error creating socket: " << strerror(errno) << endl;
            return 1;
        }

        // Настройка адреса и порта
        sockaddr_in server_addr = {};
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(DEFAULT_PORT);

        // Привязка сокета
        if (::bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            cerr << "Error binding socket: " << strerror(errno) << endl;
            close(server_socket);
            return 1;
        }

        // Прослушивание порта
        if (listen(server_socket, 5) == -1) {
            cerr << "Error listening on socket: " << strerror(errno) << endl;
            close(server_socket);
            return 1;
        }

        cout << "Server is running on port " << DEFAULT_PORT << endl;

        // Ожидание подключений клиентов
        while (true) {
            int client_socket = accept(server_socket, nullptr, nullptr);
            if (client_socket == -1) {
                cerr << "Error accepting client connection: " << strerror(errno) << endl;
                continue;
            }

            cout << "Client connected." << endl;
            // Обрабатываем клиента в отдельном потоке
            thread client_thread(handle_client, client_socket, ref(*jsonStructure), ref(*numb), cref(schemaName), tuplesLimit, ref(*keys));
            client_thread.detach();
        }

        // Освобождение ресурсов
        destroyHashTable(*jsonStructure);
        destroyArray(*keys);
        destroyHashTable(*numb);

    } catch (const exception& e) {
        cerr << "Server error: " << e.what() << endl;
    }

    return 0;
}