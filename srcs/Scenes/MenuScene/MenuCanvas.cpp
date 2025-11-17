#include "Scenes/MenuScene/MenuCanvas.hpp"
#include "Scenes/GameplayScene/GameplayScene.hpp"
#include "Engine/UI/Button/Button.hpp"
#include "Engine/WindowManager/WindowManager.hpp"
#include "Engine/UI/Events.hpp"
#include "Engine/Scenes/SceneManager/SceneManager.hpp"

MenuCanvas::MenuCanvas()
{
    UI::ACanvas::BeginCanvas(this);
    ml::vec2 size = ml::vec2(100, 100);
    playButton = UI::ACanvas::AddComponent(std::make_unique<UI::Button>("Play", "arial", (WindowManager::GetWindowSize() - size) / 2, size));
    UI::ACanvas::EndCanvas();
}

MenuCanvas::~MenuCanvas()
{
}

void MenuCanvas::HandleEvents(unsigned int componentID, unsigned int event)
{
    if (componentID == playButton)
    {
        switch (event)
        {
        case CURSEUR_ON:
            std::cout << "curseur on" << std::endl;
            break;
        case CURSEUR_OFF:
            std::cout << "curseur off" << std::endl;
            break;
        case CLICK_ON:
            std::cout << "click on" << std::endl;
            break;
        case CLICK_OFF:
            std::cout << "click off" << std::endl;
            SceneManager::SwitchScene(std::make_unique<GameplayScene>());
            break;
        default:
            break;
        }
    }
}