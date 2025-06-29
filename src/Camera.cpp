#include "Camera.h"
#include <algorithm>
#include <cmath>

Camera::Camera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
    : m_position(position),
      m_target(target),
      m_up(up),
      m_fov(45.0f),
      m_aspectRatio(16.0f / 9.0f),
      m_nearPlane(0.1f),
      m_farPlane(100.0f),
      m_mouseSensitivity(0.005f),
      m_scrollSensitivity(1.0f) {
    
    // Initialize spherical coordinates
    glm::vec3 direction = m_position - m_target;
    m_radius = glm::length(direction);
    m_theta = atan2(direction.z, direction.x);
    m_phi = acos(direction.y / m_radius);
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(m_position, m_target, m_up);
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearPlane, m_farPlane);
}

void Camera::setAspectRatio(float aspectRatio) {
    m_aspectRatio = aspectRatio;
}

void Camera::processMouseMovement(float xOffset, float yOffset) {
    m_theta += xOffset * m_mouseSensitivity;
    m_phi -= yOffset * m_mouseSensitivity;
    
    // Constrain phi to avoid flipping
    m_phi = std::clamp(m_phi, 0.1f, static_cast<float>(M_PI) - 0.1f);
    
    updateCameraPosition();
}

void Camera::processMouseScroll(float yOffset) {
    m_radius -= yOffset * m_scrollSensitivity;
    m_radius = std::clamp(m_radius, 1.0f, 50.0f);
    
    updateCameraPosition();
}

void Camera::updateCameraPosition() {
    // Convert spherical coordinates to Cartesian
    float x = m_radius * sin(m_phi) * cos(m_theta);
    float y = m_radius * cos(m_phi);
    float z = m_radius * sin(m_phi) * sin(m_theta);
    
    m_position = m_target + glm::vec3(x, y, z);
}

