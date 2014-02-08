#ifndef __ABSTRACTSURFACE
#define __ABSTRACTSURFACE

#include "Ray.h"

class AbstractSurface{

    public:
        AbstractSurface(){}
        ~AbstractSurface(){}

        virtual float checkIntersection(Ray ray){
            printf("Abstract Surface Intersect. This should never happen...\n");
        }
        
        virtual Vector3 getNormal(Vector3 hitpoint){}

        virtual int getMaterialIndex(){}

        virtual Vector3 getCenterPoint(){}

        virtual Vector3 getBBMin(){}

        virtual Vector3 getBBMax(){}

    private:
};

#endif
