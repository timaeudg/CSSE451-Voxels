#include <SFML/Graphics.hpp>
#include "RayTracer.h"

int main(int argc, char** argv)
{
    int width = 200;
    int height = 200;
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
    
    sf::RenderWindow window(sf::VideoMode(width, height), "SFML works!");
    setupScene(argc, argv);
    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
	
        Buffer buf = run()->toneMap();
        
        sf::Texture tex;
	    tex.create(width, height);
        sf::Image img;
	    img.create(width, height, sf::Color(0,0,0));
    	sf::Sprite bufferSprite;
	
	
        for (int j = 0; j < width; j++)
        {
            for (int i = 0; i < height; i++)
            {
    	      int r = buf.at(j,i)[0];
	          int g = buf.at(j,i)[1];
	          int b = buf.at(j,i)[2];
    	      img.setPixel(j,i,sf::Color(r,g,b));
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
        printf("FPS: %f\n", fps);
    }
    

    return 0;
}
