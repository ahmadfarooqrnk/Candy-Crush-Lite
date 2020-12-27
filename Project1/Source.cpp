#include <SFML/Graphics.hpp>
#include <time.h>

using namespace sf;

int tile = 54;
Vector2<int> offset(48, 25);

void load_candy_textures(Texture nc[], Texture sc[], Texture wc[]);
void candy_string(int i, String& color);
void load_sprites(Sprite normal[], Sprite striped[], Sprite wrapped[], Texture nc[], Texture sc[], Texture wc[]);

int main()
{
    srand((unsigned int)time(0));
    RenderWindow game(VideoMode(1080, 600), "Candy Crush <3");//setting window size
    //declaring textures
    Texture nc[5], sc[5], wc[5], bg, bomb;
    bomb.loadFromFile("textures/bomb.png");
    bg.loadFromFile("textures/bg.png");

    load_candy_textures(nc, sc, wc);;
    //initializing all sprites
    Sprite background(bg), c_bomb(bomb);
    Sprite normal[5], wrapped[5], striped[5];

    load_sprites(normal, striped, wrapped, nc, sc, wc);

    while (game.isOpen()) {

    }

    return 0;
}

void load_candy_textures(Texture nc[], Texture sc[], Texture wc[]) {
    String color;
    for (int i = 0;i < 5;i++) {
        candy_string(i, color);
        nc[i].loadFromFile("textures/" + color + ".png");
        sc[i].loadFromFile("textures/" + color + ".png");
        wc[i].loadFromFile("textures/" + color + ".png");
    }
}

void candy_string(int i, String& color) {
    switch (i) {
    case 0:
        color = "red";
    case 1:
        color = "blue";
    case 2:
        color = "green";
    case 3:
        color = "yellow";
    case 4:
        color = "orange";
    }
}

void load_sprites(Sprite normal[], Sprite striped[], Sprite wrapped[], Texture nc[], Texture sc[], Texture wc[]) {
    for (int i = 0;i < 5;i++) {
        normal[i].setTexture(nc[i]);
        striped[i].setTexture(sc[i]);
        wrapped[i].setTexture(wc[i]);
    }
}