#include "graph.h"

int main(){
    int n;
    bool reconhece;
    fstream File;
    string word;
    string filename;
    string expression;
    
    cout << "Bem vinde! Informe o arquivo de leitura: ";
    cin >> filename;

    File.open(filename, ios::in);

    File >> expression;

    Graph* G = new Graph(expression);
    G->buildAutomata(expression);

    File >> n;
    
    G->printGraph();
    for(int i = 0; i < n; i++){
        File >> word;

        reconhece = G->recognize(word);
        if(reconhece)
            cout << "S" << endl;
        else    
            cout << "N" << endl;
    }

    return 0;
}