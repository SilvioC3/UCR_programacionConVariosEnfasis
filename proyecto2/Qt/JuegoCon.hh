#ifndef JUEGOCON_HH
#define JUEGOCON_HH

#include <iostream>
#include <vector>
#include <utility>
#include <map>
#include "Jugador.hh"

enum tipoDeNodo {base, recurso, fabrica };

struct NodoJuego
{
    int id;
    tipoDeNodo tipo;
    std::string nombre; 
    int recursos;
    int x;
    int y;
    bool tieneMaquina = false;
    bool recursoReclamado = false;
};

struct ResultadoAlgoritmo {
    int costo = 0;
    std::vector<std::pair<int,int>> aristas;
};

class JuegoCon
{
private:
    /// @brief vector que contiene los nodos
    std::vector<NodoJuego> nodos;
    std::vector <std::vector< std::pair <int, int>>> aristas;
    int current;
    bool isLooping;
    bool primerBFSGratis = true;
    int recursosBase = 0;
    std::map<int,int> nivelMaquina;
    int maquinaAstar(int);
    std::vector<int> parentAStar;

public:
    void vaciarRecursosNodo(int id);
    int ejecutarMaquinaAStar(int nodo);
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
    std::vector<std::pair<int,int>> getCamino(int desde, int hasta) const;
    void gameLoop(Jugador * jugador);
    int maquinaBFS(int inicio, const std::vector<std::vector<std::pair<int,int>>> &adj); // buscador bfs
    int maquinaPRI(int inicio );
    int maquinaDJI(int inicio);
    int getNivelMaquina(int id) const {
        auto it = nivelMaquina.find(id);
        if (it == nivelMaquina.end()) return 0;
        return it->second;
    }
    void setNivelMaquina(int id, int nivel) {
        nivelMaquina[id] = nivel;
    }
    void generaGrafoAleatorio(int n, int conexionesMinimas = 1, int conexionesExtras = 2);

    std::vector<std::pair<int,int>> getCaminoDJ(int inicio) const;
    std::vector<std::pair<int,int>> getCaminoBFS(int inicio) const;
    std::vector<std::pair<int,int>> getCaminoPRI(int inicio) const;
    std::vector<std::pair<int,int>> getCaminoAStar(int inicio);
    const std::vector<std::vector<std::pair<int,int>>>& getAristas() const;
    int numNodos() const { return static_cast<int>(nodos.size());


    }


};


#endif
