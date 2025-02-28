#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <iterator>

using boost::asio::ip::tcp;

void handle_client(tcp::socket& socket) {
    try {
        boost::asio::streambuf buffer;
        // Читаем до конца заголовков (до "\r\n\r\n")
        boost::asio::read_until(socket, buffer, "\r\n\r\n");

        // Создаем istream на базе buffer
        std::istream is(&buffer);
        // Считываем все данные из istream в строку
        std::string request_data((std::istreambuf_iterator<char>(is)),
                                  std::istreambuf_iterator<char>());

        std::cout << "Получен запрос:\n" << request_data << std::endl;

        // Собираем HTTP-ответ, используя std::string для конкатенации
        std::string response = std::string("HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: ") + std::to_string(request_data.size()) +
            "\r\nConnection: close\r\n\r\n" + request_data;

        boost::asio::write(socket, boost::asio::buffer(response));
    } catch (std::exception& e) {
        std::cerr << "Ошибка при обработке клиента: " << e.what() << "\n";
    }
}

int main() {
    try {
        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));

        std::cout << "Сервер запущен на порту 8080...\n";

        while (true) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);
            handle_client(socket);
        }
    } catch (std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
    }

    return 0;
}
