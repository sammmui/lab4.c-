#include "fields.h"
#include <mutex>

DataStructure::DataStructure(int m)
    : fields(m, 0), locks(m) {}

int DataStructure::read(int i) {
    std::lock_guard<std::mutex> guard(locks[i]);
    return fields[i];
}

void DataStructure::write(int i, int value) {
    std::lock_guard<std::mutex> guard(locks[i]);
    fields[i] = value;
}

std::string DataStructure::toString() {
    std::lock_guard<std::mutex> guard(string_lock);

    std::string s = "{ ";
    for (size_t i = 0; i < fields.size(); ++i) {
        std::lock_guard<std::mutex> g2(locks[i]);
        s += std::to_string(fields[i]);
        if (i + 1 < fields.size()) s += ", ";
    }
    s += " }";
    return s;
}
