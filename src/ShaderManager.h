#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <GL/glew.h>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

/**
 * @brief Manages OpenGL shaders for the renderer.
 * 
 * This class handles loading, compiling, and managing shader programs
 * used for rendering different elements of the simulation.
 */
class ShaderManager {
public:
     void useShader(const std::string& name);
	 void setUniformVec2(const std::string& name, const glm::vec2& v);
     /**	
     * @brief Constructs a new ShaderManager object.
     */
    ShaderManager() = default;

    /**
     * @brief Destroys the ShaderManager object.
     */
    ~ShaderManager();

    /**
     * @brief Loads and compiles a shader program.
     * 
     * @param name The name to associate with the shader program.
     * @param vertexSource The vertex shader source code.
     * @param fragmentSource The fragment shader source code.
     * @return True if the shader was loaded successfully, false otherwise.
     */
    bool loadShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);

    /**
     * @brief Sets a uniform mat4 value in the currently active shader.
     * 
     * @param name The name of the uniform.
     * @param value The matrix value to set.
     */
    void setUniformMat4(const std::string& name, const glm::mat4& value);

    /**
     * @brief Sets a uniform vec3 value in the currently active shader.
     * 
     * @param name The name of the uniform.
     * @param value The vector value to set.
     */
    void setUniformVec3(const std::string& name, const glm::vec3& value);
	
    /**
     * @brief Sets a uniform float value in the currently active shader.
     * 
     * @param name The name of the uniform.
     * @param value The float value to set.
     */
    void setUniformFloat(const std::string& name, float value);

    /**
     * @brief Sets a uniform int value in the currently active shader.
     * 
     * @param name The name of the uniform.
     * @param value The int value to set.
     */
    void setUniformInt(const std::string& name, int value);

private:
    std::unordered_map<std::string, GLuint> m_shaders;
    GLuint m_currentShader = 0;

    /**
     * @brief Compiles a shader from source code.
     * 
     * @param source The shader source code.
     * @param type The type of shader (GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, etc.).
     * @return The compiled shader ID, or 0 if compilation failed.
     */
    GLuint compileShader(const std::string& source, GLenum type);

    /**
     * @brief Links vertex and fragment shaders into a program.
     * 
     * @param vertexShader The compiled vertex shader ID.
     * @param fragmentShader The compiled fragment shader ID.
     * @return The linked shader program ID, or 0 if linking failed.
     */
    GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);
};

#endif // SHADER_MANAGER_H
