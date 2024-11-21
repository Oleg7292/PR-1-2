#ifndef HELP_H
#define HELP_H

#include <string>
#include "DynamicArray.h"

using std::string;

// Возвращает подстроку строки str в пределах [begin, end)
string SizeIndex(string str, int begin, int end);

// Читает CSV-файл и возвращает двумерный динамический массив строк
DynamicArray<DynamicArray<string>> ReadCSV(const string& filename);

// Добавляет строку addstr в конец файла filename
int AddInFile(const string& filename, const string& addstr);

#endif // HELP_H