#include "JuegoCon.hh"

JuegoCon::JuegoCon(int n)
{
    this->nodos.resize(n);
    this->aristas.resize(n);

}

JuegoCon::~JuegoCon()
{
}

void JuegoCon::agregarNodo(int id, const NodoJuego& nodo){
    
    this->nodos[id] = nodo;

}

void JuegoCon::agregarArista(int nodoId, int vecino, int peso){
    this->aristas[nodoId].push_back({vecino, peso});

}

void JuegoCon::accederNodo(int indiceNodo){
    NodoJuego temp = this->nodos[indiceNodo];
    std::cout << "nodo: " << temp.nombre << std::endl;

}

void JuegoCon::verVecinos(int indice){
    std::cout << "vecinos de: " << indice << " son:\n";
    for (auto &vecinos : this->aristas[indice] ){
        std::cout<< "( " << vecinos.first << " peso: " << vecinos.second << " ) ";

    }
    std::cout << std::endl;
}
