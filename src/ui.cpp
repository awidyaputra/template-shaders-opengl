#include "common.h"

#include "ui.h"

#include <iostream>

namespace ui
{
    namespace _
    {
    } // namespace _

    void DrawMainScreen()
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                 ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground |
                                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                 ImGuiWindowFlags_NoScrollbar;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::Begin("Main", nullptr, flags);

        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();
        ImVec2 window_center =
            ImVec2(window_pos.x + window_size.x * 0.5f, window_pos.y + window_size.y * 0.5f);
        ImVec2 second_circle =
            ImVec2(window_pos.x + window_size.x * 0.55f, window_pos.y + window_size.y * 0.5f);
        ImGui::GetForegroundDrawList()->AddCircle(window_center, window_size.x * 0.1f,
                                                  IM_COL32(255, 0, 0, 200), 0, 10 + 4);
        ImGui::GetForegroundDrawList()->AddCircle(second_circle, window_size.x * 0.1f,
                                                  IM_COL32(255, 0, 0, 200), 0, 10 + 4);

        ImVec2 triangle_draw_area_size = ImVec2(window_size.x - 10, window_size.y - 10);

        ImGui::End();
    }

    namespace _
    {

        const char* vertexShaderSource = "#version 330 core\n"
                                         "layout (location = 0) in vec3 aPos;\n"
                                         "void main()\n"
                                         "{\n"
                                         "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                         "}\0";
        const char* fragmentShaderSource = "#version 330 core\n"
                                           "layout(location = 0) out vec4 FragColor;\n"
                                           "void main()\n"
                                           "{\n"
                                           "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                           "}\n\0";

        const GLfloat vertex_buffer_data[] = {
            -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        };

    } // namespace _

    void CanvasInitShader(CanvasGLElements* canvas_gl_elements)
    {
        using namespace _;

        // vertex shader
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertex_shader);
        // check for shader compile errors
        int success;
        char info_log[512];
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
        }
        // fragment shader
        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragment_shader);
        // check for shader compile errors
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
        }
        // link shaders
        GLuint shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);
        glLinkProgram(shader_program);
        // check for linking errors
        glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader_program, 512, NULL, info_log);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
        }
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        GLuint VBO, VAO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then
        // configure vertex attributes(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data,
                     GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        canvas_gl_elements->ShaderProgram = shader_program;
        canvas_gl_elements->VertexArrayObject = VAO;
    }

    void CanvasInvalidateTexture(CanvasGLElements* canvas_gl_elements, GLsizei width,
                                 GLsizei height)
    {
        if (canvas_gl_elements->FramebufferID)
        {
            glDeleteFramebuffers(1, &canvas_gl_elements->FramebufferID);
            glDeleteTextures(1, &canvas_gl_elements->ColorAttachmentID);
            glDeleteRenderbuffers(1, &canvas_gl_elements->DepthAndStencilAttachmentID);
        }

        // framebuffer configuration
        // -------------------------
        glGenFramebuffers(1, &canvas_gl_elements->FramebufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, canvas_gl_elements->FramebufferID);

        // create a color attachment texture
        glGenTextures(1, &canvas_gl_elements->ColorAttachmentID);
        glBindTexture(GL_TEXTURE_2D, canvas_gl_elements->ColorAttachmentID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               canvas_gl_elements->ColorAttachmentID, 0);

        // Allocate memeory for texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        // create a renderbuffer object for depth and stencil attachment (we won't be sampling
        // these)
        glGenRenderbuffers(1, &canvas_gl_elements->DepthAndStencilAttachmentID);
        glBindRenderbuffer(GL_RENDERBUFFER, canvas_gl_elements->DepthAndStencilAttachmentID);
        glRenderbufferStorage(
            GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width,
            height); // use a single renderbuffer object for both a depth AND stencil buffer.
        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
            canvas_gl_elements->DepthAndStencilAttachmentID); // now actually attach it

        // now that we actually created the framebuffer and added all attachments we want to check
        // if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, canvas_gl_elements->FramebufferID);
        glEnable(
            GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

        glViewport(0, 0, width, height);

        // make sure we clear the framebuffer's content
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(canvas_gl_elements->ShaderProgram);
        glBindVertexArray(
            canvas_gl_elements->VertexArrayObject); // seeing as we only have a single VAO there's
                                                    // no need to bind it every time, but we'll do
                                                    // so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glBindVertexArray(0); // no need to unbind it every time

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void CanvasDeleteTexture(CanvasGLElements* canvas_gl_elements)
    {
        glDeleteFramebuffers(1, &canvas_gl_elements->FramebufferID);
        glDeleteTextures(1, &canvas_gl_elements->ColorAttachmentID);
        glDeleteRenderbuffers(1, &canvas_gl_elements->DepthAndStencilAttachmentID);
    }

} // namespace ui
