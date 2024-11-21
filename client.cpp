#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#define SERVER_IP "127.0.0.1"
#define DEFAULT_PORT 7432
#define ERROR_S "CLIENT ERROR: "
#define BUFFER_SIZE 1024
#define CLIENT_CLOSE_CONNECTION_SYMBOL '~'
#define SERVER_CLOSE_CONNECTION_SYMBOL '~'

bool is_client_connection_close(const char* msg);

int main(int argc, char const* argv[]) {
    int client;

    struct sockaddr_in server_address;

    // Создание сокета
    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client < 0) {
        std::cout << ERROR_S << "establishing socket error.";
        exit(0);
    }

    server_address.sin_port = htons(DEFAULT_PORT);
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    std::cout << "\n=> Client socket created.\n";

    // Подключение к серверу
    int ret = connect(client, reinterpret_cast<const struct sockaddr*>(&server_address), sizeof(server_address));
    if (ret == 0) {
        std::cout << "=> Connected to server " << inet_ntoa(server_address.sin_addr) << " with port number: " << DEFAULT_PORT << "\n";
    } else {
        std::cout << ERROR_S << "Error connecting to the server\n";
        close(client);
        exit(0);
    }

    char buffer[BUFFER_SIZE];
    std::cout << "Waiting for server confirmation...\n";
    recv(client, buffer, BUFFER_SIZE, 0);  // Ожидаем подтверждения от сервера
    std::cout << "=> Connection established.\n" << "Enter " << SERVER_CLOSE_CONNECTION_SYMBOL << " to close the connection\n";

    while (true) {
        std::cout << "Enter a SQL request:\n";
        std::cin.getline(buffer, BUFFER_SIZE);

        // Отправляем запрос серверу
        send(client, buffer, strlen(buffer), 0);

        // Проверка, хочет ли клиент завершить соединение
        if (is_client_connection_close(buffer)) {
            break;
        }

        // Получаем ответ от сервера
        std::cout << "Server response:\n";
        ssize_t bytes_received = recv(client, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            std::cout << ERROR_S << "Error receiving data or server disconnected.\n";
            break;
        }

        buffer[bytes_received] = '\0';  // Завершаем строку
        std::cout << buffer << "\n";

        // Проверка, не хочет ли сервер завершить соединение
        if (is_client_connection_close(buffer)) {
            break;
        }

        std::cout << std::endl;
    }

    // Закрытие соединения
    close(client);
    std::cout << "\nGoodbye...\n";

    return 0;
}

bool is_client_connection_close(const char* msg) {
    // Проверка, не хочет ли клиент или сервер закрыть соединение
    for (int i = 0; i < strlen(msg); ++i) {
        if (msg[i] == CLIENT_CLOSE_CONNECTION_SYMBOL) {
            return true;
        }
    }
    return false;
}