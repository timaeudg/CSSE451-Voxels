#ifndef __AABBTREE
#define __AABBTREE

#include <vector>
#include "AABB.h"
#include "AABBNode.h"

class AABBTree{

public:
    AABBTree(){
        
    }
    
    AABBTree(std::vector<AbstractSurface*> surfaces){
        AABBNode* head = createTree(surfaces);
        this->root = head;
    }
    
    ~AABBTree(){}
    
    AbstractSurface* getIntersection(Ray& ray, float* paramVal){
        return this->root->getIntersection(ray, paramVal);
    }

private:
    AABBNode* root;
    
    AABBNode* createTree(std::vector<AbstractSurface*> surfaces){
        AABB* boundingBox = new AABB();
        for(int j = 0; j < surfaces.size(); j++){
            boundingBox->expandBox(*surfaces[j]);
        }
        
        if(surfaces.size()>1){
            Vector3 axisLengths = boundingBox->getAxisLengths();
            int longestAxis = 0;
            if(axisLengths[1] > axisLengths[2] && axisLengths[1] > axisLengths[0]){
                longestAxis = 1;
            } else if(axisLengths[2] > axisLengths[1] && axisLengths[2] > axisLengths[0]){
                longestAxis = 2;
            }
            
            std::vector<AbstractSurface*> sortedSurfaces = merge_sort(surfaces, longestAxis);
            
            int splitPoint = (sortedSurfaces.size()/2);

            std::vector<AbstractSurface*> left = std::vector<AbstractSurface*>();
            std::vector<AbstractSurface*> right = std::vector<AbstractSurface*>();
            
            for(int j = 0; j<splitPoint; j++){
                left.push_back(sortedSurfaces[j]);
            }
            for(int j = splitPoint; j < sortedSurfaces.size(); j++){
                right.push_back(sortedSurfaces[j]);
            }

            AABBNode* leftNode = createTree(left);
            AABBNode* rightNode = createTree(right);
            AABBNode* parent = new AABBNode(boundingBox, leftNode, rightNode);
            return parent;
        } else {
            //base case
            AABBNode* leaf = new AABBNode(surfaces[0], NULL, NULL);
            return leaf;
        }
    }
    
    //! \brief Performs a recursive merge sort on the given vector
    //! \param vec The vector to be sorted using the merge sort
    //! \return The sorted resultant vector after merge sort is
    //! complete.
    //! Adapted from: http://en.wikibooks.org/wiki/Algorithm_Implementation/Sorting/Merge_sort#C.2B.2B
    std::vector<AbstractSurface*> merge_sort(std::vector<AbstractSurface*>& vec, int axisIndex = 0)
    {
        // Termination condition: List is completely sorted if it
        // only contains a single element.
        if(vec.size() == 1)
        {
            return vec;
        }
    
        // Determine the location of the middle element in the vector
        std::vector<AbstractSurface*>::iterator middle = vec.begin() + (vec.size() / 2);
    
        std::vector<AbstractSurface*> left(vec.begin(), middle);
        std::vector<AbstractSurface*> right(middle, vec.end());
    
        // Perform a merge sort on the two smaller vectors
        left = merge_sort(left);
        right = merge_sort(right);
    
        return merge(left, right);
    }
    
    //! \brief Merges two sorted vectors into one sorted vector
    //! \param left A sorted vector of integers
    //! \param right A sorted vector of integers
    //! \return A sorted vector that is the result of merging two sorted
    //! vectors.
    //! Adapted from: http://en.wikibooks.org/wiki/Algorithm_Implementation/Sorting/Merge_sort#C.2B.2B
    std::vector<AbstractSurface*> merge(const std::vector<AbstractSurface*>& left, const std::vector<AbstractSurface*>& right, int axisIndex = 0)
    {
        // Fill the resultant vector with sorted results from both vectors
        std::vector<AbstractSurface*> result;
        unsigned left_it = 0, right_it = 0;
    
        while(left_it < left.size() && right_it < right.size())
        {
            // If the left value is smaller than the right it goes next
            // into the resultant vector
            if(left[left_it]->getCenterPoint()[axisIndex] < right[right_it]->getCenterPoint()[axisIndex])
            {
                result.push_back(left[left_it]);
                left_it++;
            }
            else
            {
                result.push_back(right[right_it]);
                right_it++;
            }
        }
    
        // Push the remaining data from both vectors onto the resultant
        while(left_it < left.size())
        {
            result.push_back(left[left_it]);
            left_it++;
        }
    
        while(right_it < right.size())
        {
            result.push_back(right[right_it]);
            right_it++;
        }
    
        return result;
    }
};
#endif
