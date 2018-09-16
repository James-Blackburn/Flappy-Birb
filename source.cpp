#include <SFML/Graphics.hpp> //  For graphics
#include <vector> // for dynamic array (std::vector)
#include <cstdlib> // for the rand() function
#include <string> // for string management and manipulation
#include <fstream> // for file IO

// James Blackburn
// 16/09/18

struct Birb{
    // The Birb struct represents the player bird
    int x;
    int y;
    float altitude;
    int rotation = 0;
    sf::Sprite *sprite;
};

struct PipeBody{
    // The PipeBody struct represents the main bit of the pipe
    // It is important that the pipe body is individual of the
    // Pipe top, so that it can scale in size without distortion
    int x;
    int y;
    int length;
    int beenThrough = 0;
    sf::Sprite sprite;
};

class Game{
    private:
        // Class Members
        sf::Vector2i floor;
        sf::Vector2i resolution;
        sf::RenderWindow window;
        sf::Clock clock;
        Birb player;

        // Vectors
        std::vector<PipeBody> TopPipes;
        std::vector<PipeBody> BottomPipes;

        // Constants
        const std::string FILENAME = "score.txt";
        const float JUMP_HEIGHT = 10.0;
        const float DECREASE = 0.5;
        const int PIPE_WIDTH = 64;
        const int FLOOR = 660;

        // Textures
        sf::Texture backgroundTexture;
        sf::Texture birbTexture;
        sf::Texture pipeBodyTexture;
        sf::Texture pipeTopTexture;
        sf::Texture floorTexture;

        // Sprites
        sf::Sprite backgroundSprite;
        sf::Sprite birbSprite;
        sf::Sprite pipeBodySprite;
        sf::Sprite pipeTopSprite;
        sf::Sprite floorSprite;

        // Fonts
        sf::Font font;
        sf::Text menuText;
        sf::Text scoreText;
        sf::Text FPS;

        // Class variables
        int SCROLL_SPEED = 2;
        int spawner = 120;
        int checkpoint = 0;
        int highscore;
        int score = 0;

        void startGame(){
            // This method starts the game
            // it also resets a lot of data to transition to the next game
            TopPipes.clear();
            BottomPipes.clear();
            SCROLL_SPEED = 2;
            scoreText.setString("SCORE: 0");
            scoreText.setPosition(0,42);
            score = 0;
            floor.x = 0;
            setLevel();
            update();
        }

        void menu(){
            // This method is just the menu
            int exit = 0;

            // determine new highscore
            if (score > highscore){
                highscore = score;
                saveScore();
            }

            while (!exit){

                // Draw menu
                window.clear();
                window.draw(backgroundSprite);

                menuText.setColor(sf::Color(255,0,0));
                menuText.setCharacterSize(150);
                menuText.setPosition(250,120);
                menuText.setString("FLAPPY BIRB");
                window.draw(menuText);

                menuText.setColor(sf::Color(0,255,0));
                menuText.setCharacterSize(40);
                menuText.setPosition(415,330);
                menuText.setString("Press SPACE to play!");
                window.draw(menuText);

                menuText.setColor(sf::Color(0,0,255));
                menuText.setCharacterSize(40);
                menuText.setPosition(455,430);
                menuText.setString("HIGHSCORE : "+std::to_string(highscore));
                window.draw(menuText);

                window.display();

                // Check keyboard events
                sf::Event event;
                while (window.pollEvent(event)){
                    if (event.type == sf::Event::Closed){
                        window.close();
                    } else if (event.type == sf::Event::KeyPressed){
                        if (event.key.code == sf::Keyboard::Escape){
                            window.close();
                        } else if (event.key.code == sf::Keyboard::Space){
                            exit = 1;
                        }
                    }
                }
            }
            startGame();
        }

        void loadFiles(){
            // This method loads up external files

            // Textures
            backgroundTexture.loadFromFile("background.png");
            birbTexture.loadFromFile("birb.png");
            pipeBodyTexture.loadFromFile("pipe_body.png");
            pipeTopTexture.loadFromFile("pipe_top.png");
            floorTexture.loadFromFile("floor.png");

            //Sprites
            backgroundSprite.setTexture(backgroundTexture);
            birbSprite.setTexture(birbTexture);
            pipeBodySprite.setTexture(pipeBodyTexture);
            pipeTopSprite.setTexture(pipeTopTexture);
            floorSprite.setTexture(floorTexture);
            birbSprite.setOrigin(16,16);

            //Fonts
            font.loadFromFile("font.ttf");
            menuText.setFont(font);
            scoreText.setFont(font);
            FPS.setFont(font);
        }

        void setLevel(){
            // This method sets up the level

            // Setting up player
            player.altitude = 0.0;
            player.x = resolution.x/2;
            player.y = resolution.y/2;
            player.sprite = &birbSprite;
        }

        void update(){
            // This method updates the game every frame
            while(window.isOpen()){
                handleEvents();
                updatePipes();
                updateFloor();
                updatePlayer();
                display();
                getFps();
            }
        }

        void display(){
            // This method displays everything to the screen
            window.clear();

            window.draw(backgroundSprite);
            window.draw(floorSprite);

            // Draw Pipes
            for (unsigned int i=0; i<TopPipes.size(); i++){
                pipeTopSprite.setPosition(TopPipes[i].x-3,TopPipes[i].y+PIPE_WIDTH*TopPipes[i].length);
                window.draw(TopPipes[i].sprite);
                window.draw(pipeTopSprite);

                pipeTopSprite.setPosition(BottomPipes[i].x-3,BottomPipes[i].y-32);
                window.draw(BottomPipes[i].sprite);
                window.draw(pipeTopSprite);
            }

            int rotation = player.rotation;
            player.sprite->rotate(rotation);
            window.draw(*player.sprite);
            // setting player sprite rotation back to previous rotation
            // as SFML rotates sprites based on their current rotation
            // and not on their original orientation
            player.sprite->rotate(-rotation);

            window.draw(scoreText);
            window.draw(FPS);
            window.display();
        }

        void handleEvents(){
            // This method handles events
            sf::Event event;
            while (window.pollEvent(event)){
                if (event.type == sf::Event::Closed){
                    window.close();
                } else if (event.type == sf::Event::KeyPressed){
                    if (event.key.code == sf::Keyboard::Space){
                        player.altitude = JUMP_HEIGHT;
                        player.rotation = -30;
                    } else if (event.key.code == sf::Keyboard::Escape){
                        window.close();
                    }
                }
            }
        }

        void getFps(){
            // This method gets the current fps
            double currentTime = clock.restart().asSeconds();
            int fps = (1.f / currentTime);
            FPS.setString("FPS: "+std::to_string(fps));
        }

        void updatePlayer(){
            player.altitude -= DECREASE;
            player.y -= player.altitude;
            player.sprite->setPosition(player.x,player.y);

            // Checking if player has hit the floor
            if (player.y+32 >= FLOOR){
                menu();
            }

            if (player.altitude < 0 && player.rotation < 90){
                player.rotation += 2;
            }
        }

        void updatePipes(){
            // This method is for updating the pipes

            // Spawn new Pipes
            if (spawner > 240/SCROLL_SPEED){
                spawner = 0;
                // Making sure there is always a gap between pipes
                int length1 = (rand()%6)+1;
                int length2 = (rand()%6)+1;
                if ((length1 + length2) > 7){
                    int variation = rand()%3;
                    if (variation == 0){
                        length1 = 3;
                        length2 = 3;
                    } else if (variation == 1){
                        length1 = 2;
                        length2 = 4;
                    } else if (variation == 2){
                        length1 = 4;
                        length2 = 2;
                    }

                }

                // Spawn Top Pipe
                PipeBody pipe;
                TopPipes.push_back(pipe);
                TopPipes.back().length = length1;
                TopPipes.back().x = resolution.x;
                TopPipes.back().y = 0;
                TopPipes.back().sprite = pipeBodySprite;
                TopPipes.back().sprite.setScale(1.0,TopPipes.back().length);
                TopPipes.back().sprite.setPosition(TopPipes.back().x,TopPipes.back().y);

                // Spawn Bottom Pipe
                PipeBody pipe2;
                BottomPipes.push_back(pipe2);
                BottomPipes.back().length = length2;
                BottomPipes.back().x = resolution.x;
                BottomPipes.back().y = FLOOR-64*BottomPipes.back().length;
                BottomPipes.back().sprite = pipeBodySprite;
                BottomPipes.back().sprite.setScale(1.0,BottomPipes.back().length);
                BottomPipes.back().sprite.setPosition(BottomPipes.back().x,BottomPipes.back().y);
            }

            spawner++;
            // Scroll, Collide and Cull pipes
            int index = 0;
            for (unsigned int i=0; i<TopPipes.size(); i++){
                // Scroll Pipes
                TopPipes[i].x -= SCROLL_SPEED;
                TopPipes[i].sprite.setPosition(TopPipes[i].x,TopPipes[i].y);

                BottomPipes[i].x -= SCROLL_SPEED;
                BottomPipes[i].sprite.setPosition(BottomPipes[i].x,BottomPipes[i].y);

                // Check if player has collided with pipes

                // Check Score
                if (!TopPipes[i].beenThrough){
                    if (player.x+32 >= TopPipes[i].x && player.x+32 <= TopPipes[i].x+PIPE_WIDTH){
                        TopPipes[i].beenThrough = 1;
                        score++;
                        char c[10];
                        std::sprintf(c, "%d", score);
                        std::string string(c);
                        sf::String str(string);
                        scoreText.setString("SCORE: "+str);
                    }
                }

                // Check Top pipes collision
                if (player.x+16 >= TopPipes[i].x && player.x+16 <= TopPipes[i].x+PIPE_WIDTH
                    && player.y <= TopPipes[i].y+64*TopPipes[i].length+48){
                    menu();
                }

                // Check Bottom pipes Collision
                if (player.x+16 >= BottomPipes[i].x && player.x+16 <= BottomPipes[i].x+PIPE_WIDTH
                    && player.y >= BottomPipes[i].y-48){
                    menu();
                }


                // Cull Pipes
                if (TopPipes[i].x < -64){
                    TopPipes.erase(TopPipes.begin()+index);
                    BottomPipes.erase(BottomPipes.begin()+index);
                }
                index++;
            }
        }

        void updateFloor(){
            // This method updates the floor to make it scroll
            floorSprite.setPosition(floor.x,floor.y);
            floor.x -= SCROLL_SPEED;
            if (floor.x <= -resolution.x){
                floor.x = 0;
            }
        }

        void saveScore(){
            // This method saves the highscore to an external text file
            std::ofstream outputFile;
            outputFile.open(FILENAME, std::ios::trunc);
            if (outputFile.is_open()){
                outputFile << std::to_string(highscore);
                outputFile.close();
            }
        }

        void readScore(){
            // This method reads the previous highscore from an external text file
            std::ifstream inputFile;
            inputFile.open(FILENAME);
            if (inputFile.is_open()){
                std::string value;
                while (std::getline(inputFile, value)){
                    highscore = std::stoi(value);
                }
                inputFile.close();
            }
        }

    public:
        Game() : window(sf::VideoMode(1280,720), "FLAPPY BIRB - James Blackburn"){
            // This is the constructor for the Game class
            // we are initializing the sf::vectors here as they
            // cannot be initialised anywhere else
            window.setFramerateLimit(60);
            resolution.x = 1280;
            resolution.y = 720;
            floor.x = 0;
            floor.y = FLOOR;
            readScore();
            loadFiles();
            menu();
        }
};

int main(){
    Game game;
    return 0;
}
