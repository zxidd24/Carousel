

#ifndef INCLUDE_CAMERA_D1E3E3B66116454BB27435CD57D7EDCF
#define INCLUDE_CAMERA_D1E3E3B66116454BB27435CD57D7EDCF

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace cg {

class Camera {
private:
    static constexpr float YAW         = -90.0f;
    static constexpr float PITCH       = 0.0f;
    static constexpr float SPEED       = 15.0f;
    static constexpr float SENSITIVITY = 0.1f;
    static constexpr float ZOOM        = 45.0f;

public:
    enum class CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right{};
    glm::vec3 world_up{};

    float yaw;
    float pitch;

    float movement_speed;
    float mouse_sensitivity;
    float zoom;

    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW,
                    float pitch = PITCH)
        : position(position), front(glm::vec3(0.0f, 0.0f, -1.0f)), up(up),
          yaw(yaw), pitch(pitch), movement_speed(SPEED),
          mouse_sensitivity(SENSITIVITY), zoom(ZOOM), world_up(up) {
        updateCameraVectors();
    }

    glm::mat4 getViewMatrix() {
        return glm::lookAt(position, position + front, up);
    }

    glm::mat4 getProjectionMatrix(int width, int height) {
        return glm::perspective(glm::radians(ZOOM),
                                (float) width / (float) height, 0.1f, 100.0f);
    }

    void processKeyboard(CameraMovement direction, float delta_time) {
        float delta_speed = movement_speed * delta_time;
        if (direction == CameraMovement::FORWARD) {
            position += front * delta_speed;
        }
        if (direction == CameraMovement::BACKWARD) {
            position -= front * delta_speed;
        }
        if (direction == CameraMovement::LEFT) {
            position -= right * delta_speed;
        }
        if (direction == CameraMovement::RIGHT) {
            position += right * delta_speed;
        }
        if (direction == CameraMovement::UP) { position += up * delta_speed; }
        if (direction == CameraMovement::DOWN) { position -= up * delta_speed; }
    }

    void processMouseMovement(float xoffset, float yoffset,
                              bool constrain_pitch = true) {
        xoffset *= mouse_sensitivity;
        yoffset *= mouse_sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (constrain_pitch) {
            if (pitch > 89.0f) { pitch = 89.0f; }
            if (pitch < -89.0f) { pitch = -89.0f; }
        }

        updateCameraVectors();
    }

    void processMouseScroll(float yoffset) {
        if (zoom >= 1.0f && zoom <= 45.0f) { zoom -= yoffset; }
        if (zoom <= 1.0f) { zoom = 1.0f; }
        if (zoom >= 45.0f) { zoom = 45.0f; }
    }

private:
    void updateCameraVectors() {
        glm::vec3 front_local;
        front_local.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front_local.y = sin(glm::radians(pitch));
        front_local.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front         = glm::normalize(front_local);
        right         = glm::normalize(glm::cross(front, world_up));
        up            = glm::normalize(glm::cross(right, front));
    }
};

}// namespace cg

#endif// INCLUDE_CAMERA_D1E3E3B66116454BB27435CD57D7EDCF
