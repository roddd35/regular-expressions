#include "graph.h"

// FUNCAO: CONSTRUTOR DO GRAFO
Graph::Graph(string exp){
    this->V = 0;
    this->E = 0;
    this->expression = exp;

    // inicializando o grafo com a expressão regular
    for(int i = 0; i < exp.length(); i++){
        No* node = new No();
        node->size = 0;
        node->op = exp[i];
        node->vertex = this->V;

        this->adjList.push_back(node);
        this->V++;
    }
}

// FUNCAO: CONSTROI GRAFO DO AUTOMATO
// terminar os operadores: coringa (.), conjunto ([]), complemento ([^])
// fazer tratamento para "\@", com @ sendo qualquer caracter especial e para números
void Graph::buildAutomata(string regexp){
    int sizeRE = regexp.length();
    stack<int> operatorST;  // armazenar os operadores "(" e "|"
    
    for(int i = 0; i < sizeRE; i++){
        int lp = i; // parenteses esquerdo
        if(regexp[i] == '(' || regexp[i] == '|')
            operatorST.push(i); 
        else if(regexp[i] == ')'){
            int orOP = operatorST.top();    // recebo a posição "|" ou "(" desempilhado
            operatorST.pop();

            if(regexp[orOP] == '|'){    // se for "|", recebo o parenteses que veio antes
                lp = operatorST.top();
                operatorST.pop();
                addEdge(lp, orOP + 1);  // conecto o parenteses para depois do OR
                addEdge(orOP, i);   // conecto o OR com a posição do ")"
            }
            else
                lp = orOP;  // recebo o parenteses desempilhado aqui
        }
        // se o próximo símbolo for *, coloco uma aresta dele para "(" e vice versa
        if(i < sizeRE - 1 && regexp[i + 1] == '*'){
            addEdge(lp, i + 1);
            addEdge(i + 1, lp);
        }

        // se o próximo símbolo for +, coloco uma aresta dele para "("
        if(i < sizeRE - 1 && regexp[i + 1] == '+')
            addEdge(i + 1, lp);

        // se o atual for algum desses simbolos, coloco uma aresta para o seguinte
        if((regexp[i] == '(' || regexp[i] == '*' || regexp[i] == ')' || (regexp[i] >= 65 && regexp[i] <= 90)) && i < sizeRE - 1)
            addEdge(i, i + 1);
    }
}

// FUNCAO: ADICIONA ARESTAS NO GRAFO
void Graph::addEdge(int v, int w){
    bool achou = false;

    // verifica se ja adicionamos o vertice, evita duplicar
    for(int i = 0; i < this->adjList[v]->size; i++) 
        if(this->adjList[v]->nodesList[i]->vertex == w)
            achou = true;
    
    if(!achou){
        No* node = new No();
        node->op = this->adjList[w]->op;
        node->size = this->adjList[v]->size + 1;
        node->vertex = w;

        this->adjList[v]->nodesList.push_back(node);
        this->adjList[v]->size++;
        this->E++;
    }
}

// FUNCAO: IMPRIME VERTICES E ARESTAS DO GRAFO
void Graph::printGraph(){
    for(int i = 0; i < this->V; i++){
        cout << "==========================================" << endl;
        cout << "Vértice: " << this->adjList[i]->op << endl;
        cout << "Arestas: " << endl; 
        for(int j = 0; j < this->adjList[i]->size; j++)
            cout << "   " << this->adjList[i]->nodesList[j]->op << " " << this->adjList[i]->nodesList[j]->vertex << endl;
    }
}

// FUNCAO: BUSCA EM PROFUNDIDADE NO GRAFO
void Graph::dfsR(int v, bool* marked){
    marked[v] = true;

    for(int w = 0; w < this->adjList[v]->size; w++){
        if(!marked[this->adjList[v]->nodesList[w]->vertex])
            dfsR(this->adjList[v]->nodesList[w]->vertex, marked);
    }
}

// FUNCAO: VERIFICA SE O AUTOMATO RECONHECE UMA PALAVRA
bool Graph::recognize(string word){
    bool* visited = new bool[this->V];
    for(int i = 0; i < this->V; i++)
        visited[i] = false;
    dfsR(0, visited);   // marcar todos vertices que visitamos saindo de 0

    for(int i = 0; i < word.length(); i++){
        bool* next = new bool[this->V];
        for(int j = 0; j < this->V; j++)
            next[j] = false;

        for(int j = 0; j < this->V; j++)
            if(visited[j] && this->adjList[j]->op == word[i])
                next[j] = true; // saber por quais letras da palavra já passamos

        bool* marked = new bool[this->V];
        for(int j = 0; j < this->V; j++)
            marked[j] = false;

        for(int j = 0; j < this->V; j++){
            if(next[j])
                for(int k = 0; k < this->V; k++)
                    marked[k] = false;
            dfsR(j, marked);
            for(int k = 0; k < this->V; k++)
                if(marked[k])
                    visited[k] = true;
        }
        delete[] next;
        delete[] marked;
    }

    bool retorno = visited[this->V - 1];
    delete[] visited;
    return retorno;    // se visitamos até a última letra da palavra, retorna True
}