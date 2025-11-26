#include "Scenes/MenuScene/MenuCanvas.hpp"
#include "Scenes/GameplayScene/GameplayScene.hpp"
#include "Engine/UI/Button/Button.hpp"
#include "Engine/UI/Slider/Slider.hpp"
#include "Engine/WindowManager/WindowManager.hpp"
#include "Engine/UI/Callbacks.hpp"
#include "Engine/Scenes/SceneManager/SceneManager.hpp"
#include "Engine/UI/UISprite/UISprite.hpp"
#include "Engine/UI/UIText/UIText.hpp"
#include "SaveDefines.hpp"
#include "Scenes/MenuScene/MenuScene.hpp"
#include "Scenes/MenuScene/UnlockButton.hpp"
#include "Engine/AudioManager/AudioManager.hpp"

MenuCanvas::MenuCanvas()
{
}

// probably won't change it now,
// but a good thing I could have done would be to create a function to init buttons states,
// because it's a bit duplicated with code inside callbacks
void MenuCanvas::Init()
{
    MenuScene *ptr = dynamic_cast<MenuScene *>(SceneManager::GetCurrentScene().get());

    // score
    Sprite scoreSprite = {"star", ml::vec2(1, 1), ml::vec2(0, 0), ml::vec2(50, 50)};
    AddComponent(std::make_unique<UI::UISprite>(scoreSprite, scoreSprite.size / 2));
    nbStarText = AddComponent(std::make_unique<UI::UIText>(std::to_string(ptr->GetNbStars()), "arial", scoreSprite.size, TextRenderer::TextAlign::LEFT));

    // play button
    playButtonSprite = {"rectangle_button_depth_flat", ml::vec2(1, 1), ml::vec2(0, 0), ml::vec2(300, 100)};
    playButton = AddComponent(std::make_unique<UI::SpriteButton>(playButtonSprite, "Play", "arial", ml::vec2(WindowManager::GetWindowWidth() / 2, WindowManager::GetWindowHeight() * 0.75), [this](const UI::CallbackData &data) { PlayButtonCallback(data); }));
    Character &character = ptr->GetCharacterSelect();
    unlockButton = AddComponent(std::make_unique<UnlockButton>("Unlock : " + std::to_string(character.prize), "arial", ml::vec2(WindowManager::GetWindowWidth() / 2, WindowManager::GetWindowHeight() * 0.75), ml::vec2(300, 100), [this](const UI::CallbackData &data) { UnlockButtonCallback(data); }));
    if (character.unlock)
        GetComponent(unlockButton)->Hide();
    else
    {
        GetComponent(playButton)->Hide();
        if (ptr->GetNbStars() < character.prize)
        {
            auto button = dynamic_cast<UnlockButton *>(GetComponent(unlockButton).get());
            button->Disable();
        }
    }

    // arrow button
    Sprite arrowSprite = {"arrow_basic_w", ml::vec2(1, 1), ml::vec2(0, 0), ml::vec2(50, 50)};
    leftArrow = AddComponent(std::make_unique<UI::SpriteButton>(arrowSprite, "", "", ml::vec2((WindowManager::GetWindowWidth() - playButtonSprite.size.x - arrowSprite.size.x) / 2, WindowManager::GetWindowHeight() * 0.75 - playButtonSprite.size.y), [this](const UI::CallbackData &data) { ArrowButtonCallback(data, true); }));
    arrowSprite.textureName = "arrow_basic_e";
    rightArrow = AddComponent(std::make_unique<UI::SpriteButton>(arrowSprite, "", "", ml::vec2((WindowManager::GetWindowWidth() + playButtonSprite.size.x + arrowSprite.size.x) / 2, WindowManager::GetWindowHeight() * 0.75 - playButtonSprite.size.y), [this](const UI::CallbackData &data) { ArrowButtonCallback(data, false); }));

    // black background
    Sprite backgroundSprite = {"white_texture", ml::vec2(1, 1), ml::vec2(0, 0), WindowManager::GetWindowSize()};
    settingsBackground = AddComponent(std::make_unique<UI::UISprite>(backgroundSprite, backgroundSprite.size / 2, ml::vec4(0, 0, 0, 0.5)));
    GetComponent(settingsBackground)->Hide();

    // settings button
    Sprite settingsSprite = {"settings", ml::vec2(1, 1), ml::vec2(0, 0), ml::vec2(50, 50)};
    settingsButton = AddComponent(std::make_unique<UI::SpriteButton>(settingsSprite, "", "", ml::vec2(WindowManager::GetWindowWidth() - settingsSprite.size.x / 2, settingsSprite.size.y / 2), [this](const UI::CallbackData &data) { SettingsButtonCallback(data); }));
    settingsOpen = false;

    // slider
    ml::vec2 sliderSpriteSize = ml::vec2(32, 32);
    ml::vec2 sliderSize = ml::vec2(200, 50);
    Sprite boundary = {"slider_horizontal", ml::vec2(6, 1), ml::vec2(0, 0), sliderSpriteSize};
    Sprite pipe = {"slider_horizontal", ml::vec2(6, 1), ml::vec2(1, 0), sliderSpriteSize};
    Sprite thumb = {"slider_thumb", ml::vec2(1, 1), ml::vec2(0, 0), ml::vec2(24, 32)};
    soundSlider = AddComponent(std::make_unique<UI::FloatSlider>(boundary, pipe, thumb, 0, 2, ptr->GetSoundVolume(), WindowManager::GetWindowSize() / 2, sliderSize, "arial", [this](const UI::CallbackData &data) { SoundSliderCallback(data); }));
    GetComponent(soundSlider)->Hide();

    // slider text
    soundSliderText = AddComponent(std::make_unique<UI::UIText>("Sound", "arial", (WindowManager::GetWindowSize() - ml::vec2(0, sliderSize.y)) / 2, TextRenderer::TextAlign::CENTER));
    GetComponent(soundSliderText)->Hide();
}

MenuCanvas::~MenuCanvas()
{
}

void MenuCanvas::PlayButtonCallback(const UI::CallbackData &data)
{
    if (settingsOpen)
        return;

    switch (data.callback)
    {
    case UI::EngineCallbacks::CURSEUR_ON: {
        auto button = dynamic_cast<UI::SpriteButton *>(GetComponent(playButton).get());
        playButtonSprite.textureName = "rectangle_button_flat";
        button->UpdateSprite(playButtonSprite);
        break;
    }
    case UI::EngineCallbacks::CURSEUR_OFF: {
        auto button = dynamic_cast<UI::SpriteButton *>(GetComponent(playButton).get());
        playButtonSprite.textureName = "rectangle_button_depth_flat";
        button->UpdateSprite(playButtonSprite);
        break;
    }
    case UI::EngineCallbacks::CLICK_OFF: {
        MenuScene *ptr = dynamic_cast<MenuScene *>(SceneManager::GetCurrentScene().get());
        SceneManager::SwitchScene(std::make_unique<GameplayScene>(ptr->GetCharacterSelect().path));
        AudioManager::Play("assets/sounds/click-b.mp3");
        break;
    }
    default:
        break;
    }
}

void MenuCanvas::UnlockButtonCallback(const UI::CallbackData &data)
{
    if (settingsOpen)
        return;

    switch (data.callback)
    {
    case UI::EngineCallbacks::CURSEUR_ON: {
        auto button = dynamic_cast<UnlockButton *>(GetComponent(unlockButton).get());
        button->CurseurOn();
        break;
    }
    case UI::EngineCallbacks::CURSEUR_OFF: {
        auto button = dynamic_cast<UnlockButton *>(GetComponent(unlockButton).get());
        button->CurseurOff();
        break;
    }
    case UI::EngineCallbacks::CLICK_OFF: {
        MenuScene *ptr = dynamic_cast<MenuScene *>(SceneManager::GetCurrentScene().get());
        Character &characterSelect = ptr->GetCharacterSelect();
        int &nbStars = ptr->GetNbStars();
        if (characterSelect.prize < nbStars)
        {
            nbStars -= characterSelect.prize;
            UI::UIText *ptrText = dynamic_cast<UI::UIText *>(GetComponent(nbStarText).get());
            ptrText->UpdateText(std::to_string(nbStars));
            characterSelect.unlock = true;
            GetComponent(playButton)->Show();
            GetComponent(unlockButton)->Hide();
        }
        AudioManager::Play("assets/sounds/click-b.mp3");
        break;
    }
    default:
        break;
    }
}

void MenuCanvas::ArrowButtonCallback(const UI::CallbackData &data, bool leftArrow)
{
    if (settingsOpen)
        return;

    switch (data.callback)
    {
    case UI::EngineCallbacks::CLICK_OFF: {
        MenuScene *ptr = dynamic_cast<MenuScene *>(SceneManager::GetCurrentScene().get());
        int nbOffset = (leftArrow) ? -1 : 1;
        ptr->UpdateCharacterSelect(nbOffset);

        Character &character = ptr->GetCharacterSelect();
        if (character.unlock)
        {
            GetComponent(playButton)->Show();
            GetComponent(unlockButton)->Hide();
        }
        else
        {
            GetComponent(playButton)->Hide();
            UnlockButton *ptrButton = dynamic_cast<UnlockButton *>(GetComponent(unlockButton).get());
            ptrButton->Show();
            ptrButton->UpdateText("Unlock : " + std::to_string(character.prize));
            if (ptr->GetNbStars() < character.prize)
                ptrButton->Disable();
            else
                ptrButton->Enable();
        }
        AudioManager::Play("assets/sounds/click-b.mp3");
        break;
    }
    default:
        break;
    }
}

void MenuCanvas::SoundSliderCallback(const UI::CallbackData &data)
{
    switch (data.callback)
    {
    case UI::EngineCallbacks::UPDATE_VALUE: {
        UI::FloatSlider::UpdateValueEventData dataCast = dynamic_cast<const UI::FloatSlider::UpdateValueEventData &>(data);
        AudioManager::SetVolume(dataCast.newValue);
        MenuScene *ptr = dynamic_cast<MenuScene *>(SceneManager::GetCurrentScene().get());
        ptr->SetSoundVolume(dataCast.newValue);
        break;
    }
    default:
        break;
    }
}

void MenuCanvas::SettingsButtonCallback(const UI::CallbackData &data)
{
    switch (data.callback)
    {
    case UI::EngineCallbacks::CLICK_OFF: {
        settingsOpen = !settingsOpen;
        if (settingsOpen)
        {
            GetComponent(soundSlider)->Show();
            GetComponent(settingsBackground)->Show();
            GetComponent(soundSliderText)->Show();
        }
        else
        {
            GetComponent(soundSlider)->Hide();
            GetComponent(settingsBackground)->Hide();
            GetComponent(soundSliderText)->Hide();
        }
        AudioManager::Play("assets/sounds/click-b.mp3");
        break;
    }
    default:
        break;
    }
}