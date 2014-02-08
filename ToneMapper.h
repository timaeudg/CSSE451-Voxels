#ifndef __TONEMAPPER
#define __TONEMAPPER

#include "GenVector.h"

class ToneMapper{

    public:
        ToneMapper(){
            this->width = 0;
            this->height = 0;
            this->data = NULL;
        }

        ToneMapper(int width, int height){
            this->width = width;
            this->height = height;

            alloc();
        }

        ~ToneMapper(){
            dealloc();
        }
        
        Vector3 at(unsigned int x, unsigned int y) const
    	{
    		return data[x+y*this->width];
	    }

    	Vector3 & at(unsigned int x, unsigned int y)
    	{
    		return data[x+y*this->width];
    	}
        
        Buffer toneMap(){
            int size = this->width * this->height;
            float maxVal = -1.0;
            for(int i = 0; i<size; i++){
                Vector3 pixel = data[i];
                float r = pixel[0];
                float g = pixel[1];
                float b = pixel[2];

                if(r>maxVal){
                    maxVal = r;
                }
                if(g>maxVal){
                    maxVal = g;
                }
                if(b>maxVal){
                    maxVal = b;
                }
            }

            float toneMapVal = (255.0f / maxVal);
            
            Buffer buffer = Buffer(this->width, this->height);
            for(int k = 0; k<this->width; k++){
                for(int i = 0; i<this->height; i++){
                    Vector3 mappedVec = this->at(k,i)*toneMapVal;
                    Color color = Color(abs((int) mappedVec[0]), abs((int) mappedVec[1]), abs((int) mappedVec[2]));
                    buffer.at(k,i) = color;
                }
            }
            return buffer;
        }

    private:
        int width;
        int height;
        Vector3* data;

        void alloc(){
            size_t size = this->width * this->height;
            if(size == 0){
                return;
            }
            data = (Vector3 *) malloc(size*sizeof(Vector3));
        }

        void dealloc(){
            free(data);
        }


};

#endif
