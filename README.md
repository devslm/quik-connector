[![C++](https://img.shields.io/badge/C++-%2011-green.svg)]()
[![Lua 5.3.5](https://img.shields.io/badge/Lua-%205.3.5-green.svg)]()
[![Lua 5.4.1](https://img.shields.io/badge/Lua-%205.4.1-green.svg)]()
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

Документация написана на русском языке, т.к. пользователи термина QUIK в основном русскоязычные. Английский вариант
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
В ней перейти в каталог проекта **/<path-to-quik-connector>/lua-5.3.5** (для LUA версии 5.4.1 перейти необходимо в каталог **/<path-to-quik-connector>/lua-5.4.1**) и выполнить:
```shell
build.bat
```

Данный скрипт содержит необходимые команды для сборки Lua библиотеки. После компиляции в каталоге build появятся файлы:
- **lua53.dll** (**lua54.dll** для версии **LUA 5.4.1**)
- **lua53.lib** (**lua54.lib** для версии **LUA 5.4.1**)

### Сборка TA-LIB библиотеки (Опционально)
Для запуска сборки необходимо запустить консоль **Командная строка Native Tools x64**.
В ней перейти в каталог проекта **/<path-to-quik-connector>/ta-lib** и выполнить:
```shell
build.bat
```

Будет выполнена сборка необходимых библиотек. После компиляции в каталоге **ta-lib/lib** появятся необходимые **.lib** файлы.

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

Структура готового каталога:
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

## Документация в процессе наполнения!!!
