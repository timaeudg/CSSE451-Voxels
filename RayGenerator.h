#ifndef __RAYGENERATOR
#define __RAYGENERATOR

#include "Camera.h"
#include "Ray.h"
#include <math.h>

class RayGenerator{

    public:
        RayGenerator(){

        }

        RayGenerator(Camera *cam, int w, int h, float fov){
            createViewport(cam, w, h, fov);
        }

        void createViewport(Camera *camera, int w, int h, float fov){
            this->cam = *camera;
            this->width = w;
            this->height = h;

            float halfAngle = fov/2.0;
            this->distance = (h/2.0)/tan(halfAngle* (3.14159265 / 180.0));
        }

        Ray getRay(int x, int y){
           float xOffset = (float)(x - (this->width)/2) + 0.5;
           float yOffset = (float)(y - (this->height)/2) + 0.5;

           Vector3 rayDirection = -this->cam.getW()*this->distance + xOffset*this->cam.getU() + yOffset*this->cam.getV();

           rayDirection.normalize();

           Ray ray = Ray(this->cam.getPos()  , rayDirection);
           return ray;
        }

        ~RayGenerator(){
        }

    private:
        Camera cam;
        int width;
        int height;
        float distance;
};
#endif
