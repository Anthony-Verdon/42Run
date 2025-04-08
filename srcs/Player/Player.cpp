#include "Player/Player.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/WindowManager/WindowManager.hpp"

Player::Player()
{
    position = ml::vec3(0, 0, 0);
    direction = ml::vec3(1, 0, 0);
    modelIndex = 0;
    angle = 0;
    speed = 5;
    column = 0;
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
        if (WindowManager::IsInputPressed(GLFW_KEY_D) && column != 1)
        {
            ModelManager::GetModel(modelIndex).Play("DashRight");
            column++;
        }
        else if (WindowManager::IsInputPressed(GLFW_KEY_A) && column != -1)
        {
            ModelManager::GetModel(modelIndex).Play("DashLeft");
            column--;
        }
        else if (WindowManager::IsInputPressed(GLFW_KEY_SPACE))
            ModelManager::GetModel(modelIndex).Play("Jump");
        else if (WindowManager::IsInputPressed(GLFW_KEY_LEFT_SHIFT))
            ModelManager::GetModel(modelIndex).Play("Roll");
    }
    else if (currentAnim == "DashRight")
    {
        position += (direction * speed + ml::vec3(-1, 0, 0) * 2.0f) / ModelManager::GetModel(modelIndex).GetCurrentAnimationDuration() * Time::getDeltaTime();
        if (ModelManager::GetModel(modelIndex).CurrentAnimationEnded())
            ModelManager::GetModel(modelIndex).Play("Run");
    }
    else if (currentAnim == "DashLeft")
    {
        position += (direction * speed + ml::vec3(1, 0, 0) * 2.0f) / ModelManager::GetModel(modelIndex).GetCurrentAnimationDuration() * Time::getDeltaTime();
        if (ModelManager::GetModel(modelIndex).CurrentAnimationEnded())
            ModelManager::GetModel(modelIndex).Play("Run");
    }
    else if (currentAnim == "Jump")
    {
        position += (direction * speed + ml::vec3(0, 1, 0) * 2.0f) / ModelManager::GetModel(modelIndex).GetCurrentAnimationDuration() * Time::getDeltaTime();
        if (ModelManager::GetModel(modelIndex).CurrentAnimationEnded())
            ModelManager::GetModel(modelIndex).Play("Fall", 2.0f);
    }
    else if (currentAnim == "Fall")
    {
        position += (direction * speed + ml::vec3(0, -1, 0) * 2.0f) / ModelManager::GetModel(modelIndex).GetCurrentAnimationDuration() * Time::getDeltaTime();
        if (ModelManager::GetModel(modelIndex).CurrentAnimationEnded())
            ModelManager::GetModel(modelIndex).Play("Run");
    }
    else if (currentAnim == "Roll")
    {
        position += (direction * speed + ml::vec3(0, 0, 1) * 4.0f) / ModelManager::GetModel(modelIndex).GetCurrentAnimationDuration() * Time::getDeltaTime();
        if (ModelManager::GetModel(modelIndex).CurrentAnimationEnded())
            ModelManager::GetModel(modelIndex).Play("Run");
    }
}

void Player::Draw(const ml::vec3 &camPos, const std::vector<std::unique_ptr<ALight>> &lights, const ml::mat4 &projection, const ml::mat4 &view)
{
    ml::mat4 transform;
    if (ModelManager::GetModel(modelIndex).GetCurrentAnimation() == "Roll")
        transform = ml::translate(ml::mat4(1.0f), position - ml::vec3(0, 0, 2.0f)) * ml::rotate(ml::mat4(1.0f), ml::degrees(angle), ml::vec3(0, 1, 0));
    else
        transform = ml::translate(ml::mat4(1.0f), position) * ml::rotate(ml::mat4(1.0f), ml::degrees(angle), ml::vec3(0, 1, 0));
    ModelManager::GetModel(modelIndex).Draw(camPos, lights, projection, view, transform);
}