#ifndef LOCK_H
#define LOCK_H

#include <string>
using namespace std;

int Lock1(string schemaName, string key);
int Lock0(string schemaName, string key);

#endif // LOCK_H