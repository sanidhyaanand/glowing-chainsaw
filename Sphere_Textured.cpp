// SoC Project.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "utility/Texture.h"
#include "utility/shader.h"
#include "utility/skybox.h"
#include <iostream>
#include "utility/camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 640;
bool blinn = false;
bool blinnKeyPressed = false;
int tesselation = 50;
bool wireframe = false;
bool wireKeyPressed = false;

double PI = 3.2;

//6 faces, 2 triangles/face, 3 vertices/triangle
const int num_vertices = 3600;

int tri_idx = 0;
glm::vec4 v_positions[num_vertices];
glm::vec4 v_colors[num_vertices];
glm::vec4 v_normals[num_vertices];

// Variables for wireframe
int wire_idx = 0;
glm::vec4 w_positions[num_vertices];
glm::vec4 w_colors[num_vertices];
glm::vec4 w_normals[num_vertices];

glm::vec4 color(0.6, 0.6, 0.6, 1.0);
glm::vec4 black(0.2, 0.2, 0.2, 1.0);
glm::vec4 white(1.0, 1.0, 1.0, 1.0);

double Radius = 1;

int prev_tess = 30;
int Lat = 10;
int Long = 10;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//for movement speed per frame
float deltatime = 0.0f;
float lastframe = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Allahu Akbar", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    void sphere(double radius, int Lats, int Longs);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // build and compile shaders
    // -------------------------
    Shader shader("Shaders/lightobjshad.vs", "Shaders/lightobjshad.fs");
    Shader sphereShader("Shaders/sphereshadertex.vs", "Shaders/sphereshadertex.fs");
    Shader lightShader("Shaders/spherical_light.vs", "Shaders/spherical_light.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
 
    float planeVertices[] = {
        // positions            //normals           // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
         5.0f, -0.5f,  5.0f,    0.0, 1.0, 0.0,      1.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,    0.0, 1.0, 0.0,      0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,    0.0, 1.0, 0.0,      0.0f, 1.0f,

         5.0f, -0.5f,  5.0f,    0.0, 1.0, 0.0,      1.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,    0.0, 1.0, 0.0,      0.0f, 1.0f,
         5.0f, -0.5f, -5.0f,    0.0, 1.0, 0.0,      1.0f, 1.0f
    };

    float vertices[] = {
    //light vertices
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    // light VAO
    unsigned int lightVAO, lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    // sphere VAO
    unsigned int sphereVAO[2], sphereVBO[2];
    glGenVertexArrays(2, sphereVAO);
    glGenBuffers(2, sphereVBO);

    GLuint vPosition = glGetAttribLocation(sphereShader.ID, "vPosition");
    GLuint vColor = glGetAttribLocation(sphereShader.ID, "vColor");
    GLuint vNormal = glGetAttribLocation(sphereShader.ID, "vNormal");

    glBindVertexArray(sphereVAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO[0]);
    
    // Call the sphere function
    Lat = tesselation;
    Long = tesselation;
    sphere(Radius, Lat, Long);

    glBufferData(GL_ARRAY_BUFFER, sizeof(v_positions) + sizeof(v_colors) + sizeof(v_normals), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(v_positions), v_positions);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(v_positions), sizeof(v_colors), v_colors);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(v_colors) + sizeof(v_positions), sizeof(v_normals), v_normals);

    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(v_positions)));

    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(v_positions) + sizeof(v_colors)));

    glBindVertexArray(sphereVAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(w_positions) + sizeof(w_colors) + sizeof(w_normals), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(w_positions), w_positions);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(w_positions), sizeof(w_colors), w_colors);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(w_colors) + sizeof(w_positions), sizeof(w_normals), w_normals);

    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(w_positions)));

    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(w_positions) + sizeof(w_colors)));

    Texture texture;
    unsigned int floorTexture = texture.loadTexture("Images/floor.png");
    unsigned int floorSpecular = texture.loadTexture("Images/floor_specular.png");

    shader.use();
    shader.setInt("material.diffuse", 0);
    shader.setInt("material.specular", 1);
    shader.setFloat("material.shininess", 32.0f);

    

    std::vector<std::string> faces
    {
        "Images/skybox/right.jpg",
        "Images/skybox/left.jpg",
        "Images/skybox/top.jpg",
        "Images/skybox/bottom.jpg",
        "Images/skybox/front.jpg",
        "Images/skybox/back.jpg"
    };

    std::vector<std::string> facesx
    {
        "Images/skybox_s/right.jpg",
        "Images/skybox_s/left.jpg",
        "Images/skybox_s/top.jpg",
        "Images/skybox_s/bottom.jpg",
        "Images/skybox_s/front.jpg",
        "Images/skybox_s/back.jpg"
    };

    Skybox skybox(faces);
    Skybox skybox_s(facesx);

    sphereShader.use();
    sphereShader.setFloat("material.diffuse", 0);
    sphereShader.setFloat("material.specular", 1);
    sphereShader.setFloat("material.shininess", 32.0f);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per frame time logic
        // --------------------
        float currentframe = glfwGetTime();
        deltatime = currentframe - lastframe;
        lastframe = currentframe;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.956f, 0.12f, 0.39f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // point light 1
        shader.use();
        shader.setVec3("light.position", glm::vec3(0.7f, 3.2f, 2.0f));
        shader.setVec3("light.ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("light.constant", 1.0f);
        shader.setFloat("light.linear", 0.09);
        shader.setFloat("light.quadratic", 0.032);
        shader.setInt("blinn", blinn);

        sphereShader.use();
        sphereShader.setVec3("light.position", glm::vec3(0.7f, 3.2f, 2.0f));
        sphereShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        sphereShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
        sphereShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        sphereShader.setFloat("light.constant", 1.0f);
        sphereShader.setFloat("light.linear", 0.09);
        sphereShader.setFloat("light.quadratic", 0.032);
        sphereShader.setInt("blinn", blinn);

        // draw scene
        // ----------
        shader.use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        shader.setVec3("viewPos", camera.Position); //viewer position

        // floor
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, floorSpecular);
        shader.setMat4("model", model);
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // sphere
        sphereShader.use();
       
        sphereShader.setMat4("view", view);
        sphereShader.setMat4("projection", projection);
        sphereShader.setVec3("viewPos", camera.Position); //viewer position
        float time = glfwGetTime();
        float angle = glm::radians(30.0 * time);
        model = glm::translate(model, glm::vec3(0.7 + cos(angle), 1.0, 2.0 + sin(angle)));

        // Draw the sphere
        glBindVertexArray(sphereVAO[0]);
        sphereShader.setMat4("model", model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.cubemapTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_s.cubemapTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 1296);


        if (wireframe)
        {
            glPointSize(4);
            sphereShader.setMat4("model", model);
            // Drawing a Wireframe for SPHERE
            glBindVertexArray(sphereVAO[1]);
            glDrawArrays(GL_LINE_STRIP, 0, 1296);
        }

        // draw light source
        // ---------------
        lightShader.use();
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.7f, 3.2f, 2.0f));
        model = glm::scale(model, glm::vec3(0.2f));
        lightShader.setMat4("model", model);// Make it a smaller sphere
        glBindVertexArray(sphereVAO[0]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 1296);

        std::cout << (blinn ? "Blinn-Phong" : "Phong") << std::endl;

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}



// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    //camera
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltatime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltatime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltatime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltatime);

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
    {
        blinn = !blinn;
        blinnKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
    {
        blinnKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && !wireKeyPressed)
    {
        wireframe = !wireframe;
        wireKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE)
    {
        wireKeyPressed = false;
    }
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void sphere(double radius, int Lats, int Longs)
{
    float lats, longs;

    // float slices=float(Lats)/100;
    // float sectors=float(Longs)/100;

    float slices = (180 / (float(Lats) * 10)) / 2;
    float sectors = (180 / (float(Longs) * 10)) / 2;

    float l;

    for (lats = 0.0; lats <= PI; lats += sectors)
    {
        for (longs = 0.0; longs <= 2.0 * PI; longs += slices)
        {
            float x = radius * sin(lats) * cos(longs);
            float y = radius * sin(lats) * sin(longs);
            float z = radius * cos(lats);
            glm::vec4 pt(x, y, z, 1.0);

            v_colors[tri_idx] = white; v_positions[tri_idx] = pt;
            v_normals[tri_idx] = pt; tri_idx++;

            w_colors[wire_idx] = black; w_positions[wire_idx] = pt;
            w_normals[wire_idx] = pt; wire_idx++;


            if (lats + sectors > PI)
                l = PI;
            else
                l = lats + sectors;
            x = radius * sin(l) * cos(longs);
            y = radius * sin(l) * sin(longs);
            z = radius * cos(l);
            pt = glm::vec4(x, y, z, 1.0);
            v_colors[tri_idx] = white; v_positions[tri_idx] = pt;
            v_normals[tri_idx] = pt; tri_idx++;

            w_colors[wire_idx] = black; w_positions[wire_idx] = pt;
            w_normals[wire_idx] = pt; wire_idx++;

        }
    }

    // To Complete the wireframe
    for (lats = 0.0; lats <= PI; lats += sectors)
    {
        for (longs = 0.0; longs <= 2.0 * PI; longs += slices)
        {
            float x = radius * sin(lats) * cos(longs);
            float y = radius * sin(lats) * sin(longs);
            float z = radius * cos(lats);
            glm::vec4 pt(x, y, z, 1.0);

            w_colors[wire_idx] = black; w_positions[wire_idx] = pt;
            w_normals[wire_idx] = pt; wire_idx++;

        }
    }
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
