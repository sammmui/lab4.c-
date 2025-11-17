#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <random>
#include "fields.h"

using namespace std;

struct Action {
    string op;
    int field;
    int value;
};

vector<Action> loadFile(const string& name) {
    ifstream f(name);
    vector<Action> v;
    string op;
    while (f >> op) {
        if (op == "read") {
            int i;
            f >> i;
            v.push_back({op, i, 0});
        } else if (op == "write") {
            int i, val;
            f >> i >> val;
            v.push_back({op, i, val});
        } else if (op == "string") {
            v.push_back({op, 0, 0});
        }
    }
    return v;
}

void runThread(DataStructure* ds, const vector<Action>& actions) {
    auto start = chrono::steady_clock::now();

    for (auto& a : actions) {
        if (a.op == "read") ds->read(a.field);
        else if (a.op == "write") ds->write(a.field, a.value);
        else if (a.op == "string") ds->toString();
    }

    auto end = chrono::steady_clock::now();
    cout << "Thread finished in "
         << chrono::duration_cast<chrono::milliseconds>(end - start).count()
         << " ms\n";
}

void generateFile(const string& name, int count,
                  int r0, int w0, int r1, int w1, int strp)
{
    ofstream f(name);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> rnd(1, 100);

    for (int i = 0; i < count; i++) {
        int p = rnd(gen);

        if (p <= r0) f << "read 0\n";
        else if (p <= r0 + w0) f << "write 0 1\n";
        else if (p <= r0 + w0 + r1) f << "read 1\n";
        else if (p <= r0 + w0 + r1 + w1) f << "write 1 1\n";
        else f << "string\n";
    }
}

int main() {
    // ВАРІАНТ 17
    // read0=5%, write0=5%, read1=30%, write1=5%, string=55%

    generateFile("caseA.txt", 200000, 5, 5, 30, 5, 55);

    // рівні частоти
    generateFile("caseB.txt", 200000, 20, 20, 20, 20, 20);

    // нерівні (сильно викривлені)
    generateFile("caseC.txt", 200000, 40, 1, 1, 1, 57);

    vector<string> files = { "caseA.txt", "caseB.txt", "caseC.txt" };

    for (auto& file : files) {
        cout << "\n--- FILE: " << file << " ---\n";

        auto actions = loadFile(file);

        cout << "[1 потік]\n";
        {
            DataStructure ds(2);
            runThread(&ds, actions);
        }

        cout << "[2 потоки]\n";
        {
            DataStructure ds(2);
            auto half = actions.size() / 2;
            thread t1(runThread, &ds, vector<Action>(actions.begin(), actions.begin() + half));
            thread t2(runThread, &ds, vector<Action>(actions.begin() + half, actions.end()));
            t1.join();
            t2.join();
        }

        cout << "[3 потоки]\n";
        {
            DataStructure ds(2);
            auto p1 = actions.size() / 3;
            auto p2 = 2 * actions.size() / 3;

            thread t1(runThread, &ds, vector<Action>(actions.begin(), actions.begin() + p1));
            thread t2(runThread, &ds, vector<Action>(actions.begin() + p1, actions.begin() + p2));
            thread t3(runThread, &ds, vector<Action>(actions.begin() + p2, actions.end()));
            t1.join();
            t2.join();
            t3.join();
        }
    }

    return 0;
}
