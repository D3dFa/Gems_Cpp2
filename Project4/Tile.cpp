// Tile.cpp
#include "Tile.h"

Tile::Tile() : color(0), bonus(BonusType::None), isSelected(false), isAnimating(false), animationDuration(0.5f) {
    // Настройка квадрата
    shape.setSize(sf::Vector2f(static_cast<float>(TILE_SIZE - 2), static_cast<float>(TILE_SIZE - 2)));
    shape.setOutlineThickness(1.0f);
    shape.setOutlineColor(sf::Color::Black);

    // Настройка фитиля (по умолчанию невидим)
    fuse.setRadius(5.0f);
    fuse.setFillColor(sf::Color::Red);
    fuse.setOrigin(fuse.getRadius(), fuse.getRadius());
    fuse.setPosition(-10, -10); // Вне видимой области

    // Настройка highlight (рамки для бонусов)
    highlight.setSize(shape.getSize());
    highlight.setFillColor(sf::Color::Transparent);
    highlight.setOutlineThickness(3.0f);
    highlight.setPosition(shape.getPosition());

    // Настройка анимации
    animationOverlay.setSize(shape.getSize());
    animationOverlay.setFillColor(sf::Color::Transparent);
    animationOverlay.setPosition(shape.getPosition());
}

// Метод для обновления фитиля бомбы
void Tile::updateFuse(float deltaTime) {
    if (bonus == BonusType::Bomb) {
        // Простая анимация мигания фитиля (пульсация)
        float time = animationClock.getElapsedTime().asSeconds();
        float scale = 1.0f + 0.3f * std::sin(time * 5.0f);
        fuse.setScale(scale, scale);
        // Изменение прозрачности для мигания
        sf::Color currentColor = fuse.getFillColor();
        currentColor.a = static_cast<sf::Uint8>(128 + 127 * std::sin(time * 5.0f));
        fuse.setFillColor(currentColor);
    }
}

// Метод для сброса фитиля бомбы
void Tile::resetFuse() {
    fuse.setPosition(-10, -10);
    fuse.setScale(1.0f, 1.0f);
    fuse.setFillColor(sf::Color::Red);
}
