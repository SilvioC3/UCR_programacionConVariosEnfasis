#include "Jugador.hh"
#include <algorithm>
#include <iostream>

Jugador::Jugador(int inicio) : posicion(inicio), bateria(100), recursos(0)
{
}

int Jugador::getPosicion() const 
{
    return posicion;
}

int Jugador::getBateria() const 
{
    return bateria;
}

int Jugador::getRecursos() const
{
    return recursos;
}

void Jugador::setPosicion(int p) 
{
    posicion = p;
}

//set en 100 de momento
void Jugador::setBateria(int b) 
{
   bateria = ((bateria+b) >= 100) ? 100 : (bateria + b);
}

void Jugador::setRecusos(int r)
{
    recursos += r;
}

void Jugador::moverJugador(int destino, int costo) 
{
    bateria -= costo;
    if (bateria < 0) bateria = 0;
    posicion = destino;
}

void Jugador::imprimirJugador() const
{
        std::cout << "\nEl jugador esta en el nodo: " << posicion << "\n";
        std::cout << "La bateria restante es: " << bateria << "%\n";
        std::cout << "La recursos obtenidos: " << recursos << "%\n";

}

int Jugador::tomarAccion()
{
    std::cout << "\nDigite 1 para mover, 2 para crear maquina\n";
    int accion;
    std::cin >> accion; 
    switch(accion){
        case 1:
            std::cout << "buscando nodos vecionos\n";
            return accion;
        case 2:
            std::cout << "construyendo maquibna\n";
            return accion;
        default:
            std::cout << "valor no valido\n";
            return 0;
    }

}
int Jugador::elegirMaquina()
{
    std::cout << "\nDigite 1 para maquina BFS, 2 para crear maquina prim, 3 para crear maquina diJkstra \n";
    int accion;
    std::cin >> accion; 
    switch(accion){
        case 1:
            std::cout << "construyendo maquina BFS\n";
            return accion;
        case 2:
            if (this-> recursos < costoPrim){
                std::cout << "insufiencientes recursos\n";
                return 0;
            }
            std::cout << "construyendo maquina Prim\n";
            return accion;
        case 3:
            if (this-> recursos < costoDijk){
                std::cout << "insufiencientes recursos\n";
                return 0;
            }
            std::cout << "construyendo maquina Dijikstra\n\n";
            return accion;
        case 4:
            if (this-> recursos < costoDijk){
                std::cout << "insufiencientes recursos\n";
                return 0;
            }
            std::cout << "construyendo maquina Astar\n\n";
            return accion;
        default:
            std::cout << "valor no valido\n";
            return 0;
    }
}