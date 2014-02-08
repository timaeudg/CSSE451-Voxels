#ifndef __RAY
#define __RAY

class Ray{

    public:
        
        Ray(){
            Ray(Vector3(0, 0, 0), Vector3(0, 0, 0));
        }

        Ray(Vector3 origin, Vector3 direction){
            setOrigin(origin);
            setDirection(direction);
        }

        void setOrigin(Vector3 origin){
            this->origin = origin;
        }

        void setDirection(Vector3 dir){
           // Vector3 normal = dir.normalize();
            this->direction = dir;
        }

        Vector3 getDirection(){
            return this->direction;
        }

        Vector3 getOrigin(){
            return this->origin;
        }

        Vector3 pointAtParameterValue(float t){
            return this->origin + this->direction * t;
        }

        ~Ray(){

        }

    private:
        Vector3 direction;
        Vector3 origin;


};
#endif
