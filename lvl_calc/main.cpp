#include <iostream>
#include <sstream>
#include <stack>
#include <vector>
#include <cctype>
#include <stdexcept>

// Функция для определения приоритета оператора
int precedence(char op) {
    if(op == '+' || op == '-') return 1;
    if(op == '*' || op == '/') return 2;
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
        }
        else {
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
double evaluatePostfix(const std::vector<std::string>& postfix) {
    std::stack<double> stack;
    for (const auto &token : postfix) {
        // Если токен является оператором
        if (token.size() == 1 && isOperator(token[0])) {
            if (stack.size() < 2)
                throw std::runtime_error("Неверное выражение");
            double b = stack.top(); stack.pop();
            double a = stack.top(); stack.pop();
            switch(token[0]) {
                case '+': stack.push(a + b); break;
                case '-': stack.push(a - b); break;
                case '*': stack.push(a * b); break;
                case '/':
                    if(b == 0) throw std::runtime_error("Деление на ноль");
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

int main() {
    std::cout << "Введите выражение: ";
    std::string expression;
    std::getline(std::cin, expression);

    try {
        // Преобразование инфиксного выражения в постфиксное (RPN)
        auto postfix = infixToPostfix(expression);

        // Вывод постфиксного представления для отладки
        std::cout << "Обратная польская запись: ";
        for (const auto &token : postfix)
            std::cout << token << " ";
        std::cout << std::endl;

        // Вычисление результата
        double result = evaluatePostfix(postfix);
        std::cout << "Результат: " << result << std::endl;
    } catch (const std::exception &ex) {
        std::cerr << "Ошибка: " << ex.what() << std::endl;
    }

    return 0;
}
