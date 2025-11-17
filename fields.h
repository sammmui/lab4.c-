#pragma once
#include <vector>
#include <string>
#include <mutex>

class DataStructure {
private:
    std::vector<int> fields;
    std::vector<std::mutex> locks;  // по одному м’ютексу на поле
    std::mutex string_lock;         // окремий м’ютекс для operator string

public:
    DataStructure(int m);

    int read(int i);
    void write(int i, int value);
    std::string toString();
};
