#include <iostream>
#include <fstream>
#include <mutex>

#include "HashTable.h"
#include "DynamicArray.h"
#include "data.h"
#include "lock.h"
#include "help.h"
#include "queries.h"

using namespace std;

mutex mtx;

int code(string& str, const string& sql, HashTable<string, DynamicArray<string>*>& jsonStructure, HashTable<string, int>& numb, const string& schemaName, int tuplesLimit, DynamicArray<string>& keys) {
    
    // Автоматическая блокировка мьютекса
    lock_guard<mutex> lock(mtx);

    // Проверяем запрос SQL и вызываем соответствующую функцию
    if (SizeIndex(sql, 0, 12) == "INSERT INTO ") {
        int a = Insert(sql, jsonStructure, numb, schemaName, tuplesLimit, keys);
        if (a == 1) {
            str = "The addition was successful";
        } else {
            str = "Failed to insert the data.";
        }
    } 

    else if (SizeIndex(sql, 0, 12) == "DELETE FROM ") {
    // Парсинг команды DELETE
    std::istringstream iss(sql);
    std::string deleteCmd, fromCmd, tableName, whereCmd, fullColumn, value;
    iss >> deleteCmd >> fromCmd >> tableName >> whereCmd >> fullColumn;

    if (deleteCmd != "DELETE" || fromCmd != "FROM" || whereCmd != "WHERE") {
        str = "Invalid DELETE query syntax.";
        return 0;
    }

    // Проверка на наличие знака "="
    size_t eqPos = fullColumn.find('=');
    if (eqPos == std::string::npos) {
        str = "Invalid WHERE clause: missing '='.";
        return 0;
    }

    // Разделяем на колонку и значение
    std::string column = fullColumn.substr(0, eqPos);
    value = fullColumn.substr(eqPos + 1);

    // Убираем кавычки вокруг значения
    if (!value.empty() && value.front() == '\'' && value.back() == '\'') {
        value = value.substr(1, value.size() - 2);
    }

    // Проверяем, содержит ли колонка имя таблицы
    size_t dotPos = column.find('.');
    if (dotPos == std::string::npos) {
        str = "Invalid WHERE clause: missing table prefix in column.";
        return 0;
    }

    std::string columnTableName = column.substr(0, dotPos);
    column = column.substr(dotPos + 1);

    if (columnTableName != tableName) {
        str = "Invalid WHERE clause: table name mismatch.";
        return 0;
    }

    // Удаляем строки из таблицы
    bool success = Delet(tableName, column, value, schemaName, jsonStructure);
    if (success) {
        str = "The deletion was successful.";
    } else {
        str = "Failed to delete the data. No matching rows found in table: " + tableName;
    }
    } 

    else if (SizeIndex(sql, 0, 7) == "SELECT ") {
    // Выполняем SELECT
    try {
        string result;
        Select(sql, jsonStructure, schemaName, result);

        if (result.empty()) {
            str = "No data found.";
        } else {
            str = "Data selected successfully:\n" + result;
        }
    } catch (const std::exception& e) {
        str = std::string("Failed to execute SELECT query: ") + e.what();
    }
    }

    else {
        str = "Попробуйте ввести запрос ещё раз)";
    }

    return 1;
}