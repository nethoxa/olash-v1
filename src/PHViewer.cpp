#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <cstdlib>
#include <cmath>
#include <vector>

#include "glad/glad/glad.h"

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "util/tb.h"
#include "util/lodepng.h"
#include "util/tiny_obj_loader.h"

#include "PHViewer.h"

PHViewer::PHViewer() : GLFWWindow(512, 512, "Visualizador Programación Hardware") {
    makeContextCurrent();
}

void PHViewer::initGL()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    float cameraDist = 9.0f;

    // Set View and projection matrices
    projMat = glm::perspective(45.0f, 1.0f, 0.1f, 128.0f);
    viewMat = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -0.2f, -cameraDist));

    // Trackball Init
    tbInit(GLFW_MOUSE_BUTTON_LEFT);

    int width, height;
    getFramebufferSize(width, height);
    glViewport(0, 0, width, height);
    tbReshape(width, height);
}

void PHViewer::loadShaders(const GLchar* vShaderFile, const GLchar* fShaderFile)
{
    GLint vShader = 0;
    GLint fShader = 0;
    GLint status = 0;

    // Read shader files
    char* vSource = readShaderSource(vShaderFile);
    checkError(vSource != 0, "Failed to read the vertex shader.");

    char* fSource = readShaderSource(fShaderFile);
    checkError(fSource != 0, "Failed to read the fragment shader.");

    // Create program and shader objects
    vShader = glCreateShader(GL_VERTEX_SHADER);
    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    program = glCreateProgram();

    // Attach shaders to the program object
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);

    // Read shaders
    glShaderSource(vShader, 1, (const GLchar **) &vSource, 0);
    delete[] vSource;

    glShaderSource(fShader, 1, (const GLchar **) &fSource, 0);
    delete[] fSource;

    // TODO aqui no compila el shader va ma
    // Compile shaders
    glCompileShader(vShader);
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &status);
    checkShaderError(status, vShader, "Failed to compile the vertex shader.");

    glCompileShader(fShader);
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &status);
    checkShaderError(status, fShader, "Failed to compile the fragment shader.");

    // Link
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    checkProgramError(status, program, "Failed to link the shader program object.");

    // --------------------------------------------
    // MARAKATUSA
    modelViewProjMatParam = glGetUniformLocation(program, "modelViewProjMat");
    colorParam = glGetUniformLocation(program, "color");
    vertexParam = glGetAttribLocation(program, "vertex");
    directionParam = glGetUniformLocation(program, "direction");
    steepnessParam = glGetUniformLocation(program, "steepness");
    waveLengthParam = glGetUniformLocation(program, "waveLength");
    delta_timeParam = glGetUniformLocation(program, "delta_time");
    // --------------------------------------------

}

void PHViewer::deleteShaders()
{
    glDeleteProgram(program);
}

void PHViewer::loadModel(const char *fileName)
{
    std::vector<tinyobj::shape_t> shapes;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Load obj
    std::string err = tinyobj::LoadObj(shapes, fileName);
    checkError(err.empty(), err.c_str());
    checkError(!shapes.empty(), "No meshes in obj file!");

    // Extract object only first object
    indices = shapes[0].mesh.indices;
    vertices = shapes[0].mesh.positions;
    numIndices = indices.size();

    // Sel model matrix to identity
    modelMat = glm::mat4(1.0);

    // Generate VAO and VBOs ids
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &verticesVBO);
    glGenBuffers(1, &indicesVBO);

    // Activate VAO, attach buffers and transfer data to buffers
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(vertexParam);
    glVertexAttribPointer(vertexParam, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);
}

void PHViewer::deleteModel()
{
    glDeleteBuffers(1, &verticesVBO);
    glDeleteBuffers(1, &indicesVBO);
    glDeleteVertexArrays(1, &VAO);
}

// --------------------------------------------
// MARAKATUSA
float delta_time = 0;
void PHViewer::draw()
{
    delta_time += 0.005;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use program object and set uniform values
    glUseProgram(program);

    // Activate vertex array
    glBindVertexArray(VAO);

    glUniform3fv(colorParam, 1, glm::value_ptr(glm::vec3(0.25f, 0.52f, 0.96f)));

    glUniformMatrix4fv(modelViewProjMatParam, 1, GL_FALSE,
                       glm::value_ptr(projMat * viewMat * tbGetMatrix() * modelMat));

    glUniform3fv(directionParam, 1, glm::value_ptr(glm::vec3(.5f, .5f, .5f)));
    glUniform1f(steepnessParam, 0.4);
    glUniform1f(waveLengthParam, 0.25);
    glUniform1f(delta_timeParam, delta_time);


    glDrawElements(GL_TRIANGLES, (GLsizei)numIndices, GL_UNSIGNED_INT, 0);
}
// --------------------------------------------

int main(int argc, char** argv)
{
    if (glfwInit() != GL_TRUE) {
        std::cerr << "GLFW initialization failed!" << std::endl;
        return 1;
    }
    
    // Application & window init
    PHViewer::hint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    PHViewer::hint(GLFW_CONTEXT_VERSION_MINOR, 0);
    PHViewer::hint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    PHViewer::hint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    PHViewer::hint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    PHViewer phViewer;
    
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "GLAD open failed!" << std::endl;
        return 1;
    }
    
    phViewer.setCapturedEvents(MOUSEBUTTONEVENT | CURSORPOSEVENT | SIZEEVENT | SCROLLEVENT);

    phViewer.initGL();

    // --------------------------------------------
    // MARAKATUSA
    phViewer.loadShaders(
            SOURCE_DIR "/shaders/vpassthrough.glsl",
            SOURCE_DIR "/shaders/fpassthrough.glsl"
            );
    phViewer.loadModel(
            SOURCE_DIR "/models/malla.obj"
            );
    // --------------------------------------------


    // Rendering loop
    while (!phViewer.shouldClose()) {
        phViewer.draw();
        phViewer.swapBuffers();
        glfwPollEvents();
    }
    
    phViewer.deleteModel();
    phViewer.deleteShaders();
    
    // Finish GLFW
    glfwTerminate();
    
    return 0;
}

// Auxiliar functions: load shader source from file, check errors, etc.

char* PHViewer::readShaderSource(const char* shaderFile)
{
    struct stat statBuf;
    stat(shaderFile, &statBuf);

    std::ifstream f(shaderFile);
    
    if (!f)
        return 0;
    
    char *buf = (char*) new char[statBuf.st_size + 1];
    f.read(buf, statBuf.st_size);
    buf[statBuf.st_size] = '\0';
    
    return buf;
}

void PHViewer::checkError(GLint status, const char *msg)
{
    if (!status)
    {
        std::cerr << msg << std::endl;
        exit(1);
    }
}

void PHViewer::checkShaderError(GLint status, GLint shader, const char *msg)
{
    if (status == GL_FALSE) {
        std::cerr << msg << std::endl;
        
        char log[1024];
        GLsizei written;
        glGetShaderInfoLog(shader, sizeof(log), &written, log);
        std::cerr << "Shader log:" << log << std::endl;
    }
}

void PHViewer::checkProgramError(GLint status, GLint program, const char *msg)
{
    if (status == GL_FALSE) {
        std::cerr << msg << std::endl;
        
        char log[1024];
        GLsizei written;
        glGetProgramInfoLog(program, sizeof(log), &written, log);
        std::cerr << "Program log:" << log << std::endl;
    }
}
