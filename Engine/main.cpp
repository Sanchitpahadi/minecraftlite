#include"main.h"


int main() {

    //Window
    Window window(800, 600, "Engine");

    GLFWwindow* w = window.GetNativeWindow();


    //Shaders
    Shader shader,playerShader;
    shader.Init("shader.vs", "shader.fs");      
    playerShader.Init("shader.vs", "shadert.fs");


    Light light;
    SceneObject player;

    //IMGUI
    DebugUI ui;
    FileBrowser fileBrowser;
    fileBrowser.Init("texture");
    Texture* activeTexture = nullptr;


    ui.Init(window.GetNativeWindow());

    ui.RegisterPanel("Scene", [&]() {
        DebugUI::TransformWidget("Light", light.position);
        DebugUI::ColourPicker3("Light Colour", light.color);
    });

    ui.RegisterPanel("Files", [&]() {
    fileBrowser.Render();
    });

    fileBrowser.SetOnSelect([&](const std::string& path) {
    delete activeTexture;                   // swap out old texture
    activeTexture = new Texture(path.c_str());
    });


    //OBJECT LOADING

    std::vector<float> playerVerts;
    std::vector<unsigned int> playerIndices;
    OBJLoader::Load("player.obj", playerVerts, playerIndices);


    //MESH LOADING FROM OBJECT LOADER CLASS

    Mesh playerMesh;
    playerMesh.Inito(playerVerts, playerIndices);
    printf("Verts: %zu, Indices: %zu\n", playerVerts.size(), playerIndices.size());

     
    player.mesh = &playerMesh;

    // CUBE MESH LOADING BY VERTICES

    Mesh cube;
    cube.Initc(cubeVertices, cubeIndices);  
    
    light.mesh = &cube;

    //TEXT RENDERING BUT NOW I AM DOING IT BY IMGUI BUT I MIGHT NEED IT LATTER
    TextRenderer text("roman.ttf", 45.0f, 800, 600);

    //RENDERER ALL THE RENDERING FUNCTION BY THE WAY
    Renderer render;
    render.Init();

    bool showintro = true;

    // CAMERA BY THE WAYYYY

    Camera camera;
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), 800.0f/600.0f, 0.1f, 1000.0f);

    //THE WORLD I AM CREATING LOL
    World world;
    world.Update(camera.Position.x, camera.Position.z);

    //TIMER TIME PASS DELTA TIME ONLY BUT MAYBE IN BIG USE LATTER ON
    Timer t;
    
    while (!window.ShouldClose()) {
        render.SetClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        render.Clear();

        ui.BeginFrame();

        if (!ui.IntroScreen())
        {
            float deltaTime = t.Delta();
            playerShader.use();
            camera.ProcessKeyboard(window.GetNativeWindow(), deltaTime);
            world.Update(camera.Position.x, camera.Position.z);

            glm::mat4 view = camera.GetViewMatrix();
            
            glUniformMatrix4fv(glGetUniformLocation(playerShader.ID, "view"),        1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(playerShader.ID, "projection"),  1, GL_FALSE, glm::value_ptr(projection));
            light.UploadToShader( playerShader, camera.Position); 
            glUniformMatrix4fv(glGetUniformLocation(playerShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4 (1.f)));
            glUniform3fv(glGetUniformLocation(playerShader.ID, "objectColor"),       1, glm::value_ptr(glm::vec3(0.2f, 0.f, 0.5f)));
            glUniform1i(glGetUniformLocation(playerShader.ID, "texture0"), 0);
            
            if (activeTexture)
                activeTexture->Bind(0);
            world.Render();


            shader.use();

            light.UploadToShader( shader, camera.Position); 

            player.Render(shader,view,projection);
            player.movement(window,deltaTime);




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