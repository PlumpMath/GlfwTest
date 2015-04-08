
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

int main(void)
{
    GLFWwindow* window;
    
    if (!glfwInit()) {
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);

    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    // MUST SETUP GLEW BEFORE CALLING OPEN GL FUNCTIONS!!!
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        printf("Failed to initialize GLEW\n");
        return -1;
    }
    
    GLint compile_ok = GL_FALSE;
    
    /* Vertex shader */
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    
    const char *vs_source =
    "#version 330 core                                             \n"
    "layout (location = 0) in vec3 position;                         "
    "void main(void) {                                               "
    "  gl_Position = vec4(position.x, position.y, position.z, 1.0);  "
    "}                                                               ";
    
    glShaderSource(vs, 1, &vs_source, NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &compile_ok);
    if (0 == compile_ok) {
        fprintf(stderr, "Error in vertex shader\n");
        GLchar infoLog[512];
        glGetShaderInfoLog(vs, 512, NULL, infoLog);
        printf("%s", infoLog);
        return 0;
    }
    
    /* Fragment shader */
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    
    const char *fs_source =
    "#version 330 core                                      \n"
    "out vec4 color;                                          "
    "void main(void) {                                        "
    "  color = vec4(1.0f, 0.0f, 1.0f, 1.0f);                  "
    "}                                                        ";
    
    glShaderSource(fs, 1, &fs_source, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &compile_ok);
    if (!compile_ok) {
        fprintf(stderr, "Error in fragment shader\n");
        GLchar infoLog[512];
        glGetShaderInfoLog(fs, 512, NULL, infoLog);
        printf("%s", infoLog);
        return 0;
    }
    
    GLuint program;
    
    /* GLSL program */
    GLint link_ok = GL_FALSE;
    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        fprintf(stderr, "glLinkProgram:");
        return 0;
    }
    
    static const GLfloat triangle_vertices[] = {
        -0.8f, -0.8f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f,
    };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);
    
    const char* attribute_name = "position";
    int attr_position = glGetAttribLocation(program, attribute_name);
    if (attr_position == -1) {
        fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
        return 0;
    }

    glClearColor(0.7, 0.7, 0.8, 1.0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(program);
        glEnableVertexAttribArray(attr_position);
        
        /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
        glVertexAttribPointer(attr_position, // attribute
                              2,                 // number of elements per vertex, here (x,y)
                              GL_FLOAT,          // the type of each element
                              GL_FALSE,          // take our values as-is
                              0,                 // no extra data between each position
                              0
                              );
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(attr_position);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(program);
    glfwTerminate();
    return 0;
}