#ifndef DATA_H
#define DATA_H

#include <iostream>
#include <fstream>
#include <cstdio>
#include <filesystem>
#include <regex>

#include "HashTable.h"
#include "DynamicArray.h"

#include "json.hpp"

using namespace std;

using json = nlohmann::json;
namespace fs = filesystem;

int ReadJsonFile(const string& fileName, 
                 string& schemaName, 
                 int& tuplesLimit, 
                 HashTable<string, DynamicArray<string>*>& jsonStructure, 
                 DynamicArray<string>& keys) {

    ifstream file(fileName);
    if (!file.is_open()) {
        throw runtime_error("Failed to open schema.json");
    }

    json schema;
    file >> schema;

    schemaName = schema["name"];
    fs::create_directory(schemaName);

    tuplesLimit = schema["tuples_limit"];

    json tableStructure = schema["structure"];
    for (const auto& [tableName, columns] : tableStructure.items()) {
        cout << "Processing table: " << tableName << endl;

        fs::create_directory(schemaName + "/" + tableName);

        auto* tempValue = createArray<string>(10); // Колонки таблицы
        addElement(keys, tableName);              // Добавляем название таблицы в keys

        // Добавляем первичный ключ в массив колонок
        string primaryKey = tableName + "_pk";
        addElement(*tempValue, primaryKey);
        cout << "Added primary key: " << primaryKey << endl;

        // Сохраняем первичный ключ в jsonStructure
        setHash(jsonStructure, primaryKey, createArray<string>(10));
        cout << "Primary key '" << primaryKey << "' added to jsonStructure." << endl;

        string colNames = primaryKey;

        for (const auto& column : columns) {
            string columnName = column.get<string>();
            colNames += "," + columnName;
            addElement(*tempValue, columnName);
            cout << "Added column: " << columnName << endl;

            // Добавляем колонку в jsonStructure
            setHash(jsonStructure, columnName, createArray<string>(10));
            cout << "Column '" << columnName << "' added to jsonStructure." << endl;
        }

        // Сохраняем структуру таблицы в jsonStructure
        setHash(jsonStructure, tableName, tempValue);

        // Проверяем сохранённую структуру
        auto* retrieved = getHash(jsonStructure, tableName);
        if (retrieved) {
            cout << "Table initialized: " << tableName << " with columns:" << endl;
            for (int i = 0; i < retrieved->size; ++i) {
                cout << "Column[" << i << "]: " << retrieved->data[i] << endl;
            }
        } else {
            cerr << "Error: Could not retrieve table structure for " << tableName << endl;
            continue; // Если не удалось инициализировать таблицу, пропускаем создание файлов
        }

        // Создаём необходимые файлы
        string path = schemaName + "/" + tableName + "/" + tableName + "_pk_sequence.txt";
        ofstream file1(path);
        file1 << 1; 
        file1.close();

        path = schemaName + "/" + tableName + "/" + tableName + "_lock.txt";
        ofstream file2(path);
        file2 << 0;  // Файл блокировки
        file2.close();
        
        string pathCsv = schemaName + "/" + tableName + "/1.csv";
        ofstream file3(pathCsv);
        if (file3.is_open()) {
            // Записываем заголовки в CSV файл
            file3 << colNames << "\n";
            file3.close();
            cout << "CSV file with headers created for table: " << tableName << endl;
        } else {
            cerr << "Error: Failed to create CSV file for table: " << tableName << endl;
        }
    }

    file.close();

    cout << "Keys initialized:" << endl;
    for (int i = 0; i < keys.size; ++i) {
        cout << "Key[" << i << "]: " << keys.data[i] << endl;
    }

    return 0;
}

#endif // DATA_H