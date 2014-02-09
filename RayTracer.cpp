#include "GenVector.h"
#include "simplePPM.h"
#include "Buffer.h"
#include "objLoader.h"
#include "Camera.h"
#include "RayGenerator.h"
#include <math.h>
#include <vector>
#include <memory>
#include "AbstractSurface.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Hitpoint.h"
#include "Scene.h"
#include "Material.h"
#include "Light.h"
#include "PointLight.h"
#include "ToneMapper.h"
#include <cfloat>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>

Vector3 getColor(Ray &ray, Hitpoint &hit, Scene &scene, float paramVal, float cumulativePercent);
bool traceShadowRay(Scene &scene, Ray &ray, float lightDist);
Scene loadScene(objLoader* objData, bool voxelize);

int main(int argc, char ** argv)
{
    //buffer
    int width =  1920;
    int height = 1080;

    //need at least one argument (obj file)
    if(argc < 2)
    {
        printf("Usage %s filename.obj\n", argv[0]);
        exit(0);
    }

    if(argc >= 3){
        width = atoi(argv[2]);
        if(argc == 4){
            height = atoi(argv[3]);
        }
    }
    ToneMapper buf = ToneMapper(width, height);
    
    //load camera data
    objLoader objData = objLoader();
    objData.load(argv[1]);
    
    Scene scene = loadScene(&objData, false);
    printf("scene loaded\n");
    
    RayGenerator rayGen = RayGenerator(scene.getCamera(), width, height, 90.0);
    #pragma omp parallel for
    for(int i = 0; i<width; i++){
        for(int k = 0; k<height; k++){
            Ray newRay = rayGen.getRay(i, height-1-k);

            float paramVal = -1.0;
            AbstractSurface* surface;

            bool hit = scene.getHitpoint(&newRay, &paramVal, &surface);
            if(hit){
                Hitpoint hit = Hitpoint(newRay, paramVal, surface);
                
                Color pixelColor;
                Vector3 colorVector = getColor(newRay, hit, scene, paramVal, -1);

                buf.at(i, k) = colorVector;
            } else{
                buf.at(i, k) = Vector3(0,0,0);
            }
        }
    }
    Buffer mappedBuf = buf.toneMap();

    simplePPM_write_ppm("test.ppm", mappedBuf.getWidth(), mappedBuf.getHeight(), (unsigned char*)&mappedBuf.at(0,0));

    return 0;
}

Vector3 getColor(Ray &ray, Hitpoint &hit, Scene &scene, float paramVal, float cumulativePercent){
    //grab hitpoint info
    Vector3 hitLoc = ray.pointAtParameterValue(paramVal);
    Vector3 viewToCamera = (scene.getCamera()->getPos() - hitLoc).normalize();
    Vector3 norm = hit.getNormal();
//    return Vector3(abs(norm[0]), abs(norm[1]), abs(norm[2]));
    
    //grab material and material values
    Material hitObjMat = scene.getMaterial(hit.getSurface()->getMaterialIndex());

    Vector3 matDiff = hitObjMat.getDiffColor();
    Vector3 matSpec = hitObjMat.getSpecColor();
    Vector3 matAmb = hitObjMat.getAmbColor();

    Vector3 summedColor = Vector3(0,0,0);
    
    for(int k = 0; k < scene.getLights().size(); k++){
        Light* light = scene.getLights()[k];
        
        //Light Vectors and resources
        Vector3 lightDir = (light->getPos() - hitLoc).normalize();
        Material lightMat = scene.getMaterial(light->getMaterialIndex());
        Vector3 lr = (2*(lightDir.dot(norm))*norm - lightDir).normalize();
 
        //Light shading values
        Vector3 lightAmbient = lightMat.getAmbColor();
        Vector3 lightDiffuse = lightMat.getDiffColor();
        Vector3 lightSpec = lightMat.getSpecColor();
         
        Vector3 ambientColor = (lightAmbient * matAmb);
        Vector3 diffColor = norm.dot(lightDir) * matDiff * lightDiffuse;
        Vector3 specColor = (lightSpec * matSpec * pow((viewToCamera.dot(lr)), hitObjMat.getExponent()));

        
        //Shadow code
        //get the offset location to prevent shadow acne
        Vector3 offsetHitLoc = hit.getHitpoint(0.999f);
        Ray shadowRay = Ray(offsetHitLoc, lightDir);
        float lightDist = (light->getPos() - offsetHitLoc).squaredLength();

        bool inShadow = traceShadowRay(scene, shadowRay, lightDist);

        Vector3 combinedColor = ambientColor;
        if(!inShadow){
            combinedColor = combinedColor+diffColor+specColor;
        }
        summedColor = summedColor + combinedColor;
    }

    //Reflection code
    Vector3 reflectColor = Vector3(0,0,0);
    AbstractSurface* hitpointSurface;
    float hitpointParam = -1.0;

    float reflectAmount = hitObjMat.getReflect();
    Vector3 viewReflection = (2*(viewToCamera.dot(norm))*norm - viewToCamera).normalize();
    Ray reflectionRay = Ray(hit.getHitpoint(0.9f), viewReflection);
    bool hitSomething = scene.getHitpoint(&reflectionRay, &hitpointParam, &hitpointSurface);
    
    if(hitSomething && reflectAmount>0){
        Hitpoint reflectHit = Hitpoint(reflectionRay, hitpointParam, hitpointSurface);

        float toPass = -1;
        if(cumulativePercent==-1){
            toPass = reflectAmount;
        } else {
            toPass = cumulativePercent*reflectAmount;
        }
        
        if(toPass>= 0.05){
            reflectColor = getColor(reflectionRay, reflectHit, scene, hitpointParam, toPass);
        }
        Vector3 reflectedPart = reflectAmount*reflectColor;
        Vector3 absorbedPart = (1.0f - reflectAmount)*summedColor;
        summedColor = reflectedPart + absorbedPart;

    }
    
    return summedColor;
}

bool traceShadowRay(Scene &scene, Ray &ray, float lightDist){
    float intersected = -1.0;
    AbstractSurface* index;
    //Find the distance to light and check if it hits the light first
    bool hitSomething = scene.getHitpoint(&ray, &intersected, &index);

    Vector3 intersectedLoc = ray.pointAtParameterValue(intersected);
    float hitDist = (intersectedLoc - ray.getOrigin()).squaredLength();
    if(hitSomething && hitDist<=lightDist){
        return true;
    }
    return false;
}

Scene loadScene(objLoader* objData, bool voxelize){
    Camera camera;
    std::vector<Material> materials = std::vector<Material>();
    std::vector<AbstractSurface*> surfaces = std::vector<AbstractSurface*>();
    std::vector<Light*> lights = std::vector<Light*>();
    
    if((*objData).camera != NULL)
    {
        float x = (*objData).vertexList[ (*objData).camera->camera_pos_index ]->e[0];
        float y = (*objData).vertexList[ (*objData).camera->camera_pos_index ]->e[1];
        float z = (*objData).vertexList[ (*objData).camera->camera_pos_index ]->e[2];

        Vector3 camPos = Vector3(x, y, z);

        float xLook = (*objData).vertexList[ (*objData).camera->camera_look_point_index ]->e[0];
        float yLook = (*objData).vertexList[ (*objData).camera->camera_look_point_index ]->e[1];
        float zLook = (*objData).vertexList[ (*objData).camera->camera_look_point_index ]->e[2];

        Vector3 lookAt = Vector3(xLook, yLook, zLook);

        float xUp =	(*objData).normalList[ (*objData).camera->camera_up_norm_index ]->e[0];
        float yUp =	(*objData).normalList[ (*objData).camera->camera_up_norm_index ]->e[1];
        float zUp = (*objData).normalList[ (*objData).camera->camera_up_norm_index ]->e[2];

        Vector3 up = Vector3(xUp, yUp, zUp);
        up.normalize();

        printf("Found a camera\n");
        printf(" position: ");
        printf("%f %f %f\n", x, y, z);
        printf(" looking at: ");
        printf("%f %f %f\n", lookAt[0], lookAt[1], lookAt[2]);
        printf(" up normal: ");
        printf("%f %f %f\n", xUp, yUp, zUp);

        camera = Camera(&camPos, &lookAt, &up);
    }

    if((*objData).materialCount > 0 && (*objData).materialList != NULL){
        for(int j = 0; j < (*objData).materialCount; j++){
            float ambR = (*objData).materialList[ j ]->amb[0];
            float ambG = (*objData).materialList[ j ]->amb[1];
            float ambB = (*objData).materialList[ j ]->amb[2];
            
            float diffR = (*objData).materialList[ j ]->diff[0];
            float diffG = (*objData).materialList[ j ]->diff[1];
            float diffB = (*objData).materialList[ j ]->diff[2];
            
            float specR = (*objData).materialList[ j ]->spec[0];
            float specG = (*objData).materialList[ j ]->spec[1];
            float specB = (*objData).materialList[ j ]->spec[2];

            float exponent =        (*objData).materialList[ j ]->shiny;
            float reflection =      (*objData).materialList[ j ]->reflect;
            
            Vector3 ambColor  = Vector3( ambR,  ambG,  ambB);
            Vector3 diffColor = Vector3(diffR, diffG, diffB);
            Vector3 specColor = Vector3(specR, specG, specB);


            printf("Found a material, index will be %i\n", j);
            printf("exponent,reflection: %f,%f\n", exponent, reflection);
            
            Material mat = Material(ambColor, diffColor, specColor, exponent, reflection);
            materials.push_back(mat);
        }
    }

    if((*objData).voxelCount > 0 && (*objData).voxelList != NULL){
        for(int i = 0; i < (*objData).voxelCount; i++){
            printf("voxel #%i\n", i);
            float voxX = (*objData).vertexList[ (*objData).voxelList[i]->pos_index ]->e[0];
            float voxY = (*objData).vertexList[ (*objData).voxelList[i]->pos_index ]->e[1];
            float voxZ = (*objData).vertexList[ (*objData).voxelList[i]->pos_index ]->e[2];
            printf("x,y,z: %f,%f,%f\n", voxX, voxY, voxZ);
            printf("x,y,z: %f,%f,%f\n", voxX, voxY, voxZ);
            Vector3 voxPos = Vector3(voxX, voxY, voxZ);
            int materialIndex = (*objData).voxelList[i]->material_index;
            AABB* voxel = new AABB(voxPos, materialIndex, materials[materialIndex].getRadius()); 
            surfaces.push_back(voxel);
        }
    }

    if((*objData).sphereCount > 0 && (*objData).sphereList != NULL){
        for(int i = 0; i < (*objData).sphereCount; i++){
            float sphereX =  (*objData).vertexList[ (*objData).sphereList[i]->pos_index ]->e[0];
            float sphereY =  (*objData).vertexList[ (*objData).sphereList[i]->pos_index ]->e[1];
            float sphereZ =  (*objData).vertexList[ (*objData).sphereList[i]->pos_index ]->e[2];

            Vector3 spherePos = Vector3(sphereX, sphereY, sphereZ);

            float xUp = (*objData).normalList [ (*objData).sphereList[i]->up_normal_index ]->e[0];
            float yUp = (*objData).normalList [ (*objData).sphereList[i]->up_normal_index ]->e[1];
            float zUp = (*objData).normalList [ (*objData).sphereList[i]->up_normal_index ]->e[2];

            Vector3 sphereUp = Vector3(xUp, yUp, zUp);

            float radius = sphereUp.length();
            int materialIndex = (*objData).sphereList[i]->material_index;

            Sphere* sphere = new Sphere(spherePos, radius, materialIndex);
            surfaces.push_back(sphere);
        }
    }

    if((*objData).faceCount > 0 && (*objData).faceList != NULL){
        for(int i = 0; i < (*objData).faceCount; i++){
            float p1X = (*objData).vertexList[ (*objData).faceList[i]->vertex_index[0] ]->e[0];
            float p1Y = (*objData).vertexList[ (*objData).faceList[i]->vertex_index[0] ]->e[1];
            float p1Z = (*objData).vertexList[ (*objData).faceList[i]->vertex_index[0] ]->e[2];

            float p2X = (*objData).vertexList[ (*objData).faceList[i]->vertex_index[1] ]->e[0];
            float p2Y = (*objData).vertexList[ (*objData).faceList[i]->vertex_index[1] ]->e[1];
            float p2Z = (*objData).vertexList[ (*objData).faceList[i]->vertex_index[1] ]->e[2];

            float p3X = (*objData).vertexList[ (*objData).faceList[i]->vertex_index[2] ]->e[0];
            float p3Y = (*objData).vertexList[ (*objData).faceList[i]->vertex_index[2] ]->e[1];
            float p3Z = (*objData).vertexList[ (*objData).faceList[i]->vertex_index[2] ]->e[2];

            Vector3 p1 = Vector3(p1X, p1Y, p1Z);
            Vector3 p2 = Vector3(p2X, p2Y, p2Z);
            Vector3 p3 = Vector3(p3X, p3Y, p3Z);

            int materialIndex = (*objData).faceList[i]->material_index;

            Triangle* tri = new Triangle(p1, p2, p3, materialIndex);
            surfaces.push_back(tri);
        }
    }

    if((*objData).lightPointCount > 0 && (*objData).lightPointList != NULL){
        for(int i = 0; i < (*objData).lightPointCount; i++){
           float posX = (*objData).vertexList[ (*objData).lightPointList[i]->pos_index ]->e[0]; 
           float posY = (*objData).vertexList[ (*objData).lightPointList[i]->pos_index ]->e[1]; 
           float posZ = (*objData).vertexList[ (*objData).lightPointList[i]->pos_index ]->e[2]; 

           int materialIndex = (*objData).lightPointList[i]->material_index;

           Vector3 pos = Vector3(posX, posY, posZ);
           
           PointLight* light = new PointLight(pos, materialIndex);
           printf("Light Found, x,y,z,mat: %f,%f,%f,%i\n", light->getPos()[0], light->getPos()[1], light->getPos()[2], light->getMaterialIndex());
           lights.push_back(light);
           
        }
    }

    return Scene(camera, surfaces, materials, lights, voxelize);
}

