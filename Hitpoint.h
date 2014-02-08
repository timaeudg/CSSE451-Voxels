#ifndef __HITPOINT
#define __HITPOINT

#include "Ray.h"
#include "GenVector.h"

class Hitpoint{

    public:
        Hitpoint(){}
        ~Hitpoint(){}

        Hitpoint(Ray ray, float parameterVal, AbstractSurface* surface){
            this->parameterVal = parameterVal;
            this->ray = ray;
            this->hitSurface = surface;
        }
        
        float getParameter(){
            return this->parameterVal;
        }
        
        Ray* getRay(){
            return &this->ray;
        }
        
        Vector3 getHitpoint(float offset=1.0){
            return this->ray.getOrigin() + this->parameterVal*offset*this->ray.getDirection();
        }

        AbstractSurface* getSurface(){
            return this->hitSurface;
        }
        
        Vector3 getNormal(){
            return getSurface()->getNormal(getHitpoint());
        }
        
    private:
        float parameterVal;
        Ray ray;
        AbstractSurface* hitSurface;
};

#endif
