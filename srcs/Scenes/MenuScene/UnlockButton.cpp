#include "Scenes/MenuScene/UnlockButton.hpp"
#include "Engine/2D/Renderers/TextRenderer/TextRenderer.hpp"
#include "Engine/2D/Renderers/SpriteRenderer/SpriteRenderer.hpp"

UnlockButton::UnlockButton(const std::string &text, const std::string &font, const ml::vec2 &pos, const ml::vec2 &size) : Button(text, font, pos, size), background({"rectangle_button_depth_flat", ml::vec2(1, 1), ml::vec2(0, 0), size}), star({"star", ml::vec2(1, 1), ml::vec2(0, 0), ml::vec2(size.y / 2, size.y / 2)})
{
}

UnlockButton::~UnlockButton()
{
}

void UnlockButton::Draw()
{
    SpriteRenderer::Draw(SpriteRenderDataBuilder().SetPosition(pos).SetSprite(background).SetSize(background.size).SetDrawAbsolute(true).Build());
    SpriteRenderer::Draw(SpriteRenderDataBuilder().SetPosition(ml::vec2(pos.x + (size.x * 0.9 - star.size.x) / 2, pos.y)).SetSprite(star).SetSize(star.size).SetDrawAbsolute(true).Build());
    TextRenderer::Draw(text, font, pos.x - (size.x * 0.9) / 2, pos.y, 0.75, ml::vec4(1, 1, 1, 1), TextRenderer::TextAlign::LEFT);
}

void UnlockButton::CurseurOn()
{
    background.textureName = "rectangle_button_flat";
}

void UnlockButton::CurseurOff()
{
    background.textureName = "rectangle_button_depth_flat";
}