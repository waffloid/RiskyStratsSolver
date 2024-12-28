#include <SFML/Graphics.hpp>

#ifndef RISKYGRAPH
#define RISKYGRAPH

#include <SFML/System.hpp>
#include <cstdlib> 
#include "graph.h"
#include <cmath>

template<int N>
GeometricGraph<N> RiskyGraph(int min_x, int max_x, int min_y, int max_y) {
    srand(time(NULL));

    GeometricGraph<N> graph;

    // instance points
    for (int i=0; i<N; ++i) {
        int q_x = (i % 8) * 50 + 200 + (rand() % 40 - 20);
        int q_y = (i / 8) * 50 + 200 + (rand() % 40 - 20);

        float x = static_cast<float>(q_x);
        float y = static_cast<float>(q_y);

        std::cout << "(x, y): (" << x<<", " <<y<<")"<<"\n";

        graph.SetPosition(i, sf::Vector2f{x, y});
    }

    // instance edges
    for (int i=0; i<N; ++i) {
        for (int j=i+1; j<N; ++j) {
            int distance_sq = std::round(graph.GetDistanceSq(i, j));

            if (distance_sq < 6000) {
                graph.AddEdge(i, j);
            }
        }
    }

    return graph;
};

#endif
