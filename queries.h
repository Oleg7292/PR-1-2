#ifndef QUERIES_H
#define QUERIES_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>

#include "DynamicArray.h"
#include "HashTable.h"
#include "help.h"
#include "data.h"

void Where(const string& condition, 
           HashTable<string, DynamicArray<string>*>& jsonStructure, 
           DynamicArray<string>& keys, 
           string& result) {

    result.clear();
    
    // Разделяем условие на колонку и значение
    size_t pos = condition.find('=');
    if (pos == string::npos) {
        result = "Invalid WHERE condition";
        return;
    }
    
    string column = condition.substr(0, pos);
    string value = condition.substr(pos + 1);

    // Убираем пробелы и лишние символы (например, ; или пробелы) с обеих сторон
    value.erase(remove(value.begin(), value.end(), ' '), value.end());
    value.erase(remove(value.begin(), value.end(), ';'), value.end());

    // Убираем одинарные кавычки из значения
    if (value.front() == '\'' && value.back() == '\'') {
        value = value.substr(1, value.size() - 2);
    }

    // Проверяем наличие столбца
    auto* columnData = getHash(jsonStructure, column);
    if (!columnData) {
        result = "Column not found";
        return;
    }

    // Ищем строки, удовлетворяющие условию
    for (int i = 0; i < columnData->size; ++i) {
        if (columnData->data[i] == value) {
            string filteredRow;

            // Формируем строку результата из всех ключей
            for (int j = 0; j < keys.size; ++j) {
                auto* rowData = getHash(jsonStructure, keys.data[j]);
                filteredRow += rowData->data[i] + ",";
            }

            // Убираем последнюю запятую
            if (!filteredRow.empty()) {
                filteredRow.pop_back();
                filteredRow += "\n";
            }

            result += filteredRow;
        }
    }

    // Если ничего не найдено
    if (result.empty()) {
        result = "No matching rows found";
    }
}

// Вспомогательная функция для разделения table.column
bool splitTableColumn(const string& tableColumn, string& table, string& column) {
    size_t dotPos = tableColumn.find('.');
    if (dotPos == string::npos) {
        cerr << "Error: Missing '.' in column identifier." << endl;
        return false;
    }
    table = tableColumn.substr(0, dotPos);
    column = tableColumn.substr(dotPos + 1);
    return true;
}

// Вспомогательная функция для удаления кавычек
string removeQuotes(const string& value) {
    if (value.front() == '\'' && value.back() == '\'') {
        return value.substr(1, value.size() - 2);
    }
    return value;
}

// Чтение CSV файла в динамический массив (без использования STL)
bool readCSV(const string& path, DynamicArray<DynamicArray<string>>& tableData) {
    ifstream file(path);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << path << endl;
        return false;
    }

    string line;
    while (getline(file, line)) {
        auto* row = createArray<string>(10);
        stringstream ss(line);
        string value;

        while (getline(ss, value, ',')) {
            addElement(*row, value);
        }

        addElement(tableData, *row);
    }

    file.close();
    return true;
}

// SELECT с поддержкой WHERE
void Select(const string& query, 
            HashTable<string, DynamicArray<string>*>& jsonStructure, 
            const string& schemaName, 
            string& result) {
                
    istringstream iss(query);
    string word;

    // Чтение SELECT
    iss >> word;
    if (word != "SELECT") {
        cerr << "Error: Query does not start with SELECT." << endl;
        result = "Error: Invalid SELECT query.";
        return;
    }

    // Чтение table.column
    string tableColumn;
    iss >> tableColumn;

    string table, column;
    if (!splitTableColumn(tableColumn, table, column)) {
        result = "Error: Invalid column format in SELECT.";
        return;
    }

    // Чтение FROM
    iss >> word;
    if (word != "FROM") {
        result = "Error: Missing FROM clause.";
        return;
    }

    string fromTable;
    iss >> fromTable;
    if (fromTable != table) {
        result = "Error: Table names in FROM do not match.";
        return;
    }

    // Чтение WHERE (если есть)
    string where;
    string conditionColumn, conditionValue;
    if (iss >> where && where == "WHERE") {
        string condition;
        getline(iss, condition);

        // Разделение условия table.column = 'value'
        size_t eqPos = condition.find('=');
        if (eqPos == string::npos) {
            result = "Error: Invalid WHERE condition.";
            return;
        }

        string left = condition.substr(0, eqPos);
        conditionValue = removeQuotes(condition.substr(eqPos + 1));
        if (!splitTableColumn(left, table, conditionColumn)) {
            result = "Error: Invalid WHERE condition.";
            return;
        }
    }

    // Проверка наличия таблицы в jsonStructure
    auto* tableColumns = getHash(jsonStructure, table);
    if (!tableColumns) {
        result = "Error: Table not found.";
        return;
    }

    // Проверка наличия указанной колонки
    int columnIndex = -1;
    for (int i = 0; i < tableColumns->size; ++i) {
        if (tableColumns->data[i] == column) {
            columnIndex = i;
            break;
        }
    }

    if (columnIndex == -1) {
        result = "Error: Column not found in table.";
        return;
    }

    // Чтение данных из файла CSV
    string filePath = schemaName + "/" + table + "/1.csv";
    ifstream csvFile(filePath);
    if (!csvFile.is_open()) {
        result = "Error: Could not read table data.";
        return;
    }

    string line;
    getline(csvFile, line); // Пропускаем заголовок
    result.clear();

    // Формирование результата
    while (getline(csvFile, line)) {
        istringstream rowStream(line);
        string cell;
        int currentIndex = 0;
        string rowValue;
        bool matches = true;

        while (getline(rowStream, cell, ',')) {
            if (currentIndex == columnIndex && !conditionColumn.empty() && cell != conditionValue) {
                matches = false;
            }
            rowValue += cell + ",";
            ++currentIndex;
        }

        if (matches) {
            rowValue.pop_back(); // Удаляем последнюю запятую
            result += rowValue + "\n";
        }
    }

    csvFile.close();

    if (result.empty()) {
        result = "No matching rows found.";
    }
}

int Insert(const string& sql, 
           HashTable<string, DynamicArray<string>*>& jsonStructure, 
           HashTable<string, int>& numb, 
           const string& schemaName, 
           int tuplesLimit, 
           const DynamicArray<string>& keys) {
    cout << "Insert query received: " << sql << endl;

    size_t valuesPos = sql.find("VALUES");
    if (valuesPos == string::npos) {
        cout << "Error: VALUES keyword not found" << endl;
        return -1;
    }

    string values = sql.substr(valuesPos + 6); // Данные после VALUES
    values.erase(remove(values.begin(), values.end(), ' '), values.end());
    values.erase(remove(values.begin(), values.end(), '('), values.end());
    values.erase(remove(values.begin(), values.end(), ')'), values.end());

    DynamicArray<string>* rowValues = createArray<string>(10);
    stringstream ss(values);
    string value;
    while (getline(ss, value, ',')) {
        value.erase(remove(value.begin(), value.end(), '\''), value.end());
        addElement(*rowValues, value);
    }

    // Получаем имя таблицы
    string tableName = sql.substr(12, valuesPos - 12 - 1);
    tableName.erase(remove(tableName.begin(), tableName.end(), ' '), tableName.end());

    // Проверяем наличие таблицы в jsonStructure
    auto* columnData = getHash(jsonStructure, tableName);
    if (!columnData) {
        cerr << "Error: Table " << tableName << " not found in jsonStructure." << endl;
        destroyArray(*rowValues);
        return -1;
    }

    int numColumns = columnData->size;
    cout << "Number of columns in table: " << numColumns << endl;

    // Проверяем, включен ли первичный ключ в запрос
    bool hasPrimaryKey = (rowValues->size == numColumns);
    if (!hasPrimaryKey) {
        cout << "Primary key not provided. Generating new key..." << endl;
        string pkFilePath = schemaName + "/" + tableName + "/" + tableName + "_pk_sequence.txt";
        ifstream pkFile(pkFilePath);
        if (!pkFile.is_open()) {
            cerr << "Error: Could not open PK sequence file for table " << tableName << endl;
            destroyArray(*rowValues);
            return -1;
        }

        int pkValue;
        pkFile >> pkValue;
        pkFile.close();

        ofstream pkFileOut(pkFilePath);
        pkFileOut << pkValue + 1;
        pkFileOut.close();

        addElementAt(*rowValues, 0, to_string(pkValue)); // Добавляем PK в начало строки
    }

    // Проверяем количество колонок
    if (rowValues->size != numColumns) {
        cerr << "Error: Number of values (" << rowValues->size 
             << ") does not match number of columns (" << numColumns 
             << ") in table " << tableName << endl;
        destroyArray(*rowValues);
        return -1;
    }

    // Записываем данные в CSV
    string path = schemaName + "/" + tableName + "/1.csv";
    ofstream file(path, ios::app);
    for (int i = 0; i < rowValues->size; ++i) {
        file << rowValues->data[i];
        if (i < rowValues->size - 1) file << ",";
    }
    file << "\n";
    file.close();

    // Добавляем данные в jsonStructure
    for (int i = 0; i < columnData->size; ++i) {
        string columnName = columnData->data[i];
        auto* columnArray = getHash(jsonStructure, columnName);
        if (!columnArray) {
            columnArray = createArray<string>(10);  // Создаём новую колонку, если её ещё нет
            setHash(jsonStructure, columnName, columnArray);
        }
        addElement(*columnArray, rowValues->data[i]);
    }

    destroyArray(*rowValues);
    cout << "Data insertion successful." << endl;

    // Логируем данные после вставки
    cout << "Contents of jsonStructure after insert:" << endl;
    for (int i = 0; i < keys.size; ++i) {
        try {
            auto* columnArray = getHash(jsonStructure, keys.data[i]);
            if (columnArray) {
                cout << "Key: " << keys.data[i] << ", Data: ";
                for (int j = 0; j < columnArray->size; ++j) {
                    cout << columnArray->data[j] << " ";
                }
                cout << endl;
            }
        } catch (const runtime_error& e) {
            cout << "Key not found: " << keys.data[i] << endl;
        }
    }

    return 1;
}

// Функция для обработки строк в SQL-запросах (удаление лишних символов)
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r;");
    size_t last = str.find_last_not_of(" \t\n\r;");
    return (first == string::npos || last == string::npos) ? "" : str.substr(first, last - first + 1);
}

bool Delet(const string& tableName, const string& column, string value, 
                         const string& schemaName, HashTable<string, DynamicArray<string>*>& jsonStructure) {
    cout << "Deleting rows from table: " << tableName << ", column: " << column << ", value: " << value << endl;

    // Убираем кавычки из значения, если они есть
    if (!value.empty() && value.front() == '\'' && value.back() == '\'') {
        value = value.substr(1, value.size() - 2);
    }

    auto* columnData = getHash(jsonStructure, column);
    if (!columnData) {
        cerr << "Column '" << column << "' not found in table '" << tableName << "'" << endl;
        return false;
    }

    // Логируем данные колонки
    cout << "Data in column '" << column << "': ";
    for (int i = 0; i < columnData->size; ++i) {
        cout << columnData->data[i] << " ";
    }
    cout << endl;

    // Находим индекс строки для удаления
    bool rowDeleted = false;
    for (int i = 0; i < columnData->size; ++i) {
        cout << "Comparing '" << columnData->data[i] << "' with '" << value << "'" << endl;
        if (columnData->data[i] == value) {
            cout << "Match found at row " << i << endl;

            // Удаляем строку из всех колонок таблицы
            auto* tableColumns = getHash(jsonStructure, tableName);
            if (!tableColumns) {
                cerr << "Table structure not found for table '" << tableName << "'" << endl;
                return false;
            }

            for (int j = 0; j < tableColumns->size; ++j) {
                auto* columnArray = getHash(jsonStructure, tableColumns->data[j]);
                if (columnArray) {
                    removeElement(*columnArray, i);
                }
            }

            rowDeleted = true;
            break; // Удаляем только первую найденную строку
        }
    }

    if (!rowDeleted) {
        cerr << "No matching rows found in table: " << tableName << endl;
        return false;
    }

    // Обновляем CSV-файл
    string csvPath = schemaName + "/" + tableName + "/1.csv";
    ofstream csvFile(csvPath);
    if (!csvFile.is_open()) {
        cerr << "Error: Could not open CSV file for updating: " << csvPath << endl;
        return false;
    }

    auto* tableColumns = getHash(jsonStructure, tableName);
    if (!tableColumns) {
        cerr << "Table structure not found for table '" << tableName << "'" << endl;
        return false;
    }

    // Записываем заголовки
    for (int j = 0; j < tableColumns->size; ++j) {
        csvFile << tableColumns->data[j];
        if (j < tableColumns->size - 1) {
            csvFile << ",";
        }
    }
    csvFile << "\n";

    // Записываем оставшиеся строки
    int numRows = columnData->size; // Обновлённое количество строк
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < tableColumns->size; ++j) {
            auto* columnArray = getHash(jsonStructure, tableColumns->data[j]);
            if (columnArray) {
                csvFile << columnArray->data[i];
            }
            if (j < tableColumns->size - 1) {
                csvFile << ",";
            }
        }
        csvFile << "\n";
    }

    csvFile.close();
    cout << "Row deleted successfully from table: " << tableName << endl;
    return true;
}

#endif // QUERIES_H