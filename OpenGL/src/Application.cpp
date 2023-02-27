#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

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
    window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);
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
    std::cout << glGetString(GL_VERSION) << "\n";

    {
        float positions[] = {
            -50.0f, -50.0f, 0.0f, 0.0f, // 0
             50.0f, -50.0f, 1.0f, 0.0f, // 1
             50.0f,  50.0f, 1.0f, 1.0f, // 2
            -50.0f,  50.0f, 0.0f, 1.0f  // 3
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        // blending
        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        VertexArray va;
        VertexBuffer vb(positions, 4 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        // links the vertex array with the vertex buffer
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indices, 6);

        glm::mat4 proj = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(300, 200, 0));
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));;

        // std::cout << source.VertexSource << "\n";

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.26f, 0.52f, 0.96f, 1.0f);

        // load texture
        Texture texture("res/textures/Penguin.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        // unbind
        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        Renderer renderer;

        // Setup Dear ImGui context
        ImGui::CreateContext();
        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init();
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        //translate view
        glm::vec3 translation1(300, 200, 0);
        glm::vec3 translation2(600, 300, 0);

        //color changing
        float g = 0.0f;
        float increment = 0.05f;
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            // feed inputs to dear imgui, start new frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            shader.Bind();
            shader.SetUniform4f("u_Color", 0.26f, g, 0.96f, 1.0f);

            {
                glm::mat4 view = glm::translate(glm::mat4(1.0f), translation1);
                glm::mat4 mvp = proj * view * model;
                shader.SetUniformMat4f("u_MVP", mvp);

                /* bind va and ib, then create a shape */
                renderer.Draw(va, ib, shader);
            }

            /* draw a second shape using a second MVP*/
            {
                glm::mat4 view = glm::translate(glm::mat4(1.0f), translation2);
                glm::mat4 mvp = proj * view * model;
                shader.SetUniformMat4f("u_MVP", mvp);
                renderer.Draw(va, ib, shader);
            }

            if (g > 1.0f)
                increment = -0.5f;
            else if (g < 0.0f)
                increment = 0.5f;

            g += increment;

            // render your GUI
            {
                ImGui::Begin("Hello, World!");
                ImGui::SliderFloat3("Translation 1", &translation1.x, 0.0f, 1080.0f);
                ImGui::SliderFloat3("Translation 2", &translation2.x, 0.0f, 1080.0f);
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::End();
            }

            // Render dear imgui into screen
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            GLCall(glfwPollEvents());
        }
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    //ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}