#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "fileolvas.h"
#include "camera.h"
#include "model.h"

#include <iostream>
#include "egyebek.h"


int main()
{

    // glfw window creation
    // --------------------
    GLFWwindow* window = FullScreenMode("BossEngine");

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);


    Model MClaren("model/mclaren.obj","képek/feher.png");
    Model Kocka("model/kocka.obj","képek/wood.jpg");

    Model Plane("model/plane.txt","képek/wood.jpg");

    Model skyBox("model/kocka.obj","sky/right1.jpg","sky/left1.jpg","sky/top1.jpg","sky/bottom1.jpg","sky/front1.jpg","sky/back1.jpg");

    // build and compile shaders
    // -------------------------
    Shader shader("shaders/3.2.2.point_shadows.ves", "shaders/3.2.2.point_shadows.fes");
    Shader simpleDepthShader("shaders/3.2.2.point_shadows_depth.ves", "shaders/3.2.2.point_shadows_depth.fes", "shaders/3.2.2.point_shadows_depth.ges");

    Shader skyboxShader("shaders/skybox.ves", "shaders/skybox.frs");

    // configure depth map FBO
    // -----------------------

    Arnyek shadow(1024, 1024);


    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("diffuseTexture", 0);
    shader.setInt("depthMap", 1);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        shadow.use(0.0, 30.0, lightPos);

        shadow.draw(simpleDepthShader, Plane, MClaren);


        // 2. render scene as normal
        // -------------------------
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // set lighting uniforms
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("viewPos", camera.Position);
        shader.setInt("shadows", shadows); // enable/disable shadows by pressing 'SPACE'
        shader.setFloat("far_plane", 30.0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadow.depthCubemap);

        glDisable(GL_CULL_FACE); // note that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling methods.
        //shader.setInt("reverse_normals", 1); // A small little hack to invert normals when drawing cube from the inside so lighting still works.
        Plane.draw(shader,0, 0, 0, 1.0);
        //shader.setInt("reverse_normals", 0); // and of course disable it


        //glEnable(GL_CULL_FACE);
        Kocka.draw(shader,lx, ly, lz, 0.1);

        MClaren.draw(shader, -3.0, 0, 1.0);

        MClaren.draw(shader, 3.0, 0, 1.0);


        skyBox.draw(skyboxShader,camera,projection);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
