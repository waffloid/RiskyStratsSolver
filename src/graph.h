// todo, replace weird book-keeping with set

#include <vector>
#include <array>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp> 
#include <iostream>
#include <cmath>
#include <queue>
#include <cstdlib>
#include <algorithm>

#include "vec2.h"
#include <unordered_map>
#include <set>
#include <thread>




#ifndef GRAPH
#define GRAPH

sf::Color getColorFromInt(int value) {
    // Ensure the value is within a reasonable range (0-255)
    srand(value+20);

    if (value == 0) {
        return sf::Color(255, 0, 0);
    }

    int r = value * 30;
    int b = 255 - value * 30;
    int g = 0;

    return sf::Color(r, g, b);
}


struct Node {
    sf::Vector2<float> position;  
    std::vector<int> neighbors;
};

struct QueuePair {
    int node_index;
    int depth;
};


template <int N>
class GeometricGraph {
private:
    Node nodes[N];
public:
    float radius = 5.0;
    int no_edges = 0;
    int size = N;

    sf::CircleShape circles[N];
    sf::VertexArray lines;

    GeometricGraph() {
        lines.setPrimitiveType(sf::PrimitiveType::Lines);

        for (int i = 0; i < N; ++i) {
            circles[i].setRadius(radius);
            circles[i].setFillColor(sf::Color::White);
        }
    }

    void SetPosition(int node_index, sf::Vector2<float> position) {
        nodes[node_index].position = position;
        circles[node_index].setPosition(sf::Vector2{position.x - radius, position.y - radius});
    }

    void SplitByAxis() {
        float axis_x = static_cast<float>(rand()%20 - 10) / 10.0;
        float axis_y = static_cast<float>(rand()%20 - 10) / 10.0;

        sf::Vector2f direction = Unit(sf::Vector2f(axis_x, axis_y));

    
        std::array<bool, N> left_partition;
        std::array<bool, N> separator_added;
        std::vector<int> separator;

        for (int i=0; i<N; ++i) {
            left_partition[i] = false;
            separator_added[i] = false;
        }
        

        // compute average dot so that we separate the graph with plane that cuts through median position
        float total = 0; 
        for (int i=0; i<N; ++i) {
            total += Dot(direction, nodes[i].position);
        }
        
        float avg = total / N;
        int left_partition_size = 0;

        for (int i=0; i<N; ++i) {
            if (Dot(nodes[i].position, direction) >= avg) {
                ++left_partition_size;
                left_partition[i] = true;
                SetColor(i, sf::Color(100, 255, 50));
            }
        }
        
        // find separator
        for (int i=0; i<N; ++i) {
            if (left_partition[i]) {
                for (const int& j: nodes[i].neighbors) {
                    if (!left_partition[j] & !separator_added[j]) {
                        separator_added[j] = true;
                        separator.push_back(j);
                        SetColor(j, sf::Color(0,200,250));
                    }
                }
            }
        }

        // improve separator -- abstract this out into a separate function so that it can be called multiple times?
        // for now expand in specific direction, then we fix logic to balance out graph

        std::set<int> separator_neighbors;
        std::unordered_map<int, std::set<int>> neighbors_of_separator_neighbors;

        for (const int& i:separator) {
            for (const int& j:nodes[i].neighbors) {
                if (left_partition[j]) {
                    SetColor(j, sf::Color::Blue);
                    // account for j being a nbr of separator, then init empty set of nbrs for j
                    separator_neighbors.insert(j);
                    neighbors_of_separator_neighbors.emplace(j, std::set<int>{});
                    
                    // populate nbrs of j in separator
                    for (const int& k: nodes[j].neighbors) {
                        if (separator_added[k]) {
                            neighbors_of_separator_neighbors[j].insert(k);
                            SetColor(j, sf::Color::Red);
                        }
                    }
                }
            }
        }

        std::unordered_map<int, std::set<int>> separator_subset_removal_candidate;
        
        // go through each separator node
        for (const int& i:separator) {
            // check which candidates the node lies in

            // "parents" in the sense that the nbd of i is a subset of the nbd of elt in set_of_parents
            std::set<int> set_of_parents;
            bool init = false;

            // do this by looking at the nbrs, b, of i and calculating the intersection of the nbrs of b
            for (const int& b: nodes[i].neighbors) {
                if (!left_partition[b])
                    continue;
                
                if (!init) {
                    set_of_parents = neighbors_of_separator_neighbors[b];
                    init = true;
                } else {

                    auto curr_nbrs = neighbors_of_separator_neighbors[b];
                    std::set<int> temp;
                    
                    std::set_intersection(set_of_parents.begin(), set_of_parents.end(), curr_nbrs.begin(), curr_nbrs.end(),
                          std::inserter(temp, temp.begin()));

                    set_of_parents = temp;
                }
            }

            for (const int& j: set_of_parents) {
                if (separator_subset_removal_candidate.count(j) > 0) {
                    separator_subset_removal_candidate.emplace(j, std::set<int>{});
                }

                separator_subset_removal_candidate[j].insert(i);
            }
        }


        // checks which separator subsets are good enough to remove
        for (const int& i : separator) {
            std::set<int> separator_nodes_contained_in_nbd_of_i = separator_subset_removal_candidate[i];

            int nbhd_size = 0;
            for (const int& j : nodes[i].neighbors) {
                if (left_partition[j]) nbhd_size++;
            }

            if (separator_nodes_contained_in_nbd_of_i.size() > nbhd_size) {
                for (const int& j : separator_nodes_contained_in_nbd_of_i) {
                    SetColor(j, sf::Color(100, 50, 255));
                }
            }
        }
    }

    void SetColor(int node_index, sf::Color color) {
        circles[node_index].setFillColor(color);
    }

    void BFS(int node_index) {
        bool already_seen[N];
        std::queue<QueuePair> node_queue;

        for (int i=0; i<N; ++i) {
            already_seen[i] = false;
        }

        node_queue.push(QueuePair{node_index, 0});

        while (node_queue.size() > 0) {
            QueuePair curr_queue_obj = node_queue.front();

            int curr_index = curr_queue_obj.node_index;
            int depth = curr_queue_obj.depth;

            SetColor(curr_index, getColorFromInt(depth));

            already_seen[curr_index] = true;

            node_queue.pop();
            for (const int& nbr_index:nodes[curr_index].neighbors) {
                if (already_seen[nbr_index]) continue;
                already_seen[nbr_index] = true;

                node_queue.push(QueuePair{nbr_index, depth+1});
            }
        }
    }

    float GetDistanceSq(int index_start, int index_finish) {
        sf::Vector2f pos_a = nodes[index_start].position;
        sf::Vector2f pos_b = nodes[index_finish].position;

        return (pos_a.x - pos_b.x)*(pos_a.x - pos_b.x) + (pos_a.y - pos_b.y)*(pos_a.y - pos_b.y);
    }

    void AddEdge(int node_start, int node_finish) {
        nodes[node_start].neighbors.push_back(node_finish);
        nodes[node_finish].neighbors.push_back(node_start);

        lines.resize((no_edges+1) * 2);

        lines[2*no_edges].position = nodes[node_start].position;
        lines[2*no_edges+1].position = nodes[node_finish].position;

        lines[2*no_edges].color = sf::Color(255, 255, 255, 100);
        lines[2*no_edges+1].color = sf::Color(255, 255, 255, 100);

        no_edges += 1;
    }
};

#endif