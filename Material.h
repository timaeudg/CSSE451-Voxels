#ifndef __MATERIAL
#define __MATERIAL

#include "GenVector.h"
#include "AbstractSurface.h"

class Material {

    public:
        Material(){}
        ~Material(){}
        
        Material(Vector3 amb, Vector3 diff, Vector3 spec, float exponent, float reflect){
            this->ambColor = amb;
            this->diffColor = diff;
            this->specColor = spec;
            this->exponent = exponent;
            this->reflect = reflect;
        }

        Vector3 getAmbColor(){
            return this->ambColor;
        }

        Vector3 getDiffColor(){
            return this->diffColor;
        }

        Vector3 getSpecColor(){
            return this->specColor;
        }

        float getExponent(){
            return this->exponent;
        }

        float getReflect(){
            return this->reflect;
        }
        
    private:
        Vector3 ambColor;
        Vector3 diffColor;
        Vector3 specColor;
        float exponent;
        float reflect;
};

#endif
