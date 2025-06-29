#include "ShaderManager.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

ShaderManager::~ShaderManager() {
  for (auto& kv : m_shaders)
    glDeleteProgram(kv.second);
}

bool ShaderManager::loadShader(const std::string& name,
                               const std::string& vertSrc,
                               const std::string& fragSrc)
{
  GLuint vs = compileShader(vertSrc, GL_VERTEX_SHADER);
  if (!vs) { std::cerr<<"Vertex compile failed for "<<name<<"\n"; return false; }

  GLuint fs = compileShader(fragSrc, GL_FRAGMENT_SHADER);
  if (!fs) { std::cerr<<"Fragment compile failed for "<<name<<"\n"; glDeleteShader(vs); return false; }

  GLuint prog = linkProgram(vs, fs);
  glDeleteShader(vs);
  glDeleteShader(fs);
  if (!prog) { std::cerr<<"Link failed for "<<name<<"\n"; return false; }

  m_shaders[name] = prog;
  std::cout<<"Loaded shader '"<<name<<"' (program "<<prog<<")\n";
  return true;
}

void ShaderManager::useShader(const std::string& name) {
  auto it = m_shaders.find(name);
  if (it == m_shaders.end()) {
    std::cerr<<"ShaderManager::useShader: '"<<name<<"' not loaded\n";
    return;
  }
  m_currentShader = it->second;
  glUseProgram(m_currentShader);
}

void ShaderManager::setUniformVec2(const std::string& name, const glm::vec2& v) {
  if (m_currentShader == 0) {
    std::cerr<<"setUniformVec2 called with no shader bound\n";
    return;
  }
  GLint loc = glGetUniformLocation(m_currentShader, name.c_str());
  if (loc < 0) {
    std::cerr<<"Uniform '"<<name<<"' not found in shader "<<m_currentShader<<"\n";
    return;
  }
  glUniform2fv(loc, 1, glm::value_ptr(v));
}

void ShaderManager::setUniformVec3(const std::string& name, const glm::vec3& v) {
  GLint loc = glGetUniformLocation(m_currentShader, name.c_str());
  if (loc >= 0) glUniform3fv(loc, 1, glm::value_ptr(v));
}

void ShaderManager::setUniformMat4(const std::string& name, const glm::mat4& m) {
  GLint loc = glGetUniformLocation(m_currentShader, name.c_str());
  if (loc >= 0) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}

void ShaderManager::setUniformFloat(const std::string& name, float f) {
  GLint loc = glGetUniformLocation(m_currentShader, name.c_str());
  if (loc >= 0) glUniform1f(loc, f);
}

void ShaderManager::setUniformInt(const std::string& name, int i) {
  GLint loc = glGetUniformLocation(m_currentShader, name.c_str());
  if (loc >= 0) glUniform1i(loc, i);
}

GLuint ShaderManager::compileShader(const std::string& src, GLenum type) {
  GLuint s = glCreateShader(type);
  const char* c = src.c_str();
  glShaderSource(s, 1, &c, nullptr);
  glCompileShader(s);

  GLint ok;
  glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    char buf[512];
    glGetShaderInfoLog(s, 512, nullptr, buf);
    std::cerr<<"Shader compile error: "<<buf<<"\n";
    glDeleteShader(s);
    return 0;
  }
  return s;
}

GLuint ShaderManager::linkProgram(GLuint vs, GLuint fs) {
  GLuint p = glCreateProgram();
  glAttachShader(p, vs);
  glAttachShader(p, fs);
  glLinkProgram(p);

  GLint ok;
  glGetProgramiv(p, GL_LINK_STATUS, &ok);
  if (!ok) {
    char buf[512];
    glGetProgramInfoLog(p, 512, nullptr, buf);
    std::cerr<<"Program link error: "<<buf<<"\n";
    glDeleteProgram(p);
    return 0;
  }
  return p;
}
