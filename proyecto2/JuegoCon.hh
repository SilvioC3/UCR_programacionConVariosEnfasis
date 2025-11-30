#ifndef JUEGOCON_HH
#define JUEGOCON_HH

#include <iostream>
#include <vector>

enum tipoDeNodo {base, recurso, fabrica };

struct NodoJuego
{
    int id;
    tipoDeNodo tipo;
    std::string nombre; 
    int recursos;  
    //int arista temporal
    //int x, y; // para interface grafica
};


class JuegoCon
{
private:
    /// @brief vector que contiene los nodos
    std::vector<NodoJuego> nodos;
    std::vector <std::vector< std::pair <int, int>>> aristas;
    int current;

public:
    JuegoCon(int n);
    ~JuegoCon();
    void agregarNodo(int id, const NodoJuego& nodo);
    void agregarArista(int nodo, int vecino, int peso);
    void accederNodo(int indiceNodo);
    void verVecinos(int indiceArista);
    void cargadorGrafo(const std::string& ruta);
    int getPeso(int origen, int destino) const;
    const NodoJuego& getNodo(int id) const;
    const std::vector<std::pair<int,int>>& getVecinos(int nodo) const;
};


#endif