#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <iostream>
#include <stdexcept>

template <typename T>
struct DynamicArray {
    T* data;           
    int size;          
    int capacity;      
    int expandRatio;   
};

template <typename T>
DynamicArray<T>* createArray(int initialCapacity = 10, int expandThreshold = 75);

template <typename T>
void destroyArray(DynamicArray<T>& arr);

template <typename T>
void addElement(DynamicArray<T>& arr, const T& value);

template <typename T>
void removeElement(DynamicArray<T>& arr, int index);

template <typename T>
void replaceElement(DynamicArray<T>& arr, int index, const T& value);

template <typename T>
T getElement(const DynamicArray<T>& arr, int index);

template <typename T>
std::ostream& operator<<(std::ostream& os, const DynamicArray<T>& arr);

#include "DynamicArray_impl.h" 

#endif // DYNAMIC_ARRAY_H