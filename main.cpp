#include "model.hpp"
#include "shader.hpp"
#include "window.hpp"

// Time
float deltaTime = 0.1f; // Time difference between the current frame and the last frame
float lastFrame = 0.0f;

float rotSpeed = 10.0f; // Rotation speed
float cSpeed = 1.0f;   // Movement speed

int main(int argc, char* argv[]) {
    cg::Window window{}; // Create a window object

    cg::Shader forward_shader("res/shaders/forward.vert",
        "res/shaders/forward.frag"); // Load shader program

    cg::Model cube_model("res/models/cube.obj");             // Load cube model
    cg::Model cylinder_model("res/models/cylinder.obj");     // Load cylinder model

    cg::ModelMatrix column_model_matrix{ glm::vec3{0.0f},
        glm::vec3(0.1f, 1.2f, 0.1f),
        glm::vec3{0.0f} }; // Set the model matrix for the column

    std::vector<cg::ModelMatrix> hz_model_matrix(
        3, cg::ModelMatrix{ glm::vec3(0.0f, 1.0f, 0.0),
            glm::vec3(0.05f, 0.05f, 1.5f),
            glm::vec3{0.0f} }); // Create a vector of horizontal model matrices

    cg::ModelMatrix const panel_model_matrix{
        glm::vec3{0.0f, -1.0f, 0.0f}, glm::vec3(2.0f, 0.1f, 2.0f),
        glm::vec3{0.0f} }; // Set the model matrix for the panel

    cg::ModelMatrix support_model_matrix{
        glm::vec3{0.0f}, glm::vec3(0.05, 1.1, 0.05),
        glm::vec3{0.0f} }; // Set the model matrix for the support

    // Rotate the horizontal model matrices
    hz_model_matrix[0].applyRotate(glm::vec3(1.0f, 0.0f, 0.0f),
        glm::radians(0.0f));
    hz_model_matrix[1].applyRotate(glm::vec3(1.0f, 0.0f, 0.0f),
        glm::radians(60.0f));
    hz_model_matrix[2].applyRotate(glm::vec3(1.0f, 0.0f, 0.0f),
        glm::radians(120.0f));

    float radians[3]{ glm::radians(0.0f), glm::radians(60.0f),
                     glm::radians(120.0f) }; // Array to store rotation angles

    glm::vec3 const color0{ 1.0, 0.843, 0.0 }; // Set colors
    glm::vec3 const color1{ 1.0, 1.0, 1.0 };
    glm::vec3 const color2{ 0.0, 1.0, 0.0 };

    glm::vec3 const rotate_vec{ 1.0f, 0.0f, 0.0f }; // Rotation vector

    cg::Model horse_model("res/models/cylinder-body.obj"); // Load the horse model

    cg::ModelMatrix body_model_matrix{ glm::vec3{0.0f}, glm::vec3(1.0f),
                                      glm::vec3{0.0f} }; // Set the model matrix for the horse's body

    cg::ModelMatrix head_model_matrix{ glm::vec3{0.0, 0.34, 0.38},
                                      glm::vec3(0.05f, 0.32f, 0.05f),
                                      glm::vec3{0.0f} }; // Set the model matrix for the horse's head
    horse_model.combineModel(cube_model, head_model_matrix); // Combine models

    cg::ModelMatrix face_model_matrix{ glm::vec3{0.0f, 0.70f, 0.65f},
                                      glm::vec3(0.05f, 0.05f, 0.32f),
                                      glm::vec3(0.0f) }; // Set the model matrix for the horse's face
    horse_model.combineModel(cube_model, face_model_matrix);

    cg::ModelMatrix f1_model_matrix{ glm::vec3{0.11, -0.2, -0.35},
                                    glm::vec3(0.05f, 0.32f, 0.05f),
                                    glm::vec3{0.0f} }; // Set the model matrix for the horse's front left leg
    horse_model.combineModel(cube_model, f1_model_matrix);

    cg::ModelMatrix f2_model_matrix{ glm::vec3{-0.11, -0.2, -0.35},
                                    glm::vec3(0.05f, 0.32f, 0.05f),
                                    glm::vec3{0.0f} }; // Set the model matrix for the horse's front right leg
    horse_model.combineModel(cube_model, f2_model_matrix);

    cg::ModelMatrix f3_model_matrix{ glm::vec3{-0.16, -0.2, 0.46},
                                    glm::vec3(0.05f, 0.32f, 0.05f),
                                    glm::vec3{0.0f} }; // Set the model matrix for the horse's back left leg
    horse_model.combineModel(cube_model, f3_model_matrix);

    cg::ModelMatrix f4_model_matrix{ glm::vec3{0.11, -0.2, 0.43},
                                    glm::vec3(0.05f, 0.32f, 0.05f),
                                    glm::vec3{0.0f} }; // Set the model matrix for the horse's back right leg
    horse_model.combineModel(cube_model, f4_model_matrix);

    while (!glfwWindowShouldClose(window.window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 const view = window.getViewMatrix();
        glm::mat4 const projection = window.getProjectionMatrix();
        glm::mat4 const view_projection = projection * view;

        forward_shader.use();
        forward_shader.setMat4("viewProjection", view_projection);
        forward_shader.setVec3("camPos", window.getCamera().position);

        cube_model.draw(forward_shader, color0, column_model_matrix);
        cylinder_model.draw(forward_shader, color0, panel_model_matrix);

        for (int i = 0; i < 3; ++i) {
            // Render 6 columns
            float add_radians = glm::radians(rotSpeed * deltaTime);

            static float r = 1.5f;

            // Calculate x and z coordinates
            radians[i] += add_radians;

            float x = r * sin(radians[i]);
            float y = r * cos(radians[i]);

            support_model_matrix.setTranslate(glm::vec3(x, 0.0f, y));
            cube_model.draw(forward_shader, color1, support_model_matrix);

            // Center symmetry
            support_model_matrix.setTranslate(glm::vec3(-x, 0.0f, -y));
            cube_model.draw(forward_shader, color1, support_model_matrix);

            body_model_matrix.setTranslate(glm::vec3(x, -0.3f, y));
            body_model_matrix.setRotate(rotate_vec, radians[i]);
            body_model_matrix.applyRotate(glm::vec3(1.0f, 0.0f, 0.0f),
                glm::radians(90.0f));
            horse_model.draw(forward_shader, color2, body_model_matrix);

            // Center symmetry
            body_model_matrix.setTranslate(glm::vec3(-x, -0.3f, -y));
            body_model_matrix.setRotate(rotate_vec, radians[i]);
            body_model_matrix.applyRotate(glm::vec3(1.0f, 0.0f, 0.0f),
                glm::radians(-90.0f));
            horse_model.draw(forward_shader, color2, body_model_matrix);

            hz_model_matrix[i].applyRotate(rotate_vec,
                glm::radians(rotSpeed * deltaTime));
            cube_model.draw(forward_shader, color1, hz_model_matrix[i]);
        }

        window.processEvent(); // Process window events
    }

    return 0;
}
