// Tile.h
#ifndef TILE_H
#define TILE_H

#include <SFML/Graphics.hpp>
#include "Constants.h"

struct Tile {
    int color;
    sf::RectangleShape shape;
    BonusType bonus;
    sf::CircleShape fuse; // Фитиль бомбы
    sf::Sprite recolorIcon; // Иконка перекраски
    sf::RectangleShape highlight; // Рамка для бонусов
    bool isSelected;

    // Анимация
    bool isAnimating;
    sf::Clock animationClock;
    float animationDuration;
    sf::RectangleShape animationOverlay; // Для анимаций перекраски

    Tile();

    // Методы
    void updateFuse(float deltaTime);
    void resetFuse();
};

#endif // TILE_H
