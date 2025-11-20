#pragma once

#include "Engine/UI/ACanvas/ACanvas.hpp"
#include "42RunEvents.hpp"

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
    unsigned int scoreText;

  public:
    GameplayCanvas();
    ~GameplayCanvas();

    void HandleEvents(UI::EventData &data);
};