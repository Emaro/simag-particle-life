#include <iostream>
#include <fstream>
#include <assert.h>
#include <memory>

#include <glad/gl.h>
#include <glm/mat3x3.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>
#include <imgui_glfw/imgui_impl_glfw.h>
#include <imgui_glfw/imgui_impl_opengl2.h>

#include "Globals.h"
#include "Context/ContextOpenGl.h"
#include "Context/GlobalEnvironment.h"
#include "Context/States.h"
#include "Context/App.h"
#include "Helper/Framerate.h"
#include "Helper/HelperImGui.h"
#include "Helper/HelperDraw.h"
#include "Helper/RenderingOldSchoolEffects.h"

// ----------------------------------------------------------------------------

namespace
{
    // OpenGl Context
    std::unique_ptr<ContextOpenGl> contextOpenGl;

    // Application
    std::unique_ptr<IApp> app;

    // Framerate measurement
    Framerate framerate;

    // ------------------------------------------------------------------------

    void setOpenGlStates()
    {
        glClearColor(gEnv->stateGui->bgColor[0], gEnv->stateGui->bgColor[1], gEnv->stateGui->bgColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(gEnv->camera->getProjection()));
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(glm::value_ptr(gEnv->camera->getView()));

        glDisable(GL_LIGHTING);

        if (gEnv->camera->is2d())
        {
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glCullFace(GL_BACK);
        }
        else
        {
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
        }

        if (gEnv->stateGui->bPsSmoothPoints)
        {
            glEnable(GL_POINT_SMOOTH);
            glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            glDisable(GL_POINT_SMOOTH);
            glDisable(GL_BLEND);
        }

        if (gEnv->stateGui->bUseFog)
        {
            glFogi(GL_FOG_MODE, GL_EXP);
            glFogf(GL_FOG_DENSITY, gEnv->stateGui->fogDensity);
            glFogfv(GL_FOG_COLOR, glm::value_ptr(gEnv->stateGui->fogColor));
            glEnable(GL_FOG);
        }
        else
        {
            glDisable(GL_FOG);
        }
    }

    void Draw2dOverlay()
    {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadMatrixf(glm::value_ptr(gEnv->camera->get2dMatrixScreenResolution()));
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        
        // Circle for "simulation is running"
        if (gEnv->stateSim->isRunning)
        {
            glColor4f(0, 1, 0, 1);
            glLineWidth(4.0f);
            const float dx = 15.0f;
            const float radius = 10.0f;
            const glm::vec3 pos((float)contextOpenGl->sizeFramebuffer().x - dx, (float)contextOpenGl->sizeFramebuffer().y - dx, 0.0f);
            Helper::drawCircle2d(pos, radius, 20);
            glColor4f(1, 1, 1, 1);
            glLineWidth(1.0f);
        }

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }

    // ------------------------------------------------------------------------

    void display()
    {
        // Render setup
        setOpenGlStates();

        // Update stateSim (dt etc.)
        gEnv->stateSim->dt = gEnv->stateSim->useFixedTimestep ? gEnv->stateSim->dtFixed : (float)framerate.getTimeSinceLastCall();
        gEnv->stateSim->nextFrame();

        // Main worker
        app->doWork();

        // Visuals
        glPushMatrix();

        const bool bDrawFloor = gEnv->stateGui->bDrawFloor;
        if (gEnv->camera->is2d())
        {
            // 2D
            if (bDrawFloor) { Helper::drawGrid2d(); }
            app->draw();
        }
        else
        {
            // 3D
            if (gEnv->stateGui->bDrawReflectionAndShadowsIn3d && bDrawFloor)
            {
                // With shadows & reflection
                float mirrorIntensity = 0.05f;
                auto* ptrApp = app.get();
                RenderingOldSchoolEffects::beginShadowProjection(
                    gEnv->stateGui->lightPos,
                    glm::vec3(0, gEnv->stateGui->floorY, 0), glm::vec3(0, 1, 0),
                    []() { Helper::drawFloor(gEnv->stateGui->floorY); },
                    [mirrorIntensity]() { Helper::drawFloor(gEnv->stateGui->floorY, 1.0f - mirrorIntensity); },
                    [ptrApp]() { ptrApp->draw(); }
                );
            }
            else
            {
                // Without shadows & reflection
                if (bDrawFloor) { Helper::drawFloor(gEnv->stateGui->floorY); }
                app->draw();
            }
        }

        // Draw axis
        if (gEnv->stateGui->bDrawAxis) { Helper::drawAxis(); }

        // Draw camera orientation
        if (gEnv->stateGui->bDrawCameraOrientation) { gEnv->camera->drawOrientation(); }

        // Draw light
        if (gEnv->camera->is3d()) { glColor3f(1, 1, 0); Helper::drawSphere(gEnv->stateGui->lightPos, 0.5f); glColor3f(1, 1, 1); }

        // Draw 2d overlay
        Draw2dOverlay();

        glPopMatrix();
    }

    // ------------------------------------------------------------------------

    void step()
    {
        display();
    }

    // ------------------------------------------------------------------------

    void mainLoop()
    {
        while (contextOpenGl->doFrame())
        {
            if (gEnv->stateGui->bShowTick) { std::cerr << "*"; }

            gEnv->updateCamera(1000.0f / ImGui::GetIO().Framerate);

            Helper::ImGuiFrameStart();

            app->imGui();

            step();

            contextOpenGl->idleForTargetFps((double)Globals::FORCE_FRAMERATE);

            Helper::ImGuiFrameEnd();
        }
    }

} // namespace anonymous

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

int main(int argc, char** argv)
{
    // Context
    GlobalEnvironment::init();
    contextOpenGl = std::make_unique<ContextOpenGl>();
    app = std::make_unique<App>();
    contextOpenGl->setCallbackAdditionalKey(static_cast<ICallbackKey*>(app.get()));
    contextOpenGl->setWindowName(app->name());
    gEnv->createCameras(contextOpenGl->sizeFramebuffer(), contextOpenGl->sizeWindow());

    // Launch main loop
    Helper::ImGuiInit(contextOpenGl->window());
    mainLoop();
    Helper::ImGuiExit();

    return 0;
}
