#include "JuegoCon.hh"

int main(int argc, char const *argv[])
{
    
    JuegoCon juego(3);

    juego.agregarNodo(0, {0, "A"});
    juego.agregarNodo(1, {1, "B"});
    juego.agregarNodo(2, {2, "C"});
    
    juego.agregarArista(0, 1, 10);
    juego.agregarArista(0, 2, 5);

    juego.accederNodo(0);
    juego.verVecinos(0);

    return 0;
}
