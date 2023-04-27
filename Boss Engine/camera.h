#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera
{
    public:
        // Camera Attributes
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;
        // Euler Angles
        float Yaw;
        float Pitch;
        // Camera options
        float MovementSpeed;
        float MouseSensitivity;
        float Zoom;

        float centerX;
        float centerY;

        Camera()
        {
            Position = glm::vec3(0.0f, 0.0f, 0.0f);
            WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
            Yaw = 0.0f;
            Pitch = 0.0f;
            Front = glm::vec3(0.0f, 0.0f, -1.0f);
            MovementSpeed = 5.5f;
            MouseSensitivity = 0.1f;
            Zoom = 60.0f;

            updateCameraVectors();
        }

        glm::mat4 GetViewMatrix()
        {
            return glm::lookAt(Position, Position + Front, Up);
        }

        // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        void ProcessKeyboard(GLFWwindow *window, float deltaTime)
        {
            float velocity = MovementSpeed * deltaTime;

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                Position += Front * velocity;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                Position -= Front * velocity;
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                Position -= Right * velocity;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                Position += Right * velocity;
        }

        // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        void ProcessMouseMovement(GLFWwindow* window, double xpos, double ypos)
        {

            Yaw   += (xpos - centerX) * MouseSensitivity;
            Pitch += (centerY - ypos) * MouseSensitivity;

            glfwSetCursorPos(window,centerX,centerY);

            // Make sure that when pitch is out of bounds, screen doesn't get flipped

                if (Pitch > 89.0f)
                    Pitch = 89.0f;
                if (Pitch < -89.0f)
                    Pitch = -89.0f;

            // Update Front, Right and Up Vectors using the updated Euler angles
            updateCameraVectors();
        }

        // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void ProcessMouseScroll(float yoffset)
        {
            if (Zoom >= 1.0f && Zoom <= 80.0f)
                Zoom -= yoffset;
            if (Zoom <= 1.0f)
                Zoom = 1.0f;
            if (Zoom >= 80.0f)
                Zoom = 80.0f;
        }

    private:
        // Calculates the front vector from the Camera's (updated) Euler Angles
        void updateCameraVectors()
        {
            // Calculate the new Front vector
            glm::vec3 front;
            front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            front.y = sin(glm::radians(Pitch));
            front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            Front = glm::normalize(front);
            // Also re-calculate the Right and Up vector
            Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
            Up    = glm::normalize(glm::cross(Right, Front));
        }

};

#endif
