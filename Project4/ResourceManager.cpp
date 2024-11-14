// ResourceManager.cpp
#include "ResourceManager.h"
#include <iostream>

ResourceManager& ResourceManager::getInstance() {
    static ResourceManager instance;
    return instance;
}

// Текстуры
bool ResourceManager::loadTexture(const std::string& name, const std::string& filename) {
    sf::Texture texture;
    if (!texture.loadFromFile(filename)) {
        std::cerr << "Не удалось загрузить текстуру: " << filename << "\n";
        return false;
    }
    textures[name] = texture;
    return true;
}

sf::Texture& ResourceManager::getTexture(const std::string& name) {
    return textures.at(name);
}

// Звуковые буферы
bool ResourceManager::loadSoundBuffer(const std::string& name, const std::string& filename) {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filename)) {
        std::cerr << "Не удалось загрузить звук: " << filename << "\n";
        return false;
    }
    soundBuffers[name] = buffer;
    return true;
}

sf::SoundBuffer& ResourceManager::getSoundBuffer(const std::string& name) {
    return soundBuffers.at(name);
}

// Шрифты
bool ResourceManager::loadFont(const std::string& name, const std::string& filename) {
    sf::Font font;
    if (!font.loadFromFile(filename)) {
        std::cerr << "Не удалось загрузить шрифт: " << filename << "\n";
        return false;
    }
    fonts[name] = font;
    return true;
}

sf::Font& ResourceManager::getFont(const std::string& name) {
    return fonts.at(name);
}
