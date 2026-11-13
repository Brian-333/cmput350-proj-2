#pragma once

#include <unordered_map>
#include <iostream>

#include <SFML/Audio.hpp>

namespace ECSEngine {

class SoundManager {
public:
    SoundManager() = default;
    ~SoundManager() = default;
    void RegisterSound(const std::string& soundPath, const std::string& soundName);
    void RegisterSound(const std::string& soundPath, const std::string& soundName, float volume);
    sf::Sound& GetSound(const std::string& soundName);
    void SetSoundVolume(const std::string& soundName, float volume);
    float GetSoundVolume(const std::string& soundName) const;

    void RegisterMusic(const std::string& musicPath, const std::string& musicName);
    void RegisterMusic(const std::string& musicPath, const std::string& musicName, float volume);
    void SetMusicVolume(const std::string& musicName, float volume);
    float GetMusicVolume(const std::string& musicName) const;
    sf::Music& GetMusic(const std::string& musicName);

private:
    std::unordered_map<std::string, float> mSoundVolumes;
    std::unordered_map<std::string, sf::SoundBuffer> mSoundBuffers;
    // const int mMaxSounds = 15;
    // int mNextSoundIndex;
    // std::vector<sf::Sound> mSounds;
    std::unordered_map<std::string, float> mMusicVolumes;
    std::unordered_map<std::string, std::string> mMusicPaths;
};

}  // namespace ECSEngine
