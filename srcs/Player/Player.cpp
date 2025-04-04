#include "Player/Player.hpp"
#include "Engine/3D/ModelManager/ModelManager.hpp"

Player::Player()
{
    position = ml::vec3(0, 0, 0);
    direction = ml::vec3(0, 0, 1);
    modelIndex = 0;
}

Player::~Player()
{
}