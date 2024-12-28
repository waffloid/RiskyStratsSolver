#include <SFML/Graphics.hpp>
#include "graph.h"
#include <iostream>
#include <math.h>
#include "riskygraph.h"
#include <cstdlib> 
#include <thread>


int main() {
    GeometricGraph<64> graph = RiskyGraph<64>(200, 600, 100, 500);

    sf::CircleShape shape(20.f);
    shape.setFillColor(sf::Color(100, 250, 50));


    sf::ContextSettings settings;
    settings.antiAliasingLevel = 0;

    graph.SplitByAxis();
    
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Risky Strats C++", sf::Style::Default, sf::State::Windowed, settings);

    // init graphy stuff


    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear();

        window.draw(graph.lines);
        for (int i = 0; i<graph.size; ++i) {
            window.draw(graph.circles[i]);
        }
        
        window.display();
    }
}