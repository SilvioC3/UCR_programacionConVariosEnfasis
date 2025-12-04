#include "JuegoCon.hh"

#include <fstream>
#include <sstream>
#include <string>  
#include <iostream>
#include <queue>
#include <climits>
#include <functional>
#include <random>
#include <ctime>

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
        case 2: {
            int recurso = this->getRecurso(nodoActual);
            int maquina = jugador->elegirMaquina();
            int restaPorAristas = 0;

            int nivelActual = nivelMaquina[nodoActual]; // 0,1,2,3

            // REGLAS
            if (maquina == 1 && nivelActual != 0) {
                std::cout << "Ya existe una máquina. No puedes construir BFS.\n";
                return;
            }
            if (maquina == 2 && nivelActual == 3) {
                std::cout << "Este nodo ya tiene Dijkstra.\n";
                return;
            }
            if (maquina == 3 && nivelActual == 3) {
                std::cout << "Este nodo ya tiene Dijkstra.\n";
                return;
            }

            switch (maquina)
            {
            case 1:
                restaPorAristas = maquinaBFS(nodoActual, this->aristas);
                jugador->setRecusos(recurso - restaPorAristas);
                nivelMaquina[nodoActual] = 1;
                break;

            case 2:
                restaPorAristas = maquinaPRI(nodoActual);
                jugador->setRecusos(recurso - restaPorAristas);
                nivelMaquina[nodoActual] = 2;
                break;

            case 3:
                restaPorAristas = maquinaDJI(nodoActual);
                jugador->setRecusos(recurso - restaPorAristas);
                nivelMaquina[nodoActual] = 3;
                break;

            default:
                break;
            }

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


//Metodo para sacar los recursos
int JuegoCon::getRecurso(int nodo)
{
    if (nodos[nodo].recursoReclamado)
        return 0;
    nodos[nodo].recursoReclamado = true;
    return nodos[nodo].recursos;
}


//Metodo para construir la maquina de BFS
int JuegoCon::maquinaBFS(int inicio,
                         const std::vector<std::vector<std::pair<int,int>>> &adj)
{
    int N = adj.size();
    vector<bool> visited(N, false);
    vector<int> parent(N, -1);
    queue<int> q;

    visited[inicio] = true;
    q.push(inicio);


    while (!q.empty()) {
        int u = q.front(); q.pop();

        if (u == 0) break;

        for (auto &[v, w] : adj[u]) {
            if (!visited[v]) {
                visited[v] = true;
                parent[v] = u;
                q.push(v);
            }
        }
    }


    if (!visited[0]) return 0;

    int costoTotal = 0;
    for (int v = 0; parent[v] != -1; v = parent[v]) {
        int u = parent[v];

        for (auto &p : adj[u])
            if (p.first == v)
                costoTotal += p.second;
    }
    if (primerBFSGratis) {
        primerBFSGratis = false;
        return 0;
    }

    return costoTotal;
}

//Metodo para construir la maquina de prim
int JuegoCon::maquinaPRI(int inicio) {

    int n = this->aristas.size();
    vector<int> key(n, 1e9);
    vector<bool> inMST(n, false);
    vector<int> parent(n, -1);

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

    vector<vector<pair<int,int>>> mst(n);
    for (int v = 0; v < n; v++) {
        if (parent[v] != -1) {
            int u = parent[v];
            int w = 0;
            for (auto &p : this->aristas[v])
                if (p.first == u) { w = p.second; break; }

            mst[u].push_back({v, w});
            mst[v].push_back({u, w});
        }
    }

    vector<bool> visited(n, false);
    vector<int> parMST(n, -1);
    queue<int> q;
    visited[inicio] = true;
    q.push(inicio);

    while (!q.empty()) {
        int u = q.front(); q.pop();
        if (u == 0) break;
        for (auto &pw : mst[u]) {
            int v = pw.first;
            if (!visited[v]) {
                visited[v] = true;
                parMST[v] = u;
                q.push(v);
            }
        }
    }


    if (!visited[0]) return 0;

    int totalPeso = 0;
    for (int v = 0; parMST[v] != -1; v = parMST[v]) {
        int u = parMST[v];
        for (auto &p : mst[u]) {
            if (p.first == v) {
                totalPeso += p.second;
                break;
            }
        }
    }
    return totalPeso;
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

//Metodo para sacar el camino de Dijkstra
std::vector<std::pair<int,int>> JuegoCon::getCaminoDJ(int inicio) const
{
    const int INF = 1e9;
    int n = aristas.size();
    std::vector<int> dist(n, INF), parent(n, -1);

    priority_queue<pair<int,int>,
                   vector<pair<int,int>>,
                   greater<pair<int,int>>> pq;

    dist[inicio] = 0;
    pq.push({0, inicio});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (d > dist[u]) continue;

        for (auto &[v, w] : aristas[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    // camino a la base
    std::vector<std::pair<int,int>> camino;
    for (int at = 0; parent[at] != -1; at = parent[at]) {
        camino.push_back({parent[at], at});
    }
    return camino;
}

//Metodo para sacar el camino de bfs
std::vector<std::pair<int,int>> JuegoCon::getCaminoBFS(int inicio) const
{
    int n = aristas.size();
    std::vector<bool> visited(n, false);
    std::vector<int> parent(n, -1);
    std::queue<int> q;

    visited[inicio] = true;
    q.push(inicio);

    while (!q.empty()) {
        int u = q.front(); q.pop();
        if (u == 0) break;

        for (auto &p : aristas[u]) {
            int v = p.first;
            if (!visited[v]) {
                visited[v] = true;
                parent[v] = u;
                q.push(v);
            }
        }
    }

    std::vector<std::pair<int,int>> camino;
    for (int at = 0; parent[at] != -1; at = parent[at]) {
        camino.push_back({parent[at], at});
    }
    return camino;
}

//Metodo para sacar el camino de PRIM
std::vector<std::pair<int,int>> JuegoCon::getCaminoPRI(int inicio) const
{
    int n = aristas.size();
    std::vector<int> key(n, 1e9);
    std::vector<int> parent(n, -1);
    std::vector<bool> inMST(n, false);

    priority_queue<pair<int,int>,
                   vector<pair<int,int>>,
                   greater<pair<int,int>>> pq;

    key[inicio] = 0;
    pq.push({0, inicio});

    while (!pq.empty()) {
        int u = pq.top().second; pq.pop();
        if (inMST[u]) continue;
        inMST[u] = true;

        for (auto &[v, w] : aristas[u]) {
            if (!inMST[v] && w < key[v]) {
                key[v] = w;
                parent[v] = u;
                pq.push({key[v], v});
            }
        }
    }

    //camino a la base
    std::vector<std::pair<int,int>> camino;
    for (int at = 0; parent[at] != -1; at = parent[at]) {
        camino.push_back({parent[at], at});
    }
    return camino;
}

// metodo extra implementacion A* con AI
int JuegoCon::maquinaAstar(int inicio)
{
    const int meta = 0;

    int n = aristas.size();
    if (inicio < 0 || inicio >= n) return INT_MAX;
    if (meta < 0 || meta >= n) return INT_MAX;

    vector<int> h(n, INT_MAX);
    queue<int> q;

    h[meta] = 0;
    q.push(meta);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (auto &edge : aristas[u]) {
            int v = edge.first;
            if (v < 0 || v >= n) continue;

            if (h[v] == INT_MAX) {
                h[v] = h[u] + 1;
                q.push(v);
            }
        }
    }

    vector<int> g(n, INT_MAX);
    parentAStar.assign(n, -1);

    priority_queue<
        pair<int,int>,
        vector<pair<int,int>>,
        greater<pair<int,int>>
        > pq;

    g[inicio] = 0;
    pq.push({h[inicio], inicio});

    while (!pq.empty()) {
        auto [f, u] = pq.top();
        pq.pop();

        if (u == meta)
            return g[u];

        for (auto &edge : aristas[u]) {
            int v = edge.first;
            int w = edge.second;

            if (v < 0 || v >= n) continue;
            if (g[u] == INT_MAX) continue;

            int nuevoG = g[u] + w;
            if (nuevoG < g[v]) {
                g[v] = nuevoG;
                parentAStar[v] = u;

                if (h[v] == INT_MAX) continue;

            pq.push({nuevoG + h[v], v});
            }
        }
    }

    return INT_MAX;
}

//Metodo para sacar el camino de A*
std::vector<std::pair<int,int>> JuegoCon::getCaminoAStar(int inicio)
{
    if (parentAStar.empty()) return {};

    std::vector<std::pair<int,int>> camino;
    int actual = inicio;

    while (actual != 0) {
        int p = parentAStar[actual];
        if (p == -1) break;
        camino.push_back({p, actual});
        actual = p;
    }

    std::reverse(camino.begin(), camino.end());
    return camino;
}


//metodo auxiliar paraA*
int JuegoCon::ejecutarMaquinaAStar(int nodo) {
    return maquinaAstar(nodo);
}

//Metodo auxiliar en manejo de nodos
void JuegoCon::vaciarRecursosNodo(int id) {
    nodos[id].recursos = 0;
    nodos[id].recursoReclamado = true;
}


//Metodo para crear un grafo al azar, con una base y siempre deberia tener minimo 1500 recursos
void JuegoCon::generaGrafoAleatorio(int n, int conexionesMinimas, int conexionesExtras)
{
    nodos.clear();
    aristas.clear();
    nodos.resize(n);
    aristas.resize(n);

    std::mt19937 rng(time(nullptr));
    std::uniform_int_distribution<int> tipoRand(1, 2);
    std::uniform_int_distribution<int> recursoRand(50, 300);
    std::uniform_int_distribution<int> pesoRand(5, 30);

    const int MAX_VECINOS = 3;
    const float DISTANCIA_MAX = 150;
    const int CENTRO_X = 200;
    const int CENTRO_Y = 200;
    const int RADIO_CIRCULO = 150;
    const int RUIDO_POS = 50;

    std::uniform_real_distribution<float> ruido(-RUIDO_POS, RUIDO_POS);

    nodos[0].id = 0;
    nodos[0].x = CENTRO_X + RADIO_CIRCULO + ruido(rng);
    nodos[0].y = CENTRO_Y + RADIO_CIRCULO + ruido(rng);
    nodos[0].tipo = base;
    nodos[0].recursos = 0;

    for (int i = 1; i < n; i++) {
        float angulo = 2.0 * M_PI * i / n;
        float baseX = CENTRO_X + RADIO_CIRCULO * cos(angulo);
        float baseY = CENTRO_Y + RADIO_CIRCULO * sin(angulo);

        float x = baseX + ruido(rng);
        float y = baseY + ruido(rng);

        nodos[i].id = i;
        nodos[i].x = static_cast<int>(x);
        nodos[i].y = static_cast<int>(y);

        int tipoAux = tipoRand(rng);
        nodos[i].tipo = static_cast<tipoDeNodo>(tipoAux);
        nodos[i].recursos = 0;
    }

    // Asegurar al menos 1500 recursos en total distribuidos en fábricas
    int recursos_totales = 0;
    std::vector<int> indices_fabrica;

    for (int i = 1; i < n; i++) {
        if (nodos[i].tipo == fabrica) {
            indices_fabrica.push_back(i);
        }
    }

    if (indices_fabrica.empty() && n > 1) {
        indices_fabrica.push_back(1);
        nodos[1].tipo = fabrica;
    }

    int base_recurso = 1500 / (int)indices_fabrica.size();
    int resto = 1500 % (int)indices_fabrica.size();

    for (int idx : indices_fabrica) {
        nodos[idx].recursos = base_recurso;
        recursos_totales += base_recurso;
    }

    std::uniform_int_distribution<int> fabricaRand(0, (int)indices_fabrica.size() - 1);
    while (recursos_totales < 1500) {
        int idx = indices_fabrica[fabricaRand(rng)];
        int incremento = std::min(10, 1500 - recursos_totales);
        nodos[idx].recursos += incremento;
        recursos_totales += incremento;
    }

    // Función para calcular distancia euclidiana
    auto distancia = [&](int a, int b) {
        int dx = nodos[a].x - nodos[b].x;
        int dy = nodos[a].y - nodos[b].y;
        return sqrt(dx * dx + dy * dy);
    };

    std::vector<int> grado(n, 0);

    // Crear árbol mínimo simple conectando nodos consecutivos
    for (int i = 0; i < n - 1; i++) {
        int peso = pesoRand(rng);
        aristas[i].push_back({i + 1, peso});
        aristas[i + 1].push_back({i, peso});
        grado[i]++;
        grado[i + 1]++;
    }

    // Añadir conexiones extras respetando restricciones
    std::uniform_int_distribution<int> nodoRand(0, n - 1);

    int intentos = 0;
    int maxIntentos = n * 10;
    int conexionesAgregadas = 0;
    int totalExtras = n * conexionesExtras;

    while (conexionesAgregadas < totalExtras && intentos < maxIntentos) {
        int u = nodoRand(rng);
        int v = nodoRand(rng);

        if (u == v) {
            intentos++;
            continue;
        }

        if (distancia(u, v) > DISTANCIA_MAX) {
            intentos++;
            continue;
        }

        if (grado[u] >= MAX_VECINOS || grado[v] >= MAX_VECINOS) {
            intentos++;
            continue;
        }

        bool existe = false;
        for (auto &vecino : aristas[u]) {
            if (vecino.first == v) {
                existe = true;
                break;
            }
        }

        if (existe) {
            intentos++;
            continue;
        }

        int peso = pesoRand(rng);
        aristas[u].push_back({v, peso});
        aristas[v].push_back({u, peso});
        grado[u]++;
        grado[v]++;
        conexionesAgregadas++;
        intentos = 0;
    }

    cout << "Grafo aleatorio generado con " << n << " nodos, " << totalExtras << " conexiones extras, limitando grado máximo a "
         << MAX_VECINOS << " y distancia máxima " << DISTANCIA_MAX << endl;
    cout << "Base ubicada en nodo 0" << endl;
    cout << "Recursos totales en fábricas: " << recursos_totales << endl;
}
