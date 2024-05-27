//
// Created by xmmmmmovo on 12/16/2023.
//

#ifndef INCLUDE_WINDOW_75792D3F1DB6491294182AF13A06B0D3
#define INCLUDE_WINDOW_75792D3F1DB6491294182AF13A06B0D3

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "camera.hpp"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

#include <iostream>

namespace cg {
class Window {
public:
    static inline int width{1920}, height{1080};
    GLFWwindow*       window;
    // timing
    float deltaTime = 0.1f;// time between current frame and last frame
private:
    static inline Camera camera{glm::vec3(0.0f, 0.0f, 10.0f)};

    float lastFrame = 0.0f;

    // camera
    static inline float lastX      = width / 2.0f;
    static inline float lastY      = height / 2.0f;
    static inline bool  firstMouse = true;

    static void framebufferSizeCallback(GLFWwindow* window, int w, int h) {
        width  = w;
        height = h;
        glViewport(0, 0, w, h);
    }

    static void mouseCallback(GLFWwindow* window, double xposIn,
                              double yposIn) {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse) {
            lastX      = xpos;
            lastY      = ypos;
            firstMouse = false;
        }

        float const xoffset = xpos - lastX;
        float const yoffset =
                lastY -
                ypos;// reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.processMouseMovement(xoffset, yoffset);
    }


public:
    Window() {
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

        // MSAA
        glfwWindowHint(GLFW_SAMPLES, 4);

        // enable vsync
        glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
        glfwSwapInterval(1);

        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(width, height, "Carousel", nullptr, nullptr);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glfwMakeContextCurrent(window);

        // Specify the name of the callback function when an event on the window
        // is detected
        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
        glfwSetCursorPosCallback(window, mouseCallback);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << '\n';
            exit(1);
        }

        // depth test and MSAA
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_CULL_FACE);
    }

    static glm::mat4 getViewMatrix() { return camera.getViewMatrix(); }
    static glm::mat4 getProjectionMatrix() {
        return camera.getProjectionMatrix(width, height);
    }

    static Camera const& getCamera() { return camera; }

    void processEvent() {
        float current_frame = static_cast<float>(glfwGetTime());
        deltaTime           = current_frame - lastFrame;
        lastFrame           = current_frame;
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera.processKeyboard(Camera::CameraMovement::FORWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camera.processKeyboard(Camera::CameraMovement::BACKWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.processKeyboard(Camera::CameraMovement::LEFT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.processKeyboard(Camera::CameraMovement::RIGHT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            camera.processKeyboard(Camera::CameraMovement::UP, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            camera.processKeyboard(Camera::CameraMovement::DOWN, deltaTime);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
};
}// namespace cg


#endif// INCLUDE_WINDOW_75792D3F1DB6491294182AF13A06B0D3
