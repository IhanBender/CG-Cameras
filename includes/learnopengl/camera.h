#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/spline.hpp>
#include <glm/gtc/noise.hpp>
#include <queue>

#include <vector>

struct lookAt {
    glm::vec3 Position;
    glm::vec3 InicialFront;
    glm::vec3 FinalFront;
    float InicialTime;
    float FinalTime;
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
    float Angle;
    float InicialTime;
    float FinalTime;

    glm::vec3 InicialFront;
    glm::vec3 InicialPosition;
    glm::vec3 InicialUp;
    bool Ended;
};

struct rotationRA {
    glm::vec3 Axis;
    float Angle;

    float InicialTime;
    float FinalTime;

    glm::vec3 InicialFront;
    glm::vec3 InicialPosition;
    glm::vec3 InicialUp;

    bool Ended;
};

struct spline {
    glm::vec3 p0;
    glm::vec3 p1;
    glm::vec3 p2;
    glm::vec3 p3;
    float InicialTime;
    float Time;
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
    std::queue<spline> bSplineQueue;
    std::queue<spline> bezierQueue;

    lookAt currLookAt;
    translation currTranslation;
    rotationRP currRP;
    rotationRA currRA;
    spline currBSpline;
    spline currBezier;


    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f), float zoom = ZOOM, float near = NEAR, float far = FAR) : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY)
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
        Front = front;

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
        spline b;
        b.p0 = P0;
        b.p1 = P1;
        b.p2 = P2;
        b.p3 = P3;
        b.Time = time;
        b.Ended = false;

        bSplineQueue.push(b);
    }

    void bezierPath(glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec3 P3, float time){
        spline b;
        b.p0 = P0;
        b.p1 = P1;
        b.p2 = P2;
        b.p3 = P3;
        b.Time = time;
        b.Ended = false;

        bezierQueue.push(b);
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


private:

    void ProcessTransformations(){
        currTime = glfwGetTime();
        ProcessBSPline();
        ProcessBezier();
        processTranslation();

        ProcessRP();
        ProcessRA();

        ProcessLookAt();
    }

    void ProcessBSPline(){
        if(currBSpline.Ended){
            if(!bSplineQueue.empty()){
                currBSpline = bSplineQueue.front();
                currBSpline.InicialTime = currTime;
                currBSpline.Time += currTime;
                currBSpline.Ended = false;
                bSplineQueue.pop();
            }
            else {
                return;
            }
        }
        
        spline b = currBSpline;
        float percentage = (this->currTime - b.InicialTime) / (b.Time - b.InicialTime);
        if(percentage >= 1){
            currBSpline.Ended = true;
            Position = b.p3;
            return;
        }

        glm::vec3 v;
        if(percentage <= 0.333333333){
            v = glm::catmullRom(b.p0, b.p0, b.p1, b.p2, 3 * percentage); 
        }
        else{
            if(percentage <= 0.666666666){
                v = glm::catmullRom(b.p0, b.p1, b.p2, b.p3, 3 * (percentage - 0.333333333));    
            }
            else {
                v = glm::catmullRom(b.p1, b.p2, b.p3, b.p3, 3 * (percentage - 0.666666666));    
            }
        }

        Position = v;
    }

    glm::vec3 Bezier(spline b, float t){
        return (float)pow(1-t, 3) * b.p0 + 
               3 * (float)pow(1-t, 2) * t * b.p1 +
               3 * (1-t) * (float)pow(t, 2) * b.p2 +
               (float)pow(t, 3) * b.p3;
    }

    void ProcessBezier(){
        if(currBezier.Ended){
            if(!bezierQueue.empty()){
                currBezier = bezierQueue.front();
                currBezier.InicialTime = currTime;
                currBezier.Time += currTime;
                currBezier.Ended = false;
                bezierQueue.pop();
            }
            else {
                return;
            }
        }
        
        spline b = currBezier;
        float percentage = (this->currTime - b.InicialTime) / (b.Time - b.InicialTime);
        if(percentage >= 1){
            currBezier.Ended = true;
            Position = currBezier.p3;
            return;
        }
        else {
            Position = Bezier(currBezier, percentage);
        }
    }

    void ProcessRA(){
         if(currRA.Ended){
            if(!rotationRAQueue.empty()){
                currRA = rotationRAQueue.front();
                currRA.Ended = false;   
                currRA.InicialTime = currTime;
                currRA.FinalTime += currTime;

                currRA.InicialFront = Front;
                currRA.InicialPosition = Position;
                currRA.InicialUp = Up;
                rotationRAQueue.pop(); 
            }
            else {
                return;
            }
        }

        rotationRA r = currRA;
        float percentage = (currTime - r.InicialTime) / (r.FinalTime - r.InicialTime);
        if(percentage >= 1){
            percentage = 1;
            currRA.Ended = true;
        }
    
        float angle;
        angle = r.Angle * percentage;
        glm::mat4 rotate;
        rotate = glm::rotate(glm::mat4(1), angle, r.Axis);
        
        glm::vec4 newPosition = glm::vec4(r.InicialPosition, 1);
        newPosition = newPosition * rotate;
        glm::vec4 newFront = glm::vec4((r.InicialPosition + r.InicialFront), 1);
        newFront = newFront * rotate;
        glm::vec4 newUp = glm::vec4((r.InicialPosition + r.InicialUp), 1);
        newUp = newUp * rotate;

        Position = glm::vec3(newPosition.x, newPosition.y, newPosition.z);
        Front = glm::vec3(newFront.x, newFront.y, newFront.z) - Position;
        Up = glm::vec3(newUp.x, newUp.y, newUp.z) - Position;
        Position = r.InicialPosition;

        updateCameraVectors();

        return;
    }

    void ProcessRP(){
        if(currRP.Ended){
            if(!rotationRPQueue.empty()){
                currRP = rotationRPQueue.front();
                currRP.Ended = false;   
                currRP.InicialTime = currTime;
                currRP.FinalTime += currTime;

                currRP.InicialFront = Front;
                currRP.InicialPosition = Position;
                currRP.InicialUp = Up;

                glm::vec3 newFront;
                if(currRP.Point != Position)
                    newFront = currRP.Point - Position;
                else
                    newFront = Front;

                newFront = glm::normalize(newFront);
                if(newFront != Front && (newFront + Front) != glm::vec3(0)){
                    Up = glm::normalize(glm::cross(Front, newFront));
                    if(Up.y <= 0.0)    
                        Up *= -1;
                }

                rotationRPQueue.pop(); 

            }
            else {
                return;
            }
        }
        

        rotationRP r = currRP;
        float percentage = (currTime - r.InicialTime) / (r.FinalTime - r.InicialTime);
        if(percentage >= 1){
            percentage = 1;
            currRP.Ended = true;
        }
    
        float angle;
        angle = r.Angle * percentage;
        glm::mat4 rMatrix = glm::mat4(1);
        rMatrix = glm::translate(rMatrix, r.Point);
        rMatrix = glm::rotate(rMatrix, angle, Up);
        rMatrix = glm::translate(rMatrix, -r.Point);
        
        glm::vec4 newPosition = glm::vec4(r.InicialPosition, 1);
        newPosition = newPosition * rMatrix;
        Position = glm::vec3(newPosition.x, newPosition.y, newPosition.z);

        glm::vec4 newFront = glm::normalize(glm::vec4((r.Point - Position), 1));
        updateCameraVectors();

        return;
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
                
                if(Position == currLookAt.Position){
                    currLookAt.Ended = true;
                    return;
                }

                currLookAt.InicialTime = currTime;
                currLookAt.FinalTime += currTime;
                currLookAt.Ended = false;
                currLookAt.InicialFront = Front;
                currLookAt.FinalFront = glm::normalize(currLookAt.Position - Position);
            }
            else 
                return;
        }

        lookAt t = currLookAt;
        float percentage = (currTime - t.InicialTime) / (t.FinalTime - t.InicialTime);

        if(percentage >= 1){
            percentage = 1.0f;
            currLookAt.Ended = true;
        }

        Front = t.InicialFront + percentage * (t.FinalFront - t.InicialFront);
        updateCameraVectors();

    }

    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        Front = glm::normalize(Front);
        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};
#endif