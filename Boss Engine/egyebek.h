#ifndef EGYEBEK
#define EGYEBEK

// settings
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

// camera
Camera camera;

bool shadows = true;
bool shadowsKeyPressed = false;

// timing
double deltaTime = 0.0f;
double lastFrame = 0.0f;


// lighting info
// -------------
float lx=2.0f, ly=2.0f, lz=2.0f;
glm::vec3 lightPos(lx, ly, lz);


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    camera.ProcessKeyboard(window, deltaTime);


    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        ly+=3*deltaTime;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        ly-=3*deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        lx-=3*deltaTime;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        lx+=3*deltaTime;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        lz-=3*deltaTime;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        lz+=3*deltaTime;

    lightPos = glm::vec3(lx, ly, lz);


    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !shadowsKeyPressed)
    {
        shadows = !shadows;
        shadowsKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
        shadowsKeyPressed = false;
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

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera.ProcessMouseMovement(window, xpos,ypos);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

GLFWwindow* FullScreenMode(char *windowName)
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);


    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);


    GLFWwindow* window = glfwCreateWindow(1680, 1050, windowName, primary, NULL);
    glfwSetWindowMonitor(window, primary, 0, 0, mode->width, mode->height, mode->refreshRate);
    glfwMakeContextCurrent(window);

    if (window == NULL)
    {
        printf("Hiba az ablak l%ctrehoz%csakor!\n",130,160);
        glfwTerminate();
        exit(-1);
    }

    if(GLEW_OK != glewInit())
    {
        printf("Hiba a GLFW-vel!\n");
        exit(-1);
    }

    SCR_WIDTH = mode->width;
    SCR_HEIGHT = mode->height;

    camera.centerX = SCR_WIDTH / 2.0f;
    camera.centerY = SCR_HEIGHT / 2.0f;

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwShowWindow(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);



    return window;

}


class Arnyek
{
    protected:
        unsigned int SHADOW_WIDTH, SHADOW_HEIGHT;
        unsigned int depthMapFBO;
        glm::mat4 shadowTransforms[6];
        float near_plane;
        float far_plane;

    public:
        unsigned int depthCubemap;

        Arnyek(unsigned int WIDTH, unsigned int HEIGHT)
        {
            SHADOW_WIDTH = WIDTH;
            SHADOW_HEIGHT = HEIGHT;

            CreateShadow();
        }

    protected:
        void CreateShadow()
        {
            glGenFramebuffers(1, &depthMapFBO);
            // create depth cubemap texture
            glGenTextures(1, &depthCubemap);
            glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
            for (unsigned int i = 0; i < 6; ++i)
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            // attach depth texture as FBO's depth buffer
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

        }

    public:
        void use(float near, float far, glm::vec3 lightPos1)
        {
            near_plane = near;
            far_plane = far;

            glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
            shadowTransforms[0] = (shadowProj * glm::lookAt(lightPos1, lightPos1 + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms[1] = (shadowProj * glm::lookAt(lightPos1, lightPos1 + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms[2] = (shadowProj * glm::lookAt(lightPos1, lightPos1 + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
            shadowTransforms[3] = (shadowProj * glm::lookAt(lightPos1, lightPos1 + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
            shadowTransforms[4] = (shadowProj * glm::lookAt(lightPos1, lightPos1 + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
            shadowTransforms[5] = (shadowProj * glm::lookAt(lightPos1, lightPos1 + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

        }

        void draw(Shader simpleDepthShader, Model Kocka, Model MClaren)
        {
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            simpleDepthShader.use();
            for (unsigned int i = 0; i < 6; ++i)
                simpleDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);

            simpleDepthShader.setFloat("far_plane", far_plane);
            simpleDepthShader.setVec3("lightPos", lightPos);

            Kocka.draw(simpleDepthShader, 0, 0, 0, 1.0);

            MClaren.draw(simpleDepthShader, -3.0, 0.0, 1.0);

            MClaren.draw(simpleDepthShader, 3.0, 0.0, 1.0);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

        }

};

#endif
