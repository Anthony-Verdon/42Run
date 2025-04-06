#pragma once

#include "Matrix/Matrix.hpp"
#include <memory>
#include "Engine/3D/Lights/Lights.hpp"
#include <vector>
#include "Engine/3D/ModelManager/ModelManager.hpp"

class Player
{
    private:
        int modelIndex;
        ml::vec3 position;
        ml::vec3 direction;
        float angle;
        float speed;

    public:
        Player();
        ~Player();

        void ProcessInput();
        void Draw(const ml::vec3 &camPos, const std::vector<std::unique_ptr<ALight>> &lights, const ml::mat4 &projection, const ml::mat4 &view);

        int GetModelIndex() const { return (modelIndex); }
        void SetModelIndex(int modelIndex) { this->modelIndex = modelIndex; }
        ml::vec3 GetPosition() const { return position; }
        void SetPosition(const ml::vec3 &position) { this->position = position; }
        void AddToPosition(const ml::vec3 &position) { this->position += position; }
        ml::vec3 GetDirection() const { return direction; }
        void SetDirection(const ml::vec3 &direction) { this->direction = direction; }
        float GetAngle() const { return angle; }
        void AddToAngle(float angle) { this->angle += angle; }

};