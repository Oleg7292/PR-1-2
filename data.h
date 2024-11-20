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
        fs::create_directory(schemaName + "/" + tableName);

        // Создаём массив для колонок таблицы
        auto* tempValue = createArray<string>(10); // Колонки таблицы
        addElement(keys, tableName);              // Добавляем название таблицы в keys

        string colNames = tableName + "_pk";
        for (const auto& column : columns) {
            string columnName = column.get<string>(); 
            colNames += "," + columnName;
            addElement(*tempValue, columnName);
        }

        setHash(jsonStructure, tableName, tempValue);

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
        file3 << colNames << "\n";
        file3.close();
    }

    file.close();

    // Логируем массив keys для проверки
    cout << "Keys initialized:" << endl;
    for (int i = 0; i < keys.size; ++i) {
        cout << "Key[" << i << "]: " << keys.data[i] << endl;
    }

    return 0;
}

#endif // DATA_H