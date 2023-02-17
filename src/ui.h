#pragma once

#include <string>

namespace ui
{
    namespace _
    {
        unsigned int InitShaderProgram();
        void RenderToTextureTriangle(ImVec2 draw_area_size);
    } // namespace _

    struct AppWindow
    {
        ImVec2 Position;
        ImVec2 Size;
        ImVec2 Center;
        std::string Name;
    };

    void DrawMainScreen();

    struct CanvasGLElements
    {
        GLuint ShaderProgram;
        GLuint VertexArrayObject;
        GLuint FramebufferID = 0;
        GLuint ColorAttachmentID = 0;
        GLuint DepthAndStencilAttachmentID = 0;
    };

    void CanvasInitShader(CanvasGLElements* canvas_gl_elements);
    void CanvasCreateTexture(CanvasGLElements* canvas_gl_elements);
    void CanvasInvalidateTexture(CanvasGLElements* canvas_gl_elements, GLsizei width,
                                 GLsizei height);
    void CanvasUpdateTexture(CanvasGLElements* canvas_gl_elements, GLint x, GLint y, GLsizei width,
                             GLsizei height);
    void CanvasDeleteTexture(CanvasGLElements* canvas_gl_elements);

} // namespace ui
