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

    // ������� ���������� ������������ � ��������� ������������
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // ������ �������� � ��������� �������
    bool loadTexture(const std::string& name, const std::string& filename);
    sf::Texture& getTexture(const std::string& name);

    // ������ �������� � ��������� �������� �������
    bool loadSoundBuffer(const std::string& name, const std::string& filename);
    sf::SoundBuffer& getSoundBuffer(const std::string& name);

    // ������ �������� � ��������� �������
    bool loadFont(const std::string& name, const std::string& filename);
    sf::Font& getFont(const std::string& name);

private:
    ResourceManager() {} // �������� ����������� ��� Singleton

    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;
    std::unordered_map<std::string, sf::Font> fonts;
};

#endif // RESOURCEMANAGER_H
