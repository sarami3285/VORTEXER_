#include "AudioComponent.h"
#include <SDL_log.h>

AudioComponent::AudioComponent(Actor* owner)
    : Component(owner)
    , mMusic(nullptr)
{
}

AudioComponent::~AudioComponent()
{
    for (auto& pair : mChunks)
    {
        Mix_FreeChunk(pair.second);
    }
    mChunks.clear();

    if (mMusic)
    {
        Mix_FreeMusic(mMusic);
        mMusic = nullptr;
    }
}

bool AudioComponent::LoadSE(const std::string& name, const std::string& filepath)
{
    Mix_Chunk* chunk = Mix_LoadWAV(filepath.c_str());
    if (!chunk)
    {
        SDL_Log("Failed to load SE: %s (%s)", filepath.c_str(), Mix_GetError());
        return false;
    }
    auto existing = mChunks.find(name);
    if (existing != mChunks.end()) {
        Mix_FreeChunk(existing->second);
        existing->second = chunk;
    }
    else {
        mChunks.emplace(name, chunk);
    }
    return true;
}

void AudioComponent::PlaySE(const std::string& name)
{
    auto iter = mChunks.find(name);
    if (iter != mChunks.end())
    {
        Mix_PlayChannel(-1, iter->second, 0);
    }
}

void AudioComponent::SetSEVolume(const std::string& name, int volume)
{
    auto iter = mChunks.find(name);
    if (iter != mChunks.end())
    {
        Mix_VolumeChunk(iter->second, volume);
    }
}

bool AudioComponent::LoadBGM(const std::string& filepath)
{
    Mix_Music* music = Mix_LoadMUS(filepath.c_str());
    if (!music)
    {
        SDL_Log("Failed to load BGM: %s (%s)", filepath.c_str(), Mix_GetError());
        return false;
    }

    if (mMusic) {
        Mix_HaltMusic();
        Mix_FreeMusic(mMusic);
    }
    mMusic = music;
    return true;
}

void AudioComponent::PlayBGM(bool loop)
{
    if (mMusic)
    {
        Mix_PlayMusic(mMusic, loop ? -1 : 1);
    }
}

void AudioComponent::StopBGM()
{
    Mix_HaltMusic();
}
 
void AudioComponent::SetBGMVolume(int volume)
{
    Mix_VolumeMusic(volume);
}


