#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <iterator>
#include <sstream>
#include <stack>
#include <vector>
#include <cctype>
#include <stdexcept>
#include <nlohmann/json.hpp> // Для работы с JSON

using boost::asio::ip::tcp;
using json = nlohmann::json;

// Функция для определения приоритета оператора
int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

// Проверка, является ли символ оператором
bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

// Функция преобразования инфиксного выражения в обратную польскую запись (постфиксную)
std::vector<std::string> infixToPostfix(const std::string &expr) {
    std::vector<std::string> output;
    std::stack<char> opStack;

    for (size_t i = 0; i < expr.length(); i++) {
        char c = expr[i];

        // Пропускаем пробелы
        if (isspace(c)) continue;

        // Если символ — цифра или десятичная точка, читаем число целиком
        if (isdigit(c) || c == '.') {
            std::string number;
            while (i < expr.length() && (isdigit(expr[i]) || expr[i] == '.')) {
                number.push_back(expr[i]);
                i++;
            }
            i--; // компенсируем лишнее увеличение в цикле
            output.push_back(number);
        }
        // Если открывающая скобка
        else if (c == '(') {
            opStack.push(c);
        }
        // Если закрывающая скобка, извлекаем операторы до открывающей
        else if (c == ')') {
            while (!opStack.empty() && opStack.top() != '(') {
                output.push_back(std::string(1, opStack.top()));
                opStack.pop();
            }
            if (opStack.empty())
                throw std::runtime_error("Неверное выражение: несбалансированные скобки");
            opStack.pop(); // удаляем '('
        }
        // Если оператор
        else if (isOperator(c)) {
            while (!opStack.empty() && isOperator(opStack.top()) &&
                   precedence(opStack.top()) >= precedence(c)) {
                output.push_back(std::string(1, opStack.top()));
                opStack.pop();
            }
            opStack.push(c);
        } else {
            throw std::runtime_error(std::string("Неизвестный символ: ") + c);
        }
    }

    // Извлекаем оставшиеся операторы
    while (!opStack.empty()) {
        if (opStack.top() == '(' || opStack.top() == ')')
            throw std::runtime_error("Неверное выражение: несбалансированные скобки");
        output.push_back(std::string(1, opStack.top()));
        opStack.pop();
    }
    return output;
}

// Функция вычисления выражения, заданного в обратной польской записи
double evaluatePostfix(const std::vector<std::string> &postfix) {
    std::stack<double> stack;
    for (const auto &token : postfix) {
        // Если токен является оператором
        if (token.size() == 1 && isOperator(token[0])) {
            if (stack.size() < 2)
                throw std::runtime_error("Неверное выражение");
            double b = stack.top(); stack.pop();
            double a = stack.top(); stack.pop();
            switch (token[0]) {
                case '+': stack.push(a + b); break;
                case '-': stack.push(a - b); break;
                case '*': stack.push(a * b); break;
                case '/':
                    if (b == 0) throw std::runtime_error("Деление на ноль");
                    stack.push(a / b); break;
                default:
                    throw std::runtime_error("Неизвестный оператор");
            }
        }
        // Иначе, токен — число
        else {
            stack.push(std::stod(token));
        }
    }
    if (stack.size() != 1)
        throw std::runtime_error("Ошибка вычисления выражения");
    return stack.top();
}

// Обработка клиента: отделяем заголовки от тела, парсим JSON и отправляем ответ
void handle_client(tcp::socket &socket) {
    try {
        boost::asio::streambuf buffer;
        // Читаем до разделителя заголовков
        boost::asio::read_until(socket, buffer, "\r\n\r\n");

        std::istream stream(&buffer);
        std::string headers;
        std::string line;
        while (std::getline(stream, line) && line != "\r") {
            headers += line + "\n";
        }

        // Извлекаем значение Content-Length (если указано)
        size_t content_length = 0;
        {
            std::istringstream header_stream(headers);
            while (std::getline(header_stream, line)) {
                if (line.find("Content-Length:") != std::string::npos) {
                    size_t pos = line.find(":");
                    if (pos != std::string::npos) {
                        std::string len_str = line.substr(pos + 1);
                        content_length = std::stoul(len_str);
                    }
                }
            }
        }

        // Читаем тело запроса, уже находящееся в buffer
        std::string body;
        if (buffer.size() > 0) {
            std::ostringstream oss;
            oss << &buffer;
            body = oss.str();
        }

        // Если тело прочитано не полностью, читаем оставшиеся байты
        if (body.size() < content_length) {
            size_t remaining = content_length - body.size();
            std::vector<char> additional(remaining);
            boost::asio::read(socket, boost::asio::buffer(additional));
            body.append(additional.data(), additional.size());
        }

        std::cout << "Получен JSON body:\n" << body << std::endl;

        // Парсим JSON-запрос
        json request = json::parse(body);
        json response;

        if (request.contains("cmd")) {
            if (request["cmd"] == "echo") {
                response["res"] = "echo";
            }
        } else if (request.contains("exp")) {
            try {
                auto postfix = infixToPostfix(request["exp"].get<std::string>());
                double result = evaluatePostfix(postfix);
                response["res"] = result;
            } catch (const std::exception &ex) {
                response["error"] = ex.what();
            }
        } else {
            response["error"] = "Неизвестная команда или выражение";
        }

        // Формируем HTTP-ответ
        std::string resp_json = response.dump();
        std::ostringstream resp_stream;
        resp_stream << "HTTP/1.1 200 OK\r\n"
                    << "Content-Type: application/json\r\n"
                    << "Content-Length: " << resp_json.size() << "\r\n"
                    << "Connection: close\r\n\r\n"
                    << resp_json;
        boost::asio::write(socket, boost::asio::buffer(resp_stream.str()));
    } catch (std::exception &e) {
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
    } catch (std::exception &e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
    }

    return 0;
}
