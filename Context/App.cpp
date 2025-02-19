#include "App.h"

#include <sstream>
//#include <fstream>
#include <iostream>

#include <glad/gl.h>

//#include <glm/mat3x3.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>

#include "../Integrator/IntegratorEuler.h"
#include "../Integrator/IntegratorEulerCromer.h"
#include "../Integrator/IntegratorLeapFrog.h"
#include "../Integrator/IntegratorRK2_Midpoint.h"
#include "../Integrator/IntegratorRK4.h"
#include "../Integrator/IntegratorEmpty.h"

#include "../Helper/HelperImGui.h"
//#include "../Helper/HelperReadPLY.h"

#include "GlobalEnvironment.h"
#include "States.h"

// ----------------------------------------------------------------------------

// Easy registering of tasks - not perfect, but easy to use for you...

template<class T>
void App::registerTask()
{
    for (int i = 0; i < cMaxTasks; i++)
    {
        int id = (int)m_tasks[i].size();
        std::string preParam = "T" + std::to_string(i) + "/" + std::to_string(id) + ": ";
        auto task = std::make_shared<T>();
        task->setContext(id, &m_psc, preParam);
        m_tasks[i].push_back(task);
    }
}

int App::findTaskIdByName(std::string const& name) const
{
    int result = -1;
    // find task id with same name
    for (int i = 0; i < (int)m_tasks[0].size(); i++)
    {
        if (strstr(m_tasks[0][i]->name(), name.c_str()) != nullptr) // using task[0] for simplicity, cause all m_tasks contain all tasks
        {
            result = i;
            break;
        }
    }
    return result;
}

template<class T>
void App::pushDefaultTask()
{
    if (m_defaultTaskOnInitCount >= cMaxTasks) return;

    // find task id with same name
    int id = findTaskIdByName(std::make_shared<T>()->name());
    if (id >= 0)
    {
        m_currentTask[m_defaultTaskOnInitCount] = id;
    }
    m_defaultTaskOnInitCount++;
}

#include "../Tasks/CreateAndRegisterTasks.h"

// ----------------------------------------------------------------------------

App::App()
{
    for (int i = 0; i < cMaxTasks; i++)
    {
        m_currentTask[i] = 0;
    }

    createTasks();

    // Set default tasks for initialization
    ::setDefaultTasks(this);
    //if (cMaxTasks > 2) { m_currentTask[2] = 2; } // Default on Task3: Tools

    createIntegrators();
}

namespace
{
    template<class T>
    std::string createStringList(T& list)
    {
        // Create string of all task-names for Gui -> ComboBox ("task1\0task2\0task3\0 ... \0\0")
        std::stringstream ss;
        for (auto& it : list)
        {
            ss << it->name() << '\0';
        }
        ss << '\0';
        return ss.str();
    }
}

void App::createTasks()
{
    // Register tasks from ..\Tasks\CreateAndRegisterTasks.h
    ::createTasks(this);

    // Create string of all task-names for ImGui -> ComboBox ("task1\0task2\0task3\0 ... \0\0")
    m_tasksNamesGuiCombo = createStringList(m_tasks[0]);
}

void App::createIntegrators()
{
    // Create Integrators
    m_integrators.push_back(std::make_shared<IntegratorEuler>());
    m_integrators.push_back(std::make_shared<IntegratorEulerCromer>());
    m_integrators.push_back(std::make_shared<IntegratorLeapFrog>());
    m_integrators.push_back(std::make_shared<IntegratorRK2_Midpoint>());
    m_integrators.push_back(std::make_shared<IntegratorRK4>());
    m_integrators.push_back(std::make_shared<IntegratorEmpty>());


    // Register in gEnv
    gEnv->stateSim->integrator = m_integrators[1];

    // Create string of all integrator-names for ImGui
    m_integratorNamesGuiCombo = createStringList(m_integrators);
}


namespace
{
    void setOpenGlDefaultStates()
    {
        glLineWidth(1.0f);
        glPointSize(1.0f);
        glColor4f(1, 1, 1, 1);
    }
}

void App::doWork()
{
    // Update according to style (e.g., read from file-stream)
    m_psc.update();

    // Integrate / Timestep
    int noOfTimesteps = gEnv->stateSim->useFixedTimestep ? gEnv->stateSim->dtFixedNoOfStepsPerFrame : 1;
    if (noOfTimesteps < 1) { noOfTimesteps = 1; }

    for (int actualStepNo = 0; actualStepNo < noOfTimesteps; actualStepNo++)
    {
        gEnv->stateSim->dtActualStepPerFrame = actualStepNo;

        if (gEnv->stateSim->isRunning)
        {
            const int noOfPasses = gEnv->stateSim->integrator->noOfPasses();
            for (int nPass = 0; nPass < noOfPasses; nPass++)
            {
                // Clear forces
                m_psc.setAllForcesToZero();

                // StepPre
                for (int i = 0; i < cMaxTasks; i++)
                {
                    m_tasks[i][m_currentTask[i]]->stepPre();
                }

                // Determine forces
                for (int i = 0; i < cMaxTasks; i++)
                {
                    m_tasks[i][m_currentTask[i]]->setForces();
                }

                // Clear forces for static particles
                m_psc.setAllForcesToZeroForStaticParticles();

                // Step
                m_psc.step(nPass, gEnv->stateSim->dt);

                // StepPost
                for (int i = 0; i < cMaxTasks; i++)
                {
                    m_tasks[i][m_currentTask[i]]->stepPost();
                }
            }
        }


        // DoWork
        for (int i = 0; i < cMaxTasks; i++)
        {
            m_tasks[i][m_currentTask[i]]->doWork();
        }
    }

    if (m_bWriteToFilestream && m_savePs)
    {
         m_writeToFilestream.write(*m_savePs);
    }
}

void App::draw()
{
    // Draw Tasks
    for (int i = 0; i < cMaxTasks; i++)
    {
        m_tasks[i][m_currentTask[i]]->draw();
    }

    // Draw Particle Systems
    m_psc.draw();

    // setOpenGlDefaultStates();
}

// ----------------------------------------------------------------------------

void App::imGui()
{
    static bool showDemoWindow = false;
    static bool showSimpleWindow = false;

    ImGui::Begin("Menu");

    m_psc.imGui();

    for (int i = 0; i < cMaxTasks; i++)
    {
        std::string key = "T" + std::to_string(i) + ": ";
        if (ImGui::CollapsingHeader((key+"Task").c_str()))
        {
            ImGui::Indent();

            // Same Parameter 'm_currentTask' -> 2 different selection styles
            int currentTaskSelected = m_currentTask[i];
            ImGui::Combo((key + "Selected Task").c_str(), &m_currentTask[i], m_tasksNamesGuiCombo.c_str(), 30);
            ImGui::SliderInt((key + "Selected Task As ID").c_str(), &m_currentTask[i], 0, (int)m_tasks[i].size() - 1);
            bool taskHasChanged = currentTaskSelected != m_currentTask[i];
            if (taskHasChanged) { m_tasks[i][m_currentTask[i]]->init(); }

            //ImGui::Separator();
            //ImGui::Text("%s", m_tasks[i][m_currentTask[i]]->name());

            ImGui::Separator();
            m_tasks[i][m_currentTask[i]]->imGui();
            
            ImGui::Separator();
            ImGui::Text("%s", m_tasks[i][m_currentTask[i]]->toString());

            ImGui::Unindent();
        }
    }

    if (Globals::GUI_ENABLE_FILE_MENU)
    {
        if (ImGui::CollapsingHeader("File"))
        {
            ImGui::Indent();

            if (!m_bWriteToFilestream)
            {
                ImGui::SliderInt("File: Work on PS No", &m_savePsFromPSCM_idx, 0, m_psc.size() - 1);
                ImGui::Separator();
                if (ImGui::Button("Save SingleFrame-Capture")) { m_psc.particleSystem(m_savePsFromPSCM_idx).save(Helper::cFilenameCapture); }
                ImGui::Separator();
                if (ImGui::Button("Write File-Stream")) { m_writeToFilestream.reset(); m_bWriteToFilestream = true; m_savePs = &m_psc.particleSystem(m_savePsFromPSCM_idx); }
                ImGui::Separator();
                if (ImGui::Button("Write File-Stream & Start Simulation")) { m_writeToFilestream.reset(); m_bWriteToFilestream = true; m_savePs = &m_psc.particleSystem(m_savePsFromPSCM_idx); gEnv->stateSim->isRunning = true; }
                ImGui::Separator();
            }
            if (m_bWriteToFilestream)
            {
                ImGui::Text("WRITING STREAM!!!");
                ImGui::Text("WRITING STREAM!!!");
                ImGui::Text("WRITING STREAM!!!");
                ImGui::Text("WRITING STREAM!!!");
                if (ImGui::Button("STOP Write File-Stream")) { m_bWriteToFilestream = false; }
                if (ImGui::Button("STOP Write File-Stream & Stop Simulation")) { m_bWriteToFilestream = false; gEnv->stateSim->isRunning = true; }
            }

            ImGui::Unindent();
        }
    }

    if (ImGui::CollapsingHeader("Simulation"))
    {
        ImGui::Indent();

        ImGui::Checkbox("Running", &gEnv->stateSim->isRunning);

        ImGui::Separator();
        ImGui::Checkbox("Use fixed timestep", &gEnv->stateSim->useFixedTimestep);
        if (gEnv->stateSim->useFixedTimestep)
        {
            ImGui::SliderFloat("Fixed timestep", &gEnv->stateSim->dtFixed, 0.001f, 0.1f);
            ImGui::SliderInt("No of steps per frame", &gEnv->stateSim->dtFixedNoOfStepsPerFrame, 1, 10);
        }
        else
        {
            ImGui::Text("dt\t%.8lf", gEnv->stateSim->dt);
        }

        ImGui::Separator();
        // Integrator
        int currentIntegrator = 0;
        for (int i = 0; i < (int)m_integrators.size(); i++)
        {
            if (m_integrators[i].get() == gEnv->stateSim->integrator.get()) { currentIntegrator = i; break; }
        }
        ImGui::Combo("Integrator", &currentIntegrator, m_integratorNamesGuiCombo.c_str());
        gEnv->stateSim->integrator = m_integrators[currentIntegrator];
        ImGui::Text("noOfPasses: %i", gEnv->stateSim->integrator->noOfPasses());

        ImGui::Separator();

        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeader("Draw and GUI"))
    {
        ImGui::Indent();

        Helper::ImGuiParticleSizeParameter("", gEnv->stateGui->psPointSize);
        ImGui::Separator();
        ImGui::SliderFloat("PS VectorScale", &gEnv->stateGui->psVectorScale, 0.005f, 1.0f);

        ImGui::Separator();
        ImGui::Checkbox("PsSmoothPoints", &gEnv->stateGui->bPsSmoothPoints);

        ImGui::Separator();
        ImGui::Checkbox("Draw axis", &gEnv->stateGui->bDrawAxis);
        ImGui::Checkbox("Draw Grid/Floor", &gEnv->stateGui->bDrawFloor);
        ImGui::SliderFloat("Floor Y", &gEnv->stateGui->floorY, -5.0, 0.0f);
        ImGui::Checkbox("Draw 3d Reflection/Shadows", &gEnv->stateGui->bDrawReflectionAndShadowsIn3d);

        ImGui::Separator();
        ImGui::Checkbox("UseFog", &gEnv->stateGui->bUseFog);
        if (gEnv->stateGui->bUseFog)
        {
            ImGui::SliderFloat("FogDensity", &gEnv->stateGui->fogDensity, 0.0f, 0.5f);
            ImGui::ColorEdit3("FogColor", glm::value_ptr<float>(gEnv->stateGui->fogColor));
        }

        ImGui::Separator();
        ImGui::ColorEdit3("Background Color Select", glm::value_ptr<float>(gEnv->stateGui->bgColor));

        ImGui::Separator();
        ImGui::SliderFloat("CameraSpeed", &gEnv->stateGui->cameraStateSpeedTranslationFactor, 0.1f, 2.0f);
        ImGui::Checkbox("Show Tick", &gEnv->stateGui->bShowTick);
        ImGui::Checkbox("LogDebug", &gEnv->logDebug);

        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeader("Info"))
    {
        ImGui::Indent();

        ImGui::Text("Mouse Pos SS\t%i / %i",(int)gEnv->stateGui->mousePos.x, (int)gEnv->stateGui->mousePos.y);
        glm::vec2 posWS = gEnv->camera->SSToWorld2d(gEnv->stateGui->mousePos);
        ImGui::Text("Mouse Pos WS\t%f / %f", posWS.x, posWS.y);
        ImGui::Text(gEnv->stateGui->mouseButtonLeftClick ? "Mouse Click:   TRUE" : "Mouse Click:   FALSE");
        ImGui::Text(gEnv->stateGui->mouseButtonLeftRelease ? "Mouse Release: TRUE" : "Mouse Release: FALSE");
        ImGui::Text(gEnv->stateGui->mouseButtonLeftPressed ? "Mouse Pressed: TRUE" : "Mouse Pressed: FALSE");
        ImGui::Separator();
        ImGui::Text("CamPosOld:        %f / %f / %f", gEnv->stateGui->cameraState.posOld.x, gEnv->stateGui->cameraState.posOld.y, gEnv->stateGui->cameraState.posOld.z);
        ImGui::Separator();
        ImGui::Text("Framebuffer:      %i / %i", gEnv->camera->sizeFramebuffer().x, gEnv->camera->sizeFramebuffer().y);
        ImGui::Text("Window:           %i / %i", gEnv->camera->sizeWindow().x, gEnv->camera->sizeWindow().y);
        ImGui::Separator();
        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("io.DisplaySize: %.2f,%.2f", io.DisplaySize.x, io.DisplaySize.y);
        ImGui::Text("io.DisplayFramebufferScale: %.2f,%.2f", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        ImGui::Separator();
        ImGui::Text("%s",gEnv->stateGui->cameraState.toString().c_str());
        ImGui::Unindent();
    }

    if (Globals::GUI_ENABLE_DEBUG_MENU)
    {
        if (ImGui::CollapsingHeader("Debug"))
        {
            ImGui::Indent();

            ImGui::Checkbox("bDrawCameraOrientation", &gEnv->stateGui->bDrawCameraOrientation);

            ImGui::Separator();
            ImGui::Checkbox("Simple Window", &showSimpleWindow);
            ImGui::Checkbox("Demo Window", &showDemoWindow);

            ImGui::Unindent();
        }
    }

    if (ImGui::BeginTable("Statistics#AppCpp", 4))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::Text("fps: %.1f", ImGui::GetIO().Framerate);
        ImGui::TableSetColumnIndex(1); ImGui::Text("dt: %.2fms", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::TableSetColumnIndex(2); ImGui::Text("t: %.2fs", (float)gEnv->stateSim->timeRuning());
        ImGui::TableSetColumnIndex(3); ImGui::Text("f: %i", gEnv->stateSim->frameCount);
        ImGui::EndTable();
    }

    ImGui::End();

    if (showSimpleWindow) { Helper::ImGuiShowSimpleWindow(showSimpleWindow); }
    if (showDemoWindow) { ImGui::ShowDemoWindow(&showDemoWindow); }

}

namespace
{
    void flip(bool& bState) { bState = !bState; }
}

void App::callbackKey(int key, int scancode, int action, int mods)
{
    if (gEnv->logDebug) { std::cerr << "App::callbackKey(): " << key << "\n"; }

    //const int GLFW_RELEASE = 0;
    const int GLFW_PRESS = 1;
    const int GLFW_MOD_CONTROL = 2;
    const int GLFW_KEY_KP_0 = 320;

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        default: break;
        case 'F': flip(gEnv->stateGui->bDrawFloor); break;
        case 'A': flip(gEnv->stateGui->bDrawAxis); break;
        case '@': case 81: flip(gEnv->logDebug); break;
        case '*': case 93: flip(gEnv->stateGui->bShowTick); break;
        case '#': case 92: gEnv->switchCamera(); break;
        case 'R': gEnv->createCameras(gEnv->camera->sizeFramebuffer(), gEnv->camera->sizeWindow()); break;

        case ' ': flip(gEnv->stateSim->isRunning); break;

        case 'S': case 'C': // copy / save capture.ply from PS0
            if (mods & GLFW_MOD_CONTROL)
            {
                m_psc.particleSystem(0).save(Helper::cFilenameCapture);
            }
            break;
        case 'L': case 'V': // paste / load capture.ply into PS0
            if (mods & GLFW_MOD_CONTROL)
            {
                m_psc.particleSystem(0).load(Helper::cFilenameCapture);
            }
            break;
        case '0': case GLFW_KEY_KP_0 + 0: m_psc.particleSystem(0).load(Helper::cFilenameCapture); break;
        case '1': case GLFW_KEY_KP_0 + 1: m_psc.particleSystem(0).load(Helper::cFilenameList + "01"); break;
        case '2': case GLFW_KEY_KP_0 + 2: m_psc.particleSystem(0).load(Helper::cFilenameList + "02"); break;
        case '3': case GLFW_KEY_KP_0 + 3: m_psc.particleSystem(0).load(Helper::cFilenameList + "03"); break;
        case '4': case GLFW_KEY_KP_0 + 4: m_psc.particleSystem(0).load(Helper::cFilenameList + "04"); break;
        case '5': case GLFW_KEY_KP_0 + 5: m_psc.particleSystem(0).load(Helper::cFilenameList + "05"); break;
        case '6': case GLFW_KEY_KP_0 + 6: m_psc.particleSystem(0).load(Helper::cFilenameList + "06"); break;
        case '7': case GLFW_KEY_KP_0 + 7: m_psc.particleSystem(0).load(Helper::cFilenameList + "07"); break;
        case '8': case GLFW_KEY_KP_0 + 8: m_psc.particleSystem(0).load(Helper::cFilenameList + "08"); break;
        case '9': case GLFW_KEY_KP_0 + 9: m_psc.particleSystem(0).load(Helper::cFilenameList + "09"); break;
        }
    }
}

