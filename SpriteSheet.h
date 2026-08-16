#pragma once

#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <SDL.h>
#include <string>
#include <vector>

class SpriteSheet {
public:
    SpriteSheet(SDL_Renderer* renderer, const std::string& filePath, int spriteWidth, int spriteHeight);
    ~SpriteSheet();

    void RenderSprite(int index, int x, int y);

private:
    struct Sprite {
        int x;
        int y;
        int width;
        int height;
    };

    SDL_Renderer* mRenderer;           // SDLレンダラー
    SDL_Texture* mTexture;             // スプライトシート用テクスチャ
    std::vector<Sprite> mSprites;      // スプライト情報
    int mSpriteWidth;                  // 各スプライトの幅
    int mSpriteHeight;                 // 各スプライトの高さ

    void LoadSprites();
};

#endif

