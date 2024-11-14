// Tile.h
#ifndef TILE_H
#define TILE_H

#include <SFML/Graphics.hpp>
#include "Constants.h"

struct Tile {
    int color;
    sf::RectangleShape shape;
    BonusType bonus;
    sf::CircleShape fuse; // ������ �����
    sf::Sprite recolorIcon; // ������ ����������
    sf::RectangleShape highlight; // ����� ��� �������
    bool isSelected;

    // ��������
    bool isAnimating;
    sf::Clock animationClock;
    float animationDuration;
    sf::RectangleShape animationOverlay; // ��� �������� ����������

    Tile();

    // ������
    void updateFuse(float deltaTime);
    void resetFuse();
};

#endif // TILE_H
