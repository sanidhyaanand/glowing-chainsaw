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

// declaring utility functions
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
int tesselation = 100;
bool wireframe = false;
bool wireKeyPressed = false;

// value of PI
float PI = 3.14159;

// 1/wav for RGB
glm::vec3 lambda = glm::vec3(0.670, 0.540, 0.440);

// 6 faces, 2 triangles/face, 3 vertices/triangle
const int num_vertices = 20000;

// sphere variables
int tri_idx = 0;
glm::vec4 v_positions[num_vertices];
glm::vec4 v_colors[num_vertices];
glm::vec4 v_normals[num_vertices];

// for atmos
glm::vec4 v_positions_a[num_vertices];

// Variables for wireframe
int wire_idx = 0;
glm::vec4 w_positions[num_vertices];
glm::vec4 w_colors[num_vertices];
glm::vec4 w_normals[num_vertices];

glm::vec4 color(0.6, 0.6, 0.6, 1.0);
glm::vec4 black(0.2, 0.2, 0.2, 1.0);
glm::vec4 white(1.0, 1.0, 1.0, 1.0);

// Radius of Earth
float Radius = 5000;

// Parameters for atmosphere
float Kr = 0.0025; // Rayleigh constant    
float ESun = 15.0; // Brightness constant
float Km = 0.0010; // Mie constant
float g = -0.990; 
float radiusRatio = 1.015; 
float fInnerRadius = Radius;
float fOuterRadius = float(Radius * radiusRatio);
float fScale = float(1 / (fOuterRadius - fInnerRadius));
float sunAngle = 0.0; // For rotation of light source

int prev_tess = 30; // no clue
int Lat = 10;
int Long = 10;

// camera
Camera camera(glm::vec3(Radius, 0.0f, Radius)); // Initial position of camera
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
    void sphere_atmos(double radius, int Lats, int Longs);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // build and compile shaders
    // -------------------------
    glm::vec3 planetPos = glm::vec3(0.0, 0.0, 0.0);

    Shader atmosShader("Sphere_Shaders/SkyFromSpace.vs", "Sphere_Shaders/SkyFromSpace.fs");
    Shader groundShader("Sphere_Shaders/GroundFromSpace.vs", "Sphere_Shaders/GroundFromSpace.fs");
    Shader skyboxShader("Shaders/skyboxshad.vs", "Shaders/skyboxshad.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    // sphere VAO
    unsigned int sphereVAO[4], sphereVBO[4];
    glGenVertexArrays(4, sphereVAO);
    glGenBuffers(4, sphereVBO);

    GLuint vPositionAtmos = glGetAttribLocation(atmosShader.ID, "Pos");
    GLuint vPositionGround = glGetAttribLocation(groundShader.ID, "Pos");

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

    glEnableVertexAttribArray(vPositionGround);
    glVertexAttribPointer(vPositionGround, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    sphere_atmos(fOuterRadius, Lat, Long);

    glBindVertexArray(sphereVAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO[2]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(v_positions_a), v_positions_a, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(vPositionAtmos);
    glVertexAttribPointer(vPositionAtmos, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);


    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::vector<std::string> faces
    {
        "Images/earth/px.png",
        "Images/earth/nx.png",
        "Images/earth/py.png",
        "Images/earth/ny.png",
        "Images/earth/pz.png",
        "Images/earth/nz.png"
    };


    std::vector<std::string> facesn
    {
        "Images/earth_night/px.jpg",
        "Images/earth_night/nx.jpg",
        "Images/earth_night/py.jpg",
        "Images/earth_night/ny.jpg",
        "Images/earth_night/pz.jpg",
        "Images/earth_night/nz.jpg"
    };

    std::vector<std::string> facesx
    {
        "Images/earth_s/pox.png",
        "Images/earth_s/nex.png",
        "Images/earth_s/poy.png",
        "Images/earth_s/ney.png",
        "Images/earth_s/poz.png",
        "Images/earth_s/nez.png"
    };

    std::vector<std::string> facesc
    {
        "Images/earth_clouds/px.png",
        "Images/earth_clouds/nx.png",
        "Images/earth_clouds/py.png",
        "Images/earth_clouds/ny.png",
        "Images/earth_clouds/pz.png",
        "Images/earth_clouds/nz.png"
    };

    std::vector<std::string> faces_sky
    {
        "Images/lightblue/right.png",
        "Images/lightblue/left.png",
        "Images/lightblue/top.png",
        "Images/lightblue/bot.png",
        "Images/lightblue/front.png",
        "Images/lightblue/back.png"
    };

    Skybox skybox(faces);
    Skybox skybox_n(facesn);    
    Skybox skybox_s(facesx);
    Skybox skybox_c(facesc);
    
    groundShader.use();
    groundShader.setInt("tDiffuse", 0);
    groundShader.setInt("tDiffuseNight", 1);
    groundShader.setInt("tSpecular", 2);
    groundShader.setInt("tDiffuseClouds", 3);

    Skybox skybox_sky(faces_sky);

    skyboxShader.use();
    skyboxShader.setFloat("skybox", 0);

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per frame time logic
        // --------------------
        float currentframe = float(glfwGetTime());
        deltatime = currentframe - lastframe;
        lastframe = currentframe;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.956f, 0.12f, 0.39f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw scene
        // ----------
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 20000.0f);

        // skybox
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skybox_sky.skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_sky.cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        // Main planet
        groundShader.use();

        view = camera.GetViewMatrix();
        groundShader.setMat4("view", view);
        groundShader.setMat4("projection", projection);
        groundShader.setMat4("model", model);

        groundShader.setVec3("v3CameraPos", camera.Position);
        groundShader.setVec3("v3LightDir", glm::vec3(-1.0f * cos(sunAngle), 0.0f, -1.0f * sin(sunAngle)));
        groundShader.setVec3("v3InvWavelength", glm::vec3(1 / pow(lambda.x, 4), 1 / pow(lambda.y, 4), 1 / pow(lambda.z, 4)));
        groundShader.setVec3("planetPos", planetPos);
        groundShader.setFloat("fCameraHeight", length(camera.Position));
        groundShader.setFloat("fCameraHeight2", pow(length(camera.Position), 2));
        groundShader.setFloat("fOuterRadius", fOuterRadius);
        groundShader.setFloat("fOuterRadius2", fOuterRadius * fOuterRadius);
        groundShader.setFloat("fInnerRadius", fInnerRadius);
        groundShader.setFloat("fInnerRadius2", fInnerRadius * fInnerRadius);
        groundShader.setFloat("fKrESun", (Kr * ESun));
        groundShader.setFloat("fKr4PI", (Kr * 4 * PI));
        groundShader.setFloat("fKm4PI", (Km * 4 * PI));
        groundShader.setFloat("fScale", fScale);
        groundShader.setFloat("fScaleDepth", 0.25);
        groundShader.setFloat("fScaleOverScaleDepth", fScale / 0.25);
        groundShader.setFloat("fNightScale", 0.5f);
        groundShader.setVec3("dirLight.direction", glm::vec3(-1.0f * cos(sunAngle), 0.0f, -1.0f * sin(sunAngle)));
        groundShader.setVec3("dirLight.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
        groundShader.setVec3("dirLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        groundShader.setVec3("dirLight.specular", glm::vec3(0.4f, 0.4f, 0.4f));
        groundShader.setVec3("viewPos", camera.Position);
        groundShader.setBool("blinn", blinn);
      
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.cubemapTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_n.cubemapTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_s.cubemapTexture);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_c.cubemapTexture);
        glBindVertexArray(sphereVAO[0]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4900);


        // atmosphere rendering
        atmosShader.use();
        
        view = camera.GetViewMatrix();
        atmosShader.setMat4("view", view);
        atmosShader.setMat4("projection", projection);
        atmosShader.setMat4("model", model);

        atmosShader.setVec3("v3CameraPos", camera.Position);
        atmosShader.setVec3("v3LightDir", glm::vec3(1.0f * cos(sunAngle), 0.0f, 1.0f * sin(sunAngle)));
        atmosShader.setVec3("v3InvWavelength", glm::vec3(1 / pow(lambda.x, 4), 1 / pow(lambda.y, 4), 1 / pow(lambda.z, 4)));
        atmosShader.setVec3("planetPos", planetPos);
        atmosShader.setFloat("fCameraHeight", length(camera.Position));
        atmosShader.setFloat("fCameraHeight2", pow(length(camera.Position), 2));
        atmosShader.setFloat("fOuterRadius", fOuterRadius);
        atmosShader.setFloat("fOuterRadius2", fOuterRadius * fOuterRadius);
        atmosShader.setFloat("fInnerRadius", fInnerRadius);
        atmosShader.setFloat("fInnerRadius2", fInnerRadius * fInnerRadius);
        atmosShader.setFloat("fKrESun", (Kr * ESun));
        atmosShader.setFloat("fKr4PI", (Kr * 4 * PI));
        atmosShader.setFloat("fKm4PI", (Km * 4 * PI));
        atmosShader.setFloat("fScale", fScale);
        atmosShader.setFloat("fScaleDepth", 0.25);
        atmosShader.setFloat("fScaleOverScaleDepth", fScale / 0.25);
        atmosShader.setFloat("g", g);
        atmosShader.setFloat("g2", g * g);
        atmosShader.setVec3("v3LightPos", glm::vec3(1.0f * cos(sunAngle), 0.0f, 1.0f * sin(sunAngle)));

        glCullFace(GL_FRONT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
        glBindVertexArray(sphereVAO[2]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 19600);
        glCullFace(GL_BACK);
        glDisable(GL_BLEND);

        glBindVertexArray(0);
        
        std::cout << (blinn ? "Blinn-Phong" : "Phong") << std::endl;

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(4, sphereVAO);
    glDeleteBuffers(4, sphereVBO);

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

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        sunAngle += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        sunAngle -= 0.01f;

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
            //std::cout << v_positions[tri_idx - 1].x << " " << v_positions[tri_idx - 1].y << " " << v_positions[tri_idx - 1].z << std::endl;


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

void sphere_atmos(double radius, int Lats, int Longs)
{
    float lats_a, longs_a;

    // float slices=float(Lats)/100;
    // float sectors=float(Longs)/100;

    float slices_a = (180 / (float(Lats) * 20)) / 2;
    float sectors_a = (180 / (float(Longs) * 20)) / 2;

    float l;
    tri_idx = 0;

    for (lats_a = 0.0; lats_a <= PI; lats_a += sectors_a)
    {
        for (longs_a = 0.0; longs_a <= 2.0 * PI; longs_a += slices_a)
        {
            float x = radius * sin(lats_a) * cos(longs_a);
            float y = radius * sin(lats_a) * sin(longs_a);
            float z = radius * cos(lats_a);
            glm::vec4 pt(x, y, z, 1.0);

            v_positions_a[tri_idx] = pt;
            tri_idx++;

            if (lats_a + sectors_a > PI)
                l = PI;
            else
                l = lats_a + sectors_a;
            x = radius * sin(l) * cos(longs_a);
            y = radius * sin(l) * sin(longs_a);
            z = radius * cos(l);
            pt = glm::vec4(x, y, z, 1.0);
            v_positions_a[tri_idx] = pt;
            tri_idx++;
        }
    }
    std::cout << tri_idx;
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
