//
// Created by wmy on 2023/12/15.
//

#include "shader.h"

#include <memory>

VertexShader::VertexShader(const char *pFile) {
    std::string code;
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(pFile);
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        code = stream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cout << e.what() << std::endl;
    }
    glID = glCreateShader(GL_VERTEX_SHADER);

    const char* shaderCode = code.c_str();
    glShaderSource(glID, 1, &shaderCode, nullptr);
    glCompileShader(glID);

    GLint success;
    glGetShaderiv(glID, GL_COMPILE_STATUS, &success);
    if(!success) {
        GLint logLength;
        glGetShaderiv(glID, GL_INFO_LOG_LENGTH, &logLength);

        GLchar* infoLog = (GLchar*)alloca(logLength * sizeof(GLchar));
        glGetShaderInfoLog(glID, logLength, nullptr, infoLog);
        std::cout << infoLog << std::endl;
    }
}

VertexShader::~VertexShader() {
    glDeleteShader(glID);
}

FragmentShader::FragmentShader(const char *pFile) {
    std::string code;
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(pFile);
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        code = stream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cout << e.what() << std::endl;
    }
    glID = glCreateShader(GL_FRAGMENT_SHADER);
    const char* shaderCode = code.c_str();
    glShaderSource(glID, 1, &shaderCode, nullptr);
    glCompileShader(glID);

    GLint success;
    glGetShaderiv(glID, GL_COMPILE_STATUS, &success);
    if(!success) {
        GLint logLength;
        glGetShaderiv(glID, GL_INFO_LOG_LENGTH, &logLength);

        GLchar* infoLog = (GLchar*)alloca(logLength * sizeof(GLchar));
        glGetShaderInfoLog(glID, logLength, nullptr, infoLog);
        std::cout << infoLog << std::endl;
    }
}

FragmentShader::~FragmentShader() {
    glDeleteShader(glID);
}

ShaderProgram::ShaderProgram(const char *pVertexShaderFile, const char *pFragmentShaderFile) {
    VertexShader vertexShader(pVertexShaderFile);
    FragmentShader fragmentShader(pFragmentShaderFile);
    glID = glCreateProgram();
    glAttachShader(glID, vertexShader.glID);
    glAttachShader(glID, fragmentShader.glID);
    glLinkProgram(glID);

    GLint success;
    glGetProgramiv(glID, GL_COMPILE_STATUS, &success);
    if(!success) {
        GLint logLength;
        glGetProgramiv(glID, GL_INFO_LOG_LENGTH, &logLength);

        GLchar* infoLog = (GLchar*)alloca(logLength * sizeof(GLchar));
        glGetProgramInfoLog(glID, logLength, nullptr, infoLog);
        std::cout << infoLog << std::endl;
    }
}

void Shader::SetInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(glID, name.c_str()), value);
}

void Shader::SetUint(const std::string &name, unsigned int value) const {
    glUniform1ui(glGetUniformLocation(glID, name.c_str()), value);
}

void Shader::SetFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(glID, name.c_str()), value);
}

void Shader::SetVec2(const std::string &name, const glm::vec2 &vec) const {
    glUniform2fv(glGetUniformLocation(glID, name.c_str()), 1, &vec[0]);
}

void Shader::SetVec3(const std::string &name, const glm::vec3 &vec) const {
    glUniform3fv(glGetUniformLocation(glID, name.c_str()), 1, &vec[0]);
}

void Shader::SetVec4(const std::string &name, const glm::vec4 &vec) const {
    glUniform4fv(glGetUniformLocation(glID, name.c_str()), 1, &vec[0]);
}

void Shader::SetMat2(const std::string &name, const glm::mat2 &mat) const {
    glUniformMatrix2fv(glGetUniformLocation(glID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat3(const std::string &name, const glm::mat3 &mat) const {
    glUniformMatrix3fv(glGetUniformLocation(glID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(glID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
