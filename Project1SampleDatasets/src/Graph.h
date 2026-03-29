/**
 * @file Graph.h
 * @brief Generic directed weighted graph with flow support.
 *
 * Provides Vertex, Edge and Graph template classes used as the backbone
 * of the Max-Flow network.
 * @author Gonçalo Leão;
 * updated by the DA 2024/2025 team.
 */

#ifndef DA_TP_CLASSES_GRAPH
#define DA_TP_CLASSES_GRAPH

#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include "MutablePriorityQueue.h" // not needed for now

template <class T>
class Edge;

#define INF std::numeric_limits<double>::max()

/************************* Vertex  **************************/

/**
 * @class Vertex
 * @brief Represents a node in the graph.
 *
 * Stores adjacency lists (outgoing and incoming edges), auxiliary fields
 * used by graph algorithms (BFS, DFS, Dijkstra, Tarjan SCC, topological sort)
 * and the MutablePriorityQueue index required for Prim/Dijkstra.
 *
 * @tparam T Type of the information stored at each vertex.
 */

template <class T>
class Vertex {
public:
    Vertex(T in);
    bool operator<(Vertex<T> & vertex) const; // // required by MutablePriorityQueue

    T getInfo() const;
    std::vector<Edge<T> *> getAdj() const;
    bool isVisited() const;
    bool isProcessing() const;
    unsigned int getIndegree() const;
    double getDist() const;
    Edge<T> *getPath() const;
    std::vector<Edge<T> *> getIncoming() const;

    void setInfo(T info);
    void setVisited(bool visited);
    void setProcessing(bool processing);

    int getLow() const;
    void setLow(int value);
    int getNum() const;
    void setNum(int value);

    void setIndegree(unsigned int indegree);
    void setDist(double dist);
    void setPath(Edge<T> *path);
    Edge<T> * addEdge(Vertex<T> *dest, double w);
    bool removeEdge(T in);
    void removeOutgoingEdges();

    friend class MutablePriorityQueue<Vertex>;
protected:
    T info;                // info node
    std::vector<Edge<T> *> adj;  // outgoing edges

    // auxiliary fields
    bool visited = false; // used by DFS, BFS, Prim ...
    bool processing = false; // used by isDAG (in addition to the visited attribute)
    int low = -1, num = -1; // used by SCC Tarjan
    unsigned int indegree; // used by topsort
    double dist = 0;
    Edge<T> *path = nullptr;

    std::vector<Edge<T> *> incoming; // incoming edges

    int queueIndex = 0; 		// required by MutablePriorityQueue and UFDS

    void deleteEdge(Edge<T> *edge);
};

/********************** Edge  ****************************/

/**
 * @class Edge
 * @brief Represents a directed weighted edge with optional flow and reverse pointer.
 *
 * Used both for standard graph traversal and for residual-graph operations
 * in the Edmonds–Karp Max-Flow algorithm.
 *
 * @tparam T Type of the information stored at each vertex endpoint.
 */

template <class T>
class Edge {
public:
    Edge(Vertex<T> *orig, Vertex<T> *dest, double w);

    Vertex<T> * getDest() const;
    double getWeight() const;
    bool isSelected() const;
    Vertex<T> * getOrig() const;
    Edge<T> *getReverse() const;
    double getFlow() const;

    void setSelected(bool selected);
    void setReverse(Edge<T> *reverse);
    void setFlow(double flow);
protected:
    Vertex<T> * dest; // destination vertex
    double weight; // edge weight, can also be used for capacity

    // auxiliary fields
    bool selected = false;

    // used for bidirectional edges
    Vertex<T> *orig;
    Edge<T> *reverse = nullptr;

    double flow; // for flow-related problems
};

/********************** Graph  ****************************/

/**
 * @class Graph
 * @brief Generic directed weighted graph.
 *
 * Stores a vertex set and provides operations to add/remove vertices and edges.
 * Also holds optional distance and path matrices for Floyd–Warshall.
 *
 * @tparam T Type of the information stored at each vertex.
 */

template <class T>
class Graph {
public:
    ~Graph();
    Vertex<T> *findVertex(const T &in) const;

    bool addVertex(const T &in);
    bool removeVertex(const T &in);

    bool addEdge(const T &sourc, const T &dest, double w);
    bool removeEdge(const T &source, const T &dest);
    bool addBidirectionalEdge(const T &sourc, const T &dest, double w);

    int getNumVertex() const;

    std::vector<Vertex<T> *> getVertexSet() const;


protected:
    std::vector<Vertex<T> *> vertexSet;    // vertex set

    double ** distMatrix = nullptr;   // dist matrix for Floyd-Warshall
    int **pathMatrix = nullptr;   // path matrix for Floyd-Warshall

    int findVertexIdx(const T &in) const;

};

void deleteMatrix(int **m, int n);
void deleteMatrix(double **m, int n);


/************************* Vertex  **************************/

/**
     * @brief Constructs a vertex with the given info.
     * @param in Node information/label.
     */
template <class T>
Vertex<T>::Vertex(T in): info(in) {}

/**
     * @brief Adds an outgoing edge from this vertex to @p dest with weight @p w.
     * @param d Destination vertex.
     * @param w    Edge weight / capacity.
     * @return Pointer to the newly created edge.
     * @complexity O(1)
     */
template <class T>
Edge<T> * Vertex<T>::addEdge(Vertex<T> *d, double w) {
    auto newEdge = new Edge<T>(this, d, w);
    adj.push_back(newEdge);
    d->incoming.push_back(newEdge);
    return newEdge;
}

/**
     * @brief Removes the outgoing edge whose destination has info @p in.
     * @param in Info of the destination vertex to disconnect.
     * @return true if at least one edge was removed, false otherwise.
     * @complexity O(deg(v))
     */
template <class T>
bool Vertex<T>::removeEdge(T in) {
    bool removedEdge = false;
    auto it = adj.begin();
    while (it != adj.end()) {
        Edge<T> *edge = *it;
        Vertex<T> *dest = edge->getDest();
        if (dest->getInfo() == in) {
            it = adj.erase(it);
            deleteEdge(edge);
            removedEdge = true;
        }
        else {
            it++;
        }
    }
    return removedEdge;
}

/**
     * @brief Removes and deletes all outgoing edges from this vertex.
     * @complexity O(deg(v))
     */
template <class T>
void Vertex<T>::removeOutgoingEdges() {
    auto it = adj.begin();
    while (it != adj.end()) {
        Edge<T> *edge = *it;
        it = adj.erase(it);
        deleteEdge(edge);
    }
}

/**
     * @brief Less-than operator required by MutablePriorityQueue (compares by dist).
     * @param vertex Other vertex to compare against.
     * @return true if this vertex has smaller dist.
     */
template <class T>
bool Vertex<T>::operator<(Vertex<T> & vertex) const {
    return this->dist < vertex.dist;
}

/** @brief Returns the information stored at this vertex. */
template <class T>
T Vertex<T>::getInfo() const {
    return this->info;
}

/** @brief Returns the Tarjan SCC low-link value.
  * @return Low-link value, or -1 if not yet assigned. */
template <class T>
int Vertex<T>::getLow() const {
    return this->low;
}

/** @brief Sets the Tarjan low-link value.
 * @param value New value. */
template <class T>
void Vertex<T>::setLow(int value) {
    this->low = value;
}

/** @brief Returns the Tarjan DFS discovery number.
  * @return Discovery number, or -1 if not yet assigned. */
template <class T>
int Vertex<T>::getNum() const {
    return this->num;
}

/** @brief Sets the Tarjan discovery number.
  * @param value New value. */
template <class T>
void Vertex<T>::setNum(int value) {
    this->num = value;
}

/** @brief Returns the list of outgoing edges. */
template <class T>
std::vector<Edge<T>*> Vertex<T>::getAdj() const {
    return this->adj;
}

/** @brief Returns true if this vertex has been visited. */
template <class T>
bool Vertex<T>::isVisited() const {
    return this->visited;
}

/** @brief Returns true if this vertex is currently being processed (cycle detection). */
template <class T>
bool Vertex<T>::isProcessing() const {
    return this->processing;
}

/** @brief Returns the in-degree of this vertex. */
template <class T>
unsigned int Vertex<T>::getIndegree() const {
    return this->indegree;
}

/** @brief Returns the current shortest-path distance. */
template <class T>
double Vertex<T>::getDist() const {
    return this->dist;
}

/** @brief Returns the predecessor edge on the shortest-path tree. */
template <class T>
Edge<T> *Vertex<T>::getPath() const {
    return this->path;
}

/** @brief Returns the list of incoming edges. */
template <class T>
std::vector<Edge<T> *> Vertex<T>::getIncoming() const {
    return this->incoming;
}

/** @brief Sets the information stored at this vertex. @param info New info value. */
template <class T>
void Vertex<T>::setInfo(T in) {
    this->info = in;
}

/** @brief Sets the visited flag.
 * @param visited New value. */
template <class T>
void Vertex<T>::setVisited(bool visited) {
    this->visited = visited;
}

/** @brief Sets the processing flag.
 * @param processing New value. */
template <class T>
void Vertex<T>::setProcessing(bool processing) {
    this->processing = processing;
}

/** @brief Sets the in-degree.
 * @param indegree New in-degree. */
template <class T>
void Vertex<T>::setIndegree(unsigned int indegree) {
    this->indegree = indegree;
}

/** @brief Sets the shortest-path distance.
 * @param dist New distance. */
template <class T>
void Vertex<T>::setDist(double dist) {
    this->dist = dist;
}

/** @brief Sets the predecessor edge on the shortest-path tree.
 * @param path Pointer to the edge. */
template <class T>
void Vertex<T>::setPath(Edge<T> *path) {
    this->path = path;
}

/**
     * @brief Removes @p edge from the incoming list of its destination and deletes it.
     * @param edge Edge to delete.
     * @complexity O(in-degree of dest)
     */
template <class T>
void Vertex<T>::deleteEdge(Edge<T> *edge) {
    Vertex<T> *dest = edge->getDest();
    auto it = dest->incoming.begin();
    while (it != dest->incoming.end()) {
        if ((*it)->getOrig()->getInfo() == info) {
            it = dest->incoming.erase(it);
        }
        else {
            it++;
        }
    }
    delete edge;
}

/********************** Edge  ****************************/

/**
     * @brief Constructs an edge from @p orig to @p dest with weight @p w.
     * @param orig Source vertex.
     * @param dest Destination vertex.
     * @param w    Edge weight / capacity.
     */
template <class T>
Edge<T>::Edge(Vertex<T> *orig, Vertex<T> *dest, double w): orig(orig), dest(dest), weight(w) {}

/** @brief Returns the destination vertex. */
template <class T>
Vertex<T> * Edge<T>::getDest() const {
    return this->dest;
}

/** @brief Returns the edge weight (also used as capacity). */
template <class T>
double Edge<T>::getWeight() const {
    return this->weight;
}

/** @brief Returns the source vertex. */
template <class T>
Vertex<T> * Edge<T>::getOrig() const {
    return this->orig;
}

/** @brief Returns the reverse edge (used in residual graph). */
template <class T>
Edge<T> *Edge<T>::getReverse() const {
    return this->reverse;
}

/** @brief Returns true if this edge is marked as selected. */
template <class T>
bool Edge<T>::isSelected() const {
    return this->selected;
}

/** @brief Returns the current flow through this edge. */
template <class T>
double Edge<T>::getFlow() const {
    return flow;
}

/** @brief Marks or unmarks this edge as selected.
 * @param selected New value. */
template <class T>
void Edge<T>::setSelected(bool selected) {
    this->selected = selected;
}

/** @brief Sets the reverse edge pointer.
 * @param reverse Pointer to the reverse edge. */
template <class T>
void Edge<T>::setReverse(Edge<T> *reverse) {
    this->reverse = reverse;
}

/** @brief Sets the flow through this edge.
 * @param flow New flow value. */
template <class T>
void Edge<T>::setFlow(double flow) {
    this->flow = flow;
}

/********************** Graph  ****************************/


/** @brief Returns the number of vertices in the graph.
  * @return Vertex count. */
template <class T>
int Graph<T>::getNumVertex() const {
    return vertexSet.size();
}

/** @brief Returns the full vertex set.
  * @return Vector of vertex pointers. */
template <class T>
std::vector<Vertex<T> *> Graph<T>::getVertexSet() const {
    return vertexSet;
}

/**
     * @brief Finds a vertex by its info value.
     * @param in Info to search for.
     * @return Pointer to the vertex, or nullptr if not found.
     * @complexity O(V)
     */
template <class T>
Vertex<T> * Graph<T>::findVertex(const T &in) const {
    for (auto v : vertexSet)
        if (v->getInfo() == in)
            return v;
    return nullptr;
}

/**
     * @brief Finds the index of the vertex with info @p in in vertexSet.
     * @param in Info to search for.
     * @return Index in vertexSet, or -1 if not found.
     * @complexity O(V)
     */
template <class T>
int Graph<T>::findVertexIdx(const T &in) const {
    for (unsigned i = 0; i < vertexSet.size(); i++)
        if (vertexSet[i]->getInfo() == in)
            return i;
    return -1;
}

/**
     * @brief Adds a new vertex with info @p in.
     * @param in Info for the new vertex.
     * @return true if added, false if a vertex with that info already exists.
     * @complexity O(V)
     */
template <class T>
bool Graph<T>::addVertex(const T &in) {
    if (findVertex(in) != nullptr)
        return false;
    vertexSet.push_back(new Vertex<T>(in));
    return true;
}

/**
     * @brief Removes the vertex with info @p in and all its incident edges.
     * @param in Info of the vertex to remove.
     * @return true if removed, false if not found.
     * @complexity O(V + E)
     */
template <class T>
bool Graph<T>::removeVertex(const T &in) {
    for (auto it = vertexSet.begin(); it != vertexSet.end(); it++) {
        if ((*it)->getInfo() == in) {
            auto v = *it;
            v->removeOutgoingEdges();
            for (auto u : vertexSet) {
                u->removeEdge(v->getInfo());
            }
            vertexSet.erase(it);
            delete v;
            return true;
        }
    }
    return false;
}

/**
     * @brief Adds a directed edge from @p sourc to @p dest with weight @p w.
     * @param sourc Source vertex info.
     * @param dest  Destination vertex info.
     * @param w     Edge weight / capacity.
     * @return true if added, false if either endpoint does not exist.
     * @complexity O(V)
     */
template <class T>
bool Graph<T>::addEdge(const T &sourc, const T &dest, double w) {
    auto v1 = findVertex(sourc);
    auto v2 = findVertex(dest);
    if (v1 == nullptr || v2 == nullptr)
        return false;
    v1->addEdge(v2, w);
    return true;
}

/**
     * @brief Removes the directed edge from @p sourc to @p dest.
     * @param sourc Source vertex info.
     * @param dest   Destination vertex info.
     * @return true if removed, false if edge does not exist.
     * @complexity O(V + deg(source))
     */
template <class T>
bool Graph<T>::removeEdge(const T &sourc, const T &dest) {
    Vertex<T> * srcVertex = findVertex(sourc);
    if (srcVertex == nullptr) {
        return false;
    }
    return srcVertex->removeEdge(dest);
}

/**
     * @brief Adds two directed edges (sourc→dest and dest→sourc) linked as reverse pairs.
     * @param sourc Source vertex info.
     * @param dest  Destination vertex info.
     * @param w     Edge weight / capacity.
     * @return true if both edges were added, false if either endpoint does not exist.
     * @complexity O(V)
     */
template <class T>
bool Graph<T>::addBidirectionalEdge(const T &sourc, const T &dest, double w) {
    auto v1 = findVertex(sourc);
    auto v2 = findVertex(dest);
    if (v1 == nullptr || v2 == nullptr)
        return false;
    auto e1 = v1->addEdge(v2, w);
    auto e2 = v2->addEdge(v1, w);
    e1->setReverse(e2);
    e2->setReverse(e1);
    return true;
}

/**
 * @brief Frees a 2D int matrix of size n×n.
 * @param m Pointer to the matrix.
 * @param n Number of rows/columns.
 */
inline void deleteMatrix(int **m, int n) {
    if (m != nullptr) {
        for (int i = 0; i < n; i++)
            if (m[i] != nullptr)
                delete [] m[i];
        delete [] m;
    }
}

/**
 * @brief Frees a 2D double matrix of size n×n.
 * @param m Pointer to the matrix.
 * @param n Number of rows/columns.
 */
inline void deleteMatrix(double **m, int n) {
    if (m != nullptr) {
        for (int i = 0; i < n; i++)
            if (m[i] != nullptr)
                delete [] m[i];
        delete [] m;
    }
}

/** @brief Destructor — frees all vertices and distance/path matrices. */
template <class T>
Graph<T>::~Graph() {
    deleteMatrix(distMatrix, vertexSet.size());
    deleteMatrix(pathMatrix, vertexSet.size());
}

#endif /* DA_TP_CLASSES_GRAPH */