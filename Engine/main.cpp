#include"main.h"


int main() {
    

    //Window
    Window window(800, 600, "Engine");
    GLFWwindow* w = window.GetNativeWindow();

    //Shaders
    Shader shader, lightShader;
    shader.Init("shader.vs", "shader.fs");      
    lightShader.Init("light.vs", "light.fs");

    Light light;
    light.Init();
    SceneObject player;

    //IMGUI
    DebugUI ui;
    FileBrowser fileBrowser;
    fileBrowser.Init("texture");
    Texture* activeTexture = nullptr;

    ui.Init(window.GetNativeWindow());

    // Background color control
    glm::vec3 bgColor(0.2f, 0.3f, 0.3f);

    ui.RegisterPanel("Scene", [&]() {
        DebugUI::TransformWidget("Light Position", light.position);
        DebugUI::ColourPicker3("Light Colour", light.color);
        DebugUI::ColourPicker3("Background Color", bgColor);
    });

    ui.RegisterPanel("Files", [&]() {
        fileBrowser.Render();
    });

    fileBrowser.SetOnSelect([&](const std::string& path) {
        delete activeTexture;
        activeTexture = new Texture(path.c_str());
    });

    // Create main cube mesh
    Mesh cubeMesh;
    cubeMesh.Initc(cubeVertices, cubeIndices);
    player.mesh = &cubeMesh;

    // Create light indicator cube
    Mesh lightCube;
    lightCube.Initc(cubeVertices, cubeIndices);
    light.mesh = &lightCube;

    //RENDERER
    Renderer render;
    render.Init();

    // CAMERA
    Camera camera;
    camera.Position = glm::vec3(0.0f, 2.0f, 5.0f);
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), 800.0f/600.0f, 0.1f, 1000.0f);

    //TIMER
    Timer t;
    
    while (!window.ShouldClose()) {
        render.SetClearColor(bgColor.r, bgColor.g, bgColor.b, 1.0f);
        render.Clear();

        ui.BeginFrame();

        if (!ui.IntroScreen())
        {
            float deltaTime = t.Delta();
            camera.ProcessKeyboard(window.GetNativeWindow(), deltaTime);

            glm::mat4 view = camera.GetViewMatrix();


            shader.use();
            shader.setMat4("view", view);
            shader.setMat4("projection", projection);
            shader.setMat4("model", glm::mat4(1.f));
            shader.setVec3("objectColor", glm::vec3(0.2f, 0.f, 0.5f));
            shader.setInt("texture0", 0);
            light.UploadToShader(shader, camera.Position);

            
            if (activeTexture)
                activeTexture->Bind(0);

            cubeMesh.Draw();
            player.movement(window, deltaTime);


            light.Render(view, projection);
        }

        DebugUI::FPSOverlay();
        ui.RenderPanels();
        ui.RenderLog();
        ui.EndFrame();

        window.SwapBuffers();
        window.PollEvents();
    }

    ui.Shutdown();
    delete activeTexture;

    return 0;
}