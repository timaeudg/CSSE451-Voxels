#ifndef __AABBNODE
#define __AABBNODE

#include <vector>
#include <typeinfo>

class AABBNode{

public:
    AABBNode(){
        
    }
    
    AABBNode(AbstractSurface* surface, AABBNode* left, AABBNode* right){
        this->surface = surface;
        this->left = left;
        this->right = right;
    }
    
    ~AABBNode(){}
    
    AbstractSurface* getIntersection(Ray& ray, float* paramMutate){
        float paramVal = this->surface->checkIntersection(ray);
        if(paramVal >= 0){
            //Clearly we hit something, and we need to either return the value we have if at a leaf, or traverse left or right,
            //which we traverse to depends on which child hits
            if(left == NULL && right == NULL){
                //No children, just return this value
                *paramMutate = paramVal;
                return surface;
            }
            float leftParam;
            float rightParam;
            float closerParam;
            AABBNode* otherNode;

            left->getAABBInstersection(ray, &leftParam);
            right->getAABBInstersection(ray, &rightParam);


            AbstractSurface* returnSurface = NULL;

            if(leftParam < rightParam){
                returnSurface = left->getIntersection(ray, &closerParam);
                otherNode = right;
            } else{
                returnSurface = right->getIntersection(ray, &closerParam);
                otherNode = left;
            }
            

            if(returnSurface == NULL || closerParam == -1.0){
                //Even though we hit the super box, we didn't hit anything inside, so we need to check the other child
                returnSurface = otherNode->getIntersection(ray, &closerParam);
            }

            *paramMutate = closerParam;
            return returnSurface;
            /*

            AbstractSurface* leftSurface = left->getIntersection(ray, &leftParam);
            AbstractSurface* rightSurface = right->getIntersection(ray, &rightParam);
            
            if(leftParam < 0 && rightParam < 0){
                *paramMutate = -1.0f;
                return NULL;
            } else if(leftParam < 0){
                *paramMutate = rightParam;
                return rightSurface;
            } else if(rightParam < 0){
                *paramMutate = leftParam;
                return leftSurface;
            } else{
                *paramMutate =(rightParam < leftParam) ? rightParam : leftParam;
                return (rightParam < leftParam) ? rightSurface : leftSurface;
            }
            */
        } else {
            *paramMutate = paramVal;
            return NULL;
        }
        
    }

    void getAABBInstersection(Ray& ray, float* paramMutate){
        float paramVal = this->surface->checkIntersection(ray);
        *paramMutate = paramVal;
    }

private:
    AbstractSurface* surface;
    AABBNode* left;
    AABBNode* right;
};
#endif
