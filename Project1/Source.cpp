#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>
#include <fstream>

using namespace sf;
using namespace std;

int tile = 65;
Vector2<int> offset(400, 9);

void load_candy_textures(Texture nc[], Texture sc[], Texture wc[]);
void candy_string(int i, string& color);
void load_sprites(Sprite normal[], Sprite striped[], Sprite wrapped[], Texture nc[], Texture sc[], Texture wc[]);
void load_grid(int type, int pieces[][9]);
bool adjacent_block(int x1, int y1, int x2, int y2);
void check_match_four(int grid[][9]);
void check_match(int grid[][9]);
void check_L(int grid[][9]);
void replace_piece(int grid[][9]);
void check_match_five(int grid[][9]);

void color_bomb(int grid[][9], int x1, int y1, int x2, int y2);
void striped_destruct(int grid[][9], int ID);
void wrapped_destruct(int grid[][9], int ID);
void wrapped_destruct_single(int grid[][9], int x, int y);
void wrapped_trigger(int grid[][9], int x1, int y1, int x2, int y2);

void wrap_stripe_trigger(int grid[][9]);

void save(int pieces[][9], int turn, int score);
void load(int pieces[][9], int turn, int score);
void score_manager(int value, int candies);

int points = 0, turns = 0;

int main()
{
    srand((unsigned int)time(0));
    points = 0;
    turns = 20;
    RenderWindow game(VideoMode(1080, 600), "Candy Crush <3");//setting window size
    
    //setting up text
    Text score_message, score_val, turn_message, turn_val, save_load_msg;
    Font calibri;
    Color color(0, 0, 128);
    calibri.loadFromFile("fonts/Calibri_Bold.ttf");
    score_message.setFont(calibri);
    score_message.setCharacterSize(48);
    score_message.setFillColor(color);
    score_message.setPosition(30, 30);
    score_message.setString("Points:");

    score_val.setFont(calibri);
    score_val.setCharacterSize(48);
    score_val.setFillColor(color);
    score_val.setPosition(45, 90);
    score_val.setString(to_string(points));

    turn_message.setFont(calibri);
    turn_message.setCharacterSize(48);
    turn_message.setFillColor(color);
    turn_message.setPosition(30, 150);
    turn_message.setString("Turns:");

    turn_val.setFont(calibri);
    turn_val.setCharacterSize(48);
    turn_val.setFillColor(color);
    turn_val.setPosition(45, 210);
    turn_val.setString(to_string(turns));

    save_load_msg.setFont(calibri);
    save_load_msg.setCharacterSize(30);
    save_load_msg.setFillColor(color);
    save_load_msg.setPosition(30, 400);
    save_load_msg.setString("To save game, press: S\nTo load game, press: L\n\nScor to beat: 50,000");

    //declaring textures
    Texture nc[5], sc[5], wc[5], bg, bomb, circle_img;
    bomb.loadFromFile("textures/bomb.png");
    bg.loadFromFile("textures/bg.png");
    circle_img.loadFromFile("textures/select_circle.png");

    load_candy_textures(nc, sc, wc);;

    //initializing all sprites
    Sprite background(bg), c_bomb(bomb), circle(circle_img);
    Sprite normal[5], wrapped[5], striped[5];

    load_sprites(normal, striped, wrapped, nc, sc, wc);

    //initializing game grid
    int pieces[9][9] = { {0},{0} };
    load_grid(1, pieces);

    //To control everything inside the graphics window
    int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
    int clicks = 0;
    Vector2<int> mouse_pos;
    while (game.isOpen()) {
        Event x;
        while (game.pollEvent(x)) {
            if (x.type == Event::Closed) {
                game.close();
            }
            if (x.type == Event::MouseButtonPressed) {
                if (x.key.code == Mouse::Left) {
                    if (turns > 0) {
                        mouse_pos = Mouse::getPosition(game);
                        mouse_pos -= offset;
                        mouse_pos /= tile;
                        clicks += 1;
                    }
                }
            }
            if (x.type == Event::KeyPressed) {
                if (x.key.code == Keyboard::S) {
                    save(pieces, turns, points);
                }
            }
            if (x.type == Event::KeyPressed) {
                if (x.key.code == Keyboard::L) {
                    load(pieces, turns, points);
                }
            }
            if (x.type == Event::KeyPressed) {
                if (x.key.code == Keyboard::Space) {//debuggin
                    cout << endl;
                    for (int i = 0;i < 9;i++) {
                        for (int j = 0;j < 9;j++) {
                            cout << pieces[j][i] << ", ";
                        }
                        cout << endl;
                    }
                }
            }
        }
        if (turns == 20) {
            points = 0;
        }
        if (clicks == 1) {
            x1 = mouse_pos.x;
            y1 = mouse_pos.y;
        }
        else if (clicks == 2) {
            x2 = mouse_pos.x;
            y2 = mouse_pos.y;
            if (adjacent_block(x1, y1, x2, y2)) {
                turns--;
                color_bomb(pieces, x1, y1, x2, y2);
                color_bomb(pieces, x2, y2, x1, y1);
                wrapped_trigger(pieces, x1, y1, x2, y2);
                wrapped_trigger(pieces, x2, y2, x1, y1);
                load_grid(2, pieces);
                int temp = pieces[x1][y1];
                pieces[x1][y1] = pieces[x2][y2];
                pieces[x2][y2] = temp;
            }
            clicks = 0;
        }
        check_match_five(pieces);
        check_match_four(pieces);
        check_L(pieces);
        wrap_stripe_trigger(pieces);
        check_match(pieces);
        for (int i = 0;i < 9;i++) {
            replace_piece(pieces);//this function is called multiple times to make sure all tiles go as down as possible
        }
        load_grid(2, pieces);

        //draw
        game.draw(background);
        game.draw(score_message);
        score_val.setString(to_string(points));
        game.draw(score_val);
        game.draw(turn_message);
        turn_val.setString(to_string(turns));
        game.draw(turn_val);
        game.draw(save_load_msg);
        for (int j = 0;j < 9;j++) {
            for (int i = 0;i < 9;i++) {
                if (pieces[i][j] >= 0 && pieces[i][j] < 5) {
                    normal[pieces[i][j]].setPosition(i * 65.0f + offset.x, j * 65.0f + offset.y);
                    game.draw(normal[pieces[i][j]]);
                }
                else if (pieces[i][j] >= 5 && pieces[i][j] < 10) {
                    striped[pieces[i][j] - 5].setPosition(i * 65.0f + offset.x, j * 65.0f + offset.y);
                    game.draw(striped[pieces[i][j] - 5]);
                }
                else if (pieces[i][j] >= 10 && pieces[i][j] < 15) {
                    wrapped[pieces[i][j] - 10].setPosition(i * 65.0f + offset.x, j * 65.0f + offset.y);
                    game.draw(wrapped[pieces[i][j] - 10]);
                }
                else if (pieces[i][j] == 16) {
                    c_bomb.setPosition(i * 65.0f + offset.x, j * 65.0f + offset.y);
                    game.draw(c_bomb);
                }
            }
        }
        if (clicks == 1) {
            circle.setPosition(x1 * 65.0f + offset.x, y1 * 65.0f + offset.y);
            game.draw(circle);
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
        sc[i].loadFromFile("textures/" + color + "_striped.png");
        wc[i].loadFromFile("textures/" + color + "_wrapped.png");
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

void load_grid(int type, int pieces[][9]) {
    for (int i = 0;i < 9;i++) {
        for (int j = 0;j < 9;j++) {
            if (type == 1 && pieces[i][j] >= 0) {
                pieces[i][j] = rand() % 5;
            }
            if (type == 2 && pieces[j][i] < 0) {
                pieces[j][i] = rand() % 5;
            }
        }
    }
}

bool adjacent_block(int x1, int y1, int x2, int y2) {
    if ((x1 == x2 && y1 == (y2 - 1)) || (x1 == (x2 - 1) && y1 == y2) || (x1 == x2 && y1 == (y2 + 1)) || (x1 == (x2 + 1) && y1 == y2)) {
        return true;
    }
    else {
        return false;
    }
}

void check_match(int grid[][9]) {
    for (int i = 0;i < 9;i++) {
        for (int j = 0;j < 9;j++) {
            if (grid[i][j] >= 0) {
                if (grid[i][j] == grid[i + 1][j] && grid[i][j] == grid[i + 2][j]) {
                    cout << "found at: " << i << "," <<j << endl;
                    score_manager(grid[i][j], 3);
                    grid[i][j] = -3;
                    grid[i + 1][j] = -3;
                    grid[i + 2][j] = -3;
                }
                if (grid[i][j] == grid[i][j + 1] && grid[i][j] == grid[i][j + 2]) {
                    cout << "found at: " << i << "," << j << endl;
                    score_manager(grid[i][j], 3);
                    grid[i][j] = -3;
                    grid[i][j + 1] = -3;
                    grid[i][j + 2] = -3;
                }
            }
        }
    }
}

void check_L(int grid[][9]) {
    for (int j = 0;j < 9;j++) {
        for (int i = 0;i < 9;i++) {
            if (grid[i][j] == grid[i + 1][j] && grid[i][j] == grid[i + 2][j]) {
                //check L-shape
                if (grid[i][j] == grid[i][j + 1] && grid[i][j] == grid[i][j + 2]) {
                    score_manager(grid[i][j], 4);
                    grid[i + 1][j] = -3;
                    grid[i + 2][j] = -3;
                    grid[i][j + 1] = -3;
                    grid[i][j + 2] = -3;
                    grid[i][j] += 10;
                }
                else if (j - 2 >= 0 && grid[i][j] == grid[i][j - 1] && grid[i][j] == grid[i][j - 2]) {
                    score_manager(grid[i][j], 4);
                    grid[i + 1][j] = -3;
                    grid[i + 2][j] = -3;
                    grid[i][j - 1] = -3;
                    grid[i][j - 2] = -3;
                    grid[i][j] += 10;
                }
                //check T-shape
                if (grid[i + 1][j] == grid[i + 1][j + 1] && grid[i + 1][j] == grid[i + 1][j + 2]) {
                    score_manager(grid[i][j], 4);
                    grid[i][j] = -3;
                    grid[i + 2][j] = -3;
                    grid[i + 1][j + 1] = -3;
                    grid[i + 1][j + 2] = -3;
                    grid[i + 1][j] += 10;
                    cout << "yos\n";
                }
                else if (j - 2 >= 0 && grid[i + 1][j] == grid[i + 1][j - 1] && grid[i + 1][j] == grid[i + 1][j - 2]) {
                    score_manager(grid[i][j], 4);
                    grid[i][j] = -3;
                    grid[i + 2][j] = -3;
                    grid[i + 1][j - 1] = -3;
                    grid[i + 1][j - 2] = -3;
                    grid[i + 1][j] += 10;
                    cout << "yos2\n";
                }
            }
            if (grid[i][j] == grid[i - 1][j] && grid[i][j] == grid[i - 2][j]) {
                //check L-shape
                if (grid[i][j] == grid[i][j + 1] && grid[i][j] == grid[i][j + 2]) {
                    score_manager(grid[i][j], 4);
                    grid[i - 1][j] = -3;
                    grid[i - 2][j] = -3;
                    grid[i][j + 1] = -3;
                    grid[i][j + 2] = -3;
                    grid[i][j] += 10;
                }
                else if (j - 2 >= 0 && grid[i][j] == grid[i][j - 1] && grid[i][j] == grid[i][j - 2]) {
                    score_manager(grid[i][j], 4);
                    grid[i - 1][j] = -3;
                    grid[i - 2][j] = -3;
                    grid[i][j - 1] = -3;
                    grid[i][j - 2] = -3;
                    grid[i][j] += 10;
                }
            }
            if (j - 1 >= 0 && grid[i][j] == grid[i][j + 1] && grid[i][j] == grid[i][j - 1]) {
                if (grid[i][j] == grid[i + 1][j] && grid[i][j] == grid[i + 2][j]) {
                    score_manager(grid[i][j], 4);
                    grid[i + 1][j] = -3;
                    grid[i + 2][j] = -3;
                    grid[i][j - 1] = -3;
                    grid[i][j + 1] = -3;
                    grid[i][j] += 10;
                }
                else if (grid[i][j] == grid[i - 1][j] && grid[i][j] == grid[i - 2][j]) {
                    score_manager(grid[i][j], 4);
                    grid[i - 1][j] = -3;
                    grid[i - 2][j] = -3;
                    grid[i][j - 1] = -3;
                    grid[i][j + 1] = -3;
                    grid[i][j] += 10;
                }
            }
        }
    }
}

void check_match_four(int grid[][9])
{
    for (int i = 0;i < 9;i++)
    {
        for (int j = 0;j < 9;j++)
        {
            if (grid[i][j] >= 0)
            {
                //column wise
                if (j - 1 >= 0 && grid[i][j] == grid[i][j - 1] && grid[i][j] == grid[i][j + 1] && grid[i][j] == grid[i][j + 2])
                {
                    score_manager(grid[i][j], 3);
                    grid[i][j] += 5;
                    grid[i][j - 1] = -3;
                    grid[i][j + 1] = -3;
                    grid[i][j + 2] = -3;
                }
                //row wise
                if ((grid[i][j] == grid[i - 1][j]) && (grid[i][j] == grid[i + 1][j]) && (grid[i][j] == grid[i + 2][j]))
                {
                    score_manager(grid[i][j], 3);
                    grid[i][j] += 5;
                    grid[i - 1][j] = -3;
                    grid[i + 1][j] = -3;
                    grid[i + 2][j] = -3;
                }
            }

        }
    }
}

void check_match_five(int grid[][9])
{
    for (int i = 0;i < 9;i++)
    {
        for (int j = 0;j < 9;j++)
        {
            if (grid[i][j] >= 0)
            {
                //column wise
                if ((grid[i][j] == grid[i][j + 1]) && (grid[i][j] == grid[i][j + 2]) && (grid[i][j] == grid[i][j + 3]) && (grid[i][j] == grid[i][j + 4]))
                {
                    score_manager(grid[i][j], 4);
                    grid[i][j] = -3;
                    grid[i][j + 1] = -3;
                    grid[i][j + 2] = 16;
                    grid[i][j + 3] = -3;
                    grid[i][j + 4] = -3;
                }
                //row wise
                if ((grid[i][j] == grid[i + 1][j]) && (grid[i][j] == grid[i + 2][j]) && (grid[i][j] == grid[i + 3][j]) && (grid[i][j] == grid[i + 4][j]))
                {
                    score_manager(grid[i][j], 4);
                    grid[i][j] = -3;
                    grid[i + 1][j] = -3;
                    grid[i + 2][j] = 16;
                    grid[i + 3][j] = -3;
                    grid[i + 4][j] = -3;
                }
            }
        }
    }
}

void replace_piece(int grid[][9]) {
    for (int i = 8;i > 0;i--) {
        for (int j = 8;j > 0;j--) {
            if (grid[j][i] == -3) {
                int temp = grid[j][i - 1];
                grid[j][i] = temp;
                grid[j][i - 1] = -3;
            }
        }
    }
}

void save(int pieces[][9], int turn, int score)
{
    ofstream fout;
    fout.open("Save.txt");
    fout << turn << " ";
    fout << score << endl;
    for (int i = 0;i < 9;i++)
    {
        for (int j = 0;j < 9;j++)
        {
            fout << pieces[i][j] << " ";
        }
        cout << endl;
    }
    fout.close();
}

void load(int pieces[][9], int turn, int score)
{
    ifstream fin;
    fin.open("Save.txt");
    if (!fin)
    {
        cout << "Error loading the save game" << endl;
    }
    else
    {
        while (!fin.eof())
        {
            fin >> turn;
            fin >> score;
            for (int i = 0;i < 9;i++)
            {
                for (int j = 0;j < 9;j++)
                {
                    fin >> pieces[i][j];
                }
            }
        }
    }
    fin.close();
}

void color_bomb(int grid[][9], int x1, int y1, int x2, int y2) {
    int striped_id = 0, wrapped_id = 0;
    if (grid[x1][y1] == 16) {
        grid[x1][y1] = -3;
        //swapped bomb with bomb
        if (grid[x2][y2] == 16) {
            for (int i = 0;i < 9;i++) {
                for (int j = 0;j < 9;j++) {
                    score_manager(grid[i][j], 1);
                    grid[i][j] = -3;
                }
            }
        }
        //swapped bomb with normal candy
        else if (grid[x2][y2] >= 0 && grid[x2][y2] < 5) {
            int value = grid[x2][y2];
            for (int i = 0;i < 9;i++) {
                for (int j = 0;j < 9;j++) {
                    if (grid[i][j] == value) {
                        score_manager(grid[i][j], 1);
                        grid[i][j] = -3;
                    }
                }
            }
        }
        //swapped bomb with striped candy
        else if (grid[x2][y2] >= 5 && grid[x2][y2] < 10) {
            striped_id = grid[x2][y2];
            for (int i = 0;i < 9;i++) {
                for (int j = 0;j < 9;j++) {
                    if (grid[i][j] == grid[x2][y2] - 5) {
                        grid[i][j] += 5;
                    }
                }
            }
            for (int i = 0;i < 9;i++) {
                for (int j = 0;j < 9;j++) {
                    striped_destruct(grid, striped_id);
                }
            }
        }
        //swapped bomb with wrapped candy
        else if (grid[x2][y2] >= 10 && grid[x2][y2] < 15) {
            wrapped_id = grid[x2][y2];
            for (int i = 0;i < 9;i++) {
                for (int j = 0;j < 9;j++) {
                    if (grid[i][j] == grid[x2][y2] - 10) {
                        grid[i][j] += 10;
                    }
                }
            }
            for (int i = 0;i < 9;i++) {
                for (int j = 0;j < 9;j++) {
                    wrapped_destruct(grid, wrapped_id);
                }
            }
        }
    }
}

void striped_destruct(int grid[][9], int ID) {
    for (int i = 0;i < 9;i++) {
        for (int j = 0; j < 9; j++)
        {
            if (grid[i][j] == ID) {
                if (ID == 5 || ID == 6) {//for red and green. since, red and green have vertical stripes
                    for (int k = 0;k < 9;k++) {
                        grid[i][k] = -3;
                        score_manager(grid[i][k], 1);
                    }
                }
                else if (ID >= 7 && ID <= 9) {//for blue, yellow and orange. since, they have horizontal stripes
                    for (int k = 0;k < 9;k++) {
                        grid[k][j] = -3;
                        score_manager(grid[k][j], 1);
                    }
                }
            }
        }
    }
}

void wrapped_destruct(int grid[][9], int ID) {
    for (int i = 0;i < 9;i++) {
        for (int j = 0; j < 9; j++)
        {
            if (grid[i + 1][j + 1] == ID) {
                grid[i + 1][j + 1] = -3;
                score_manager(grid[i + 1][j + 1], 1);
                grid[i + 1][j + 2] = -3;
                score_manager(grid[i + 1][j + 2], 1);
                grid[i + 1][j] = -3;
                score_manager(grid[i + 1][j], 1);
                grid[i + 2][j + 1] = -3;
                score_manager(grid[i + 2][j + 1], 1);
                grid[i + 2][j + 2] = -3;
                score_manager(grid[i + 2][j + 2], 1);
                grid[i + 2][j] = -3;
                score_manager(grid[i + 2][j], 1);
                grid[i][j + 1] = -3;
                score_manager(grid[i][j + 1], 1);
                grid[i][j + 2] = -3;
                score_manager(grid[i][j + 2], 1);
                grid[i][j] = -3;
                score_manager(grid[i][j], 1);
                ///detroy it's neighbours
            }
        }
    }
}

void score_manager(int value, int candies) {
    cout << "came here" << endl;
    switch (value) {
    case 0:
        points += 30 * candies;
        break;
    case 1:
        points += 30 * candies;
        break;
    case 2:
        points += 40 * candies;
        break;
    case 3:
        points += 50 * candies;
        break;
    case 5:
        points += 60 * candies;
        break;
    }
    cout << points << endl;
}

void wrapped_trigger(int grid[][9], int x1, int y1, int x2, int y2) {
    if (grid[x1][y1] >= 10 && grid[x1][y1] <= 14) {
        if (grid[x2][y2] >= 10 && grid[x2][y2] <= 14) {
            for (int i = x1 - 2; i <= x1 + 2; i++)
            {
                for (int j = x1 - 2; j < y1 + 2; j++)
                {
                    if (grid[i][j] >= 0) {
                        grid[i][j] = -3;
                        score_manager(grid[i][j], 1);
                    }
                }
            }
        }
        else if (grid[x2][y2] >= 5 && grid[x2][y2] <= 9) {
            for (int i = x1 - 1; i <= x1 + 1; i++)
            {
                for (int j = 0; j < 9; j++)
                {
                    if (grid[i][j] >= 0) {
                        grid[j][i] = -3;//vertical clearance
                        score_manager(grid[j][i], 1);
                    }
                }
            }
            for (int i = y1 - 1; i <= y1 + 1; i++)
            {
                for (int j = 0; j < 9; j++)
                {
                    if (grid[i][j] >= 0) {
                        grid[i][j] = -3;//horizontal clearance
                        score_manager(grid[i][j], 1);
                    }
                }
            }
        }
    }
}

void wrap_stripe_trigger(int grid[][9]) {
    //trigger striped candies
    for (int i = 0;i < 9;i++) {
        for (int j = 0; j < 9; j++)
        {
            if (grid[i][j] >= 5 && grid[i][j] < 10) {
                int temp = grid[i][j] - 5;
                if ((grid[i - 1][j] == temp && grid[i + 1][j] == temp) || (grid[i - 1][j] == temp && grid[i - 2][j] == temp) || (grid[i + 1][j] == temp && grid[i + 2][j] == temp)) {
                    striped_destruct(grid, temp + 5);
                    cout<<"papa phone uthayen";
                }
                if ((j - 1 >= 0 && grid[i][j - 1] == temp && grid[i][j + 1] == temp) || (j - 1 >= 0 && j - 2 >= 0 && grid[i][j - 1] == temp && grid[i][j - 2] == temp) || (grid[i][j + 1] == temp && grid[i][j + 2] == temp)) {
                    striped_destruct(grid, temp + 5);
                    cout<<"papa phone uthayen";
                }
            }
        }
    }
    //trigger wrapped candies
    for (int i = 0;i < 9;i++) {
        for (int j = 0; j < 9; j++)
        {
            if (grid[i][j] >= 10 && grid[i][j] < 15) {
                int temp = grid[i][j] - 10;
                if ((grid[i - 1][j] == temp && grid[i + 1][j] == temp) || (grid[i - 1][j] == temp && grid[i - 2][j] == temp) || (grid[i + 1][j] == temp && grid[i + 2][j] == temp)) {
                    wrapped_destruct_single(grid, i, j);
                }
                if ((j - 1 >= 0 && grid[i][j - 1] == temp && grid[i][j + 1] == temp) || (j - 1 >= 0 && j - 2 >= 0 && grid[i][j - 1] == temp && grid[i][j - 2] == temp) || (grid[i][j + 1] == temp && grid[i][j + 2] == temp)) {
                    wrapped_destruct_single(grid, i, j);
                }
            }
        }
    }
}

void wrapped_destruct_single(int grid[][9], int x, int y) {
    for (int i = x - 1; i <= x + 1; i++)
    {
        for (int j = y - 1; j <= y + 1; j++)
        {
            cout << "wrapping" << endl;
            cout << i << "," << j << endl;
            grid[i][j] = -3;
        }
    }
}