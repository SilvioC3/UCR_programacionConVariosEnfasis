#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QPen>
#include <QBrush>
#include <QMessageBox>
#include <QPushButton>
#include <QLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    juego(0) //constructor de JuegoCon, con esto empieza en 0
{
    ui->setupUi(this);

    //crea la escena y asocia el objeto graphicsView
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->graphicsView->scale(1, -1);


    //carga el grafo
    juego.cargadorGrafo("Grafo.txt");
    posicionJugador = 0;
    bateria = 100;
    ui->labelBateria->setText(QString("Bateria: %1").arg(bateria));

    dibujarGrafo();

    const NodoJuego &n = juego.getNodo(posicionJugador);
    jugadorItem = scene->addEllipse(n.x - 6, n.y - 6, 12, 12,
                                    QPen(Qt::black), QBrush(Qt::green));
    jugadorItem->setZValue(100);  // por encima de los nodos

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dibujarGrafo()
{
    for (QGraphicsItem *item : scene->items()) {
        if (item != jugadorItem) {
            scene->removeItem(item);
            delete item;
        }
    }

    int N = juego.numNodos();
    int r = 12;

    //Dibuja las aristas
    QPen penLine(Qt::gray);
    penLine.setWidth(2);

    for (int u = 0; u < N; ++u) {
        const auto &vecs = juego.getVecinos(u);
        for (const auto &p : vecs) {
            int v = p.first;
            //No es dirigido, solo dibuja una vez
            if (v < 0 || v >= N) continue;
            const NodoJuego &A = juego.getNodo(u);
            const NodoJuego &B = juego.getNodo(v);
            scene->addLine(A.x, A.y, B.x, B.y, penLine);
            //Calcula el punto medio
            float mx = (A.x + B.x) / 2.0;
            float my = (A.y + B.y) / 2.0;

            //texto del peso
            auto *w = scene->addText(QString::number(p.second));
            w->setTransform(QTransform::fromScale(1, -1), true);

            QRectF bb = w->mapToScene(w->boundingRect()).boundingRect();
            w->setPos(mx - bb.width()/2, my + bb.height()/2);
        }
    }

    //Dibuja los nodos
    for (int u = 0; u < N; ++u) {
        const NodoJuego &n = juego.getNodo(u);
        QBrush brush;

        if (n.tipo == 0)        //vacio
            brush = QBrush(Qt::lightGray);
        else if (n.tipo == 1)   //base
            brush = QBrush(Qt::red);
        else if (n.tipo == 2)   //recurso
            brush = QBrush(Qt::yellow);
        else
            brush = QBrush(Qt::red);
        scene->addEllipse(n.x - r, n.y - r, r*2, r*2, QPen(Qt::black), brush);
        auto *txt = scene->addText(QString::number(n.id));
        txt->setTransform(QTransform::fromScale(1, -1), true);

        //Para centrarlo dentro de los nodos
        QRectF bb = txt->mapToScene(txt->boundingRect()).boundingRect();
        txt->setPos(n.x - bb.width()/2, n.y + bb.height()/2);
        crearBotonesMovimiento();
        scene->setSceneRect(scene->itemsBoundingRect());

    }
}

void MainWindow::onNodoClicked(int destino)
{
    moverJugador(destino);
}

void MainWindow::crearBotonesMovimiento()
{

    QLayout *lo = ui->layoutOpcionesMovimiento;
    if (!lo) return;

    //limpia los botones
    QLayoutItem *item;
    while ((item = lo->takeAt(0))) {
        delete item->widget();
        delete item;
    }

    //saca los vecinos del nodo y crea botones para cada uno
    const auto &vecinos = juego.getVecinos(posicionJugador);
    for (auto &par : vecinos) {
        int destino = par.first;
        int costo   = par.second;

        QPushButton *btn = new QPushButton(
            QString("Ir a %1 (costo %2)").arg(destino).arg(costo)
            );

        connect(btn, &QPushButton::clicked, this, [=]() {
            moverJugador(destino);
        });

        lo->addWidget(btn);

    }
    //Opcion para recargar al 60% si el nodo tiene una maquina
    if (maquinas.count(posicionJugador))
    {
        QPushButton *btnRec = new QPushButton("Recargar bateria (60%)");
        connect(btnRec, &QPushButton::clicked, this, [&]() {

            if (bateria < 60)
                bateria = 60;

            ui->labelBateria->setText(QString("Bateria: %1").arg(bateria));

            QMessageBox::information(this, "Recarga",
                                     "La bateria se ha recargado al 60%.");
        });

        lo->addWidget(btnRec);
    }
}

void MainWindow::moverJugador(int destino)
{
    //para sacar el costo
    int costo = -1;
    for (auto &p : juego.getVecinos(posicionJugador)) {
        if (p.first == destino) {
            costo = p.second;
            break;
        }
    }
    if (costo == -1) return;

    bateria -= costo;
    ui->labelBateria->setText(QString("Bateria: %1").arg(bateria));
    if (bateria <= 0) {
        QMessageBox::warning(this, "GAME OVER",
                             "Te quedaste sin bateria durante el camino.\nGAME OVER.");
        return;
    }
    posicionJugador = destino;

    //para el movimiento del jugador
    const NodoJuego &n = juego.getNodo(posicionJugador);
    jugadorItem->setRect(n.x - 6, n.y - 6, 12, 12);
    //si es base
    if (n.tipo == 1)
    {
        bateria = 100;
        ui->labelBateria->setText(QString("Bateria: %1").arg(bateria));

        QMessageBox::information(this, "Base",
                                 "Ha regresado a la base.\nLa bateria se ha restaurado al 100%!");
    }

    //si es recurso
    else if (n.tipo == 2)
    {
        submenuRecurso();
    }

    //si es máquina construida
    else if (maquinas.count(posicionJugador))
    {
        QMessageBox::information(this, "Maquina",
                                 "Esta maquina puede recargar tu bateria hasta 60%.");
    }

    // game over
    if (bateria <= 0 && juego.getNodo(destino).tipo != base) {
        QMessageBox::warning(this, "GAME OVER",
                             "Te quedaste sin bateria lejos de la base.\nHas perdido.");
        return;
    }

    dibujarGrafo();
    crearBotonesMovimiento();
}

//Metodo para la funcionalidad de nodos de recurso
void MainWindow::submenuRecurso()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Recurso encontrado");
    dialog.setModal(true);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QPushButton *btnNivel1 = new QPushButton("Nivel 1 (BFS)", &dialog);
    QPushButton *btnNivel2 = new QPushButton("Nivel 2 (Greedy)", &dialog);
    QPushButton *btnNivel3 = new QPushButton("Nivel 3 (Dijkstra)", &dialog);
    QPushButton *btnSalir   = new QPushButton("Regresar al menu de movimientos", &dialog);

    layout->addWidget(btnNivel1);
    layout->addWidget(btnNivel2);
    layout->addWidget(btnNivel3);
    layout->addWidget(btnSalir);

    int nodo = posicionJugador;
    int recurso = juego.getRecurso(nodo);

    //Nivel 1: BFS
    connect(btnNivel1, &QPushButton::clicked, [&]() {
        int costo = juego.maquinaBFS(nodo, juego.getAristas());
        int ganancia = recurso - costo;

        QMessageBox::information(this, "Resultado BFS",
                                 QString("Costo tuberia: %1\nRecurso: %2\nGanancia neta: %3")
                                     .arg(costo).arg(recurso).arg(ganancia));
        maquinas.insert(nodo);
        dialog.accept();
    });

    //Nivel 2: greedy PRI
    connect(btnNivel2, &QPushButton::clicked, [&]() {
        int costo = juego.maquinaPRI(nodo);
        int ganancia = recurso - costo;

        QMessageBox::information(this, "Resultado Greedy",
                                 QString("Costo tuberia: %1\nRecurso: %2\nGanancia neta: %3")
                                     .arg(costo).arg(recurso).arg(ganancia));
        maquinas.insert(nodo);
        dialog.accept();
    });

    //Nivel 3: Dijkstra
    connect(btnNivel3, &QPushButton::clicked, [&]() {
        int costo = juego.maquinaDJI(nodo);
        int ganancia = recurso - costo;

        QMessageBox::information(this, "Resultado Dijkstra",
                                 QString("Costo tuberia: %1\nRecurso: %2\nGanancia neta: %3")
                                     .arg(costo).arg(recurso).arg(ganancia));
        maquinas.insert(nodo);
        dialog.accept();
    });

    connect(btnSalir, &QPushButton::clicked, [&]() {
        dialog.reject();
    });

    dialog.exec();
}

