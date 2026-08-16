#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "Component.h"
#include "UIButton.h"
#include "Math.h"

struct ScrollItem {
    SDL_Texture* icon = nullptr;
    SDL_Texture* nameTexture = nullptr;
    SDL_Texture* priceTexture = nullptr;
    SDL_Texture* descriptionTexture = nullptr;
    SDL_Texture* textTexture = nullptr;
    SDL_Rect iconRect;
    SDL_Rect nameRect;
    SDL_Rect buttonRect;
    std::string name;
    std::string description;
    int price = 0;
    std::function<void()> onBuy;
    std::unique_ptr<UIButton> buyButton;
    ~ScrollItem() {
        if (nameTexture) {
            SDL_DestroyTexture(nameTexture);
            nameTexture = nullptr;
        }
        if (priceTexture) {
            SDL_DestroyTexture(priceTexture);
            priceTexture = nullptr;
        }
        if (descriptionTexture) {
            SDL_DestroyTexture(descriptionTexture);
            descriptionTexture = nullptr;
        }
        if (textTexture) {
            SDL_DestroyTexture(textTexture);
            textTexture = nullptr;
        }
    }
};

class ScrollItemComponent : public Component {
public:
    ScrollItemComponent(class Actor* owner, SDL_Renderer* renderer, TTF_Font* font, int updateOrder = 200);
    ~ScrollItemComponent();
    void AddItem(SDL_Texture* texture, const std::string& name, std::function<void()> onClick, const std::string& buttonText, int price, const std::string& description = "");
    void AddItem(SDL_Texture* icon, const std::string& name, std::function<void()> onClick, const std::string& buttonLabel, const std::string& description = "");
    void RemoveItem(const std::string& name);
    void SetViewport(const SDL_Rect& rect);
    void SetItemHeight(int height);
    void SetItemSize(int width, int height);
    SDL_Rect& GetViewport() { return mViewport; };
    void Draw(SDL_Renderer* renderer, const Vector2& cameraPos) override;
    void ProcessEvent(const SDL_Event& event) override;
    void UpdateButtonText(const std::string& itemName, const std::string& newText);
    void ResetAllButtonTexts(const std::string& defaultText);
    void ClearItems();
    void SortItemsByPrice();
private:
    std::vector<std::unique_ptr<ScrollItem>> mItems;
    SDL_Rect mViewport;
    int mItemHeight = 64;
    int mItemWidth = 128;
    int mScrollOffset = 0;
    SDL_Renderer* mRenderer;
    TTF_Font* mFont;
    TTF_Font* mSmallFont = nullptr;
};
