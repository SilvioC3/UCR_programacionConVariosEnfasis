#ifndef JUGADOR_HH
#define JUGADOR_HH

#include <iostream>


/// @brief 
// Clase Jugador para controlar los movimientos y variables del jugador en si
// Sets y gets basicos temporales para pruebas
class Jugador 
{
    private:
        int posicion;
        int bateria;
        int recursos;

    public:
        Jugador(int inicio = 0);

        //sets y gets temporales
        int getPosicion() const;
        int getBateria() const;
        int getRecursos() const;
        void setPosicion(int p);
        void setBateria(int b);
        void setRecusos(int r);

        //metodos para toma de decisiones
        int tomarAccion(); 
        int elegirMaquina(); 

        //metodos para controlar el jugador
        void moverJugador(int destino, int costo);
        void imprimirJugador() const;

};

#endif