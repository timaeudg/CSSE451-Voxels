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
#include <string>
#include <fstream>
#include <iostream>

#include "Voxel.h"
using namespace std;

typedef unsigned char byte;

vector<byte>* read_binvox(string filespec, int* dim);
Vector3 getColor(Ray &ray, Hitpoint &hit, Scene &scene, float paramVal, float cumulativePercent);
bool traceShadowRay(Scene &scene, Ray &ray, float lightDist);
Scene loadScene(objLoader* objData, char* filename);

int main(int argc, char ** argv)
{
    //buffer
    int width =  1920;
    int height = 1080;

    //need at least one argument (obj file)
    if(argc < 3)
    {
        printf("Usage %s filename.obj, filename.binvox\n", argv[0]);
        exit(0);
    }

    if(argc >= 4){
        width = atoi(argv[3]);
        if(argc == 5){
            height = atoi(argv[4]);
        }
    }
    ToneMapper buf = ToneMapper(width, height);
    //load camera data
    
    objLoader objData = objLoader();
    objData.load(argv[1]);
    
    Scene scene = loadScene(&objData, argv[2]);
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
    //return Vector3(abs(norm[0]), abs(norm[1]), abs(norm[2]));
    
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
        Vector3 offsetHitLoc = hit.getHitpoint(0.9999f);
        Ray shadowRay = Ray(offsetHitLoc, lightDir);
        float lightDist = (light->getPos() - offsetHitLoc).squaredLength();

        bool inShadow = traceShadowRay(scene, shadowRay, lightDist);

        Vector3 combinedColor = ambientColor;
        if(!inShadow){
            combinedColor = combinedColor+diffColor+specColor;
        }
        summedColor = summedColor + combinedColor;
    }

    /*
    //Reflection code
    Vector3 reflectColor = Vector3(0,0,0);
    AbstractSurface* hitpointSurface;
    float hitpointParam = -1.0;

    float reflectAmount = hitObjMat.getReflect();
    Vector3 viewReflection = (2*(viewToCamera.dot(norm))*norm - viewToCamera).normalize();
    Ray reflectionRay = Ray(hit.getHitpoint(0.999f), viewReflection);
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

    } else if(reflectAmount > 0){
        Vector3 reflectedPart = reflectAmount*Vector3(0,0,0);
        Vector3 absorbedPart = (1.0f - reflectAmount)*summedColor;
        summedColor = reflectedPart + absorbedPart;
    }
    */
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

Scene loadScene(objLoader* objData, char* filename){
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

    int dim = 0;
    vector<byte>* voxels = read_binvox(filename, &dim);

    printf("size of vector: %i\n", dim);
    Vector3 voxPos = Vector3(0,0,0);
    for(int i = 0; i<dim; i++){
        for(int j = 0; j<dim; j++){
            for(int k = 0; k<dim; k++){
                byte to_print = (*voxels)[i * dim*dim + k*dim + j];
                if(to_print == 1){
                    voxPos = Vector3((float)i, (float)j, (float)k);
                    Voxel* voxel = new Voxel(voxPos, 0, 0.5);
                    surfaces.push_back(voxel);
                }
            }
        }
    }

    printf("at least one voxel at: %f,%f,%f\n", voxPos[0], voxPos[1], voxPos[2]);

    printf("Created %i Voxels\n", surfaces.size());
    return Scene(camera, surfaces, materials, lights);
}


vector<byte>* read_binvox(string filespec, int* dim){
    int version;
    int depth, height, width;
    int size;
    float tx, ty, tz;
    float scale;
    byte* voxels = 0;

    ifstream *input = new ifstream(filespec.c_str(), ios::in | ios::binary);

    //
    // read header
    //
    string line;
    *input >> line;  // #binvox
    if (line.compare("#binvox") != 0) {
        cout << "Error: first line reads [" << line << "] instead of [#binvox]" << endl;
        delete input;
        return 0;
  }
    *input >> version;
    cout << "reading binvox version " << version << endl;

    depth = -1;
    int done = 0;
    while(input->good() && !done) {
        *input >> line;
        if (line.compare("data") == 0) done = 1;
        else if (line.compare("dim") == 0) {
            *input >> depth >> height >> width;
        }
        else if (line.compare("translate") == 0) {
            *input >> tx >> ty >> tz;
        }
        else if (line.compare("scale") == 0) {
            *input >> scale;
        }
        else {
            cout << "  unrecognized keyword [" << line << "], skipping" << endl;
            char c;
            do {  // skip until end of line
                c = input->get();
            } while(input->good() && (c != '\n'));

        }
    }
    if (!done) {
        cout << "  error reading header" << endl;
        return 0;
    }
    if (depth == -1) {
        cout << "  missing dimensions in header" << endl;
        return 0;
    }

    size = width * height * depth;
    *dim = depth;
    
    voxels = new byte[size];
    if (!voxels) {
        cout << "  error allocating memory" << endl;
        return 0;
    }
    

    //
    // read voxel data
    //
    byte value;
    byte count;
    int index = 0;
    int end_index = 0;
    int nr_voxels = 0;
  
    input->unsetf(ios::skipws);  // need to read every byte now (!)
    *input >> value;  // read the linefeed char

    while((end_index < size) && input->good()) {
        *input >> value >> count;

        if (input->good()) {
            end_index = index + count;
            if (end_index > size) return 0;
            for(int i=index; i < end_index; i++){
                voxels[i] = value;
            }
      
            if (value) nr_voxels += count;
                index = end_index;
            }  // if file still ok
    
    }  // while

    input->close();
    cout << "  read " << nr_voxels << " voxels" << endl;
    printf("number of voxels: %i\n", index);
    vector<byte>* voxelVector = new vector<byte>(voxels, voxels + index);
    free(voxels);
    return voxelVector;
}
