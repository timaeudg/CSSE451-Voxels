#ifndef __SCENE
#define __SCENE

#include "Sphere.h"
#include "Triangle.h"
#include "Material.h"
#include "Light.h"
#include <vector>
#include <cfloat>
#include "AABBTree.h"

class Scene{

    public:
        Scene(){}
        
        Scene(Camera cam, std::vector<AbstractSurface*> surfaces, std::vector<Material> mats, std::vector<Light*> lights){
            this->cam = cam;
            this->surfaces = surfaces;
            this->materials = mats;
            this->lights = lights;
            //get bb max & min
            this->sceneBBMin = getSceneBBMin(surfaces);
            this->sceneBBMax = getSceneBBMax(surfaces);
            this->sceneTree = AABBTree(this->surfaces);
        }
        
        Camera* getCamera(){
            return &this->cam;
        }
        
        std::vector<AbstractSurface*>* getSurfaces(){
            return &this->surfaces;
        }

        Material getMaterial(int index){
            return this->materials[index];
        }

        std::vector<Light*> getLights(){
            return this->lights;
        }
        
        bool getHitpoint(Ray* newRay, float* intersected, AbstractSurface** shapeIndex){
            /*
            float smallestIntersected = FLT_MAX;
            int index = -1;
            for(int j = 0; j<surfaces.size(); j++){
                AbstractSurface* current = surfaces[j];
                float intersected = current->checkIntersection(*newRay);
                if(intersected>=0){
                    if(intersected < smallestIntersected){
                        smallestIntersected = intersected;
                        index = j;
                    }
                }
            }
            if(index>=0){
                *intersected = smallestIntersected;
                *shapeIndex = surfaces[index];
                return true;
            }
            */
            float intersectedVal = -1.0f;
            AbstractSurface* surface = this->sceneTree.getIntersection(*newRay, &intersectedVal);

            if(intersectedVal >= 0){
                *intersected = intersectedVal;
                *shapeIndex = surface;
                return true;
            }
            
            return false;
            
        }
        
        ~Scene(){}

    private:
        Camera cam;
        std::vector<AbstractSurface*> surfaces;
        std::vector<Material> materials;
        std::vector<Light*> lights;

        AABBTree sceneTree;
        
        Vector3 sceneBBMax;
        Vector3 sceneBBMin;
        
        Vector3 getSceneBBMax(std::vector<AbstractSurface*> surfaces){
            Vector3 sceneMax = Vector3(0,0,0);
            bool sceneMaxSet = false;
            for(int j = 0; j<surfaces.size(); j++){
                AbstractSurface* current = surfaces[j];
                Vector3 bbMax = current->getBBMax();
                if(!sceneMaxSet){
                    sceneMax = bbMax;
                } else{
                    if(bbMax[0] > sceneMax[0]){
                        sceneMax[0] = bbMax[0];
                    }
                    if(bbMax[1] > sceneMax[1]){
                        sceneMax[1] = bbMax[1];
                    }
                    if(bbMax[2] > sceneMax[2]){
                        sceneMax[2] = bbMax[2];
                    }
                }
            }
            return sceneMax;
        }
        
        Vector3 getSceneBBMin(std::vector<AbstractSurface*> surfaces){
            Vector3 sceneMin = Vector3(0,0,0);
            bool sceneMinSet = false;
            for(int j = 0; j<surfaces.size(); j++){
                AbstractSurface* current = surfaces[j];
                Vector3 bbMin = current->getBBMin();
                if(!sceneMinSet){
                    sceneMin = bbMin;
                } else{
                    if(bbMin[0] < sceneMin[0]){
                        sceneMin[0] = bbMin[0];
                    }
                    if(bbMin[1] < sceneMin[1]){
                        sceneMin[1] = bbMin[1];
                    }
                    if(bbMin[2] < sceneMin[2]){
                        sceneMin[2] = bbMin[2];
                    }
                }
            }
            return sceneMin;
        }
};

#endif
