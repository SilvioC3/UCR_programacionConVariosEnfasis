#include "Jugador.hh"
#include <algorithm>
#include <iostream>

Jugador::Jugador(int inicio) : posicion(inicio), bateria(100)
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

void Jugador::setPosicion(int p) 
{
    posicion = p;
}

//set en 100 de momento
void Jugador::setBateria(int b) 
{
    bateria = 100;
}

void Jugador::moverJugador(int destino, int costo) 
{
    bateria -= costo;
    if (bateria < 0) bateria = 0;
    posicion = destino;
}

void Jugador::imprimirJugador() const
{
        std::cout << "El jugador esta en el nodo: " << posicion << "\n";
        std::cout << "La bateria restante es: " << bateria << "%\n";

}