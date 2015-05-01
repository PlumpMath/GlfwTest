
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ResourcePath.h"
#include "SOIL.h"

using namespace std;

GLuint LoadTexture(string name) {
    int width, height;
    string path = getResourcePath(name, "jpg");
    unsigned char* image = SOIL_load_image(path.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
    if(!image) cout << "Failed to load image." << endl;
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    return texture;
}

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
    
    window = glfwCreateWindow(640, 480, "Wie geht's?", NULL, NULL);

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
    "layout (location = 1) in vec2 texCoord;                         "
    "uniform float i;"
    "out float instid;                                               "
    "out vec2 TexCoord;                                              "
    "void main(void) {                                               "
    "  float xmod = -0.9 + i * 0.05f;                    "
    "  instid = gl_InstanceID;                                       "
    "  gl_Position = vec4(position.x + xmod, position.y, position.z, 1.0);  "
    "  TexCoord = texCoord;                                          "
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
    "in float instid;                                         "
    "in vec2 TexCoord;                                        "
    "uniform sampler2D ourTexture;                            "
    "out vec4 color;                                          "
    "void main(void) {                                        "
    " color = texture(ourTexture, TexCoord) * vec4(0.9, 0.9, 1.0, 1.0f);                "
    //"  color = vec4(1.0f, 0.5f, instid * 0.1f, 1.0f);                  "
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
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f,   // Top Right
        0.5f, -0.5f, 0.0f,   1.0f, 1.0f,   // Bottom Right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f,   // Bottom Left
        
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f,   // Bottom Left
        0.5f, 0.5f, 0.0f,   1.0f, 0.0f,   // Bottom Right
        -0.5f,  0.5f, 0.0f,  0.0f, 0.0f    // Top Left
    };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);
    
//    const char* attribute_name = "position";
//    int attr_position = glGetAttribLocation(program, attribute_name);
//    if (attr_position == -1) {
//        fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
//        return 0;
//    }
    
    
    // TEXTURE!
    
    int texture = LoadTexture("jelly");
    int texture2 = LoadTexture("bleh");
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glClearColor(0.98, 0.98, 0.98, 1.0);
    
    GLuint uniform_i = glGetUniformLocation(program, "i");

    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(program);
        
        /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
        glVertexAttribPointer(0, // attribute
                              3,                 // number of elements per vertex, here (x,y)
                              GL_FLOAT,          // the type of each element
                              GL_FALSE,          // take our values as-is
                              5 * sizeof(float),                 // no extra data between each position
                              0
                              );
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, // attribute
                              2,                 // number of elements per vertex, here (x,y)
                              GL_FLOAT,          // the type of each element
                              GL_FALSE,          // take our values as-is
                              5 * sizeof(float),                 // no extra data between each position
                              (const void*)(3 * sizeof(float))
                              );
        glEnableVertexAttribArray(1);
        
        glUniform1f(uniform_i, 0.0f);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, 20);

        glUniform1f(uniform_i, 20.0f);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glDrawArrays(GL_TRIANGLES, 0, 20);
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(program);
    glfwTerminate();
    return 0;
}