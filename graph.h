#pragma once
#include <vector>
#include <string>
#include <iostream>

using namespace std;

// state
struct No{
    char op;
    int size;
    int vertex;
    vector<No*> nodesList; // saving which state connects to which
};

class Graph {
    private:
        int V;  // number of states on the regular expression
        int E;
        string expression;
        vector<No*> adjList;

    public:
        Graph(string exp);
        void buildAutomata();
        void dfsR(int v, bool* marked);
        bool recognize(string word);
};