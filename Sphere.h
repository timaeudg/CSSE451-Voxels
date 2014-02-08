#ifndef __SPHERE
#define __SPHERE

#include "Ray.h"
#include "GenVector.h"
#include "AbstractSurface.h"
#include <math.h>

class Sphere : public AbstractSurface{

    public:
        Sphere(){}

        ~Sphere(){}

        Sphere(Vector3 pos, float radius, int materialIndex){
            setRadius(radius);
            setCenterPoint(pos);
            this->materialIndex = materialIndex;
        }

        void setRadius(float radius){
            this->radius = radius;
        }

        void setCenterPoint(Vector3 pos){
            this->centerPoint = pos;
        }

        float checkIntersection(Ray ray){
            Vector3 d = ray.getDirection();
            Vector3 e = ray.getOrigin();
            Vector3 c = this->centerPoint;
            float A = d.dot(d);
            float B = 2*d.dot(e-c);
            float C = (e-c).dot(e-c) - this->radius*this->radius;

            float rootPart = B*B - 4*A*C;
            if(rootPart >= 0){
                rootPart = sqrt(rootPart);
                float topPartPlus = -B+rootPart;
                float topPartMinus = -B-rootPart;

                float tPlus = topPartPlus / (2.0f*A);
                float tMinus = topPartMinus / (2.0f*A);
                
                float tFinal = 0.0;
                bool finalSet = false;
                if((tPlus < tMinus && tPlus>=0) || (tPlus>=0 && tMinus<0)){
                    tFinal = tPlus;
                    finalSet = true;
                } else if((tMinus < tPlus && tMinus>=0) || (tMinus>=0 && tPlus<0)){
                    tFinal = tMinus;
                    finalSet = true;
                }
                
                if(finalSet){
                    return tFinal;
                } else {
                    return -1.0;
                }
            } else {
                return -1.0;
            }
        }
        
        Vector3 getNormal(Vector3 hitpoint){
            Vector3 normVector = (hitpoint- this->centerPoint).normalize();
            return normVector;
        }

        int getMaterialIndex(){
            return this->materialIndex;
        }

        Vector3 getCenterPoint(){
            return this->centerPoint;
        }

        Vector3 getBBMax(){
            return this->centerPoint + Vector3(this->radius, this->radius, this->radius);
        }

        Vector3 getBBMin(){
            return this->centerPoint - Vector3(this->radius, this->radius, this->radius);
        }

    private:
        float radius;
        Vector3 centerPoint;
        int materialIndex;
};
#endif
