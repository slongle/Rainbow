#include "gui.h"
#include "integrator.h"

// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#include "ext/imgui/imgui.h"
#include "ext/imgui/examples/imgui_impl_glfw.h"
#include "ext/imgui/examples/imgui_impl_opengl3.h"
#include <stdio.h>

// About OpenGL function loaders: modern OpenGL doesn't have a standard header file and requires individual function pointers to be loaded manually. 
// Helper libraries are often used for this purpose! Here we are supporting a few common ones: gl3w, glew, glad.
// You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h> 
#include <string>
#include <iostream>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma. 
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

RAINBOW_NAMESPACE_BEGIN

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


int show(Integrator* integrator, Scene* scene)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

                         // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

                                                           // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup Style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    ImFont* font = io.Fonts->AddFontDefault();
    //ImFont* font = io.Fonts->AddFontFromFileTTF("F:/Document/Graphics/code/Rainbow/ext/imgui/misc/fonts/Roboto-Medium.ttf", 16.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("F:/Document/Graphics/code/Rainbow/ext/imgui/misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("F:/Document/Graphics/code/Rainbow/ext/imgui/misc/fonts/DroidSans.ttf", 16.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("F:/Document/Graphics/code/Rainbow/ext/imgui/misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    IM_ASSERT(font != NULL);

    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    int image_width, image_height;

    image_width = integrator->camera->film->resolution.x;
    image_height = integrator->camera->film->resolution.y;
    
    unsigned char * image_data = new unsigned char[image_width * image_height * 4];

    GLuint my_opengl_texture;
    glGenTextures(1, &my_opengl_texture);
    glBindTexture(GL_TEXTURE_2D, my_opengl_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    Timer timer;
    bool render = true;
    bool halt = false;

    // Main loop
    while (!glfwWindowShouldClose(window)) {        
        static int counter = 0;
        for (int y = 0; y < integrator->camera->film->resolution.y; y++) {
            if (render) {                
                for (int x = 0; x < integrator->camera->film->resolution.x; x++) {
                    //if (y % 10 == 9 && x == 0) integrator->ProgressiveRender(*scene, x, y, true);
                    //else integrator->ProgressiveRender(*scene, x, y);
                    integrator->AdaptiveProgressiveRender(*scene, x, y);
                    integrator->camera->film->UpdateToUnsignedCharPointer(image_data, x, y);
                }                
            }
            else {
                y--;
            }

            // Poll and handle events (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            glfwPollEvents();
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
            ImGui::Begin("Image Test");
            ImGui::Image((void*)(intptr_t)my_opengl_texture, ImVec2(image_width, image_height));
            ImGui::End();
            
            // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
            {
                static float f = 0.0f;
                //static int counter = 0;
                static char a[35] = "filename.png";

                // Create a window called "Information" and append into it.
                ImGui::Begin("Information");                          

                // Stop / Continue Rendering
                if (render) {
                    if (ImGui::Button("Stop Rendering")) {
                        render = false;
                        timer.Stop();
                    }
                }
                else {
                    if (ImGui::Button("Continue Rendering")) {
                        render = true;
                        timer.Continue();
                    }
                }
                // Halt Button
                ImGui::SameLine(); // SameLine with Stop/Continue Rendering button
                if (ImGui::Button("Halt")) {
                    halt = true;
                    break;
                }

                // Progressive Render Counter
                ImGui::Text("Counter = %d", counter);

                // Save as Button                
                ImGui::InputText("Filename", a, 35);
                ImGui::SameLine();
                if (ImGui::Button("Save as")) {
                    ExportToPNG(std::string(a), image_data, image_width, image_height);
                }

                ImGui::Text("Rendering %5.2f%%", 100.*(y + 1) / integrator->camera->film->resolution.y);                
                ImGui::Text("Time %s", timer.LastTime());
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::End();
            }

            // Rendering
            ImGui::Render();
            int display_w, display_h;
            glfwMakeContextCurrent(window);
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwMakeContextCurrent(window);
            glfwSwapBuffers(window);            
        }
        counter++;
        if (halt) {
            break;
        }
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

int AdaptiveShow(Integrator* integrator, Scene* scene)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

                         // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

                                                           // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup Style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    ImFont* font = io.Fonts->AddFontDefault();
    //ImFont* font = io.Fonts->AddFontFromFileTTF("F:/Document/Graphics/code/Rainbow/ext/imgui/misc/fonts/Roboto-Medium.ttf", 16.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("F:/Document/Graphics/code/Rainbow/ext/imgui/misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("F:/Document/Graphics/code/Rainbow/ext/imgui/misc/fonts/DroidSans.ttf", 16.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("F:/Document/Graphics/code/Rainbow/ext/imgui/misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    IM_ASSERT(font != NULL);

    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    int image_width, image_height;

    image_width = integrator->camera->film->resolution.x;
    image_height = integrator->camera->film->resolution.y;

    unsigned char * image_data = new unsigned char[image_width * image_height * 4];

    GLuint my_opengl_texture;
    glGenTextures(1, &my_opengl_texture);
    glBindTexture(GL_TEXTURE_2D, my_opengl_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    Timer timer;
    bool render = true;
    bool halt = false;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        static int counter = 0;
        for (int y = 0; y < integrator->camera->film->resolution.y; y++) {
            if (render) {
                for (int x = 0; x < integrator->camera->film->resolution.x; x++) {
                    integrator->AdaptiveProgressiveRender(*scene, x, y);
                    integrator->camera->film->UpdateToUnsignedCharPointer(image_data, x, y);                            

                    glfwPollEvents();
                    // Start the Dear ImGui frame
                    ImGui_ImplOpenGL3_NewFrame();
                    ImGui_ImplGlfw_NewFrame();
                    ImGui::NewFrame();

                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
                    ImGui::Begin("Image Test");
                    ImGui::Image((void*)(intptr_t)my_opengl_texture, ImVec2(image_width, image_height));
                    ImGui::End();

                    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
                    {
                        static float f = 0.0f;
                        //static int counter = 0;
                        static char a[35] = "filename.png";

                        // Create a window called "Information" and append into it.
                        ImGui::Begin("Information");

                        // Stop / Continue Rendering
                        if (render) {
                            if (ImGui::Button("Stop Rendering")) {
                                render = false;
                                timer.Stop();
                            }
                        }
                        else {
                            if (ImGui::Button("Continue Rendering")) {
                                render = true;
                                timer.Continue();
                            }
                        }
                        // Halt Button
                        ImGui::SameLine(); // SameLine with Stop/Continue Rendering button
                        if (ImGui::Button("Halt")) {
                            halt = true;
                            break;
                        }

                        // Progressive Render Counter
                        ImGui::Text("Counter = %d", counter);

                        // Save as Button                
                        ImGui::InputText("Filename", a, 35);
                        ImGui::SameLine();
                        if (ImGui::Button("Save as")) {
                            ExportToPNG(std::string(a), image_data, image_width, image_height);
                        }

                        ImGui::Text("Rendering %5.2f%%", 100.*(y + 1) / integrator->camera->film->resolution.y);
                        ImGui::Text("Time %s", timer.LastTime());
                        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                        ImGui::End();
                    }


                    // Rendering
                    ImGui::Render();
                    int display_w, display_h;
                    glfwMakeContextCurrent(window);
                    glfwGetFramebufferSize(window, &display_w, &display_h);
                    glViewport(0, 0, display_w, display_h);
                    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
                    glClear(GL_COLOR_BUFFER_BIT);
                    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

                    glfwMakeContextCurrent(window);
                    glfwSwapBuffers(window);
                }
            }
            else {
                y--;
            }
        }
        counter++;
        if (halt) {
            break;
        }
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}


RAINBOW_NAMESPACE_END