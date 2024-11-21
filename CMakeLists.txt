# Минимальная версия CMake
cmake_minimum_required(VERSION 3.10)

# Имя проекта
project(MyServerProject)

# Указываем стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Добавляем каталог с заголовочными файлами
include_directories(include)
include_directories(libs)

# Указываем исходные файлы проекта
set(SOURCES
    server.cpp
    lock.cpp
    help.cpp
)

# Создаём исполняемый файл
add_executable(server ${SOURCES})

# Указываем, что использовать pthread для многопоточности
find_package(Threads REQUIRED)
target_link_libraries(server PRIVATE Threads::Threads)
