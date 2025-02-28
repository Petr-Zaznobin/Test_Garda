# Используем официальный образ Ubuntu
FROM ubuntu:latest

# Устанавливаем необходимые утилиты и компилятор
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    make \
    git \
    curl \
    wget \
    build-essential \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

# Устанавливаем nlohmann/json
RUN mkdir -p /usr/local/include/nlohmann \
    && wget -qO /usr/local/include/nlohmann/json.hpp https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp

# Устанавливаем рабочую директорию
WORKDIR /app

# Копируем исходный код в контейнер
COPY vars.cpp .

# Компилируем сервер с использованием стандарта C++17
RUN g++ -std=c++17 -o vars vars.cpp -lboost_system -lboost_thread -lpthread

# Прокидываем порт 8080
EXPOSE 8080

# Запускаем сервер при старте контейнера
CMD ["/app/vars"]