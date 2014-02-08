#ifndef __TRIANGLE
#define __TRIANGLE

#include "GenVector.h"
#include "AbstractSurface.h"

class Triangle : public AbstractSurface{

    public:
        Triangle(){}

        ~Triangle(){}

        Triangle(Vector3 p1, Vector3 p2, Vector3 p3, int material){
            this->p1 = p1;
            this->p2 = p2;
            this->p3 = p3;

            Vector3 v1 = p2 - p1;
            Vector3 v2 = p3 - p2;

            Vector3 nor = v1.cross(v2);
            nor.normalize();
            this->normal = nor;
            this->materialIndex = material;
        }

        float checkIntersection(Ray ray){
            if(checkRayHittingPlane(ray)){
                return checkHitpointInTriangle(ray);
            }
        
        }

        int getMaterialIndex(){
            return this->materialIndex;
        }

        Vector3 getNormal(Vector3 hitpoint){            
            return this->normal;
        }

        Vector3 getCenterPoint(){
           return (this->p1 + this->p2 + this->p3)/3.0;
        }

        Vector3 getBBMax(){
            float maxX =(p1[0]<p2[0])?p2[0]:p1[0];
            maxX = (maxX < p3[0])?p3[0]:maxX;
            float maxY =(p1[1]<p2[1])?p2[1]:p1[1];
            maxY = (maxY < p3[1])?p3[1]:maxY;
            float maxZ =(p1[2]<p2[2])?p2[2]:p1[2];
            maxZ = (maxZ < p3[2])?p3[2]:maxZ;

            return Vector3(maxX, maxY, maxZ);
        }

        Vector3 getBBMin(){
            float minX =(p1[0]>p2[0])?p2[0]:p1[0];
            minX = (minX > p3[0])?p3[0]:minX;
            float minY =(p1[1]>p2[1])?p2[1]:p1[1];
            minY = (minY > p3[1])?p3[1]:minY;
            float minZ =(p1[2]>p2[2])?p2[2]:p1[2];
            minZ = (minZ > p3[2])?p3[2]:minZ;

            return Vector3(minX, minY, minZ);

        }

    private:
        Vector3 p1;
        Vector3 p2;
        Vector3 p3;

        Vector3 normal;

        int materialIndex;

        bool checkRayHittingPlane(Ray ray){
            Vector3 a = this->p1;

            float top = this->normal.dot((a - ray.getOrigin()));
            float bottom = ray.getDirection().dot(this->normal);
            if(bottom == 0){
                //The ray is parallel to the plane, so no intersection
                return false;
            }

            return true;
        }

        float checkHitpointInTriangle(Ray ray){
            float top = this->normal.dot((this->p1 - ray.getOrigin()));
            float bottom = ray.getDirection().dot(this->normal);

            float intersectParam = top/bottom;
            
            if(intersectParam < 0){
                return -1.0;
            }

            Vector3 rayPoint = ray.getOrigin() + intersectParam*ray.getDirection();

            float testVal1 = ((p2-p1).cross(rayPoint-p1)).dot(this->normal);
            float testVal2 = ((p3-p2).cross(rayPoint-p2)).dot(this->normal);
            float testVal3 = ((p1-p3).cross(rayPoint-p3)).dot(this->normal);

            if(testVal1>0 && testVal2>0 && testVal3>0){
                return intersectParam;
            }

            return -1.0;
        }

};

#endif
