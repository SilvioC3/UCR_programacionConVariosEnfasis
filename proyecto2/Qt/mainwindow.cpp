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

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dibujarGrafo()
{
    scene->clear();

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
}

void MainWindow::moverJugador(int destino)
{
    ui->labelBateria->setText(QString("Bateria: %1").arg(bateria));
    //costo del movimiento
    int costo = -1;
    for (auto &p : juego.getVecinos(posicionJugador)) {
        if (p.first == destino) {
            costo = p.second;
            break;
        }
    }

    if (costo == -1) {
        qDebug() << "Movimiento ilegal";
        return;
    }

    // baja la bateria
    bateria -= costo;
    qDebug() << "BATERIA = " << bateria;

    posicionJugador = destino;

    //Game over si se queda sin bateria
    if (bateria <= 0 && juego.getNodo(destino).tipo != base) {
        QMessageBox::warning(this, "Derrota",
                             "Te quedaste sin bateria lejos de la base.\nHas perdido.");
        return;
    }

    dibujarGrafo();
    crearBotonesMovimiento();
}
