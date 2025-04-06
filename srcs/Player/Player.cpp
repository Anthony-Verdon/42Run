#include "Player/Player.hpp"
#include "Engine/3D/ModelManager/ModelManager.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/WindowManager/WindowManager.hpp"

Player::Player()
{
    position = ml::vec3(0, 0, 0);
    direction = ml::vec3(1, 0, 0);
    modelIndex = 0;
    angle = 0;
    speed = 5;
}

Player::~Player()
{
}

void Player::ProcessInput()
{
    direction = ml::normalize(ml::vec3(-sinf(angle), 0, cosf(angle)));
    
    std::string currentAnim = ModelManager::GetModel(modelIndex).GetCurrentAnimation();
    
    if (currentAnim == "Run")
    {
        position += direction * speed * Time::getDeltaTime();
        if (WindowManager::IsInputPressed(GLFW_KEY_D))
            ModelManager::GetModel(modelIndex).Play("DashRight");
        else if (WindowManager::IsInputPressed(GLFW_KEY_A))
            ModelManager::GetModel(modelIndex).Play("DashLeft");
    }
    else if (currentAnim == "DashRight")
    {
        position += (direction * speed + ml::vec3(-1, 0, 0) * 2.0f) * Time::getDeltaTime();
        if (ModelManager::GetModel(modelIndex).CurrentAnimationEnded())
            ModelManager::GetModel(modelIndex).Play("Run");
    }
    else if (currentAnim == "DashLeft")
    {
        position += (direction * speed + ml::vec3(1, 0, 0) * 2.0f) * Time::getDeltaTime();
        if (ModelManager::GetModel(modelIndex).CurrentAnimationEnded())
            ModelManager::GetModel(modelIndex).Play("Run");
    }
}

void Player::Draw(const ml::vec3 &camPos, const std::vector<std::unique_ptr<ALight>> &lights, const ml::mat4 &projection, const ml::mat4 &view)
{
    ml::mat4 transform = ml::translate(ml::mat4(1.0f), position) * ml::rotate(ml::mat4(1.0f), ml::degrees(angle), ml::vec3(0, 1, 0));
    ModelManager::GetModel(modelIndex).Draw(camPos, lights, projection, view, transform);
}