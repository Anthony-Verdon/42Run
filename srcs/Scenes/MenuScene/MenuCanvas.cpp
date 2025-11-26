#include "Scenes/MenuScene/MenuCanvas.hpp"
#include "Scenes/GameplayScene/GameplayScene.hpp"
#include "Engine/UI/Button/Button.hpp"
#include "Engine/UI/Slider/Slider.hpp"
#include "Engine/WindowManager/WindowManager.hpp"
#include "Engine/UI/Events.hpp"
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
// because it's a bit duplicated with code inside events
void MenuCanvas::Init()
{
    MenuScene *ptr = dynamic_cast<MenuScene *>(SceneManager::GetCurrentScene().get());

    // score
    Sprite scoreSprite = {"star", ml::vec2(1, 1), ml::vec2(0, 0), ml::vec2(50, 50)};
    AddComponent(std::make_unique<UI::UISprite>(scoreSprite, scoreSprite.size / 2));
    nbStarText = AddComponent(std::make_unique<UI::UIText>(std::to_string(ptr->GetNbStars()), "arial", scoreSprite.size, TextRenderer::TextAlign::LEFT));

    // play button
    playButtonSprite = {"rectangle_button_depth_flat", ml::vec2(1, 1), ml::vec2(0, 0), ml::vec2(300, 100)};
    playButton = AddComponent(std::make_unique<UI::SpriteButton>(playButtonSprite, "Play", "arial", ml::vec2(WindowManager::GetWindowWidth() / 2, WindowManager::GetWindowHeight() * 0.75)));
    Character &character = ptr->GetCharacterSelect();
    unlockButton = AddComponent(std::make_unique<UnlockButton>("Unlock : " + std::to_string(character.prize), "arial", ml::vec2(WindowManager::GetWindowWidth() / 2, WindowManager::GetWindowHeight() * 0.75), ml::vec2(300, 100)));
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
    leftArrow = AddComponent(std::make_unique<UI::SpriteButton>(arrowSprite, "", "", ml::vec2((WindowManager::GetWindowWidth() - playButtonSprite.size.x - arrowSprite.size.x) / 2, WindowManager::GetWindowHeight() * 0.75 - playButtonSprite.size.y)));
    arrowSprite.textureName = "arrow_basic_e";
    rightArrow = AddComponent(std::make_unique<UI::SpriteButton>(arrowSprite, "", "", ml::vec2((WindowManager::GetWindowWidth() + playButtonSprite.size.x + arrowSprite.size.x) / 2, WindowManager::GetWindowHeight() * 0.75 - playButtonSprite.size.y)));

    // black background
    Sprite backgroundSprite = {"white_texture", ml::vec2(1, 1), ml::vec2(0, 0), WindowManager::GetWindowSize()};
    settingsBackground = AddComponent(std::make_unique<UI::UISprite>(backgroundSprite, backgroundSprite.size / 2, ml::vec4(0, 0, 0, 0.5)));
    GetComponent(settingsBackground)->Hide();

    // settings button
    Sprite settingsSprite = {"settings", ml::vec2(1, 1), ml::vec2(0, 0), ml::vec2(50, 50)};
    settingsButton = AddComponent(std::make_unique<UI::SpriteButton>(settingsSprite, "", "", ml::vec2(WindowManager::GetWindowWidth() - settingsSprite.size.x / 2, settingsSprite.size.y / 2)));
    settingsOpen = false;

    // slider
    ml::vec2 sliderSpriteSize = ml::vec2(32, 32);
    ml::vec2 sliderSize = ml::vec2(200, 50);
    Sprite boundary = {"slider_horizontal", ml::vec2(6, 1), ml::vec2(0, 0), sliderSpriteSize};
    Sprite pipe = {"slider_horizontal", ml::vec2(6, 1), ml::vec2(1, 0), sliderSpriteSize};
    Sprite thumb = {"slider_thumb", ml::vec2(1, 1), ml::vec2(0, 0), ml::vec2(24, 32)};
    soundSlider = AddComponent(std::make_unique<UI::FloatSlider>(boundary, pipe, thumb, 0, 2, 1, WindowManager::GetWindowSize() / 2, sliderSize, "arial"));
    GetComponent(soundSlider)->Hide();

    // slider text
    soundSliderText = AddComponent(std::make_unique<UI::UIText>("Sound", "arial", (WindowManager::GetWindowSize() - ml::vec2(0, sliderSize.y)) / 2, TextRenderer::TextAlign::CENTER));
    GetComponent(soundSliderText)->Hide();
}

MenuCanvas::~MenuCanvas()
{
}

void MenuCanvas::HandleEvents(UI::EventData &data)
{
    if (data.componentID == playButton && !settingsOpen)
    {
        // maybe it should be moved or handle inside the button but I don't know for the moment
        switch (data.event)
        {
        case UI::EngineEvents::CURSEUR_ON: {
            auto button = dynamic_cast<UI::SpriteButton *>(GetComponent(playButton).get());
            playButtonSprite.textureName = "rectangle_button_flat";
            button->UpdateSprite(playButtonSprite);
            break;
        }
        case UI::EngineEvents::CURSEUR_OFF: {
            auto button = dynamic_cast<UI::SpriteButton *>(GetComponent(playButton).get());
            playButtonSprite.textureName = "rectangle_button_depth_flat";
            button->UpdateSprite(playButtonSprite);
            break;
        }
        case UI::EngineEvents::CLICK_OFF: {
            MenuScene *ptr = dynamic_cast<MenuScene *>(SceneManager::GetCurrentScene().get());
            SceneManager::SwitchScene(std::make_unique<GameplayScene>(ptr->GetCharacterSelect().path));
            AudioManager::Play("assets/sounds/click-b.mp3");
            break;
        }
        default:
            break;
        }
    }
    else if (data.componentID == unlockButton && !settingsOpen)
    {
        // maybe it should be moved or handle inside the button but I don't know for the moment
        switch (data.event)
        {
        case UI::EngineEvents::CURSEUR_ON: {
            auto button = dynamic_cast<UnlockButton *>(GetComponent(unlockButton).get());
            button->CurseurOn();
            break;
        }
        case UI::EngineEvents::CURSEUR_OFF: {
            auto button = dynamic_cast<UnlockButton *>(GetComponent(unlockButton).get());
            button->CurseurOff();
            break;
        }
        case UI::EngineEvents::CLICK_OFF: {
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
    else if ((data.componentID == leftArrow || data.componentID == rightArrow) && !settingsOpen)
    {
        switch (data.event)
        {
        case UI::EngineEvents::CLICK_OFF: {
            MenuScene *ptr = dynamic_cast<MenuScene *>(SceneManager::GetCurrentScene().get());
            int nbOffset = (data.componentID == leftArrow) ? -1 : 1;
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
            break;
        }
        default:
            break;
        }
    }
    else if (data.componentID == soundSlider)
    {
        if (data.event == UI::EngineEvents::UPDATE_VALUE)
        {
            UI::FloatSlider::UpdateValueEventData dataCast = dynamic_cast<UI::FloatSlider::UpdateValueEventData &>(data);
            AudioManager::SetVolume(dataCast.newValue);
        }
    }
    else if (data.componentID == settingsButton)
    {
        switch (data.event)
        {
        case UI::EngineEvents::CLICK_OFF: {
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
}