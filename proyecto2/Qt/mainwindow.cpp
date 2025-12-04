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
#include <cmath>


//setup del ui
    MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    juego(0)//constructor de JuegoCon, con esto empieza en 0
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

    totalRecursos = 25;
    recursosRecolectados = 0;

    //carga el grafo, esta parte del codigo pregunta si se quiere hacer uno al azar o sacarlo del .txt
    auto respuesta = QMessageBox::question(this,
                                           "Generar grafo",
                                           "¿Quieres generar un grafo aleatorio?\n"
                                           "Si eliges 'No', se cargará desde archivo.",
                                           QMessageBox::Yes | QMessageBox::No);

    if (respuesta == QMessageBox::Yes) {
        int numNodos = 10;
        int minConexiones = 1;
        int extraConexiones = 2;
        juego.generaGrafoAleatorio(numNodos, minConexiones, extraConexiones);
    } else {
        juego.cargadorGrafo("Grafo.txt");
    }

    posicionJugador = 0;
    bateria = 100;
    ui->labelBateria->setText(QString("Bateria: %1").arg(bateria));

    dibujarGrafo();

    const NodoJuego &n = juego.getNodo(posicionJugador);
    jugadorItem = scene->addEllipse(n.x - 6, n.y - 6, 12, 12,
                                    QPen(Qt::black), QBrush(Qt::green));
    jugadorItem->setZValue(100);  // por encima de los nodos
    connect(ui->btnMotorPlasma, &QPushButton::clicked, this, [&]() {

        if (totalRecursos < 1000) {
            QMessageBox::warning(this, "No se puede comprar",
                                 "No tienes suficientes recursos.");
            return;
        }

        totalRecursos -= 1000;
        ui->labelRecursos->setText(QString("Recursos: %1").arg(totalRecursos));
        actualizarBotonPlasma();

        QMessageBox::information(this,
                                 "Victoria",
                                 "¡Has comprado el Motor de Plasma!\nHas ganado el juego.");
        QApplication::quit();
        actualizarBotonPlasma();

    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

//dibuja el grafo
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

    std::map<std::pair<int,int>, std::vector<int>> groups;
    for (int u = 0; u < N; ++u) {
        const NodoJuego &n = juego.getNodo(u);
        groups[{n.x, n.y}].push_back(u);
    }

    std::vector<std::pair<float,float>> posAdjusted(N);
    for (auto &g : groups) {
        int origX = g.first.first;
        int origY = g.first.second;
        auto &vec = g.second;
        int m = (int)vec.size();

        if (m == 1) {
            posAdjusted[vec[0]] = { (float)origX, (float)origY };
        } else {
            float radius = 10.0f + std::min(20, m * 2);
            for (int i = 0; i < m; ++i) {
                float angle = (2.0f * M_PI * i) / m;
                float nx = origX + std::cos(angle) * radius;
                float ny = origY + std::sin(angle) * radius;
                posAdjusted[vec[i]] = { nx, ny };
            }
        }
    }

    for (int u = 0; u < N; ++u) {
        const auto &vecs = juego.getVecinos(u);
        for (const auto &p : vecs) {
            int v = p.first;
            if (v < 0 || v >= N) continue;

            float Ax = posAdjusted[u].first;
            float Ay = posAdjusted[u].second;
            float Bx = posAdjusted[v].first;
            float By = posAdjusted[v].second;

            const NodoJuego &A = juego.getNodo(u);
            const NodoJuego &B = juego.getNodo(v);
            bool esDJ = false, esPRI = false, esBFS = false, esAStar = false;
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
            for (auto &camino : historialAStar) {
                for (auto &e : camino) {
                    if ((e.first == u && e.second == v) ||
                        (e.first == v && e.second == u)) {
                        esAStar = true;
                        break;
                    }
                }
                if (esAStar) break;
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
            else if (esAStar) {
                penLine.setColor(Qt::green);
                penLine.setWidth(4);
            }

            scene->addLine(Ax, Ay, Bx, By, penLine);

            float mx = (Ax + Bx) / 2.0f;
            float my = (Ay + By) / 2.0f;
            auto *w = scene->addText(QString::number(p.second));
            w->setTransform(QTransform::fromScale(1, -1), true);
            QRectF bb = w->mapToScene(w->boundingRect()).boundingRect();
            w->setPos(mx - bb.width()/2, my + bb.height()/2);
        }
    }
//Dibuja cada nodo
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

        float nx = posAdjusted[u].first;
        float ny = posAdjusted[u].second;

        scene->addEllipse(nx - r, ny - r, r*2, r*2, QPen(Qt::black), brush);

        auto *txt = scene->addText(QString::number(n.id));
        txt->setTransform(QTransform::fromScale(1, -1), true);
        QRectF bb = txt->mapToScene(txt->boundingRect()).boundingRect();
        txt->setPos(nx - bb.width()/2, ny + bb.height()/2);
    }

    crearBotonesMovimiento();
    scene->setSceneRect(scene->itemsBoundingRect());
}
void MainWindow::onNodoClicked(int destino)
{
    moverJugador(destino);
}

//Para crear los botones de movimiento
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
    if (bateria <= 0 && juego.getNodo(destino).tipo != base) {
        QMessageBox::warning(this, "GAME OVER", "Te quedaste sin bateria lejos de la base.\nHas perdido.");
        QApplication::quit();
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

//Metodo para la funcionalidad de los recurso
void MainWindow::submenuRecurso()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Recursos encontrados!");
    dialog.setModal(true);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    int nodo = posicionJugador;
    int recursoNodo = juego.getNodo(nodo).recursos;
    QLabel *lblInfo = new QLabel(
        QString("Recursos en este nodo: %1").arg(recursoNodo)
        );
    layout->addWidget(lblInfo);
    int costoBFS = 25;
    int costoPRI = 50;
    int costoDJI =  100;
    int costoAStar = 130;
    if (recursosOriginales.count(nodo) == 0) {
        recursosOriginales[nodo] = recursoNodo;
    }
    auto rutaBFS = juego.getCaminoBFS(nodo);
    for (auto &e : rutaBFS)
        costoBFS += juego.getPeso(e.first, e.second);
    auto rutaPRI = juego.getCaminoPRI(nodo);
    for (auto &e : rutaPRI)
        costoPRI += juego.getPeso(e.first, e.second);

    auto rutaDJ = juego.getCaminoDJ(nodo);
    for (auto &e : rutaDJ)
        costoDJI += juego.getPeso(e.first, e.second);

    auto rutaAStar = juego.getCaminoAStar(nodo);

    for (auto &e : rutaAStar)
        costoAStar += juego.getPeso(e.first, e.second);


    int recursoOriginal = recursosOriginales[nodo];

    int gananciaBFS = recursoOriginal - costoBFS;
    if (gananciaBFS < 0) gananciaBFS = 0;

    int gananciaPRI = recursoOriginal - costoPRI;
    if (gananciaPRI < 0) gananciaPRI = 0;

    int gananciaDJI = recursoOriginal - costoDJI;
    if (gananciaDJI < 0) gananciaDJI = 0;

    int gananciaAStar = recursoOriginal - costoAStar;
    if (gananciaAStar < 0) gananciaAStar = 0;
    //botones
    QPushButton *btnNivel1 = new QPushButton(
        QString("Nivel 1 (BFS) - Costo máquina: 25 - Ganancia: %1")
            .arg(gananciaBFS)
        );
    QPushButton *btnNivel2 = new QPushButton(
        QString("Nivel 2 (Prim) - Costo máquina: 50 - Ganancia: %1")
            .arg(gananciaPRI)
        );
    QPushButton *btnNivel3 = new QPushButton(
        QString("Nivel 3 (Dijkstra) - Costo máquina: 100 - Ganancia: %1")
            .arg(gananciaDJI)
        );

    QPushButton *btnNivel4 = new QPushButton(
        QString("Nivel 4 (A*) - Costo: %1 - Ganancia: %2")
            .arg(costoAStar)
            .arg(gananciaAStar),
        &dialog);
    QPushButton *btnSalir = new QPushButton("Regresar al menu de movimientos", &dialog);

    layout->addWidget(btnNivel1);
    layout->addWidget(btnNivel2);
    layout->addWidget(btnNivel3);
    layout->addWidget(btnNivel4);
    layout->addWidget(btnSalir);

    ui->labelRecursos->setText(QString("Recursos: %1").arg(totalRecursos));

    //BFS
    connect(btnNivel1, &QPushButton::clicked, [&]() {

        if (juego.getNivelMaquina(nodo) >= 1) {
            QMessageBox::warning(this, "No permitido",
                                 "Ya existe una maquina de nivel igual o mayor en este nodo.");
            return;
        }

        const int COSTO = 25;

        if (totalRecursos < COSTO) {
            QMessageBox::warning(this, "No se puede construir",
                                 "No tienes suficientes recursos para construir esta maquina.");
            return;
        }

        totalRecursos -= COSTO;

        auto ruta = juego.getCaminoBFS(nodo);
        if (ruta.empty()) return;

        int costoRutas = 0;
        for (auto &e : ruta)
            costoRutas += juego.getPeso(e.first, e.second);

        int recursoOriginal = recursosOriginales[nodo];

        int enviadosNuevo = recursoOriginal - costoRutas;
        if (enviadosNuevo < 0) enviadosNuevo = 0;
        int enviadosViejo = recursosGanadosPorNodo[nodo];

        totalRecursos -= enviadosViejo;
        totalRecursos += enviadosNuevo;

        recursosGanadosPorNodo[nodo] = enviadosNuevo;
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
        const int COSTO = 50;

        if (totalRecursos < COSTO) {
            QMessageBox::warning(this, "No se puede construir",
                                 "No tienes suficientes recursos para construir esta maquina.");
            return;
        }

        totalRecursos -= COSTO;
        juego.maquinaPRI(nodo);

        int recursoOriginal = recursosOriginales[nodo];
        int enviadosNuevo = std::max(0, recursoOriginal - costoPRI);
        int enviadosViejo = recursosGanadosPorNodo[nodo];
        totalRecursos -= enviadosViejo;
        totalRecursos += enviadosNuevo;
        recursosGanadosPorNodo[nodo] = enviadosNuevo;

        ui->labelRecursos->setText(QString("Recursos: %1").arg(totalRecursos));
        actualizarBotonPlasma();

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
        const int COSTO = 100;
        if (totalRecursos < COSTO) {
            QMessageBox::warning(this, "No se puede construir",
                                 "No tienes suficientes recursos para construir esta maquina.");
            return;
        }
        totalRecursos -= COSTO;
        juego.maquinaDJI(nodo);

        int recursoOriginal = recursosOriginales[nodo];
        int enviadosNuevo = std::max(0, recursoOriginal - costoDJI);
        int enviadosViejo = recursosGanadosPorNodo[nodo];
        totalRecursos -= enviadosViejo;
        totalRecursos += enviadosNuevo;
        recursosGanadosPorNodo[nodo] = enviadosNuevo;

        ui->labelRecursos->setText(QString("Recursos: %1").arg(totalRecursos));
        actualizarBotonPlasma();

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

    //A*
    connect(btnNivel4, &QPushButton::clicked, [&]() {
        if (juego.getNivelMaquina(nodo) >= 4) {
            QMessageBox::warning(this, "No permitido",
                                 "Este nodo ya tiene A*.");
            return;
        }

        const int COSTO = 150;
        if (totalRecursos < COSTO) {
            QMessageBox::warning(this, "No se puede construir",
                                 "No tienes suficientes recursos para construir esta maquina.");
            return;
        }

        totalRecursos -= COSTO;


        auto ruta = juego.getCaminoAStar(nodo);

        int costoR = 0;
        for (auto &e : ruta)
            costoR += juego.getPeso(e.first, e.second);

        int recursoOriginal = recursosOriginales[nodo];
        int enviadosNuevo = std::max(0, recursoOriginal - costoR);
        int enviadosViejo = recursosGanadosPorNodo[nodo];
        totalRecursos = totalRecursos - enviadosViejo + enviadosNuevo;
        recursosGanadosPorNodo[nodo] = enviadosNuevo;

        ui->labelRecursos->setText(QString("Recursos: %1").arg(totalRecursos));
        actualizarBotonPlasma();

        historialAStar.erase(
            std::remove_if(historialAStar.begin(), historialAStar.end(),
                           [&](auto &cam){ return cam.front().first == nodo || cam.back().first == nodo; }),
            historialAStar.end()
            );
        historialAStar.push_back(ruta);

        juego.setNivelMaquina(nodo, 4);
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


