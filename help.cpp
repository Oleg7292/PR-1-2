#include "help.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

string SizeIndex(string str, int begin, int end) {
    if (str.size() < end) {
        cout << "Попробуйте ввести запрос ещё раз)";
        return "";
    }
    string strcut;
    for (int i = begin; i < end; i++) {
        strcut.push_back(str[i]);
    }
    return strcut;
}