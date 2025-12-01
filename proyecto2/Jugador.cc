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
    bateria = 100;
}

void Jugador::setRecusos(int r)
{
    recursos = r;
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