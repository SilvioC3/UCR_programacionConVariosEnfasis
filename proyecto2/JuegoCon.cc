#include "JuegoCon.hh"
#include <fstream>
#include <sstream>
#include <string>  
#include <iostream>


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
    std::cout << "Nodo: " << temp.nombre << std::endl;

}

void JuegoCon::verVecinos(int indice){
    std::cout << "Los vecinos de: " << indice << " son:\n";
    for (auto &vecinos : this->aristas[indice] ){
        std::cout<< "( " << vecinos.first << " peso: " << vecinos.second << " ) ";

    }
    std::cout << std::endl;
}

/// @brief Metodo para cargar el grafo desde un txt
void JuegoCon::cargadorGrafo(const std::string& ruta)       
{
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) 
    {
        std::cout << "ERROR: No se pudo abrir el archivo.\n";
        return;
    }
    std::string linea; 
    while (std::getline(archivo, linea)) 
    {
        if (linea == "NODOS:") break;
    }

    int maxId = -1;

    while (std::getline(archivo, linea)) 
    {
        if (linea == "CONEXIONES:") break;
        if (linea.empty()) continue;
           std::stringstream ss(linea);
        int id, tipoInt, recursos, x, y;
        char coma;

        ss >> id >> coma >> tipoInt >> coma >> recursos >> coma >> x >> coma >> y;
        if (id >= nodos.size()) 
        {
            nodos.resize(id + 1);
            aristas.resize(id + 1);
        }   


        //Crea el nodo en si con todos los atributos
        NodoJuego nodo;
        nodo.id = id;
        nodo.tipo = static_cast<tipoDeNodo>(tipoInt); 
        nodo.nombre = "Nodo " + std::to_string(id);
        nodo.recursos = recursos;
        this->agregarNodo(id, nodo);
        maxId = std::max(maxId, id);
    }

    while (std::getline(archivo, linea)) 
    {
        if (linea.empty()) continue;

        std::stringstream ss(linea);
        int u, v, peso;
        char coma;
        ss >> u >> coma >> v >> coma >> peso;
        this->agregarArista(u, v, peso);
    }

    archivo.close();
    std::cout << "Se ha creado el mapa inicial, con un total de: " << nodos.size() << " nodos.\n";
}

int JuegoCon::getPeso(int origen, int destino) const 
{
    for (auto &par : aristas[origen]) {
        if (par.first == destino)
            return par.second;
    }
    return -1;
}

//Metodo para hacer get a los vecinos del nodo actual
const std::vector<std::pair<int,int>>& JuegoCon::getVecinos(int nodo) const
{
    return aristas[nodo];
}

//Metodo para hacer get a todos los datos del nodo actual
const NodoJuego& JuegoCon::getNodo(int id) const 
{
    return nodos[id];
}
