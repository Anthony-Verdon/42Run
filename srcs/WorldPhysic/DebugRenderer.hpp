#pragma once

#include <Jolt/Renderer/DebugRendererSimple.h>
#include "Engine/3D/Camera3D/Camera3D.hpp"
#include "Engine/3D/LineRenderer3D/LineRenderer3D.hpp"
#include "globals.hpp"

class DebugRendererImpl : public JPH::DebugRendererSimple
{
    private:
        const Camera3D &camera;

    public:
        DebugRendererImpl(const Camera3D &camera) : camera(camera) {}
        
        virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor)
        {
            std::cout << "DrawLine" << std::endl;
            (void)inFrom;
            (void)inTo;
            (void)inColor;
        }

        virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow = ECastShadow::Off)
        {
            ml::mat4 projection = ml::perspective(ml::radians(camera.getFov()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
            ml::mat4 view = ml::lookAt(camera.getPosition(), camera.getPosition() + camera.getFrontDirection(), camera.getUpDirection());

            LineRenderer3D::Draw(projection, view, ml::vec3(inV1.GetX(), inV1.GetY(), inV1.GetZ()), ml::vec3(inV2.GetX(), inV2.GetY(), inV2.GetZ()), ml::vec3(1, 1, 1));
            LineRenderer3D::Draw(projection, view, ml::vec3(inV1.GetX(), inV1.GetY(), inV1.GetZ()), ml::vec3(inV3.GetX(), inV3.GetY(), inV3.GetZ()), ml::vec3(1, 1, 1));
            LineRenderer3D::Draw(projection, view, ml::vec3(inV2.GetX(), inV2.GetY(), inV2.GetZ()), ml::vec3(inV3.GetX(), inV3.GetY(), inV3.GetZ()), ml::vec3(1, 1, 1));
            (void)inCastShadow;
            (void)inColor;
        }

        virtual void DrawText3D(JPH::RVec3Arg inPosition, const JPH::string_view &inString, JPH::ColorArg inColor = JPH::Color::sWhite, float inHeight = 0.5f)
        {
            std::cout << "DrawText3D" << std::endl;
            (void)inPosition;
            (void)inString;
            (void)inColor;
            (void)inHeight;
        }
};