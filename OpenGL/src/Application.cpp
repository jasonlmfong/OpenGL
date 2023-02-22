#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };
    
    std::ifstream stream(filepath);
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    std::string line;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else if (type != ShaderType::NONE)
        {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // Error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)malloc(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile" << 
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment") 
            << "shader" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // version 3.3 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 960, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(10);

    /* GLEW init after the context and window exist */
    if (glewInit() != GLEW_OK)
        std::cout << "Error \n";

    // show version
    // std::cout << glGetString(GL_VERSION) << "\n";
    {
        float positions[] = {
            -0.5f, -0.5f,
             0.5f, -0.5f,
             0.5f,  0.5f,
            -0.5f,  0.5f
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        // vao
        unsigned int vao;
        /* Allocate and assign a Vertex Array Object to our handle */
        GLCall(glGenVertexArrays(1, &vao));
        /* Bind our Vertex Array Object as the current used object */
        GLCall(glBindVertexArray(vao));

        VertexBuffer vb(positions, 4 * 2 * sizeof(float));

        // links the vertex array wiht the vertex buffer
        GLCall(glEnableVertexAttribArray(0));
        GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

        IndexBuffer ib(indices, 6);

        ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

        // std::cout << source.VertexSource << "\n";

        unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
        GLCall(glUseProgram(shader));

        GLCall(int location = glGetUniformLocation(shader, "u_Color"));
        ASSERT(location != -1);
        GLCall(glUniform4f(location, 0.26f, 0.52f, 0.96f, 1.0f));

        // unbind
        GLCall(glBindVertexArray(0));
        GLCall(glUseProgram(0));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

        float g = 0.0f;
        float increment = 0.05f;
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClear(GL_COLOR_BUFFER_BIT));

            GLCall(glUseProgram(shader));
            GLCall(glUniform4f(location, 0.26f, g, 0.96f, 1.0f));

            GLCall(glBindVertexArray(vao));
            ib.Bind();

            /* Create a shape */
            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

            if (g > 1.0f)
                increment = -0.5f;
            else if (g < 0.0f)
                increment = 0.5f;

            g += increment;

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            GLCall(glfwPollEvents());
        }

        glDeleteProgram(shader);
    }
    glfwTerminate();
    return 0;
}