[![C++](https://img.shields.io/badge/C++-%2011-green.svg)]()
[![Lua 5.3.5](https://img.shields.io/badge/Lua-%205.3.5-green.svg)]()
[![Lua 5.4.1](https://img.shields.io/badge/Lua-%205.4.1-green.svg)]()
[![TA-Lib 0.4.0](https://img.shields.io/badge/TA--Lib-%200.4.0-green.svg)]()
[![Windows 7/8/10](https://img.shields.io/badge/Windows-7/8/10-blue.svg)](https://www.microsoft.com/)
[![Arqa Quik 8.13+](https://img.shields.io/badge/Arqa%20Quik-8.13+-blue.svg)](https://arqatech.com/en/products/quik/)

# C++ Connector for Arqa Quik Terminal

## Документация
[Страница проекта](https://slm-dev.com/quik-connector/)

## О проекте
### Важно!
**Т.к. коннектор имеет полный доступ к терминалу, то он может легко выполнить любую операцию (в том числе вывести деньги
со счета), поэтому доверять можно только коду и бинарным файлам, скачанным с данного репозитория!
Я не несу ответственности за код или бинарные файлы, скачанные с других ресурсов!
Все релизные файлы подписываются сертификатом, соответственно я могу проверить при возникающих проблемах оригинальный файл или нет.**

Документация написана на русском языке, т.к. пользователи терминала QUIK в основном русскоязычные. Английский вариант
возможно появится в будущем если будет интерес.

Данный коннектор реализует не весь фукционал из документации, только часть самых необходимых функций. Коннектор
используется в другом проекте как интерфейс к терминалу.

В качестве транспортной шины данных используется Redis. Единственная из доступных для Windows (нативно без подсистемы Linux)
in-memory БД. Данный транспорт обеспечивает большое количество плюсов по сравнению, например, с ZeroMQ или Memory Maped
Files (данные решения не являются полноценными очередями, кэшами и БД в одном приложении). Redis же добавляет коннектору
следующие возможности помимо скорости работы:
- Сохранение данных на диск (персистентность данных между перезапусками, не 100%, но абсолютно достаточная в данном профиле работы)
- Позволяет временно (с использованием TTL на запись) кэшировать требуемые данные
- Позволяет долговременно и персистентно хранить некоторые данные (например, записи о трэйдах хранятся долго и позволяет
  вновь подключаемым клиентам получать их)
- Механизм Pub/Sub позволяет отправлять широковещательные сообщения всем подключенным клиентам без сохранения

Соответственно каждый клиент может общаться с терминалом через Redis и сам Redis может находиться как на локальной
машине около терминала или на удаленном (выделенном сервере). Последний вариант позволяет получать данные клиентам,
находящимся на удаленных машинах. Это позволяет реализовывать логику и алгоритмы не только внутри DLL библиотеки, но и
на любом другом языке и ОС (например, Python, Kotlin и т.д.) обращаясь за данными через Redis.

Дополнительно в коннектор встроена поддержка SQLite для локального сохранения всех ордеров. Возможно они могут потребоваться
для каких-либо целей в будущем (например, для истории).

Добавлена поддержка библиотеки TA-LIB для построения индикаторов. Библиотека может опционально подключаться во время сборки. По-умолччанию выключена. Документация
на библиотеку: [TA-LIB](https://ta-lib.org/).

## Поддерживаемые версии LUA
Проект может собираться для 2-х версий:
- **LUA 5.3.5**
- **LUA 5.4.1**

Версия задается при сборке через флаг cmake (см. описание в блоке **Сборка проекта**).

Поэтому роботы могут создаваться на базе существующего кода (написанного для **LUA 5.3.5**) или нового, предпочтительно
использовать **LUA 5.4.1**, т.к. она содержит ряд исправленных багов.

## Сборка проекта
Перед сборкой необходимо установить следующие компоненты:
- **Microsoft Visual Studio 2015+**

Все зависимости для сборки закачиваются и собираются автоматически с использованием **Cmake FetchContent** во время
сборки проекта.

Сборка выполняется в несколько этапов:
1. Сборка Lua библиотеки (добавить ее как зависимость для Cmake FetchContent не получилось)
1. Сборка TA-LIB библиотеки индикаторов **опционально** (добавить ее как зависимость для Cmake FetchContent не получилось)
2. Сборка DLL библиотеки
3. Установка Redis-сервера

### Сборка Lua библиотеки
Для запуска сборки необходимо запустить консоль **Командная строка Native Tools x64**.
В ней перейти в каталог проекта **/path-to-quik-connector/external/lua-5.3.5** (для LUA версии 5.4.1 перейти необходимо в каталог **/path-to-quik-connector/external/lua-5.4.1**) и выполнить:
```shell
build.bat
```

Данный скрипт содержит необходимые команды для сборки Lua библиотеки. После компиляции в каталоге build появятся файлы:
- **lua53.dll** (**lua54.dll** для версии **LUA 5.4.1**)
- **lua53.lib** (**lua54.lib** для версии **LUA 5.4.1**)

### Сборка TA-LIB библиотеки (Опционально)
Для запуска сборки необходимо запустить консоль **Командная строка Native Tools x64**.
В ней перейти в каталог проекта **/path-to-quik-connector/external/ta-lib** и выполнить:
```shell
build.bat
```

Будет выполнена сборка необходимых библиотек. После компиляции в каталоге **external/ta-lib/lib** появятся необходимые **.lib** файлы.

### Сборка проекта
Необходимо вернуться в корень проекта. Для сборки необходимо в консоли **Командная строка Native Tools x64** выполнить
следующие команды (сборка релизной версии):
```shell
mkdir build

cd build
 
cmake -DCMAKE_BUILD_TYPE=Release -DUSE_LUA_VERSION_5_4=ON -DQUIK_INCLUDE_TA_LIB=OFF .. -A x64

cmake --build . --config Release
```

Параметры сборки:
- USE_LUA_VERSION_5_4 - если ON то использовать Lua 5.4.1, иначе версию 5.3.5
- QUIK_INCLUDE_TA_LIB - если ON то подключить необходимые зависимости для работы (требуются предварительно собранные библиотеки),
  иначе не подключается и не требуется собирать библиотеки.

После полной сборки проекта в корне каталога будет создан каталог quik-connector с требуемой структурой, который необходимо
скопировать в каталог терминала QUIK (либо в любое другое место и в дальнейшем выбрать его в терминале при запуске коннектора).

Структура собранного каталога:
```shell
  |-- bin
  |    |-- quik-connector.dll
  |  
  |-- config
  |    |-- config.yml
  |  
  |-- db
  |    |-- migrations
  |    |    |-- *.sql
  |    |
  |    |-- stocks.db
  |  
  |-- logs
  |    |-- *.log
  |  
  |-- quik-connector.lua
```

#### Каталог **logs** и файл БД **stocks.db** будут созданы при первом запуске DLL в QUIK.

Для запуска необходимо в терминале QUIK выбрать в качестве скрипта файл **quik-connector.lua** и выбрать версию Lua 5.3.5
(Lua 5.4.1 если сборка была с версией 5.4.1).

В уже собранной DLL версию использованной Lua можно узнать в свойствах файла в разделе подробно.

### Установка Redis-сервера
Последняя официальная версия Redis для запуска под windows: 3.2.100. Она сильно устарела, но содержит весь необходимый для
работы коннектора функционал и нативно устанавливается в Windows без Linux подсистемы.

Скачиваем инсталлятор с официального репозитория: [Redis-3.2.100](https://github.com/microsoftarchive/redis/releases/tag/win-3.2.100).
При установке оставляем параметры по умолчанию, только в ограничении памяти можем поставить 200-300Мб.
Redis установится как windows-сервис и будет автоматически загружаться при старте ОС.

Далее остается только задать пароль для подключения в файле конфигурации Redis. В каталоге установки необходимо открыть
файл **redis.windows-service.conf** в блоке **security**, например: **C:\Program Files\Redis\redis.windows-service.conf**
и найти строку: **requirepass**, раскомментировать ее и задать свой пароль. Закомментировать строку: **bind 127.0.0.1**,
если нужно, чтобы Redis слушал все сетевые интерфейсы (по умолчанию доступен только по 127.0.0.1) Перезапустить Redis сервис.

На этом настройка Redis закончена. Он будет доступен по IP: **127.0.0.1** и порту: **6379** с заданным паролем.

## API запросов к терминалу через Redis
### Формат ответа
Ответ всегда отправляется в едином формате (схожем со спецификацией JSON:API, но не поддерживается обязательное поле **meta**) и может быть либо
успешным, либо с кодом ошибки:

#### Успешный формат ответа:
```json
{
  "id": "UUID",
  "data": "List or Map with data"
}
```

#### Ответ с ошибкой:
```json
{
  "id": "UUID",
  "code": "Error code constant",
  "detail": "Error detail message"
}
```
В обоих случаях в поле id с типом UUID записываться может:
  - id запроса
  - рандомный UUID, если ответ формируется без запроса самим коннектором, например, в callback'е обновления свечи

В первом случае по id из запроса клиент может ожидать ответ, использовать timeout на ожидание ответа и может ожидать ошибку если она
возникнет. Во втором случае id можно игнорировать или использовать, например, для теоретической дедупликации сообщений и т.п.

Коды ошибок:
  - **QUIK_LUA_ERROR** - ошибки в работе с Lua-стеком или с функциями QUIK (в таких случаях как правило поле detail не будет содержать подробностей)
  - **COMMAND_REQUEST_ERROR** - ошибка парсинга данных в запросе, например, код и название инструмента для получения свечей и т.п. 
  - **CANDLES_TIMEOUT_ERROR** - timeout ожидания готовности свечей (20 секунд время ожидания)


### Формат команд запроса данных
Каждая команда от клиента состоит из обязательных полей и полей, необходимых для передачи в качестве параметров в функции Lua QUIK. Команда 
отправляется через топик запросов **topic:quik:commands** (Redis publish command).

Для получения ответа клиенту необходиму подключиться к указанному в команде топику в Redis (команда subscribe).

Обязательные поля:
  - **id** запроса с типом UUID, который будет отправлен в id ответа для идентификации ответа клиентом
  - **command** - строковая константа с именем команды

Дополнительные поля передаются в соответствии с описанием команды.

### Типы данных в запросе/ответе
В примерах используются/поддерживаются следующие типы данных:
  - **0** - целое число типа uint64_t (64 битное целое)
  - **0.0** - число с типом double
  - **""** - строка (uuid тип так же передается как строка)
  - **true | false** - булево значение

#### Команды API
#### Получение текущего пользователя терминала
Команда: **GET_USER**

Дополнительные параметры: **нет**

Пример запроса:
```json
{
  "id": "3ec8b89d-3c95-460d-9cf8-aebd700412ec",
  "command": "GET_USER"
}
```
Пример ответа:
```json
{
  "id": "3ec8b89d-3c95-460d-9cf8-aebd700412ec",
  "data": {
    "name": "User name"
  }
}
```

#### Получение списка инструментов по коду
Команда: **GET_TICKERS**

Дополнительные параметры:
  - **classCode** - код инструмента

Пример запроса:
```json
{
  "id": "5da58aa5-a0d6-4b94-ba8d-d05606c0bc31",
  "command": "GET_TICKERS",
  "classCode": "TQBR"
}
```
Пример ответа (описание полей - http://luaq.ru/getSecurityInfo.html):
```json
{
  "id": "5da58aa5-a0d6-4b94-ba8d-d05606c0bc31",
  "data": [
    {
      "code": "",
      "name": "",
      "shortName": "",
      "classCode": "",
      "className": "",
      "faceValue": "",
      "faceUnit": "",
      "scale": "",
      "matDate": "",
      "lotSize": "",
      "isinCode": "",
      "minPriceStep": ""
    }
  ]
}
```

#### Получение списка ордеров
Команда: **GET_NEW_ORDERS**

Дополнительные параметры: **нет**

Пример запроса:
```json
{
  "id": "6ef32b92-376a-471b-ad02-4489a61296e2",
  "command": "GET_NEW_ORDERS",
}
```
Пример ответа (описание полей - http://luaq.ru/OnOrder.html):
```json
{
  "id": "6ef32b92-376a-471b-ad02-4489a61296e2",
  "data": [
    {
      "orderNum": 0,
      "flags": 0.0,
      "brokerRef": "",
      "userId": "",
      "firmId": "",
      "account": "",
      "price": 0.0,
      "qty": 0.0,
      "balance": 0.0,
      "value": 0.0,
      "accruedInt": 0.0,
      "yield": 0.0,
      "transId": 0,
      "clientCode": "",
      "price2": 0.0,
      "settleCode": "",
      "uid": 0,
      "canceledUid": 0,
      "exchangeCode": "",
      "activationTime": 0.0,
      "linkedOrder": 0,
      "expiry": 0.0,
      "ticker": "",
      "name": "",
      "classCode": "",
      "classType": "",
      "status": "",
      "type": "",
      "currency": "",
      "lotSize": 0.0,
      "date": "",
      "withdrawDate": 0,
      "bankAccId": 0.0,
      "valueEntryType": 0,
      "repoTerm": 0.0,
      "repoValue": 0.0,
      "repo2value": 0.0,
      "repoValueBalance": 0.0,
      "startDiscount": 0.0,
      "rejectReason": "",
      "extOrderFlags": 0.0,
      "minQty": 0.0,
      "execType": 0.0,
      "sideQualifier": 0.0,
      "acntType": 0.0,
      "capacity": 0.0,
      "passiveOnlyOrder": 0.0,
      "visible": 0.0,
      "priceStepCost": 0.0,
      "commission": {
        "broker": 0.0,
        "clearing": 0.0,
        "techCenter": 0.0,
        "exchange": 0.0
      }
    }
  ]
}
```
В дополнение к стандартному ответу QUIK здесь добавлены следующие поля:
  - **status** - код статуса ордера полученный из поля **flags**, принимает следующие значения:
    - **ORDER_STATUS_CANCELED** - ордер снят
    - **ORDER_STATUS_SUCCESS_COMPLETED** - ордер полностью выполнен (куплено/продано заданное количество лотов)
    - **ORDER_STATUS_ACTIVE** - ордер еще в работе
    - **ORDER_STATUS_INACTIVE** - ордер не активен
    - **ORDER_STATUS_UNDEFINED** - не удалось правильно распарсить поле **flags**, это баг
  - **type** - тип ордера, принимает следующие значения:
    - **BUY** - ордер на покупку
    - **SELL** - ордер на продажу
  - **classType** - тип инструмента, принимает следующие значения:
    - **FUTURE** - фьючерс
    - **STOCK** - акция
    - **BOND** - облигация
    - **INDEX** - индекс (например, IMOEX)
    - **OPTION** - опцион
    - **ETF** - етф
    - **CURRENCY** - валюта
    - **INCOMPLETE_LOT** - неполные лоты
    - **PAI** - паи
  - **currency** - валюта ордера
  - **name** - короткое название инструмента в ордере
  - **lotSize** - размер лота для инструмента
  - **priceStepCost** - стоимость шага цены для инструмента
  - **commission** - информация о комиссии если удалось получить из таблицы **trades** в QUIK

## Документация в процессе наполнения!!!
