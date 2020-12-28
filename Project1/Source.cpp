#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>

using namespace sf;
using namespace std;

int tile = 65;
Vector2<int> offset(400, 9);

void load_candy_textures(Texture nc[], Texture sc[], Texture wc[]);
void candy_string(int i, string& color);
void load_sprites(Sprite normal[], Sprite striped[], Sprite wrapped[], Texture nc[], Texture sc[], Texture wc[]);

int main()
{
    srand((unsigned int)time(0));
    RenderWindow game(VideoMode(1080, 600), "Candy Crush <3");//setting window size
    game.setFramerateLimit(60);
    
    //declaring textures
    Texture nc[5], sc[5], wc[5], bg, bomb;
    bomb.loadFromFile("textures/bomb.png");
    bg.loadFromFile("textures/bg.png");

    load_candy_textures(nc, sc, wc);;
    //initializing all sprites
    Sprite background(bg), c_bomb(bomb);
    Sprite normal[5], wrapped[5], striped[5];

    load_sprites(normal, striped, wrapped, nc, sc, wc);

    //initializing game grid

    int pieces[9][9] = { {0},{0} };
    for (int i = 0;i < 9;i++) {
        for (int j = 0;j < 9;j++) {
            pieces[i][j] = rand() % 5;
        }
    }

    //To control everything inside the graphics window
    while (game.isOpen()) {
        Vector2<int> mouse_pos;
        int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
        Event x;
        while (game.pollEvent(x)) {
            if (x.type == Event::Closed) {
                game.close();
            }
            if (x.type == Event::MouseButtonPressed) {
                if (x.key.code == Mouse::Left) {
                    mouse_pos = Mouse::getPosition(game);
                    mouse_pos -= offset;
                    mouse_pos /= tile;
                    x1 = mouse_pos.x;
                    y1 = mouse_pos.y;
                }
            }
        }
        game.clear();
        //draw
        game.draw(background);
        for (int i = 0;i < 9;i++) {
            for (int j = 0;j < 9;j++) {
                normal[pieces[i][j]].setPosition(i * 65.0f + offset.x, j * 65.0f + offset.y);
                game.draw(normal[pieces[i][j]]);
            }
        }
        game.display();
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void load_candy_textures(Texture nc[], Texture sc[], Texture wc[]) {
    string color;
    for (int i = 0;i < 5;i++) {
        candy_string(i, color);
        nc[i].loadFromFile("textures/" + color + ".png");
        sc[i].loadFromFile("textures/" + color + ".png");
        wc[i].loadFromFile("textures/" + color + ".png");
    }
}

void candy_string(int i, string& color) {
    switch (i) {
    case 0:
        color = "red";
        break;
    case 1:
        color = "blue";
        break;
    case 2:
        color = "green";
        break;
    case 3:
        color = "yellow";
        break;
    case 4:
        color = "orange";
        break;
    }
}

void load_sprites(Sprite normal[], Sprite striped[], Sprite wrapped[], Texture nc[], Texture sc[], Texture wc[]) {
    for (int i = 0;i < 5;i++) {
        normal[i].setTexture(nc[i]);
        striped[i].setTexture(sc[i]);
        wrapped[i].setTexture(wc[i]);
    }
}