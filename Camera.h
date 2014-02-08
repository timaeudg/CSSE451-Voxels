#ifndef __CAMERA
#define __CAMERA

#include "GenVector.h"

class Camera{

    public:
        Camera(){
        }

        Camera(Vector3 *position, Vector3 *look, Vector3 *up){
            setBasis(position, look, up);
        }

        ~Camera(){
        }

        void setBasis(Vector3 *pos, Vector3 *lookAtPoint, Vector3 *up){
            this->position = *pos;

            Vector3 lookAtVector = (*lookAtPoint) - this->position;
            lookAtVector.normalize();

            Vector3 w = -lookAtVector;
            Vector3 u = (*up).cross(w);
            Vector3 v = w.cross(u);
            
            w.normalize();
            u.normalize();
            v.normalize();

            this->w = w;
            this->u = u;
            this->v = v;
        }

        Vector3 getU(){
            return this->u;
        }

        Vector3 getV(){
            return this->v;
        }

        Vector3 getW(){
            return this->w;
        }

        Vector3 getPos(){
            return this->position;
        }

    private:
        Vector3 position;
        Vector3 w;
        Vector3 u;
        Vector3 v;

};
#endif
