#ifndef JUEGOCON_HH
#define JUEGOCON_HH

#include <iostream>
#include <vector>
#include "Jugador.hh"

enum tipoDeNodo {base, recurso, fabrica };

struct NodoJuego
{
    int id;
    tipoDeNodo tipo;
    std::string nombre; 
    int recursos;  
    //int arista temporal
    int x;
    int y;
};


class JuegoCon
{
private:
    /// @brief vector que contiene los nodos
    std::vector<NodoJuego> nodos;
    std::vector <std::vector< std::pair <int, int>>> aristas;
    int current;
    bool isLooping;


public:
    JuegoCon(int n);
    ~JuegoCon();
    void agregarNodo(int id, const NodoJuego& nodo);
    void agregarArista(int nodo, int vecino, int peso);
    void accederNodo(int indiceNodo, Jugador * jugador);
    void verVecinos(int indiceArista);
    void cargadorGrafo(const std::string& ruta);
    int getPeso(int origen, int destino) const;
    int getRecurso(int nodo);
    const NodoJuego& getNodo(int id) const;
    void moverANodo(Jugador * jugador, int nodoActual);
    const std::vector<std::pair<int,int>>& getVecinos(int nodo) const;
    void ronda(Jugador * jugador);
    void gameLoop(Jugador * jugador);
    int maquinaBFS(int inicio, const std::vector<std::vector<std::pair<int,int>>> &adj); // buscador bfs
    int maquinaPRI(int inicio );
    int maquinaDJI(int inicio);
    int numNodos() const { return static_cast<int>(nodos.size());}
};


#endif
