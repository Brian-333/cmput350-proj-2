#pragma once

#include <unordered_map>
#include <iostream>

#include <SFML/Audio.hpp>

namespace ECSEngine {

/**
 * Loads and owns sound effects and music, looked up by name.
 */
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
    sf::SoundBuffer mFallbackBuffer;
    // Sounds are owned here, not as function statics, so they are destroyed with the engine
    // before SFML's audio device. They must be declared after the buffers they reference.
    std::unordered_map<std::string, sf::Sound> mSounds;
    sf::Sound mFallbackSound{mFallbackBuffer};
    std::unordered_map<std::string, float> mMusicVolumes;
    std::unordered_map<std::string, std::string> mMusicPaths;
    sf::Music mMusic;
};

}  // namespace ECSEngine
