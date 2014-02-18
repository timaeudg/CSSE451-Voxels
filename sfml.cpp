#include <SFML/Graphics.hpp>
#include "RayTracer.h"

int main(int argc, char** argv)
{
    int width = 1920;
    int height = 1080;
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
    objLoader objData = objLoader();
    objData.load(argv[1]);
    
    Scene scene = loadScene(&objData, argv[2]);
    printf("scene loaded\n");
    sf::RenderWindow window(sf::VideoMode(width, height), "SFML works!");

    RayGenerator rayGen = RayGenerator(scene.getCamera(), width, height, 90.0);
    sf::Clock clock;
    float lastTime = 0;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
	
        //Buffer* buf = run();
        
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
        Buffer mappedBuff = buf.toneMap();

        sf::Texture tex;
	    tex.create(width, height);
        sf::Image img;
	    img.create(width, height, sf::Color(0,0,0));
    	sf::Sprite bufferSprite;
	
	
        for (int j = 0; j < width; j++)
        {
            for (int i = 0; i < height; i++)
            {
	      int r = mappedBuff.at(j,i)[0];
	      int g = mappedBuff.at(j,i)[1];
	      int b = mappedBuff.at(j,i)[2];
	      img.setPixel(j,i,sf::Color(r,g,b));
                //imageColor[j][i] = sf::Color((byte)i, (byte)j, 0);
            }
        }
    	tex.loadFromImage(img);
	    bufferSprite.setTexture(tex);
    	window.clear();
	
        window.draw(bufferSprite);

        window.display();
        float currentTime = clock.getElapsedTime().asSeconds();
        clock.restart();
        float fps = 1.0 / (currentTime);
        lastTime = currentTime;
        printf("FPS: %f\n", fps);
    }

    return 0;
}
