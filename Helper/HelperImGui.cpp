#include "HelperImGui.h"

#include <string>

//#include <glad/gl.h>
#include <imgui/imgui.h>
#include <imgui_glfw/imgui_impl_glfw.h>
#include <imgui_glfw/imgui_impl_opengl2.h>

#include "../Globals.h"

namespace Helper
{

    void ImGuiInit(GLFWwindow* window)
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

        // set viewport
        // io.DisplaySize = ImVec2((float)width, (float)height);


        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Setup Platform/Renderer backends
        const bool instantCallbacks = false;
        ImGui_ImplGlfw_InitForOpenGL(window, instantCallbacks);
        //ImGui_ImplGLUT_InstallFuncs();
        ImGui_ImplOpenGL2_Init();

        // Load Fonts
        // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
        // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
        // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
        // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
        // - Read 'docs/FONTS.md' for more instructions and details.
        // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
        //io.Fonts->AddFontDefault();
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
        //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
        //IM_ASSERT(font != NULL);

        // App Setup
        ImGui::GetIO().FontGlobalScale = Globals::GUI_FONT_SCALE;
    }

    void ImGuiExit()
    {
        // Cleanup
        ImGui_ImplOpenGL2_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiFrameStart()
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiFrameEnd()
    {
        // Rendering
        ImGui::Render();
        ImGuiIO& io = ImGui::GetIO();

        //glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
        //static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        //glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        //glClear(GL_COLOR_BUFFER_BIT);
        //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound, but prefer using the GL3+ code.
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    }

    void ImGuiShowSimpleWindow(bool& showSimpleWindow)
    {
        static float f = 0.0f;
        static int counter = 0;


        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Another Window", &showSimpleWindow);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::Begin("Another Window", &showSimpleWindow);
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me")) { showSimpleWindow = false; }
        ImGui::End();

    }

    void MySliderDouble(const char* label, double* v, double v_min, double v_max, const char* format, ImGuiSliderFlags flags)
    {
        ImGui::SliderScalar(label, ImGuiDataType_Double, v, &v_min, &v_max, format, flags);
    }

    void ImGuiParticleSizeParameter(std::string const& pre, float& particleSize)
    {
        ImGui::SliderFloat((pre + "PS PointSize").c_str(), &particleSize, 1.0f, 10.0f);
    }

    void ImGuiParticleRenderStyleParameter(std::string const& pre, int& particleRenderStyle)
    {
        ImGui::SliderInt((pre + "PS RenderStyle").c_str(), &particleRenderStyle, 0, 2);
        switch (particleRenderStyle)
        {
        case 0: ImGui::Text("Colors"); break;
        case 1: ImGui::Text("Normals"); break;
        case 2: ImGui::Text("NormalsAsColors"); break;
        }

    }
}