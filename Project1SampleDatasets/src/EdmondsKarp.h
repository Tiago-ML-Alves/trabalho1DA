#ifndef PROJECT1_EDMONDSKARP_H
#define PROJECT1_EDMONDSKARP_H
#include <queue>
#include "Graph.h"

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

// Function to find an augmenting path using Breadth-First Search
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

// Main function implementing the Edmonds-Karp algorithm
template <class T>
void edmondsKarp(Graph<T> *g, NodeInfo source, NodeInfo target) {
    Vertex<T>* s = g->findVertex(source);
    Vertex<T>* t = g->findVertex(target);

    for (Vertex<T>* v : g->getVertexSet())
    {
        for (Edge<T>* e : v->getAdj())
        {
            e->setFlow(0);
        }
    }

   while (findAugmentingPath(g, s, t))
   {
       double bottleneck = findMinResidualAlongPath(s, t);
       augmentFlowAlongPath(s, t, bottleneck);
   }
}
#endif //PROJECT1_EDMONDSKARP_H