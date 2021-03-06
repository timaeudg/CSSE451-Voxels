#ifndef __VOXEL
#define __VOXEL

#include "AbstractSurface.h"
#include "GenVector.h"

class Voxel : public AbstractSurface{

    public:
        Voxel(){
            this->max = Vector3(-FLT_MAX,-FLT_MAX, -FLT_MAX);
            this->min = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
            this->materialIndex = 0;
        }

        Voxel(Vector3 min, Vector3 max){
            this->min = min;
            this->max = max;
            this->materialIndex = 0;
        }

        Voxel(Vector3 centerPoint, int materialIndex, float radius){
            this->materialIndex = materialIndex;

            this->max = Vector3(centerPoint[0]+radius, centerPoint[1]+radius, centerPoint[2]+radius);
            this->min = Vector3(centerPoint[0]-radius, centerPoint[1]-radius, centerPoint[2]-radius);
        }

        ~Voxel(){}
        
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
            
            //printf("Voxel intersection: %f,%f\n", intersection[0], intersection[1]);

            if(intersection[0] < intersection[1]){
                return intersection[0];
            } else { 
                return intersection[1];
            }
        }
     
/*
float checkIntersection(Ray ray){
    const size_t vecDim = 3;
    float entrance = 0.0f;
    float exit = FLT_MAX;
    Vector3 normal = Vector3(0,0,0);

    for(int i = 0; i<vecDim; i++){
	float slabA = this->min[i];
	float slabB = this->max[i];
	float invDir = 1.0f / ray.getDirection()[i];
	float origin = ray.getOrigin()[i];

	float closestHit = (slabA - origin) * invDir;
	float farthestHit = (slabB - origin) * invDir;

	if(farthestHit < closestHit){
	    std::swap(closestHit, farthestHit);
	}

	bool tooClose = farthestHit < entrance;
	bool tooFar = closestHit > exit;

	if(tooClose || tooFar){
	    //printf("closest,exit %f,%f\n", closestHit, exit);
	    //printf("no hit\n");
	    return -1.0;
	}

	bool foundNewEntrance = closestHit > entrance;
	entrance = foundNewEntrance ? closestHit : entrance;

	bool foundNewExit = farthestHit < exit;
	exit = foundNewExit ? farthestHit : exit;
    }
    
    if(entrance == 0.0f){
	printf("hit; entrance: %f\n", entrance);
	return -1.0;
    }
    
    return entrance;
}
*/

        Vector3 getNormal(Vector3 hitpoint){
           if(hitpoint[0] == this->max[0]){
               return Vector3(1.0f,0.0f,0.0f);
           }
           if(hitpoint[0] == this->min[0]){
               return Vector3(-1.0f,0.0f,0.0f);
           }
           if(hitpoint[1] == this->min[1]){
               return Vector3(0.0f,-1.0f,0.0f);
           }
           if(hitpoint[1] == this->max[1]){
               return Vector3(0.0f,1.0f,0.0f);
           }
           if(hitpoint[2] == this->min[2]){
               return Vector3(0.0f,0.0f,-1.0f);
           }
           if(hitpoint[2] == this->max[2]){
               return Vector3(0.0f,0.0f,1.0f);
           }
        }

        int getMaterialIndex(){
            return this->materialIndex;
        }
        

    private:
        Vector3 max;
        Vector3 min;
        int materialIndex;
        
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
