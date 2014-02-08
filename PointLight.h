#ifndef __POINTLIGHT
#define __POINTLIGHT

#include "Light.h"

class PointLight : public Light{

    public:
        PointLight(Vector3 pos, int materialIndex){
            this->position = pos;
            this->materialIndex = materialIndex;
        }

        Vector3 getPos(){
            return this->position;
        }

        int getMaterialIndex(){
            return this->materialIndex;
        }

    private:
        Vector3 position;
        int materialIndex;
};

#endif
