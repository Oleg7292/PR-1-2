#include "lock.h"
#include <fstream>

int Lock1(string schemaName, string key) {
    string Path = schemaName + "/" + key + "/" + key + "_lock.txt"; 
    ofstream file2(Path);
    file2 << 1;
    file2.close();
    return 0;
}

int Lock0(string schemaName, string key) {
    string Path = schemaName + "/" + key + "/" + key + "_lock.txt"; 
    ofstream file2(Path);
    file2 << 0;
    file2.close();
    return 0;
}