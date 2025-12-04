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
    ui->labelRecursos->setText("Recursos: 0");
    ui->btnMotorPlasma->setEnabled(false);

    totalRecursos = 0;
    recursosRecolectados = 0;

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
    connect(ui->btnMotorPlasma, &QPushButton::clicked, this, [&]() {

        if (totalRecursos < 700) {
            QMessageBox::warning(this, "No se puede comprar",
                                 "No tienes suficientes recursos.");
            return;
        }

        totalRecursos -= 700;
        ui->labelRecursos->setText(QString("Recursos: %1").arg(totalRecursos));
        actualizarBotonPlasma();

        QMessageBox::information(this,
                                 "Victoria",
                                 "¡Has comprado el Motor de Plasma!\nHas ganado el juego.");

        QApplication::quit();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dibujarGrafo()
{
    //Limpia la escena excepto el jugador
    for (QGraphicsItem *item : scene->items()) {
        if (item != jugadorItem) {
            scene->removeItem(item);
            delete item;
        }
    }

    int N = juego.numNodos();
    int r = 12;

    //Dibuja las aristas
    for (int u = 0; u < N; ++u) {
        const auto &vecs = juego.getVecinos(u);
        for (const auto &p : vecs) {
            int v = p.first;
            if (v < 0 || v >= N) continue;

            const NodoJuego &A = juego.getNodo(u);
            const NodoJuego &B = juego.getNodo(v);

            //Comprueba si se uso la arista antes para no borrarlas

            bool esDJ = false, esPRI = false, esBFS = false;
            for (auto &camino : historialDJ) {
                for (auto &e : camino) {
                    if ((e.first == u && e.second == v) ||
                        (e.first == v && e.second == u)) {
                        esDJ = true;
                        break;
                    }
                }
                if (esDJ) break;
            }
            for (auto &camino : historialPRI) {
                for (auto &e : camino) {
                    if ((e.first == u && e.second == v) ||
                        (e.first == v && e.second == u)) {
                        esPRI = true;
                        break;
                    }
                }
                if (esPRI) break;
            }

            for (auto &camino : historialBFS) {
                for (auto &e : camino) {
                    if ((e.first == u && e.second == v) ||
                        (e.first == v && e.second == u)) {
                        esBFS = true;
                        break;
                    }
                }
                if (esBFS) break;
            }

            QPen penLine(Qt::gray);
            penLine.setWidth(2);

            if (esDJ) {
                penLine.setColor(Qt::red);
                penLine.setWidth(4);
            }
            else if (esPRI) {
                penLine.setColor(Qt::cyan);
                penLine.setWidth(4);
            }
            else if (esBFS) {
                penLine.setColor(Qt::yellow);
                penLine.setWidth(4);
            }

            scene->addLine(A.x, A.y, B.x, B.y, penLine);

            // Peso de la arista
            float mx = (A.x + B.x) / 2.0;
            float my = (A.y + B.y) / 2.0;
            auto *w = scene->addText(QString::number(p.second));
            w->setTransform(QTransform::fromScale(1, -1), true);
            QRectF bb = w->mapToScene(w->boundingRect()).boundingRect();
            w->setPos(mx - bb.width()/2, my + bb.height()/2);
        }
    }

    // Dibuja los nodos
    for (int u = 0; u < N; ++u) {
        const NodoJuego &n = juego.getNodo(u);
        QBrush brush;

        if (n.tipo == 0)        // vacio
            brush = QBrush(Qt::lightGray);
        else if (n.tipo == 1)   // base
            brush = QBrush(Qt::red);
        else if (n.tipo == 2)   // recursos
            brush = QBrush(Qt::yellow);
        else
            brush = QBrush(Qt::red);

        scene->addEllipse(n.x - r, n.y - r, r*2, r*2, QPen(Qt::black), brush);

        auto *txt = scene->addText(QString::number(n.id));
        txt->setTransform(QTransform::fromScale(1, -1), true);
        QRectF bb = txt->mapToScene(txt->boundingRect()).boundingRect();
        txt->setPos(n.x - bb.width()/2, n.y + bb.height()/2);
    }

    crearBotonesMovimiento();
    scene->setSceneRect(scene->itemsBoundingRect());
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

        QMessageBox::information(
            this,
            "Nodo de recursos",
            "Este nodo contiene recursos.\nConstruye una tubería para enviarlos a la base."
            );

        submenuRecurso();
    }

    //si hay una maquina
    else if (maquinas.count(posicionJugador))
    {
        QMessageBox::information(this, "Maquina",
                                 "Esta maquina puede recargar tu bateria hasta 60%.");
    }

    //game over
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
    dialog.setWindowTitle("Recursos encontrados!");
    dialog.setModal(true);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    int nodo = posicionJugador;

    int costoBFS = juego.maquinaBFS(nodo, juego.getAristas());
    int costoPRI = juego.maquinaPRI(nodo);
    int costoDJI = juego.maquinaDJI(nodo);

    //botones
    QPushButton *btnNivel1 = new QPushButton(
        QString("Nivel 1 (BFS) - Costo: %1").arg(costoBFS), &dialog);

    QPushButton *btnNivel2 = new QPushButton(
        QString("Nivel 2 (Greedy) - Costo: %1").arg(costoPRI), &dialog);

    QPushButton *btnNivel3 = new QPushButton(
        QString("Nivel 3 (Dijkstra) - Costo: %1").arg(costoDJI), &dialog);

    QPushButton *btnSalir = new QPushButton("Regresar al menú de movimientos", &dialog);

    layout->addWidget(btnNivel1);
    layout->addWidget(btnNivel2);
    layout->addWidget(btnNivel3);
    layout->addWidget(btnSalir);

    ui->labelRecursos->setText(QString("Recursos: %1").arg(totalRecursos));

    //BFS
    connect(btnNivel1, &QPushButton::clicked, [&]() {

        if (juego.getNivelMaquina(nodo) >= 1) {
            QMessageBox::warning(this, "No permitido",
                                 "Ya existe una máquina de nivel igual o mayor en este nodo.");
            return;
        }

        auto ruta = juego.getCaminoBFS(nodo);
        if (ruta.empty()) return;

        int costo = 0;
        for (auto &e : ruta)
            costo += juego.getPeso(e.first, e.second);

        int recursoNodo = juego.getNodo(nodo).recursos;
        juego.vaciarRecursosNodo(nodo);

        int enviados = recursoNodo - costo;
        if (enviados < 0) enviados = 0;

        totalRecursos += enviados;
        ui->labelRecursos->setText(QString("Recursos: %1").arg(totalRecursos));
        actualizarBotonPlasma();
        historialBFS.erase(
            std::remove_if(historialBFS.begin(), historialBFS.end(),
                           [&](auto &cam){ return cam.front().first == nodo || cam.back().first == nodo; }),
            historialBFS.end()
            );
        historialBFS.push_back(ruta);

       juego.setNivelMaquina(nodo, 1);
        maquinas.insert(nodo);

        dibujarGrafo();
        dialog.accept();
    });


    //Prim
    connect(btnNivel2, &QPushButton::clicked, [&]() {

        if (juego.getNivelMaquina(nodo) >= 2) {
            QMessageBox::warning(this, "No permitido",
                                 "Ya existe Prim o Dijkstra en este nodo.");
            return;
        }

        auto rutaPRI = juego.getCaminoPRI(nodo);
        if (rutaPRI.empty()) return;

        int costo = 0;
        for (auto &e : rutaPRI)
            costo += juego.getPeso(e.first, e.second);

        int recursoNodo = juego.getNodo(nodo).recursos;
        juego.vaciarRecursosNodo(nodo);

        int enviados = recursoNodo - costo;
        if (enviados < 0) enviados = 0;
        totalRecursos += enviados;

        ui->labelRecursos->setText(QString("Recursos: %1").arg(totalRecursos));


        historialBFS.erase(
            std::remove_if(historialBFS.begin(), historialBFS.end(),
                           [&](auto &cam){ return cam.front().first == nodo || cam.back().first == nodo; }),
            historialBFS.end()
            );

        historialPRI.erase(
            std::remove_if(historialPRI.begin(), historialPRI.end(),
                           [&](auto &cam){ return cam.front().first == nodo || cam.back().first == nodo; }),
            historialPRI.end()
            );

        historialPRI.push_back(rutaPRI);

        juego.setNivelMaquina(nodo, 2);
        maquinas.insert(nodo);

        dibujarGrafo();
        dialog.accept();
    });


    //Dijkstra
    connect(btnNivel3, &QPushButton::clicked, [&]() {

        if (juego.getNivelMaquina(nodo) >= 3) {
            QMessageBox::warning(this, "No permitido",
                                 "Este nodo ya tiene Dijkstra.");
            return;
        }

        auto rutaDJ = juego.getCaminoDJ(nodo);
        if (rutaDJ.empty()) return;

        int costo = 0;
        for (auto &e : rutaDJ)
            costo += juego.getPeso(e.first, e.second);

        int recursoNodo = juego.getNodo(nodo).recursos;
        juego.vaciarRecursosNodo(nodo);

        int enviados = recursoNodo - costo;
        if(enviados < 0) enviados = 0;
        totalRecursos += enviados;

        ui->labelRecursos->setText(QString("Recursos: %1").arg(totalRecursos));

        // BORRAR BFS y PRIM anteriores de este nodo
        historialBFS.erase(
            std::remove_if(historialBFS.begin(), historialBFS.end(),
                           [&](auto &cam){ return cam.front().first == nodo || cam.back().first == nodo; }),
            historialBFS.end()
            );
        historialPRI.erase(
            std::remove_if(historialPRI.begin(), historialPRI.end(),
                           [&](auto &cam){ return cam.front().first == nodo || cam.back().first == nodo; }),
            historialPRI.end()
            );
        historialDJ.erase(
            std::remove_if(historialDJ.begin(), historialDJ.end(),
                           [&](auto &cam){ return cam.front().first == nodo || cam.back().first == nodo; }),
            historialDJ.end()
            );

        historialDJ.push_back(rutaDJ);

        juego.setNivelMaquina(nodo, 3);
        maquinas.insert(nodo);
        dibujarGrafo();
        dialog.accept();
    });

    connect(btnSalir, &QPushButton::clicked, [&]() {
        dialog.reject();
    });

    dialog.exec();
}

void MainWindow::actualizarBotonPlasma()
{
    if (!ui->btnMotorPlasma) return;

    bool tieneBFS = !historialBFS.empty();
    bool tienePrim = !historialPRI.empty();

    bool permitir = true;

    ui->btnMotorPlasma->setEnabled(permitir);
}
