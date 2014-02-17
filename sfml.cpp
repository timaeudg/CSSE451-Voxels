#include <SFML/Graphics.hpp>
#include "RayTracer.h"

int main(int argc, char** argv)
{
    int width = 200;
    int heith = 200;
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
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    setupScene(argc, argv);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        Buffer* buf = run();

        sf::Color imageColor[width][height];

        for (int j = 0; j < width; j++)
        {
            for (int i = 0; i < height; i++)
            {
                imageColor[j][i] = sf::Color((byte)i, (byte)j, 0);
            }
        }
        sf::Image image= sf::Image(imageColor);

        sf::Texture texture = sf::Texture(image);

        sf::Sprite sprite = sf::Sprite(texture);

        window.draw(sprite);

        window.display();
    }

    return 0;
}
