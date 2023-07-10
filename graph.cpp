#include "graph.h"

Graph::Graph(string exp){
    this->V = 0;
    this->E = 0;
    this->expression = exp;
}

bool Graph::recognize(string word){
    bool* visited = new bool[this->V];
    for(int i = 0; i < this->V; i++)
        visited[i] = false;
    dfsR(0, visited);   // marcar todos vertices que visitamos saindo de 0

    for(int i = 0; i < this->V; i++){
        bool* next = new bool[this->V];
        for(int j = 0; j < this->V; j++)
            next[j] = false;

        for(int j = 0; j < this->V; j++)
            if(visited[j] && this->expression[j] == word[i])
                next[i + 1] = true; // saber por quais letras da palavra já passamos

        bool* marked = new bool[this->V];
        for(int j = 0; j < this->V; j++)
            marked[j] = false;

        for(int j = 0; j < this->V; j++){
            if(next[j]){
                for(int k = 0; k < this->V; k++)
                    marked[k] = false;
                dfsR(j, marked);
                for(int k = 0; k < this->V; k++)
                    if(marked[k])
                        visited[k] = true;
            }
        }
    }
    return visited[this->V - 1];    // se visitamos até a última letra da palavra, retorna True
}

void Graph::dfsR(int v, bool* marked){
    marked[v] = true;

    for(int w = 0; w < this->V; w++){
        if(!marked[this->adjList[v]->nodesList[w]->vertex])
            dfsR(this->adjList[v]->nodesList[w]->vertex, marked);
    }
}