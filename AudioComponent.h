// AudioComponent.h
#pragma once
#include "Component.h"
#include <string>
#include <unordered_map>
#include <SDL_mixer.h>

class AudioComponent : public Component
{
public:
    AudioComponent(class Actor* owner);
    ~AudioComponent();

    // --- SE関連 ---
    bool LoadSE(const std::string& name, const std::string& filepath);
    void PlaySE(const std::string& name);
    void SetSEVolume(const std::string& name, int volume);

    // --- BGM関連 ---
    bool LoadBGM(const std::string& filepath);
    void PlayBGM(bool loop = true);
    void StopBGM();
    void SetBGMVolume(int volume);

private:
    std::unordered_map<std::string, Mix_Chunk*> mChunks; // SE用
    Mix_Music* mMusic; // BGM用
};




