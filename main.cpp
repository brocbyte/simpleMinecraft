#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <cmath>
#include <GL/glu.h>

#ifndef GL_SRGB8_ALPHA8
#define GL_SRGB8_ALPHA8 0x8C43
#endif

#define MAP_SIZE 20
#define BLOCK_SIZE 20
#define SKY_SIZE 1000

int map[MAP_SIZE][MAP_SIZE][MAP_SIZE];
static const GLfloat unitCube[] =
        {
                // positions    // texture coordinates
                -1, -1, -1,  1, 1,   // left
                -1,  1, -1,  1, 0,
                -1, -1,  1,  0, 1,
                -1, -1,  1,  0, 1,
                -1,  1, -1,  1, 0,
                -1,  1,  1,  0, 0,

                1, -1, -1,  1, 1,    // right
                1,  1, -1,  1, 0,
                1, -1,  1,  0, 1,
                1, -1,  1,  0, 1,
                1,  1, -1,  1, 0,
                1,  1,  1,  0, 0,

                -1, -1, -1,  0, 0,   // bottom
                1, -1, -1,  1, 0,
                -1, -1,  1,  0, 1,
                -1, -1,  1,  0, 1,
                1, -1, -1,  1, 0,
                1, -1,  1,  1, 1,

                -1,  1, -1,  0, 0,   // up
                1,  1, -1,  1, 0,
                -1,  1,  1,  0, 1,
                -1,  1,  1,  0, 1,
                1,  1, -1,  1, 0,
                1,  1,  1,  1, 1,

                -1, -1, -1,  1, 1,   // back
                1, -1, -1,  0, 1,
                -1,  1, -1,  1, 0,
                -1,  1, -1,  1, 0,
                1, -1, -1,  0, 1,
                1,  1, -1,  0, 0,

                -1, -1,  1,  1, 1,  // front
                1, -1,  1,  0, 1,
                -1,  1,  1,  1, 0,
                -1,  1,  1,  1, 0,
                1, -1,  1,  0, 1,
                1,  1,  1,  0, 0
        };
class Player{
public:
    double x, y, z;
    double dx, dy, dz;
    double w, h, d;
    bool onGround;
    float angleX, angleY;
    float speed;
    Player(): x(100), y(200), z(100),
              angleX(0), angleY(0),
              speed(5),dx(0),dy(0),dz(0),
              onGround(false), w(5),h(20),d(5){}
    void update(float time);
    void keyboard();
    void collision(double Dx,double Dy,double Dz);
};
bool check(int x, int y, int z){
    if ((x < 0) || (x >= MAP_SIZE) ||
        (y < 0) || (y >= MAP_SIZE) ||
        (z < 0) || (z >= MAP_SIZE)) return false;
    return map[x][y][z];
}
void Player::keyboard(){
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) if (onGround) {onGround=false; dy=12;};

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        dx =- sin(angleX/180*M_PI) * speed;
        dz =- cos(angleX/180*M_PI) * speed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        dx = sin(angleX/180*M_PI) * speed;
        dz = cos(angleX/180*M_PI) * speed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        dx = sin((angleX+90)/180*M_PI) * speed;
        dz = cos((angleX+90)/180*M_PI) * speed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        dx = sin((angleX-90)/180*M_PI) * speed;
        dz = cos((angleX-90)/180*M_PI) * speed;
    }
}

void drawSurface(const GLfloat surf[]){
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, 5 * sizeof(GLfloat), surf);
    glTexCoordPointer(2, GL_FLOAT, 5 * sizeof(GLfloat), surf + 3);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
int drawCube(sf::Texture texture[], const GLfloat cube[]){
    for(int i = 0 ; i < 6 ; i++) {
        glEnable(GL_TEXTURE_2D);
        sf::Texture::bind(&texture[i]);
        drawSurface(cube + i*30);
    }
}
void Player::collision(double Dx,double Dy,double Dz){
    for (int X=(x-w)/BLOCK_SIZE;X<(x+w)/BLOCK_SIZE;X++)
        for (int Y=(y-h)/BLOCK_SIZE;Y<(y+h)/BLOCK_SIZE;Y++)
            for (int Z=(z-d)/BLOCK_SIZE;Z<(z+d)/BLOCK_SIZE;Z++)
                if (check(X,Y,Z))  {
                    if (Dx>0)  x = X*BLOCK_SIZE-w;
                    if (Dx<0)  x = X*BLOCK_SIZE+BLOCK_SIZE+w;
                    if (Dy>0)  y = Y*BLOCK_SIZE-h;
                    if (Dy<0) {y = Y*BLOCK_SIZE+BLOCK_SIZE+h; onGround=true; dy=0;}
                    if (Dz>0)  z = Z*BLOCK_SIZE-d;
                    if (Dz<0)  z = Z*BLOCK_SIZE+BLOCK_SIZE+d;
                }
}
void Player::update(float time)
{
    if (!onGround)
        dy-=1.5*time;
    onGround=false;

    x+=dx*time;
    collision(dx,0,0);
    y+=dy*time;
    collision(0,dy,0);
    z+=dz*time;
    collision(0,0,dz);

    dx=dz=0;
}
int main()
{
    bool exit = false;
    Player player;
    GLfloat block[36 * 5], skyBlock[36 * 5];
    bool mLeft = false, mRight = false;
    for(int i = 0 ; i < 180 ; i++){
        block[i] = skyBlock[i] = unitCube[i];
        if(i % 5 != 3 && i % 5 != 4){
            skyBlock[i] *= SKY_SIZE;
            block[i] *= (BLOCK_SIZE / 2.);
        }
    }
    for(int x = 0 ; x < MAP_SIZE ; x ++){
        for(int z = 0 ; z < MAP_SIZE ; z ++){
            for(int y = 0 ; y < MAP_SIZE ; y ++) {
                if(!y || rand() < 2000)
                    map[x][y][z] = 1;
            }
        }
    }
    while (!exit)
    {
        // Request a 24-bits depth buffer when creating the window
        sf::ContextSettings contextSettings;
        contextSettings.depthBits = 24;

        // Create the main window
        sf::RenderWindow window(sf::VideoMode(1920, 1080), "SFML graphics with OpenGL", sf::Style::Default, contextSettings);
        window.setVerticalSyncEnabled(true);

        // Create a sprite for the background
        sf::Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("resources/background.jpg"))
            return EXIT_FAILURE;
        sf::Sprite background(backgroundTexture);

        // OpenGl - инициализации
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glClearDepth(1.f);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(90.f, 1.f, 1.f, 3000.f);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);

        // Make the window the active window for OpenGL calls
        window.setActive(true);

        // Configure the viewport (the same size as the window)
        glViewport(0, 0, window.getSize().x, window.getSize().y);

        // Setup a perspective projection
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        GLfloat ratio = static_cast<float>(window.getSize().x) / window.getSize().y;
        glFrustum(-ratio, ratio, -1.f, 1.f, 1.f, 2000.f);

        // Textures
        sf::Texture blockTextures[6];
        blockTextures[0].loadFromFile("resources/side.jpg");
        blockTextures[1].loadFromFile("resources/side.jpg");
        blockTextures[2].loadFromFile("resources/bottom.jpg");
        blockTextures[3].loadFromFile("resources/top.jpg");
        blockTextures[4].loadFromFile("resources/side.jpg");
        blockTextures[5].loadFromFile("resources/side.jpg");

        sf::Texture skyboxTextures[6];
        skyboxTextures[0].loadFromFile("resources/skybox/left.png");
        skyboxTextures[1].loadFromFile("resources/skybox/right.png");
        skyboxTextures[2].loadFromFile("resources/skybox/bottom.png");
        skyboxTextures[3].loadFromFile("resources/skybox/up.png");
        skyboxTextures[4].loadFromFile("resources/skybox/back.png");
        skyboxTextures[5].loadFromFile("resources/skybox/front.png");
        for(int i = 0 ; i < 6 ; i++) {
            skyboxTextures[i].generateMipmap();
            blockTextures[i].generateMipmap();
        }

        // Enable position and texture coordinates vertex components
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(3, GL_FLOAT, 5 * sizeof(GLfloat), block);
        glTexCoordPointer(2, GL_FLOAT, 5 * sizeof(GLfloat), block + 3);

        // Disable normal and color vertex components
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);

        // Make the window no longer the active window for OpenGL calls
        window.setActive(false);

        // Create a clock for measuring the time elapsed
        sf::Clock clock;
        float time;

        // Start game loop
        while (window.isOpen())
        {
            // Process events
            sf::Event event;
            while (window.pollEvent(event))
            {
                // Close window: exit
                if (event.type == sf::Event::Closed)
                {
                    exit = true;
                    window.close();
                }

                // Escape key: exit
                if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))
                {
                    exit = true;
                    window.close();
                }
                // Adjust the viewport when the window is resized
                if (event.type == sf::Event::Resized)
                {
                    // Make the window the active window for OpenGL calls
                    window.setActive(true);
                    glViewport(0, 0, event.size.width, event.size.height);
                    // Make the window no longer the active window for OpenGL calls
                    window.setActive(false);
                }
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Right)
                        mRight = true;
                    if (event.mouseButton.button == sf::Mouse::Left)
                        mLeft = true;
                }
            }
            // Обработка WASD
            player.keyboard();
            time = clock.getElapsedTime().asMilliseconds();
            clock.restart();
            time = time/50;
            if (time>3) time=3;
            player.update(time);

            // Make the window the active window for OpenGL calls
            window.setActive(true);

            // Clear the depth buffer
            glClear(GL_DEPTH_BUFFER_BIT);

            // Поворот камеры
            POINT mousexy;
            GetCursorPos(&mousexy);
            int xt = window.getPosition().x + 1920 / 2;
            int yt = window.getPosition().y + 1080 / 2;
            player.angleX += (xt - mousexy.x) / 3;
            player.angleY += (yt - mousexy.y) / 3;
            if(player.angleY < -89.0)
                player.angleY = -89.;
            if(player.angleY > 89.0)
                player.angleY = 89.;
            SetCursorPos(xt,yt);

            if (mRight || mLeft)
            {
                double x = player.x;
                double y = player.y+player.h/2;
                double z = player.z;

                int X,Y,Z,oldX,oldY,oldZ;
                int dist=0;
                while(dist<120)  // радиус действия
                {
                    dist++;

                    x += -sin(player.angleX/180*M_PI);    X=x/BLOCK_SIZE;
                    y +=  tan(player.angleY/180*M_PI);    Y=y/BLOCK_SIZE;
                    z += -cos(player.angleX/180*M_PI);    Z=z/BLOCK_SIZE;

                    if (check(X,Y,Z)) {
                        if (mLeft) {
                            map[X][Y][Z] = 0;
                            break;
                        } else {
                            map[oldX][oldY][oldZ] = 1;
                            break;
                        }
                    }
                    oldX=X; oldY=Y; oldZ=Z;
                }
            }

            mLeft = mRight = false;



            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            gluLookAt(player.x,player.y + player.h / 2,player.z,player.x-sin(player.angleX/180*M_PI),player.y+player.h/2+(tan(player.angleY/180*M_PI)),player.z-cos(player.angleX/180*M_PI), 0, 1, 0);


            // Draw the map
            for(int x = 0 ; x < MAP_SIZE ; x ++){
                for(int z = 0 ; z < MAP_SIZE ; z ++){
                    for(int y = 0 ; y < MAP_SIZE ; y ++){
                        if(map[x][y][z]){
                            glTranslatef((float)x * BLOCK_SIZE + BLOCK_SIZE/2.,float(y) * BLOCK_SIZE + BLOCK_SIZE/2.,float(z) * BLOCK_SIZE + BLOCK_SIZE/2.);
                            drawCube(blockTextures,block);
                            glTranslatef(-(float)x * BLOCK_SIZE - BLOCK_SIZE/2.,-float(y) * BLOCK_SIZE - BLOCK_SIZE/2.,-float(z) * BLOCK_SIZE - BLOCK_SIZE/2.);
                        }
                    }
                }
            }
            
            // Draw skybox
            glTranslatef(player.x,player.y,player.z);
            drawCube(skyboxTextures,skyBlock);
            glTranslatef(-player.x,-player.y,-player.z);

            window.setActive(false);
            window.display();
        }
    }

    return EXIT_SUCCESS;
}
