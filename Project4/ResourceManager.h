// ResourceManager.h
#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>

class ResourceManager {
public:
    static ResourceManager& getInstance();

    // Удаляем копирующие конструкторы и операторы присваивания
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // Методы загрузки и получения текстур
    bool loadTexture(const std::string& name, const std::string& filename);
    sf::Texture& getTexture(const std::string& name);

    // Методы загрузки и получения звуковых буферов
    bool loadSoundBuffer(const std::string& name, const std::string& filename);
    sf::SoundBuffer& getSoundBuffer(const std::string& name);

    // Методы загрузки и получения шрифтов
    bool loadFont(const std::string& name, const std::string& filename);
    sf::Font& getFont(const std::string& name);

private:
    ResourceManager() {} // Закрытый конструктор для Singleton

    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;
    std::unordered_map<std::string, sf::Font> fonts;
};

#endif // RESOURCEMANAGER_H
