#pragma once

#include <stack>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

// state
struct No{
    char op;
    int size;
    int vertex;
    bool ehLambdaT;
    string operationType;
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
        bool recognize(string word);
        void printGraph();
        void addEdge(int v, int w);
        void dfsR(int v, bool* marked);
        void buildAutomata(string regexp);
        bool defineLambdaT(string exp, int index);
        string defineOp(string exp, int index);
};