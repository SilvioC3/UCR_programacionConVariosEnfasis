#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <unordered_set>
#include "JuegoCon.hh"
#include "Jugador.hh"
#include <map>

namespace Ui { class MainWindow; }



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void finalizarJuego(const QString &mensaje);
    void reiniciarJuego();


private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    JuegoCon juego;
    QGraphicsEllipseItem *jugadorItem;
    int posicionJugador = -1;
    int bateria = 100;
    int totalRecursos;
    std::unordered_set<int> maquinas;
    void dibujarGrafo();
    void crearBotonesMovimiento();
    void moverJugador(int nodoDestino);
    void submenuRecurso();
    int recursosRecolectados;
    std::vector<std::pair<int,int>> caminoDijkstra;
    std::vector<std::pair<int,int>> caminoBFS;
    std::vector<std::pair<int,int>> caminoPRI;
    std::vector<std::vector<std::pair<int,int>>> historialBFS;
    std::vector<std::vector<std::pair<int,int>>> historialPRI;
    std::vector<std::vector<std::pair<int,int>>> historialDJ;
      std::vector<std::vector<std::pair<int,int>>> historialAStar;
    void actualizarBotonPlasma();
    std::map<int, int> recursosOriginales;
    std::map<int, int> recursosGanadosPorNodo;

private slots:
    void onNodoClicked(int id);
};

#endif
