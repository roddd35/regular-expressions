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
                if(regexp[orOP + 2] == '*')
                    addEdge(lp, orOP + 2);
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

        // 33-126 sao as letras, numeros e caracteres especiais na tabela ascii, pular pro estado seguinte
        if(regexp[i] >= 33 && regexp[i] <= 126 && regexp[i] != '|' && i < sizeRE - 1)
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
    marked[v] = true;   // marcar o proprio vertice, caso o DFS nao encontre mais nenhum

    // fazer DFS apenas em lambda-t
    for(int w = 0; w < this->adjList[v]->size; w++){    
        if(!marked[this->adjList[v]->nodesList[w]->vertex] && (this->adjList[v]->op == '|' ||this->adjList[v]->op == '(' || this->adjList[v]->op == ')' || this->adjList[v]->op == '+' || this->adjList[v]->op == '*'))
            dfsR(this->adjList[v]->nodesList[w]->vertex, marked);
    }
}

// FUNCAO: VERIFICA SE O AUTOMATO RECONHECE UMA PALAVRA
bool Graph::recognize(string word){
    bool rodou = false;

    bool* visited = new bool[this->V];
    for(int i = 0; i < this->V; i++)
        visited[i] = false;

    dfsR(0, visited);   // marcar todos vertices que visitamos saindo de 0

    // cout << "VETOR VISITED INICIO: " << endl;
    // for(int i = 0; i < this->V; i++)
    //     cout << visited[i] << " ";
    // cout << endl;

    for(int i = 0; i < word.length(); i++){
        bool* next = new bool[this->V];
        for(int j = 0; j < this->V; j++)
            next[j] = false;

        // determinar as proximas posicoes que devemos visitar no grafo, com o caractere que queremos ler
        for(int j = 0; j < this->V; j++)    // tratamento pro coringa ser lido como qualquer caractere
            if(visited[j] && (this->adjList[j]->op == word[i] || (this->adjList[j]->op == '.' && this->adjList[j - 1]->op != '\\')))
                next[j + 1] = true;

        // cout << "VETOR NEXT: " << endl;
        // for(int j = 0; j < this->V; j++)
        //     cout << next[j] << " ";
        // cout << endl;

        bool* marked = new bool[this->V];
        for(int j = 0; j < this->V; j++)
            marked[j] = false;

        // encontrar, da letra que lemos, onde podemos chegar por lambda-t
        for(int j = 0; j < this->V; j++){
            if(next[j]){
                rodou = true;
                dfsR(j, marked);
            }
        }
        if(!rodou)
            return false;

        // cout << "VETOR MARKED: " << endl;
        // for(int j = 0; j < this->V; j++)
        //     cout << marked[j] << " ";
        // cout << endl;

        // manter o resultado obtido no DFS pra rodar na proxima iteracao do grafo
        for(int j = 0; j < this->V; j++)
            visited[j] = marked[j];

        delete[] next;
        delete[] marked;
        rodou = false;
    }
    // cout << "VETOR VISITED FIM: " << endl;
    // for(int i = 0; i < this->V; i++)
    //     cout << visited[i] << " ";
    // cout << endl;

    bool retorno = visited[this->V - 1];
    delete[] visited;
    return retorno;    // se visitamos até a última letra da palavra, retorna True
}