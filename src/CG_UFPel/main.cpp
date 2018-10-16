#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void printCameraData();
void changeCamera();
void createCamera();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
vector<Camera> cameras;
unsigned int currentCamera = 0;
unsigned int numberOfCameras = 1;
// default camera configuration values
glm::vec3 position = glm::vec3(0,20,3);
glm::vec3 up = glm::vec3(0,1,0);
glm::vec3 front = glm::vec3(0,0,-1);
float zoom = 45;
float yaw = -90.0f;
float pitch = 0.0f;
float near = 0.01f;
float far = 100.0f;

// camera controls
bool tab = false, enter = false;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

bool obj1 = false, obj2 = false, obj3 = false, obj4 = false, obj5 = false;
bool t1 = false, t2 = false, t3 = false;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader(FileSystem::getPath("resources/cg_ufpel.vs").c_str(), FileSystem::getPath("resources/cg_ufpel.fs").c_str());

    // load models
    // -----------
    //Model city(FileSystem::getPath("resources/objects/city/Castelia City.obj"));
    Model rock(FileSystem::getPath("resources/objects/rock/rock.obj"));
    Model planet(FileSystem::getPath("resources/objects/planet/planet.obj"));
    Model cyborg(FileSystem::getPath("resources/objects/cyborg/cyborg.obj"));
    
    // creates a default camera at 0,5,3
    Camera newCamera = Camera(glm::vec3(0, 5, 3));
    cameras.push_back(newCamera);

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = cameras[currentCamera].GetProjectionMatrix(SCR_WIDTH, SCR_HEIGHT);
        glm::mat4 view = cameras[currentCamera].GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        printCameraData();

        // render the loaded model
        glm::mat4 model = glm::mat4(1);
        /*model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.002f, 0.002f, 0.002f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        city.Draw(ourShader);
        */

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(0, 10, -10));
        model = glm::scale(model, glm::vec3(0.2));
        ourShader.setMat4("model", model);
        rock.Draw(ourShader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(0, 10, 10));
        model = glm::scale(model, glm::vec3(0.2));
        ourShader.setMat4("model", model);
        planet.Draw(ourShader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(5, 5, 5));
        model = glm::scale(model, glm::vec3(0.2));
        ourShader.setMat4("model", model);
        cyborg.Draw(ourShader);



        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


void printCameraData(){
    printf("------------------- New camera data ------------------\n");
    printf("| Position Value: (%f %f %f)\n", position.x, position.y, position.z);
    printf("| Up Vector Value: (%f %f %f)\n", up.x, up.y, up.z);
    printf("| Front Vector Value: (%f %f %f)\n", front.x, front.y, front.z);
    printf("| Zoom: %f\n", zoom);
    printf("| Near clipping: %f\n", near);
    printf("| Far clipping: %f\n", far);
    printf("| Yaw: %f\n", yaw);
    printf("| Pitch: %f\n", pitch);

    printf("\n----------------- Current camera data ---------------\n");
    printf("| Number of Cameras: %u\n", numberOfCameras);
    printf("| Current camera id: %u\n", currentCamera);
    printf("| Position Value: (%f %f %f)\n", cameras[currentCamera].Position.x, cameras[currentCamera].Position.y, cameras[currentCamera].Position.z);
    printf("| Up Vector Value: (%f %f %f)\n", cameras[currentCamera].Up.x, cameras[currentCamera].Up.y, cameras[currentCamera].Up.z);
    printf("| Front Vector Value: (%f %f %f)\n", cameras[currentCamera].Front.x, cameras[currentCamera].Front.y, cameras[currentCamera].Front.z);
    printf("| Zoom: %f\n", cameras[currentCamera].Zoom);
    printf("| Near clipping: %f\n", cameras[currentCamera].Near);
    printf("| Far clipping: %f\n", cameras[currentCamera].Far);
    printf("| Yaw: %f\n", cameras[currentCamera].Yaw);
    printf("| Pitch: %f\n", cameras[currentCamera].Pitch);
    printf("-----------------------------------------------------\n");
}

void changeCamera() {
    if(numberOfCameras - 1 == currentCamera)
        currentCamera = 0;
    else
        ++currentCamera;
}

void createCamera() {
    Camera newCamera = Camera(position, up, yaw, pitch, zoom, near, far);
    cameras.push_back(newCamera);
    ++numberOfCameras;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Changing camera
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
        tab = true;
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE && tab){
        tab = false;
        changeCamera();
    }

    // Creating camera
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        enter = true;
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE && enter){
        enter = false;
        createCamera();
    }

    // LookAt
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)   obj1 = true;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)   obj2 = true;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)   obj3 = true;
    //if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)   obj4 = true;
    //if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)   obj5 = true;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE && obj1){
        cameras[currentCamera].LookAt(glm::vec3(0, 10, -10), 0);
        obj1 = false;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE && obj2){
        cameras[currentCamera].LookAt(glm::vec3(0, 10, 10), 5);
        obj2 = false;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE && obj3){
        cameras[currentCamera].LookAt(glm::vec3(5, 5, 5), 8);
        obj3 = false;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameras[currentCamera].ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameras[currentCamera].ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameras[currentCamera].ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameras[currentCamera].ProcessKeyboard(RIGHT, deltaTime);

    // Translate
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)   t1 = true;
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)   t2 = true;
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)   t3 = true;
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE && t1){
        cameras[currentCamera].Translate(glm::vec3(0, 10, -10), 0);
        t1 = false;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_RELEASE && t2){
        cameras[currentCamera].Translate(glm::vec3(0, 10, 10), 5);
        t2 = false;
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_RELEASE && t3){
        cameras[currentCamera].Translate(glm::vec3(5, 5, 5), 8);
        t3 = false;
    }


    // Camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameras[currentCamera].ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameras[currentCamera].ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameras[currentCamera].ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameras[currentCamera].ProcessKeyboard(RIGHT, deltaTime);


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

    cameras[currentCamera].ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cameras[currentCamera].ProcessMouseScroll(yoffset);
}
