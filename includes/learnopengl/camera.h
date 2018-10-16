#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/noise.hpp>
#include <queue>

#include <vector>

struct lookAt {
    glm::vec3 Position;
    float InicialTime;
    float FinalTime;
    float InicialPitch;
    float InicialYaw;
    float FinalPitch;
    float FinalYaw;
    bool Ended;
};

struct translation {
    glm::vec3 InicialPosition;
    glm::vec3 Position;
    float InicialTime;
    float FinalTime;
    bool Ended;
};

struct rotationRP {
    glm::vec3 Point;
    float InicialAngle;
    float Angle;
    float InicialTime;
    float FinalTime;
    bool Ended;
};

struct rotationRA {
    glm::vec3 Axis;
    float InicialAngle;
    float Angle;
    float InicialTime;
    float FinalTime;
    bool Ended;
};

struct bSpline {
    glm::vec3 P0, P1, P2, P3;
    float InicialTime;
    float FinalTime;
    bool Ended;
};

struct bezier {
    glm::vec3 P0, P1, P2, P3;
    float InicialTime;
    float FinalTime;
    bool Ended;
};

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;
const float NEAR        =  0.1f;    // Near clipping plane
const float FAR         =  100.0f;  // Far  clipping plane

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Euler Angles
    float Yaw;
    float Pitch;
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    float Near;
    float Far;

    bool noiseActive;
    float currTime;

    std::queue<lookAt> lookAtQueue;
    std::queue<translation> translationQueue;
    std::queue<rotationRP> rotationRPQueue;
    std::queue<rotationRA> rotationRAQueue;
    std::queue<bSpline> bSplineQueue;
    std::queue<bezier> bezierQueue;

    lookAt currLookAt;
    translation currTranslation;
    rotationRP currRP;
    rotationRA currRA;
    bSpline currBSpline;
    bezier currBezier;


    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, float zoom = ZOOM, float near = NEAR, float far = FAR) : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY)
    {
        currLookAt.Ended = true;
        currTranslation.Ended = true;
        currRP.Ended = true;
        currRA.Ended = true;
        currBSpline.Ended = true;
        currBezier.Ended = true;
        noiseActive = false;

        Near = near;
        Far = far;
        Zoom = zoom;
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;

        updateCameraVectors();
    }

    void LookAt(glm::vec3 P, float time){
        lookAt l;
        l.Position = P;
        l.FinalTime = time;
        l.Ended = false;

        lookAtQueue.push(l);
    }

    void Translate(glm::vec3 P, float time){
        translation t;
        t.Position = P;
        t.FinalTime = time;
        t.Ended = false;

        translationQueue.push(t);
    }

    void rotateRP(glm::vec3 P, float angle, float time){
        rotationRP r;
        r.Point = P;
        r.Angle = angle;
        r.FinalTime = time;
        r.Ended = false;

        rotationRPQueue.push(r);
    
    }
    
    void rotateRA(glm::vec3 axis, float angle, float time){
        rotationRA r;
        r.Axis = axis;
        r.Angle = angle;
        r.FinalTime = time;
        r.Ended = false;

        rotationRAQueue.push(r);
    }

    void bSplinePath(glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec3 P3, float time){
        bSpline b;
        b.P0 = P0;
        b.P1 = P1;
        b.P2 = P2;
        b.P3 = P3;
        b.FinalTime = time;
        b.Ended = false;

        bSplineQueue.push(b);
    }

    void bezierPath(glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec3 P3, float time){
        bezier b;
        b.P0 = P0;
        b.P1 = P1;
        b.P2 = P2;
        b.P3 = P3;
        b.FinalTime = time;
        b.Ended = false;

        bezierQueue.push(b);
    }

    void activateNoise(){
        noiseActive = true;
    }

    void deactivateNoise(){
        noiseActive = false;
    }

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        
        ProcessTransformations();
        return glm::lookAt(Position, Position + Front, Up);
    }

    // Returns projection matrix for width and height screen values
    glm::mat4 GetProjectionMatrix(float width, float height)
    {
        return glm::perspective(glm::radians(Zoom), (float)width / (float)height, Near, Far);
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;
    }

private:

    void ProcessTransformations(){
        currTime = glfwGetTime();
        //ProcessBSPline();
        //ProcessBezier();
        processTranslation();

        //ProcessRP();
        ProcessRA();

        //ProcessLookAt();
        updateCameraVectors();
    }

    void ProcessRA(){
        if(currRA.Ended){
            if(!rotationRAQueue.empty()){
                currRA = translationQueue.front();
                rotationRAQueue.pop();

                currRA.InicialTime = currTime;
                currRA.FinalTime += currTime;
                currRA.Ended = false;
                currRA.InicialAngle = Position;
            }
            else 
                return;
        }

        translation t = currRA;
        float percentage = (currTime - t.InicialTime) / (t.FinalTime - t.InicialTime);

        if(percentage >= 1){
            percentage = 1.0f;
            currRA.Ended = true;
        }

        this->Position = t.InicialPosition + percentage * (t.Position - t.InicialPosition);
    }

    void processTranslation(){
        if(currTranslation.Ended){
            if(!translationQueue.empty()){
                currTranslation = translationQueue.front();
                translationQueue.pop();

                currTranslation.InicialTime = currTime;
                currTranslation.FinalTime += currTime;
                currTranslation.Ended = false;
                currTranslation.InicialPosition = Position;
            }
            else 
                return;
        }

        translation t = currTranslation;
        float percentage = (currTime - t.InicialTime) / (t.FinalTime - t.InicialTime);

        if(percentage >= 1){
            percentage = 1.0f;
            currTranslation.Ended = true;
        }

        this->Position = t.InicialPosition + percentage * (t.Position - t.InicialPosition);
    }

    void ProcessLookAt(){
        if(currLookAt.Ended){
            if(!lookAtQueue.empty()){
                currLookAt = lookAtQueue.front();
                lookAtQueue.pop();

                currLookAt.InicialTime = currTime;
                currLookAt.FinalTime += currTime;
                currLookAt.InicialPitch = Pitch;
                currLookAt.InicialYaw = Yaw;

                glm::vec3 FinalFront = glm::normalize(currLookAt.Position - Position);
                currLookAt.FinalPitch = Pitch + glm::degrees(glm::orientedAngle(glm::vec3(FinalFront.x, Front.y, FinalFront.z), FinalFront, glm::vec3(0,1,0)));
                currLookAt.FinalYaw = Yaw + glm::degrees(glm::orientedAngle(glm::vec2(Front.x, Front.z), glm::vec2(FinalFront.x, FinalFront.z)));

                currLookAt.Ended = false;
            }
            else
                return;
        }

        lookAt l = currLookAt;
        float percentage = (currTime - l.InicialTime) / (l.FinalTime - l.InicialTime);
        if(percentage >= 1){
            currLookAt.Ended = true;
            percentage = 1;
        }

        currLookAt.Ended = true;
        Pitch = l.FinalPitch;
        Yaw = l.FinalYaw;
        return;

        Pitch = l.InicialPitch + percentage * (l.FinalPitch - l.InicialPitch);
        Yaw = l.InicialYaw + percentage * (l.FinalYaw - l.InicialYaw);

    }

    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};
#endif