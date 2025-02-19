#include <glad/gl.h>

#include <memory>
#include <stdlib.h>
#include <stdio.h>
//#include <string.h>
//#include <iostream>

#include <imgui_glfw/imgui_impl_glfw.h>

#include "../Globals.h"
#include "ContextOpenGl.h"
#include "GlobalEnvironment.h"

ContextOpenGl::ContextOpenGl(const char* titleWindow)
    : m_sizeFramebuffer(Globals::WINDOW_INIT_WIDTH, Globals::WINDOW_INIT_HEIGHT)
    , m_sizeWindow(Globals::WINDOW_INIT_WIDTH, Globals::WINDOW_INIT_HEIGHT)
{

    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }

    const bool useMonitor = false;

    if (useMonitor)
    {
        m_monitor = glfwGetPrimaryMonitor();

        if (m_monitor)
        {
            const GLFWvidmode* mode = glfwGetVideoMode(m_monitor);

            glfwWindowHint(GLFW_RED_BITS, mode->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

            setResolution(mode->width, mode->height, mode->width, mode->height);
        }
    }

    if (Globals::USE_MULTISAMPLING_SAMPLES > 0)
    {
        glfwWindowHint(GLFW_SAMPLES, Globals::USE_MULTISAMPLING_SAMPLES);
    }

    m_window = glfwCreateWindow(m_sizeWindow.x, m_sizeWindow.y, titleWindow, m_monitor, nullptr);

    if (!m_window)
    {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    if (m_monitor)
    {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    glfwMakeContextCurrent(m_window);
    gladLoadGL(glfwGetProcAddress);

    // vsync, force framerate, fixed framerate
    if (Globals::FORCE_VSYNC)
    {
        glfwSwapInterval(1); // fixed framerate, vsync
        //glfwWindowHint(GLFW_REFRESH_RATE, 60); // for full screen windows
    }

    // Callbacks
    glfwSetWindowUserPointer(m_window, this); // add user pointer to register member as callbacks

    auto funcResize = [](GLFWwindow* w, int x, int y) { static_cast<ContextOpenGl*>(glfwGetWindowUserPointer(w))->callbackResize(w, x, y); };
    glfwSetFramebufferSizeCallback(m_window, funcResize);

    auto funcKey = [](GLFWwindow* w, int key, int scancode, int action, int mods) { static_cast<ContextOpenGl*>(glfwGetWindowUserPointer(w))->callbackKey(w, key, scancode, action, mods); };
    glfwSetKeyCallback(m_window, funcKey);

    auto funcChar = [](GLFWwindow* w, unsigned int c) { static_cast<ContextOpenGl*>(glfwGetWindowUserPointer(w))->callbackChar(w, c); };
    glfwSetCharCallback(m_window, funcChar);

    auto funcMouseButton = [](GLFWwindow* w, int button, int action, int mods) { static_cast<ContextOpenGl*>(glfwGetWindowUserPointer(w))->callbackMouseButton(w,button,action,mods); };
    glfwSetMouseButtonCallback(m_window, funcMouseButton);

    auto funcCursorPos = [](GLFWwindow* w, double x, double y) { static_cast<ContextOpenGl*>(glfwGetWindowUserPointer(w))->callbackCursorPosCallback(w, x, y); };
    glfwSetCursorPosCallback(m_window, funcCursorPos);

    auto funcScroll = [](GLFWwindow* w, double xoffset, double yoffset) { static_cast<ContextOpenGl*>(glfwGetWindowUserPointer(w))->callbackScroll(w, xoffset, yoffset); };
    glfwSetScrollCallback(m_window, funcScroll);

    // Set initial aspect ratio
    glfwGetFramebufferSize(m_window, &m_sizeFramebuffer.x, &m_sizeFramebuffer.y);
    callbackResize(m_window, m_sizeFramebuffer.x, m_sizeFramebuffer.y);

    // Main Looop
    glfwSetTime(0.0);

    if (Globals::USE_MULTISAMPLING_SAMPLES > 0)
    {
        glEnable(GL_MULTISAMPLE);
    }
}

ContextOpenGl::~ContextOpenGl()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool ContextOpenGl::doFrame()
{
    if (glfwWindowShouldClose(m_window))
    {
        return false;
    }

    glfwSwapBuffers(m_window);
    gEnv->stateGui->prePollEvents();
    glfwPollEvents();
    return true;
}

double ContextOpenGl::getTime()
{
    return glfwGetTime();
}

void ContextOpenGl::idleForTargetFps(double targetFPS)
{
    if (targetFPS < 3.0) return;
    while (glfwGetTime() < m_lastTime + 1.0 / targetFPS)
    {
        // TODO: Put the thread to sleep
    }
    m_lastTime = glfwGetTime();
}


void ContextOpenGl::setResolution(int widthFramebuffer, int heightFramebuffer, int widthWindow, int heightWindow)
{
    m_sizeFramebuffer.x = widthFramebuffer;
    m_sizeFramebuffer.y = heightFramebuffer;
    m_sizeWindow.x = widthWindow;
    m_sizeWindow.y = heightWindow;
}


void ContextOpenGl::callbackResize(GLFWwindow* window, int w, int h)
{
    if (w == 0) w = 1;
    if (h == 0) h = 1;

    m_sizeFramebuffer.x = w;
    m_sizeFramebuffer.y = h;

    // Get window size
    int winWidth, winHeight;
    glfwGetWindowSize(window, &winWidth, &winHeight);

    if (winWidth == 0) winWidth = 1;
    if (winHeight == 0) winHeight = 1;

    m_sizeWindow = glm::ivec2(winWidth, winHeight);

    glViewport(0, 0, m_sizeFramebuffer.x, m_sizeFramebuffer.y);

    if (gEnv->stateGui->camera2d)
    {
        gEnv->stateGui->camera2d->setResolution(m_sizeFramebuffer, m_sizeWindow);
    }
    if (gEnv->stateGui->camera3d)
    {
        gEnv->stateGui->camera3d->setResolution(m_sizeFramebuffer, m_sizeWindow);
    }
}

namespace
{
    void updateCameraState(GLFWwindow* window)
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        gEnv->stateGui->cameraState.update((int)x, (int)y);
    }

    void updateCameraStateScaleImmediate(GLFWwindow* window, float scaleFactor)
    {
        updateCameraState(window);
        gEnv->stateGui->cameraState.scale2d *= scaleFactor;
        gEnv->camera->move(gEnv->stateGui->cameraState);
        gEnv->stateGui->cameraState.clear();
    }
}

void ContextOpenGl::callbackKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //std::cout << "Key: " << key << ":" << action << ":" << mods << "\n";

    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) { exit(0); }

	bool keyHasBeenHandled = false;
    auto& cameraState = gEnv->stateGui->cameraState;

    if (action == GLFW_PRESS)
    {
        if (cameraState.isCameraMoving)
        {
            cameraState.speedFactorTranslate = (mods & GLFW_MOD_SHIFT) ? gEnv->stateGui->cameraSpeedFactorKeyShift : 1.0f;

            // Move camera
            switch (key)
            {
            default: break;
            case 'W': case 'w': cameraState.moveFwd = 1.0f; keyHasBeenHandled = true; break;
            case 'S': case 's': cameraState.moveFwd = -1.0f; keyHasBeenHandled = true; break;
            case 'D': case 'd': cameraState.moveRight = 1.0f; keyHasBeenHandled = true; break;
            case 'A': case 'a': cameraState.moveRight = -1.0f; keyHasBeenHandled = true; break;
            case 'E': case 'e': cameraState.moveUp = 1.0f; keyHasBeenHandled = true; break;
            case 'Q': case 'q': cameraState.moveUp = -1.0f; keyHasBeenHandled = true; break;
            }
        }
    }

    if (action == GLFW_RELEASE)
    {
        cameraState.speedFactorTranslate = (mods & GLFW_MOD_SHIFT) ? gEnv->stateGui->cameraSpeedFactorKeyShift : 1.0f;

        if (cameraState.isCameraMoving)
        {
            switch (key)
            {
            default: break;
            case 'W': case 'w': if (cameraState.moveFwd > 0)   cameraState.moveFwd = 0.0f; keyHasBeenHandled = true; break;
            case 'S': case 's': if (cameraState.moveFwd < 0)   cameraState.moveFwd = 0.0f; keyHasBeenHandled = true; break;
            case 'D': case 'd': if (cameraState.moveRight > 0) cameraState.moveRight = 0.0f; keyHasBeenHandled = true; break;
            case 'A': case 'a': if (cameraState.moveRight < 0) cameraState.moveRight = 0.0f; keyHasBeenHandled = true; break;
            case 'E': case 'e': if (cameraState.moveUp > 0)    cameraState.moveUp = 0.0f; keyHasBeenHandled = true; break;
            case 'Q': case 'q': if (cameraState.moveUp < 0)    cameraState.moveUp = 0.0f; keyHasBeenHandled = true; break;
            }

            updateCameraState(window); // ensure new camera parameters on key-release, to stabilize camera after euler-rotation
        }
    }

    if (!keyHasBeenHandled && m_callbackAdditionalKey)
    {
        m_callbackAdditionalKey->callbackKey(key, scancode, action, mods);
    }
}

void ContextOpenGl::callbackChar(GLFWwindow* window, unsigned int c)
{
    ImGui_ImplGlfw_CharCallback(window, c);
    if (ImGui::GetIO().WantCaptureKeyboard) return;
}

void ContextOpenGl::callbackMouseButton(GLFWwindow* window, int button, int action, int mods)
{
    // Forward go ImGui
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    if (ImGui::GetIO().WantCaptureMouse)
    {
        gEnv->stateGui->mouseButtonLeftPressed = false;
        gEnv->stateGui->mouseButtonLeftRelease = false;
        gEnv->stateGui->mouseButtonLeftClick = false;
        return;
    }

    const bool bPress = (action == GLFW_PRESS);
    const bool bRelease = (action == GLFW_RELEASE);

    const bool bRight = (button == GLFW_MOUSE_BUTTON_RIGHT);
    const bool bLeft = (button == GLFW_MOUSE_BUTTON_LEFT);
    const bool bMiddle = (button == GLFW_MOUSE_BUTTON_MIDDLE);

    // Handle gEnv-states
    if (bLeft)
    {
        gEnv->stateGui->mouseButtonLeftPressed = bPress;
        gEnv->stateGui->mouseButtonLeftRelease = bRelease; // pre frame: set to false in prePollEvents()
        gEnv->stateGui->mouseButtonLeftClick = bPress;     // pre frame: set to false in prePollEvents()
    }

    const bool bCam2d  = gEnv->camera->is2d();

    if (bRight || (bCam2d && bMiddle))
    {
        if (bPress)
        {
            updateCameraState(window);
        }
        if (bRelease)
        {
            gEnv->stateGui->cameraState.clear();
        }
    }

    if (bLeft)
    {
        if (bPress) {}
        if (bRelease) {}
    }

    if (bMiddle)
    {
        if (bPress) {}
        if (bRelease) {}
    }
}

void ContextOpenGl::callbackCursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gEnv->stateGui->cameraState.isCameraMoving)
    {
        gEnv->stateGui->cameraState.rotatePosSSprevious = gEnv->stateGui->cameraState.rotatePosSSactual;
        gEnv->stateGui->cameraState.rotatePosSSactual = glm::vec2((float)xpos, (float)ypos);
    }

    gEnv->stateGui->mousePos = glm::vec2(xpos, ypos);

    // debug 
    //glm::vec2 posWS = gEnv->camera->SSToWorld2d(glm::vec2((float)xpos, (float)ypos));
    //std::cerr << "debug: " << posWS.x << " / " << posWS.y << "\n";
    //glm::vec2 posSS = gEnv->camera->worldToSS2d(posWS);
    //std::cerr << "DOUBLE CHECK: debug: " << posSS.x << " / " << posSS.y << "        Truth:" << xpos << ":" << ypos << "\n";

    // ImGui - forward position
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)xpos, (float)ypos);
}

void ContextOpenGl::callbackScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    if (ImGui::GetIO().WantCaptureMouse) return;

    const bool bCam2d = gEnv->camera->is2d();
    const bool bCam3d = gEnv->camera->is3d();

    if (gEnv->stateGui->cameraState.isCameraMoving || bCam2d)
    {
        const float cScaleFactor = 1.05f;
        const float scaleFactor = (yoffset > 0) ? cScaleFactor : (1.0f/cScaleFactor);

        if (bCam3d)
        {
            gEnv->stateGui->cameraStateSpeedTranslationFactor *= scaleFactor;
        }
        if (bCam2d)
        {
            if (gEnv->stateGui->cameraState.isCameraMoving)
            {
                gEnv->stateGui->cameraState.scale2d *= scaleFactor;
            }
            else
            {
                updateCameraStateScaleImmediate(window, scaleFactor);
            }
        }
    }
}

void ContextOpenGl::setWindowName(std::string const& name)
{
    glfwSetWindowTitle(m_window, name.c_str());
}
