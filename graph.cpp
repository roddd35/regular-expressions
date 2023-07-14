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
        node->operationType = defineOp(exp, i);
        node->ehLambdaT = defineLambdaT(exp, i);

        this->adjList.push_back(node);
        this->V++;
    }
}

// define se um vértice realiza lambda transição
bool Graph::defineLambdaT(string exp, int index){
    if(exp[index] == '|' || exp[index] == '(' || exp[index] == ')' || exp[index] == '[' || exp[index] == ']' || exp[index] == '+' || exp[index] == '*' || exp[index] == '\\'){
        if(index == 0) return true;

        else if(exp[index - 1] != '\\') return true;
    }
    return false;
}

// definir se é operação de conjunto, intervalo ou complemento, no primeiro elemento após "["
string Graph::defineOp(string exp, int index){
    if(exp[index - 1] == '['){
        if(exp[index] == '^')
            return "complemento";
        else if(exp[index + 1] == '-')
            return "intervalo";
        else
            return "conjunto";
    }
    else
        return "";
}

// FUNCAO: CONSTROI ARESTAS DO AUTOMATO
// terminar os operadores: conjunto ([]), complemento ([^]), intervalo ([-])
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

        node->vertex = w;
        node->op = this->adjList[w]->op;
        node->size = this->adjList[v]->size + 1;
        node->operationType = this->adjList[w]->operationType;

        this->adjList[v]->nodesList.push_back(node);
        this->adjList[v]->size++;
        this->E++;
    }
}

// FUNCAO: IMPRIME VERTICES E ARESTAS DO GRAFO
void Graph::printGraph(){
    for(int i = 0; i < this->V; i++){
        cout << "==========================================" << endl;
        cout << "Vértice: " << this->adjList[i]->op << "    " << this->adjList[i]->operationType << endl;
        cout << "Arestas: " << endl; 
        for(int j = 0; j < this->adjList[i]->size; j++)
            cout << "   " << this->adjList[i]->nodesList[j]->op << " " << this->adjList[i]->nodesList[j]->vertex << "    " << this->adjList[i]->nodesList[j]->operationType << endl;
    }
}

// FUNCAO: BUSCA EM PROFUNDIDADE NO GRAFO
void Graph::dfsR(int v, bool* marked){
    marked[v] = true;   // marcar o proprio vertice, caso o DFS nao encontre mais nenhum

    // fazer DFS apenas em lambda-t
    for(int w = 0; w < this->adjList[v]->size; w++){    
        if(!marked[this->adjList[v]->nodesList[w]->vertex] && this->adjList[v]->ehLambdaT)
            dfsR(this->adjList[v]->nodesList[w]->vertex, marked);
    }
}

// FUNCAO: VERIFICA SE O AUTOMATO RECONHECE UMA PALAVRA
bool Graph::recognize(string word){
    if(word == "" && this->adjList[this->V - 1]->op == '*')
        return true;

    bool* visited = new bool[this->V];
    for(int i = 0; i < this->V; i++)
        visited[i] = false;

    dfsR(0, visited);   // marcar todos vertices que visitamos saindo de 0

    // iterar por cada letra da palavra que queremos
    for(int i = 0; i < word.length(); i++){
        bool* next = new bool[this->V];
        for(int j = 0; j < this->V; j++)
            next[j] = false;

        // determinar as proximas posicoes que devemos visitar no grafo, com o caractere que queremos ler
        for(int j = 0; j < this->V; j++){    
            // letra solta ou coringa
            if(visited[j] && this->adjList[j]->operationType == "" && (this->adjList[j]->op == word[i] || (this->adjList[j]->op == '.' && this->adjList[j - 1]->op != '\\')))
                next[j + 1] = true;

            // conjunto
            else if(visited[j] && this->adjList[j]->operationType == "conjunto"){
                int k = j;
                bool encaixaConjunto = false;
                while(this->adjList[k]->op != ']'){
                    if(this->adjList[k]->op == word[i])
                        encaixaConjunto = true;
                    k++;
                }
                if(encaixaConjunto)
                    next[k] = true; // marco true na casa do "]"
            }

            // complemento
            else if(visited[j] && this->adjList[j]->operationType == "complemento"){
                if(this->adjList[j + 2]->op == '-'){    // complemento de intervalo
                    int ini = this->adjList[j + 1]->op;
                    int fim = this->adjList[j + 3]->op;
                    if(!(word[i] >= ini && word[i] <= fim))
                        next[j + 4] = true;
                }
                else{   // complemento padrao
                    int k = j;
                    bool encaixaComplemento = true;
                    while(this->adjList[k]->op != ']'){
                        if(this->adjList[k]->op == word[i])
                            encaixaComplemento = false;
                        k++;
                    }
                    if(encaixaComplemento)
                        next[k] = true;
                }
            }
            
            // intervalo
            else if(visited[j] && this->adjList[j]->operationType == "intervalo"){
                int ini = this->adjList[j]->op;
                int fim = this->adjList[j + 2]->op;
                if(word[i] >= ini && word[i] <= fim)
                    next[j + 3] = true;
            }
        }

        bool* marked = new bool[this->V];
        for(int j = 0; j < this->V; j++)
            marked[j] = false;

        // encontrar, da letra que lemos, onde podemos chegar por lambda-t
        bool rodou = false;
        for(int j = 0; j < this->V; j++){
            if(next[j]){
                rodou = true;
                dfsR(j, marked);
            }
        }

        // se não chegamos ao próximo caractere, quebra
        if(!rodou)
            return false;

        // manter o resultado obtido no DFS pra rodar na proxima iteracao do grafo
        for(int j = 0; j < this->V; j++)
            visited[j] = marked[j];

        delete[] next;
        delete[] marked;
    }

    bool retorno = visited[this->V - 1];
    delete[] visited;
    return retorno;    // se visitamos até a última letra da palavra, retorna True
}