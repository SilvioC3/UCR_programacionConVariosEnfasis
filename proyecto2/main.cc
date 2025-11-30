#include "JuegoCon.hh"
#include "Jugador.hh"
#include <iostream>


///@brief Metodo temporal? para probar el movimiento, juega una vez y se llama en el main

void ronda(JuegoCon& juego)
{
    Jugador jugador(20);
    std::cout << "EL JUEGO HA EMPEZADO\n";
    jugador.imprimirJugador();
    juego.verVecinos(jugador.getPosicion());
    int nodoActual = jugador.getPosicion();
    const NodoJuego& nodoStruct = juego.getNodo(nodoActual);
    std::cout << "Tipo de nodo actual: " << nodoStruct.tipo << "\n";

    //Imprime las rutas, este get se deberia poder usar para UI y logica futura
    std::cout << "\nLas rutas disponibles desde el nodo " << nodoActual << "son: \n";
    const auto& vecinos = juego.getVecinos(nodoActual);
    for (int i = 0; i < vecinos.size(); i++) 
        {
            std::cout << i+1 << ") Moverse al nodo " << vecinos[i].first << " (costo:" << vecinos[i].second << "%)\n";
        }
    
    //UI temporal en terminal para una sola partida
    std::cout << "Eliga una opcion: ";
    int opcion;
    std::cin >> opcion;
    int destino = vecinos[opcion-1].first;
    int costo   = vecinos[opcion-1].second;
    jugador.moverJugador(destino, costo);
    std::cout << "\nSe ha movido hacia el nodo " << destino << " (Ha utilizado " << costo << "% de bateria)\n";
    jugador.imprimirJugador();
    juego.verVecinos(jugador.getPosicion());
    std::cout << "Tipo de nodo actual: " << nodoStruct.tipo << "\n";
}





int main(int argc, char const *argv[])
{
    JuegoCon juego(0);
    juego.cargadorGrafo("Grafo.txt");
    ronda(juego);
    juego.accederNodo(5);
    juego.verVecinos(5);
    return 0;
}



// int main(int argc, char const *argv[])
// {
    
//     JuegoCon juego(3);

//     juego.agregarNodo(0, {0, "A"});
//     juego.agregarNodo(1, {1, "B"});
//     juego.agregarNodo(2, {2, "C"});
    
//     juego.agregarArista(0, 1, 10);
//     juego.agregarArista(0, 2, 5);

//     juego.accederNodo(0);
//     juego.verVecinos(0);

//     return 0;
// }
