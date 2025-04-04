#pragma once

#include "Matrix/Matrix.hpp"

class Player
{
    private:
        int modelIndex;
        ml::vec3 position;
        ml::vec3 direction;
        float angle;

    public:
        Player();
        ~Player();

        int GetModelIndex() const { return (modelIndex); }
        void SetModelIndex(int modelIndex) { this->modelIndex = modelIndex; }
        ml::vec3 GetPosition() const { return position; }
        void AddToPosition(const ml::vec3 &position) { this->position += position; }
        ml::vec3 GetDirection() const { return direction; }
        void SetDirection(const ml::vec3 &direction) { this->direction = direction; }
        float GetAngle() const { return angle; }
        void AddToAngle(float angle) { this->angle += angle; }

};