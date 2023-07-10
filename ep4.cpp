#include "graph.h"

int le_conjunto(int indexExp, int indexPal, string word, string exp);
int le_complemento(int indexExp, int indexPal, string word, string exp);

int main(){
    int n;
    string word;
    string expression;
    
    cout << "Bem vinde! Informe a expressão regular: ";
    cin >> expression;

    cout << endl << "Informe quantas palavras serão testadas ";
    cin >> n;

    for(int i = 0; i < n; i++){
        cout << endl << "Informe a palavra: ";
        cin >> word;

    }

    return 0;
}

int le_conjunto(int indexExp, int indexPal, string word, string exp){
    while(exp[indexExp] != ']'){
        if(exp[indexExp] == word[indexPal])
            return 1;
        indexExp++;
    }
    return 0;
}

int le_complemento(int indexExp, int indexPal, string word, string exp){
    while(exp[indexExp] != ']'){
        if(exp[indexExp] == word[indexPal])
            return 0;
        indexExp++;
    }
    return 1;
}