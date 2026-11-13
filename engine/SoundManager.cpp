#include "SoundManager.h"

namespace ECSEngine {

/**
 * Register a sound file with the sound manager.
 * @param soundPath The path to the sound file.
 * @param soundName The name of the sound.
 */
void SoundManager::RegisterSound(const std::string& soundPath, const std::string& soundName) {
    RegisterSound(soundPath, soundName, 100.0f);
}

/**
 * Register a sound file with the sound manager.
 * @param soundPath The path to the sound file.
 * @param soundName The name of the sound.
 * @param volume The volume of the sound.
 */
void SoundManager::RegisterSound(const std::string& soundPath, const std::string& soundName, float volume) {
    if (mSoundBuffers.contains(soundName)) {
        std::cerr << "Warning: Overwriting sound file: " << soundName << std::endl;
    } else {
        mSoundBuffers[soundName] = sf::SoundBuffer();
    }
    
    if (!mSoundBuffers[soundName].loadFromFile(soundPath)) {
        std::cerr << "Warning: Failed to load sound file: " << soundPath << std::endl;
    }
    mSoundVolumes[soundName] = volume;
}

/**
 * Get a sound from the sound manager.
 * @param soundName The name of the sound.
 * @return The sound.
 */
sf::Sound& SoundManager::GetSound(const std::string& soundName) {
    static sf::SoundBuffer dummyBuffer;
    static sf::Sound dummySound(dummyBuffer);
    if (!mSoundBuffers.contains(soundName)) {
        std::cerr << "Warning: Sound file not found: " << soundName << std::endl;
        return dummySound;
    }
    dummySound.setBuffer(mSoundBuffers.at(soundName));
    dummySound.setVolume(mSoundVolumes.at(soundName));
    return dummySound;
}

/**
 * Set the volume of a sound.
 * @param soundName The name of the sound.
 * @param volume The volume of the sound.
 */
void SoundManager::SetSoundVolume(const std::string& soundName, float volume) {
    mSoundVolumes[soundName] = volume;
}

/**
 * Get the volume of a sound.
 * @param soundName The name of the sound.
 * @return The volume of the sound.
 */
float SoundManager::GetSoundVolume(const std::string& soundName) const {
    return mSoundVolumes.at(soundName);
}

/**
 * Register a music file with the sound manager.
 * @param musicPath The path to the music file.
 * @param musicName The name of the music.
 */
void SoundManager::RegisterMusic(const std::string& musicPath, const std::string& musicName) {
    RegisterMusic(musicPath, musicName, 100.0f);
}

/**
 * Register a music file with the sound manager.
 * @param musicPath The path to the music file.
 * @param musicName The name of the music.
 * @param volume The volume of the music.
 */
void SoundManager::RegisterMusic(const std::string& musicPath, const std::string& musicName, float volume) {
    mMusicPaths[musicName] = musicPath;
    mMusicVolumes[musicName] = volume;
}

/**
 * Get a music from the sound manager.
 * @param musicName The name of the music.
 * @return The music.
 */
sf::Music& SoundManager::GetMusic(const std::string& musicName) {
    static sf::Music dummyMusic;
    if (!dummyMusic.openFromFile(mMusicPaths[musicName])) {
        std::cerr << "Failed to load music file: " << mMusicPaths[musicName] << std::endl;
    }
    dummyMusic.setVolume(mMusicVolumes[musicName]);
    return dummyMusic;
}

/**
 * Set the volume of a music.
 * Note: This will not change the volume of the music playing.
 * @param musicName The name of the music.
 * @param volume The volume of the music.
 */
void SoundManager::SetMusicVolume(const std::string& musicName, float volume) {
    mMusicVolumes[musicName] = volume;
}

/**
 * Get the volume of a music.
 * Note: This will not get the volume of the music playing but only the volume in the buffer.
 * @param musicName The name of the music.
 * @return The volume of the music.
 */
float SoundManager::GetMusicVolume(const std::string& musicName) const {
    return mMusicVolumes.at(musicName);
}

}  // namespace ECSEngine
