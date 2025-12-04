#include "JuegoCon.hh"

#include <fstream>
#include <sstream>
#include <string>  
#include <iostream>
#include <queue>
#include <climits>
#include <functional>

using namespace std;

JuegoCon::JuegoCon(int n): isLooping(true)
{
    this->nodos.resize(n);
    this->aristas.resize(n);

}

JuegoCon::~JuegoCon()
{
}

void JuegoCon::agregarNodo(int id, const NodoJuego& nodo){
    
    this->nodos[id] = nodo;

}

void JuegoCon::agregarArista(int nodoId, int vecino, int peso){
    this->aristas[nodoId].push_back({vecino, peso});

}

void JuegoCon::accederNodo(int indiceNodo, Jugador * jugador){
    NodoJuego temp = this->nodos[indiceNodo];
    cout << "\nNodo: " << temp.nombre << endl;
    cout << "Tipo: " << temp.tipo << endl;
    cout << "recursos " << temp.recursos << endl;
    this->verVecinos(indiceNodo);

    if(temp.tipo == base)
    {
        jugador->setBateria(100);
    }
    else if(temp.tipo == fabrica)
    {
        jugador->setBateria(60);
    }

}

void JuegoCon::verVecinos(int indice){
    cout << "Los vecinos de: " << indice << " son:\n";
    for (auto &vecinos : this->aristas[indice] ){
        cout<< "( " << vecinos.first << " peso: " << vecinos.second << " ) ";

    }
    cout << endl;
}

/// @brief Metodo para cargar el grafo desde un txt
void JuegoCon::cargadorGrafo(const string& ruta)       
{
    ifstream archivo(ruta);
    if (!archivo.is_open()) 
    {
        cout << "ERROR: No se pudo abrir el archivo.\n";
        return;
    }
    string linea; 
    while (getline(archivo, linea)) 
    {
        if (linea == "NODOS:") break;
    }

    int maxId = -1;

    while (getline(archivo, linea)) 
    {
        if (linea == "CONEXIONES:") break;
        if (linea.empty()) continue;
           stringstream ss(linea);
        int id, tipoInt, recursos, x, y;
        char coma;

        ss >> id >> coma >> tipoInt >> coma >> recursos >> coma >> x >> coma >> y;
        if (id >= nodos.size()) 
        {
            nodos.resize(id + 1);
            aristas.resize(id + 1);
        }   


        //Crea el nodo en si con todos los atributos
        NodoJuego nodo;
        nodo.id = id;
        nodo.tipo = static_cast<tipoDeNodo>(tipoInt); 
        nodo.nombre = "Nodo " + to_string(id);
        nodo.recursos = recursos;
        nodo.x = x;
        nodo.y = y;
        this->agregarNodo(id, nodo);
        maxId = max(maxId, id);
    }

    while (getline(archivo, linea)) 
    {
        if (linea.empty()) continue;

        stringstream ss(linea);
        int u, v, peso;
        char coma;
        ss >> u >> coma >> v >> coma >> peso;
        this->agregarArista(u, v, peso);
        this->agregarArista(v, u, peso);
    }

    archivo.close();
    cout << "Se ha creado el mapa inicial, con un total de: " << nodos.size() << " nodos.\n";
}

int JuegoCon::getPeso(int origen, int destino) const 
{
    for (auto &par : aristas[origen]) {
        if (par.first == destino)
            return par.second;
    }
    return -1;
}

//Metodo para hacer get a los vecinos del nodo actual
const vector<pair<int,int>>& JuegoCon::getVecinos(int nodo) const
{
    return aristas[nodo];
}

//Metodo para hacer get a todos los datos del nodo actual
const NodoJuego& JuegoCon::getNodo(int id) const 
{
    return nodos[id];
}

void JuegoCon::moverANodo(Jugador * jugador, int nodoActual)
{
    const NodoJuego& nodoStruct = this->getNodo(nodoActual);
    cout << "\nLas rutas disponibles desde el nodo " << nodoActual << "son: \n";
    const auto& vecinos = this->getVecinos(nodoActual);
    for (int i = 0; i < vecinos.size(); i++) 
    {
        cout << i+1 << ") Moverse al nodo " << vecinos[i].first << " (costo:" << vecinos[i].second << "%)\n";
    }
    
    cout << "Eliga una opcion: ";
    int opcion;
    cin >> opcion; 
    if(opcion > 0 && opcion <= vecinos.size()+1){  
        int destino = vecinos[opcion-1].first;
        int costo   = vecinos[opcion-1].second;
        jugador->moverJugador(destino, costo);
        cout << "\nSe ha movido hacia el nodo " << destino << " (Ha utilizado " << costo << "%d de bateria)\n";
    }else{
        cout << "valor incorrecto\n";
        return;
    }
}

// ronda del juego
void JuegoCon::ronda(Jugador * jugador)
{
    
    
    jugador->imprimirJugador();
    int nodoActual = jugador->getPosicion();
    this->accederNodo(nodoActual, jugador);

    int accion = jugador->tomarAccion();

    switch(accion)
    {
        case 1:{
            moverANodo(jugador, nodoActual);
            break;
        }
        case 2:{
            int recurso = this->getRecurso(nodoActual);  //recurso de nodo actual
            int maquina = jugador->elegirMaquina();
            int restaPorAristas = 0;

            switch (maquina)
            {
                case 1:{
                    restaPorAristas = maquinaBFS(nodoActual, this->aristas);
                    jugador->setRecusos(recurso - restaPorAristas);
                    break;
                }
                case 2:{
                    restaPorAristas = maquinaPRI(nodoActual);
                    jugador->setRecusos(recurso - restaPorAristas);
                    break;
                }
                case 3:{ 
                    restaPorAristas = maquinaDJI(nodoActual);
                    jugador->setRecusos(recurso - restaPorAristas);
                    break;
                }

                default:{
                    break;
                }
            }

            break;
        }
        default:{
            break;
        }

    }
  
    if( jugador->getBateria() <= 0 || jugador->getRecursos() >= 100)
    {
        this->isLooping = false;

    }

}

void JuegoCon::gameLoop(Jugador * jugador)
{
    cout << "\nEL JUEGO HA EMPEZADO\n";

    while(this->isLooping ){
        this->ronda(jugador);

    }

    if (jugador->getBateria() <= 0){
        cout << "\njuego perdido\n";

    }else{
         cout << "\njuego ganado\n";
    }

}

int JuegoCon::getRecurso(int nodo)
{
    int recurso = this->nodos[nodo].recursos;
    return recurso;
}

int JuegoCon::maquinaBFS(int inicio, const vector<vector<pair<int,int>>> &adj) {
    int n = adj.size();
    vector<bool> visited(n, false);
    vector<int> parent(n, -1);
    queue<int> q;

    visited[inicio] = true;
    q.push(inicio);

    while (!q.empty()) {
        int u = q.front(); q.pop();

        if (u == 0)
            break;

        for (auto [v, w] : adj[u]) {
            if (!visited[v]) {
                visited[v] = true;
                parent[v] = u;
                q.push(v);
            }
        }
    }

    int total = 0;
    for (int at = 0; parent[at] != -1; at = parent[at]) {
        int p = parent[at];

        // search for weight p -> at
        for (auto [to, w] : adj[p]) {
            if (to == at) {
                total += w;
                break;
            }
        }
    }

    return total;
    
}

int JuegoCon::maquinaPRI(int inicio) {

    int n = this->aristas.size();
    vector<int> key(n, 1e9);
    vector<bool> inMST(n, false);
    vector<int> parent(n, -1);

    // Min-heap: (key, node)
    priority_queue<pair<int,int>,
                   vector<pair<int,int>>,
                   greater<pair<int,int>>> pq;

    key[inicio] = 0;
    pq.push({0, inicio});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        if (inMST[u]) continue;
        inMST[u] = true;

        for (auto &[v, w] : this->aristas[u]) {
            if (!inMST[v] && w < key[v]) {
                key[v] = w;
                parent[v] = u;
                pq.push({key[v], v});
            }
        }
    }

    // Build MST this->aristasacency list
    vector<vector<pair<int,int>>> mst(n);

    for (int v = 0; v < n; v++) {
        if (parent[v] != -1) {
            int u = parent[v];
            int w = 0;

            // find weight of edge (u,v)
            for (auto &p : this->aristas[v])
                if (p.first == u) w = p.second;

            mst[u].push_back({v, w});
            mst[v].push_back({u, w});
        }
    }

    int total = maquinaBFS(inicio, mst);
    return total;

}



int JuegoCon::maquinaDJI(int inicio )
{    
    const int INF = 1e9;

    // Dijkstra from a single source
    int n = this->aristas.size();
    vector<int> dist(n, INF);

    // min-heap priority queue: (distance, node)
    priority_queue<pair<int,int>,
         vector<pair<int,int>>, 
            greater<pair<int,int>>> pq;

    dist[inicio] = 0;
    pq.push({0, inicio});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        // Skip outdated states
        if (d > dist[u]) continue;

        // Relax edges
        for (auto &[v, w] : this->aristas[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }

    // se suma la distancia ecnotrada
    return dist[0];
  
}

const std::vector<std::vector<std::pair<int,int>>>& JuegoCon::getAristas() const
{
    return aristas;
}
