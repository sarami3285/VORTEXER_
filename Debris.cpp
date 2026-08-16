#include "Debris.h"
#include "SpriteComponent.h"

Debris::Debris(Game* game, std::string TexturePath)
    : Actor(game)
{
    sprite = new SpriteComponent(this, 1);
    sprite->SetTexture(game->GetTexture(TexturePath));
}
