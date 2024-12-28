#ifndef VEC2_UTIL
#define VEC2_UTIL

#include <SFML/Graphics.hpp>

float Dot(sf::Vector2f vec_a, sf::Vector2f vec_b) {
    return vec_a.x * vec_b.x + vec_a.y * vec_b.y;
}

float NormSq(sf::Vector2f vec) {
    return Dot(vec, vec);
}

float Norm(sf::Vector2f vec) {
    return std::sqrt(NormSq(vec));
}

sf::Vector2f Unit(sf::Vector2f vec) {
    float mag = Norm(vec);

    return sf::Vector2f(vec.x / mag, vec.y / mag);
}

#endif