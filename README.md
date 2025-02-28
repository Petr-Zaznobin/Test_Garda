# Test_Garda
## agregat

1. Скачать библиотеку boost
2. Скачать библиотеку nlohmann
3. Изменить в "Конфигурация C/C++" путь включения, если не видит библиотеки
4. Собрать контейнер докер (docker build -t agregat .)
5. Развернуть сервер (docker run -it --rm -p 8080:8080 agregat)
6. Через Postman подключиться к сессии
7. raw -> json
8. {
  "cmd": "echo"
}
9. {
  "exp": "2 + 2"
}


