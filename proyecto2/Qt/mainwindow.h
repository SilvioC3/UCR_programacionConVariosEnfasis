#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <unordered_set>
#include "JuegoCon.hh"
#include "Jugador.hh"

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    JuegoCon juego;
    QGraphicsEllipseItem *jugadorItem;
    int posicionJugador = -1;
    int bateria = 100;
    std::unordered_set<int> maquinas;
    void dibujarGrafo();
    void crearBotonesMovimiento();
    void moverJugador(int nodoDestino);
    void submenuRecurso();

private slots:
    void onNodoClicked(int id);

};

#endif
