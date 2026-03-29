/**
* @file EdmondsKarp.h
 * @brief Implementation of the Edmonds–Karp Max-Flow algorithm.
 */

#ifndef PROJECT1_EDMONDSKARP_H
#define PROJECT1_EDMONDSKARP_H
#include <queue>
#include "Graph.h"

/**
 * @brief BFS helper: enqueues vertex @p w if unvisited and residual capacity > 0.
 * @tparam T Node info type.
 * @param q BFS queue.
 * @param e Edge being traversed.
 * @param w Candidate vertex.
 * @param residual Available residual capacity.
 * @complexity O(1)
 */

template <class T>
void testAndVisit(std::queue< Vertex<T>*> &q, Edge<T> *e, Vertex<T> *w, double residual) {
    // Check if the vertex 'w' is not visited and there is residual capacity
    if (! w->isVisited() && residual > 0) {
        // Mark 'w' as visited, set the path through which it was reached, and enqueue it
        w->setVisited(true);
        w->setPath(e);
        q.push(w);
    }
}

/**
 * @brief Finds an augmenting path via BFS in the residual graph.
 * @tparam T Node info type.
 * @param g Pointer to the graph.
 * @param s Source NodeInfo.
 * @param t Sink NodeInfo.
 * @return true if a path was found.
 * @complexity O(V + E)
 */

template <class T>
bool findAugmentingPath(Graph<T> *g, Vertex<T> *s, Vertex<T> *t) {
    // Mark all vertices as not visited
    for(auto v : g->getVertexSet()) {
        v->setVisited(false);
    }

    std::queue<Vertex<T>*> q;
    s->setVisited(true);
    q.push(s);
    while (!q.empty())
    {
        Vertex<T>* u = q.front();
        q.pop();
        for (Edge<T>* e : u->getAdj())
        {
            Vertex<T>* w = e->getDest();
            testAndVisit(q, e, w, e->getWeight() - e->getFlow());
        }

        for (Edge<T>* e : u->getIncoming())
        {
            Vertex<T>* v = e->getOrig();
            testAndVisit(q, e, v, e->getFlow());
            }
        }
    return t->isVisited();
    }

    // Return true if a path to the target is found, false otherwise

/**
 * @brief Computes the bottleneck capacity along the augmenting path.
 * @tparam T Node info type.
 * @param source Source vertex.
 * @param sink Sink vertex.
 * @return Minimum residual capacity (bottleneck).
 * @complexity O(V)
 */

// Function to find the minimum residual capacity along the augmenting path
template <class T>
double findMinResidualAlongPath(Vertex<T> *source, Vertex<T> *sink) {
    double bottleneck = INF;
    Vertex<T>* cur = sink;
    while (cur != source)
    {
        Edge<T>* e = cur->getPath();
        if (e == nullptr) break;
        if (e->getDest() == cur)
        {
            bottleneck = std::min(bottleneck, e->getWeight() - e->getFlow());
            cur = e->getOrig();
        }
        else
        {
            bottleneck = std::min(bottleneck, e->getFlow());
            cur = e->getDest();
        }
    }
    // Return the minimum residual capacity
    return bottleneck;
}

/**
 * @brief Augments flow by @p f units along the current augmenting path.
 * @tparam T Node info type.
 * @param source Source vertex.
 * @param sink Sink vertex.
 * @param f Flow amount to add.
 * @complexity O(V)
 */

// Function to augment flow along the augmenting path with the given flow value
template <class T>
void augmentFlowAlongPath(Vertex<T> *source, Vertex<T> *sink, double f) {
    Vertex<T>* cur = sink;
    while (cur != source)
    {
        Edge<T>* e = cur->getPath();
        if (e == nullptr) break;
        if (e->getDest() == cur)
        {
            e->setFlow(e->getFlow() + f);
            cur = e->getOrig();
        }
        else
        {
            e->setFlow(e->getFlow() - f);
            cur = e->getDest();
        }
    }
}

/**
 * @brief Resets all edge flows to 0 in the graph.
 * @tparam T Node info type.
 * @param g Pointer to the graph.
 * @complexity O(E)
 */

template <class T>
void resetFlow(Graph<T>* g)
{
    for (Vertex<T>* v : g->getVertexSet())
    {
        for (Edge<T>* e : v->getAdj()) e->setFlow(0);
    }
}

/**
 * @brief Entry point: runs Edmonds–Karp Max-Flow from @p source to @p target.
 * @tparam T Node info type.
 * @param g      Pointer to the graph.
 * @param source NodeInfo of the source vertex.
 * @param target NodeInfo of the sink vertex.
 * @complexity O(V·E²)
 */

// Main function implementing the Edmonds-Karp algorithm
template <class T>
void edmondsKarp(Graph<T> *g, NodeInfo source, NodeInfo target) {
    Vertex<T>* s = g->findVertex(source);
    Vertex<T>* t = g->findVertex(target);

   while (findAugmentingPath(g, s, t))
   {
       double bottleneck = findMinResidualAlongPath(s, t);
       augmentFlowAlongPath(s, t, bottleneck);
   }
}
#endif //PROJECT1_EDMONDSKARP_H