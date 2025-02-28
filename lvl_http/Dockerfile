# Базовый образ Ubuntu
FROM ubuntu:latest

# Обновляем пакеты и устанавливаем g++, make и библиотеки Boost
RUN apt-get update && apt-get install -y \
    g++ \
    make \
    libboost-all-dev

# Устанавливаем рабочую директорию
WORKDIR /app

# Копируем исходный код в контейнер
COPY echo_server.cpp .

# Компилируем сервер с использованием стандарта C++17
RUN g++ -std=c++17 -o echo_server echo_server.cpp -lboost_system -lboost_thread

# Прокидываем порт 8080
EXPOSE 8080

# Запускаем сервер при старте контейнера
CMD ["/app/echo_server"]
