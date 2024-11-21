#ifndef DYNAMIC_ARRAY_IMPL_H
#define DYNAMIC_ARRAY_IMPL_H

#include "DynamicArray.h"

using namespace std;

template <typename T>
DynamicArray<T>* createArray(int initialCapacity, int expandThreshold) {
    if (initialCapacity <= 0 || expandThreshold <= 0 || expandThreshold > 100) {
        throw std::invalid_argument("Invalid capacity or threshold value.");
    }

    auto* arr = new DynamicArray<T>;
    arr->data = new T[initialCapacity];
    arr->size = 0;
    arr->capacity = initialCapacity;
    arr->expandRatio = expandThreshold;

    return arr;
}

template <typename T>
void destroyArray(DynamicArray<T>& arr) {
    delete[] arr.data;
    arr.data = nullptr;
    arr.size = 0;
    arr.capacity = 0;
}

template <typename T>
void expandArray(DynamicArray<T>& arr) {
    int newCapacity = arr.capacity * 2;
    T* newData = new T[newCapacity];

    for (int i = 0; i < arr.size; ++i) {
        newData[i] = arr.data[i];
    }

    delete[] arr.data;
    arr.data = newData;
    arr.capacity = newCapacity;
}

template <typename T>
void addElement(DynamicArray<T>& arr, const T& value) {
    if (arr.data == nullptr) {
        cerr << "Error: Array data is null. Initializing array." << endl;
        arr.data = new T[arr.capacity];
    }

    if ((arr.size + 1) * 100 / arr.capacity >= arr.expandRatio) {
        expandArray(arr);
    }

    arr.data[arr.size++] = value;
}

template <typename T>
void addElementAt(DynamicArray<T>& arr, int index, const T& value) {
    if (index < 0 || index > arr.size) {
        throw out_of_range("Index out of range.");
    }

    if ((arr.size + 1) * 100 / arr.capacity >= arr.expandRatio) {
        expandArray(arr);
    }

    for (int i = arr.size; i > index; --i) {
        arr.data[i] = arr.data[i - 1];
    }

    arr.data[index] = value;
    arr.size++;
}

template <typename T>
void removeElement(DynamicArray<T>& arr, int index) {
    if (index < 0 || index >= arr.size) {
        throw std::out_of_range("Index out of range.");
    }

    for (int i = index; i < arr.size - 1; ++i) {
        arr.data[i] = arr.data[i + 1];
    }

    arr.size--;
}

template <typename T>
void replaceElement(DynamicArray<T>& arr, int index, const T& value) {
    if (index < 0 || index >= arr.size) {
        throw std::out_of_range("Index out of range.");
    }
    arr.data[index] = value;
}

template <typename T>
T getElement(const DynamicArray<T>& arr, int index) {
    if (index < 0 || index >= arr.size) {
        throw std::out_of_range("Index out of range.");
    }
    return arr.data[index];
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const DynamicArray<T>& arr) {
    for (int i = 0; i < arr.size; ++i) {
        os << arr.data[i];
        if (i < arr.size - 1) os << "\t";
    }
    return os;
}

#endif // DYNAMIC_ARRAY_IMPL_H