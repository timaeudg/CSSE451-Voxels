#ifndef __LIGHT
#define __LIGHT

class Light{
    
    public:
        Light(){}
        ~Light(){}

        virtual Vector3 getPos(){}

        virtual int getMaterialIndex(){}

    private:
};
#endif
