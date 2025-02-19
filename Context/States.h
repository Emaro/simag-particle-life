#pragma once

#include <memory>
#include <chrono>
#include <glm/vec3.hpp>

#include "../Integrator/IIntegrator.h"

#include "../Helper/Camera.h"
#include "../Helper/GameCameraState.h"

struct StateSimulation
{
    float dt;
    bool isRunning = false;

    bool useFixedTimestep = true;
    float dtFixed = 0.0166f;
    int dtFixedNoOfStepsPerFrame = 1;
    int dtActualStepPerFrame = 0; // running from 0..dtFixedNoOfStepsPerFrame-1

    std::shared_ptr<IIntegrator> integrator;
    int frameCount = 0;

    StateSimulation()
        : dt(0)
    {
        timeStart = std::chrono::high_resolution_clock::now();
    }

    void nextFrame() { frameCount++; }
    double timeRuning() { return (double)(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timeStart).count() / 1000.0); }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> timeStart;
};

struct StateGui
{
    // UI stats - draw particles
    float psPointSize = 6.0f;
    float psVectorScale = 1.0f;  // scale factor for rendering of e.g., velocities, forces
    bool  bPsSmoothPoints = true;

    // UI stats - draw other
    bool bDrawFloor = true;
    float floorY = -1.0f;
    bool bDrawAxis = true;
    bool bDrawCameraOrientation = false;
    glm::vec3 bgColor;
    bool bDrawReflectionAndShadowsIn3d = true;

    // Fog
    bool bUseFog = false;
    float fogDensity = 0.1f;
    glm::vec3 fogColor;

    // UI stats - other
    glm::vec2 mousePos;
    bool mouseButtonLeftClick = false; // only set for one frame
    bool mouseButtonLeftRelease = false; // only set for one frame
    bool mouseButtonLeftPressed = false; // true, until release
    bool bShowTick = false;

    // Camera setup
    const float cameraStateSpeedRotation = 0.004f;
    const float cameraStateSpeedTranslation = 0.01f;
    float cameraStateSpeedTranslationFactor = 1.0f; // change speed in GUI with mouse-wheel
    const float cameraSpeedFactorKeyShift = 2.5f;
    GameCameraState cameraState;

    // Cameras
    std::shared_ptr<Camera> camera2d;
    std::shared_ptr<Camera> camera3d;

    // Light
    glm::vec3 lightPos;

    StateGui()
        : bgColor(0.0f)
        , fogColor(0.0f)
        , mousePos(0)
        , cameraState(cameraStateSpeedTranslation, cameraStateSpeedRotation)
        , lightPos(10.0f, 10.0f, 10.0f)
    {
    }

    void prePollEvents() // caled before glfw poll events
    {
        mouseButtonLeftClick = false;
        mouseButtonLeftRelease = false;
    }

};

