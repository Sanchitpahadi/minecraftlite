#include"Mesh.h"
#include"shader.h"
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#include <fstream>
#include <sstream>
#include <string>
class Light {
public:
    glm::vec3 position = glm::vec3(1.f, 20.f, 1.f);
    glm::vec3 color    = glm::vec3(1.f, 1.f, 1.f);
    Mesh*   mesh;
    Shader    lightShader;

    Light()
    {
    lightShader.Init("light.vs", "light.fs");
    }


    void UploadToShader(Shader& shader, glm::vec3& viewPos) {
        glUniform3fv(glGetUniformLocation(shader.ID, "lightPos"),   1, glm::value_ptr(position));
        glUniform3fv(glGetUniformLocation(shader.ID, "lightColor"), 1, glm::value_ptr(color));
        glUniform3fv(glGetUniformLocation(shader.ID, "viewPos"),    1, glm::value_ptr(viewPos));
    }

    void Render(glm::mat4& view, glm::mat4& projection) {
        if (!mesh) return;
        lightShader.use();  // ← renamed to lightShader everywhere
        glm::mat4 m = glm::translate(glm::mat4(1.f), position);
        m = glm::scale(m, glm::vec3(0.2f));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"),      1, GL_FALSE, glm::value_ptr(m));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "view"),       1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(glGetUniformLocation(lightShader.ID, "color"), 1, glm::value_ptr(color));

        mesh->Draw();
    }
};