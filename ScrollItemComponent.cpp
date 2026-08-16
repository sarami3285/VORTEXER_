#include "ScrollItemComponent.h"
#include <algorithm>
#include <SDL_ttf.h>
#include "UIButton.h"

ScrollItemComponent::ScrollItemComponent(Actor* owner, SDL_Renderer* renderer, TTF_Font* font, int updateOrder)
    : Component(owner, updateOrder)
    , mRenderer(renderer)
    , mFont(font) {
    mSmallFont = TTF_OpenFont("Assets/NSJP.ttf", 16);
}

ScrollItemComponent::~ScrollItemComponent() {
    if (mSmallFont) {
        TTF_CloseFont(mSmallFont);
        mSmallFont = nullptr;
    }
}

void ScrollItemComponent::SetViewport(const SDL_Rect& rect) {
    mViewport = rect;
}

void ScrollItemComponent::SetItemSize(int width, int height) {
    mItemWidth = width;
    mItemHeight = height;
}

void ScrollItemComponent::ClearItems() {
    mItems.clear();
    mScrollOffset = 0;
}

void ScrollItemComponent::AddItem(SDL_Texture* texture, const std::string& name, std::function<void()> onClick, const std::string& buttonText, int price, const std::string& description) {
    auto newItem = std::make_unique<ScrollItem>();
    newItem->icon = texture;
    newItem->name = name;
    newItem->description = description;
    newItem->price = price;
    newItem->onBuy = onClick;

    newItem->buttonRect.w = 150;
    newItem->buttonRect.h = (mItemHeight / 2) - 10;
    newItem->buyButton = std::make_unique<UIButton>(0, 0, newItem->buttonRect.w, newItem->buttonRect.h, buttonText, mRenderer, mFont);

    SDL_Color white = { 255, 255, 255, 255 };
    if (mFont) {
        SDL_Surface* nameSurf = TTF_RenderUTF8_Blended(mFont, name.c_str(), white);
        if (nameSurf) {
            newItem->nameTexture = SDL_CreateTextureFromSurface(mRenderer, nameSurf);
            SDL_FreeSurface(nameSurf);
        }

        std::string priceStr = std::to_string(price);
        SDL_Surface* priceSurf = TTF_RenderUTF8_Blended(mFont, priceStr.c_str(), white);
        if (priceSurf) {
            newItem->priceTexture = SDL_CreateTextureFromSurface(mRenderer, priceSurf);
            SDL_FreeSurface(priceSurf);
        }

        TTF_Font* descFont = mSmallFont ? mSmallFont : mFont;

        if (descFont) {
            int wrapWidth = mViewport.w - 150;
            SDL_Surface* descSurf = TTF_RenderUTF8_Blended_Wrapped(descFont, description.c_str(), white, wrapWidth);

            if (descSurf) {
                newItem->descriptionTexture = SDL_CreateTextureFromSurface(mRenderer, descSurf);
                SDL_FreeSurface(descSurf);
            }
        }
    }
    mItems.push_back(std::move(newItem));
}

void ScrollItemComponent::AddItem(SDL_Texture* icon, const std::string& name, std::function<void()> onClick, const std::string& buttonLabel, const std::string& description) {
    auto newItem = std::make_unique<ScrollItem>();
    newItem->icon = icon;
    newItem->name = name;
    newItem->onBuy = onClick;
    int buttonHeight = mItemHeight - 10;
    newItem->description = description;

    newItem->buttonRect.w = 150;
    newItem->buttonRect.h = buttonHeight;
    newItem->buyButton = std::make_unique<UIButton>(0, 0, newItem->buttonRect.w, newItem->buttonRect.h, buttonLabel, mRenderer, mFont);

    SDL_Color white = { 255, 255, 255, 255 };
    if (mFont) {
        SDL_Surface* nameSurf = TTF_RenderUTF8_Blended(mFont, name.c_str(), white);
        if (nameSurf) {
            newItem->nameTexture = SDL_CreateTextureFromSurface(mRenderer, nameSurf);
            SDL_FreeSurface(nameSurf);
        }

        TTF_Font* descFont = mSmallFont ? mSmallFont : mFont;
        if (descFont) {
            int wrapWidth = mViewport.w - 150;
            SDL_Surface* descSurf = TTF_RenderUTF8_Blended_Wrapped(descFont, description.c_str(), white, wrapWidth);

            if (descSurf) {
                newItem->descriptionTexture = SDL_CreateTextureFromSurface(mRenderer, descSurf);
                SDL_FreeSurface(descSurf);
            }
        }
    }
    mItems.push_back(std::move(newItem));
}

void ScrollItemComponent::RemoveItem(const std::string& name) {}
void ScrollItemComponent::SetItemHeight(int height) { mItemHeight = height; }
void ScrollItemComponent::UpdateButtonText(const std::string& itemName, const std::string& newText) {
    for (auto& item : mItems) {
        if (item->name == itemName) {
            if (item->buyButton) {
                item->buyButton->SetText(newText);
            }
            break;
        }
    }
}
void ScrollItemComponent::ResetAllButtonTexts(const std::string& defaultText) {}

void ScrollItemComponent::Draw(SDL_Renderer* renderer, const Vector2& cameraPos) {
    SDL_RenderSetClipRect(renderer, &mViewport);

    for (size_t i = 0; i < mItems.size(); ++i) {
        ScrollItem& item = *mItems[i];
        int y = mViewport.y + static_cast<int>(i) * mItemHeight - mScrollOffset;
        if (y + mItemHeight < mViewport.y || y > mViewport.y + mViewport.h)
            continue;

        int iconHeight = (mItemHeight / 2) - 4;
        int iconWidth = iconHeight * 2;

        item.iconRect.x = mViewport.x + 5;
        item.iconRect.y = y + 5;
        item.iconRect.h = iconHeight;
        item.iconRect.w = iconWidth;

        if (item.icon) {
            SDL_RenderCopy(renderer, item.icon, nullptr, &item.iconRect);
        }

        int startX = item.iconRect.x + item.iconRect.w + 10;

        int row1CenterY = y + 5 + (iconHeight / 2);

        int nameTextW = 0, nameTextH = 0;
        if (item.nameTexture) SDL_QueryTexture(item.nameTexture, nullptr, nullptr, &nameTextW, &nameTextH);

        SDL_Rect nameTextRect = {
            startX,
            row1CenterY - (nameTextH / 2),
            nameTextW,
            nameTextH
        };
        if (item.nameTexture) { SDL_RenderCopy(renderer, item.nameTexture, nullptr, &nameTextRect); }

        if (item.priceTexture) {
            int priceTextW = 0, priceTextH = 0;
            SDL_QueryTexture(item.priceTexture, nullptr, nullptr, &priceTextW, &priceTextH);

            SDL_Rect priceTextRect = {
                nameTextRect.x + nameTextW + 10,
                row1CenterY - (priceTextH / 2),
                priceTextW,
                priceTextH
            };
            SDL_RenderCopy(renderer, item.priceTexture, nullptr, &priceTextRect);
        }

        item.buttonRect.x = mViewport.x + mViewport.w - item.buttonRect.w - 5;
        if (item.buyButton) {
            item.buyButton->SetRect(item.buttonRect.x, row1CenterY - (item.buttonRect.h / 2), item.buttonRect.w, item.buttonRect.h);
            item.buyButton->Draw(renderer);
        }

        if (item.descriptionTexture) {
            int descTextW = 0, descTextH = 0;
            SDL_QueryTexture(item.descriptionTexture, nullptr, nullptr, &descTextW, &descTextH);

            SDL_Rect descTextRect = {
                item.iconRect.x,
                y + mItemHeight / 2 + 5,
                descTextW,
                descTextH
            };

            SDL_RenderCopy(renderer, item.descriptionTexture, nullptr, &descTextRect);
        }

        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderDrawLine(renderer, mViewport.x, y + mItemHeight, mViewport.x + mViewport.w, y + mItemHeight);
    }
    SDL_RenderSetClipRect(renderer, nullptr);
}

void ScrollItemComponent::ProcessEvent(const SDL_Event& event) {
    if (event.type == SDL_MOUSEWHEEL) {
        if (event.wheel.y > 0) {
            mScrollOffset -= mItemHeight;
        }
        else if (event.wheel.y < 0) {
            mScrollOffset += mItemHeight;
        }
        if (mScrollOffset < 0) mScrollOffset = 0;

        if (static_cast<int>(mItems.size()) * mItemHeight > mViewport.h) {
            int maxScroll = static_cast<int>(mItems.size()) * mItemHeight - mViewport.h;
            if (mScrollOffset > maxScroll) mScrollOffset = maxScroll;
        }
        else {
            mScrollOffset = 0;
        }
    }

    for (size_t i = 0; i < mItems.size(); ++i) {
        ScrollItem& item = *mItems[i];

        int y_item_top = mViewport.y + static_cast<int>(i) * mItemHeight - mScrollOffset;
        if (y_item_top + mItemHeight < mViewport.y || y_item_top > mViewport.y + mViewport.h)
            continue;

        if (item.buyButton) {
            int iconHeight = (mItemHeight / 2) - 4;
            int row1CenterY = y_item_top + 5 + (iconHeight / 2);
            int buttonY = row1CenterY - (item.buttonRect.h / 2);
            int buttonX = mViewport.x + mViewport.w - item.buttonRect.w - 5;
            item.buyButton->SetRect(buttonX, buttonY, item.buttonRect.w, item.buttonRect.h);
            item.buyButton->HandleEvent(event);

            if (item.buyButton->IsClicked()) {
                if (item.onBuy) {
                    item.onBuy();
                }
            }
        }
    }
}

void ScrollItemComponent::SortItemsByPrice() {
    std::sort(mItems.begin(), mItems.end(), [](const std::unique_ptr<ScrollItem>& a, const std::unique_ptr<ScrollItem>& b) {
        return a->price < b->price;
        });
}
