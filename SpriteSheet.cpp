#include "SpriteSheet.h"
#include <iostream>

SpriteSheet::SpriteSheet(SDL_Renderer* renderer, const std::string& filePath, int spriteWidth, int spriteHeight)
    : mRenderer(renderer), mSpriteWidth(spriteWidth), mSpriteHeight(spriteHeight)
{
    SDL_Surface* tempSurface = SDL_LoadBMP(filePath.c_str());
    if (!tempSurface) {
        std::cerr << "Failed to load sprite sheet: " << SDL_GetError() << std::endl;
        return;
    }

    mTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    if (!mTexture) {
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
        return;
    }

    LoadSprites();
}

SpriteSheet::~SpriteSheet() {
    if (mTexture) {
        SDL_DestroyTexture(mTexture);
    }
}

void SpriteSheet::LoadSprites() {
    int sheetWidth, sheetHeight;
    SDL_QueryTexture(mTexture, NULL, NULL, &sheetWidth, &sheetHeight);
    int cols = sheetWidth / mSpriteWidth;  
    int rows = sheetHeight / mSpriteHeight; 

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            mSprites.push_back(Sprite{
                col * mSpriteWidth,  // X座標
                row * mSpriteHeight, // Y座標
                mSpriteWidth,        // 幅
                mSpriteHeight        // 高さ
                });
        }
    }
}

void SpriteSheet::RenderSprite(int index, int x, int y) {
    if (index < 0 || index >= mSprites.size()) {
        std::cerr << "Invalid sprite index!" << std::endl;
        return;
    }

    Sprite& sprite = mSprites[index];
    SDL_Rect srcRect = { sprite.x, sprite.y, sprite.width, sprite.height };
    SDL_Rect destRect = { x, y, sprite.width, sprite.height };

    SDL_RenderCopy(mRenderer, mTexture, &srcRect, &destRect);
}
