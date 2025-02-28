# Test_Garda
## vars

1. Собрать контейнер докер (docker build -t vars .)
2. Развернуть сервер (docker run -it --rm -p 8080:8080 vars)
4. bash ```docker build -t vars_server .```
5. bash ```docker run -d -p 8080:8080 --name vars_server_instance vars_server```

### Пример тестирования команды echo:

1. Откройте Postman и создайте новый запрос.
2. Установите метод запроса на POST.
3. В поле URL введите http://localhost:8080.
4. Перейдите на вкладку Body и выберите raw.
5. Убедитесь, что выбран формат JSON.
6. В поле ввода введите следующий JSON:
   ```json
   {
    "cmd": "echo",
    "message": "Hello, World!"}
7. Радуемся, если заработает)


