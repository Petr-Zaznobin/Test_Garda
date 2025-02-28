#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

using boost::asio::ip::tcp;
using json = nlohmann::json;

// Функция для отправки запроса серверу и получения ответа
std::string send_request(const std::string& host, const std::string& port, const std::string& request_data) {
    try {
        boost::asio::io_context io_context;

        // Разрешаем адрес хоста и порта
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(host, port);

        // Создаем сокет
        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        // Отправляем запрос
        boost::asio::write(socket, boost::asio::buffer(request_data));

        // Читаем ответ
        boost::asio::streambuf buffer;
        boost::asio::read_until(socket, buffer, "\r\n\r\n");
        std::istream is(&buffer);
        std::string response_data((std::istreambuf_iterator<char>(is)),
                                  std::istreambuf_iterator<char>());

        return response_data;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return "";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Неверное количество аргументов." << std::endl;
        return 1;
    }

    std::string command = argv[1];
    std::string expression;

    if (command == "-c" && argc == 3) {
        // Команда echo
        expression = argv[2];

        // Формируем JSON запрос для echo
        json request;
        request["cmd"] = "echo";
        std::string request_data = request.dump();

        // Отправляем запрос и получаем ответ
        std::string response = send_request("localhost", "8080", request_data);

        std::cout << response << std::endl;

    } else if (command == "-e" && argc == 4) {
        // Команда для вычисления выражения
        expression = argv[2] + std::string(" ") + argv[3];

        // Формируем JSON запрос для выражения
        json request;
        request["exp"] = expression;
        std::string request_data = request.dump();

        // Отправляем запрос и получаем ответ
        std::string response = send_request("localhost", "8080", request_data);

        // Парсим JSON ответ
        try {
            json response_json = json::parse(response);
            if (response_json.contains("res")) {
                std::cout << response_json["res"] << std::endl;
            } else if (response_json.contains("error")) {
                std::cerr << "Ошибка: " << response_json["error"] << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Ошибка при разборе ответа: " << e.what() << std::endl;
        }
    } else {
        std::cerr << "Неверные аргументы." << std::endl;
        return 1;
    }

    return 0;
}
