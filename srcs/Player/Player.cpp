#include "Player/Player.hpp"
#include "Engine/3D/ModelManager/ModelManager.hpp"
#include "Engine/Time/Time.hpp"

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
    position += direction * Time::getDeltaTime() * speed;
}

void Player::Draw(const ml::vec3 &camPos, const std::vector<std::unique_ptr<ALight>> &lights, const ml::mat4 &projection, const ml::mat4 &view)
{
    ml::mat4 transform = ml::translate(ml::mat4(1.0f), position) * ml::rotate(ml::mat4(1.0f), ml::degrees(angle), ml::vec3(0, 1, 0));
    ModelManager::GetModel(modelIndex).Draw(camPos, lights, projection, view, transform);
}