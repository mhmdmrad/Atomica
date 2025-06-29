#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @brief Manages the 3D camera for the simulation viewport.
 * 
 * This class handles camera position, orientation, and provides
 * view and projection matrices for rendering.
 */
class Camera {
public:
    /**
     * @brief Constructs a new Camera object.
     * 
     * @param position The initial position of the camera.
     * @param target The initial target point the camera looks at.
     * @param up The up vector for the camera.
     */
    Camera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 10.0f),
           const glm::vec3& target = glm::vec3(0.0f, 0.0f, 0.0f),
           const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));

    /**
     * @brief Gets the view matrix.
     * 
     * @return The view matrix.
     */
    glm::mat4 getViewMatrix() const;

    /**
     * @brief Gets the projection matrix.
     * 
     * @return The projection matrix.
     */
    glm::mat4 getProjectionMatrix() const;

    /**
     * @brief Sets the camera position.
     * 
     * @param position The new camera position.
     */
    void setPosition(const glm::vec3& position) { m_position = position; }

    /**
     * @brief Gets the camera position.
     * 
     * @return The camera position.
     */
    const glm::vec3& getPosition() const { return m_position; }

    /**
     * @brief Sets the camera target.
     * 
     * @param target The new camera target.
     */
    void setTarget(const glm::vec3& target) { m_target = target; }

    /**
     * @brief Gets the camera target.
     * 
     * @return The camera target.
     */
    const glm::vec3& getTarget() const { return m_target; }

    /**
     * @brief Updates the projection matrix with new aspect ratio.
     * 
     * @param aspectRatio The new aspect ratio (width/height).
     */
    void setAspectRatio(float aspectRatio);

    /**
     * @brief Processes mouse movement for camera rotation.
     * 
     * @param xOffset The horizontal mouse movement.
     * @param yOffset The vertical mouse movement.
     */
    void processMouseMovement(float xOffset, float yOffset);

    /**
     * @brief Processes mouse scroll for camera zoom.
     * 
     * @param yOffset The scroll wheel offset.
     */
    void processMouseScroll(float yOffset);

private:
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;
    
    // Camera parameters
    float m_fov;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;
    
    // Mouse sensitivity
    float m_mouseSensitivity;
    float m_scrollSensitivity;
    
    // Spherical coordinates for orbit camera
    float m_radius;
    float m_theta; // Azimuthal angle
    float m_phi;   // Polar angle
    
    /**
     * @brief Updates the camera position based on spherical coordinates.
     */
    void updateCameraPosition();
};

#endif // CAMERA_H

