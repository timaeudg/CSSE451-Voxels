#ifndef __AABB
#define __AABB

#include "AbstractSurface.h"
#include "GenVector.h"

class AABB : public AbstractSurface{

    public:
        AABB(){
            this->min = Vector3(0,0,0);
            this->max = Vector3(0,0,0);
        }

        AABB(Vector3 min, Vector3 max){
            this->min = min;
            this->max = max;
        }

        ~AABB(){}
        
        Vector3 getBBMin(){
            return this->min;
        }
        
        Vector3 getBBMax(){
            return this->max;
        }
        
        Vector3 getCenterPoint(){
            return Vector3((this->min[0]+this->max[0])/2.0f, (this->min[1]+this->max[1])/2.0f, (this->min[2]+this->max[2])/2.0f);
        }
        
        void expandBox(AbstractSurface& shape){
            Vector3 shapeMax = shape.getBBMax();
            Vector3 shapeMin = shape.getBBMin();
            
            if(shapeMax[0] > max[0]){
                this->max[0] = shapeMax[0];
            }
            if(shapeMax[1] > max[1]){
                this->max[1] = shapeMax[1];
            }
            if(shapeMax[2] > max[2]){
                this->max[2] = shapeMax[2];
            }
            if(shapeMin[0] < min[0]){
                this->min[0] = shapeMin[0];
            }
            if(shapeMin[1] < min[1]){
                this->min[1] = shapeMin[1];
            }
            if(shapeMin[2] < min[2]){
                this->min[2] = shapeMin[2];
            }
        }
        
        Vector3 getAxisLengths(){
            Vector3 axisVector = Vector3(0,0,0);
            axisVector[0] = max[0]-min[0];
            axisVector[1] = max[1]-min[1];
            axisVector[2] = max[2]-min[2];
            return axisVector;
        }

        float checkIntersection(Ray ray){
            Vector3 origin = ray.getOrigin();
            float oX = origin[0];
            float oY = origin[1];
            float oZ = origin[2];

            Vector3 direction = ray.getDirection();
            float dirX = direction[0];
            float dirY = direction[1];
            float dirZ = direction[2];

            float minTx = (min[0] - oX) / dirX;
            float minTy = (min[1] - oY) / dirY;
            float minTz = (min[2] - oZ) / dirZ;

            float maxTx = (max[0] - oX) / dirX;
            float maxTy = (max[1] - oY) / dirY;
            float maxTz = (max[2] - oZ) / dirZ;
            
            //due to the direction that the ray could be facing, the max value from bbmin and bbmax, may not result in coordination between
            //the parameter values, so we need to fix that
            
            float transfer = 0.0;
            if(minTx > maxTx){
                transfer = minTx;
                minTx = maxTx;
                maxTx = transfer;
            }
            
            if(minTy > maxTy){
                transfer = minTy;
                minTy = maxTy;
                maxTy = transfer;
            }
            
            if(minTz > maxTz){
                transfer = minTz;
                minTz = maxTz;
                maxTz = transfer;
            }
            
            //printf("min x,y,z: %f,%f,%f\n", minTx, minTy, minTz);
            //printf("max x,y,z: %f,%f,%f\n", maxTx, maxTy, maxTz);
            Vector2 intersection = getIntersection(minTx, maxTx, minTy, maxTy);
            intersection = getIntersection(intersection[0], intersection[1], minTz, maxTz);
            
            //printf("AABB intersection: %f,%f\n", intersection[0], intersection[1]);

            if(intersection[0] > intersection[1]){
                return intersection[0];
            } else { 
                return intersection[1];
            }
        }
        

    private:
        Vector3 max;
        Vector3 min;
        
        Vector2 getIntersection(float minA, float maxA, float minB, float maxB){
            float resultMax;
            float resultMin;
            if(maxB < maxA){
                resultMax = maxB;
                if(resultMax > minA){
                    if(minA > minB){
                        resultMin = minA;
                    } else {
                        resultMin = minB;
                    }
                    return Vector2(resultMin, resultMax);
                }
            } else {
                resultMax = maxA;
                if(resultMax > minB){
                    if(minA > minB){
                        resultMin = minA;
                    } else { 
                        resultMin = minB;
                    }
                    return Vector2(resultMin, resultMax);
                }
            }
            
            return Vector2(-1.0, -1.0);
        }

};

#endif
