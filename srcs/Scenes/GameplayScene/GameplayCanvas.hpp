#pragma once

#include "Engine/UI/ACanvas/ACanvas.hpp"
#include "GameEvents.hpp"

struct UpdateScoreEventData : public UI::EventData
{
    int score;

    UpdateScoreEventData()
    {
        event = UPDATE_SCORE;
    }
};

class GameplayCanvas : public UI::ACanvas
{
  private:
    std::string score;
    UI::ComponentID scoreText;
    UI::ComponentID menuButton;

  public:
    GameplayCanvas();
    ~GameplayCanvas();

    void GameFinished();

    void HandleEvents(UI::EventData &data);
};