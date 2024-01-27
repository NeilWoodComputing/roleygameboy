#include <gb/gb.h>
#include <stdint.h>
#include "roleymap1.h"
#include "roley.h"
#include <stdbool.h>
#include <gb/metasprites.h>




uint8_t playerX=80, playerY=135, nextPlayerX =0, nextPlayerY =0;
uint8_t joypadCurrent=0, joypadPrevious=0;
uint8_t roleyXAnimationStage = 1;
uint8_t animationDelay = 4;
uint8_t animationXDelayTimeout = 0;
bool movementDirectionForward = false;
uint8_t movePixels = 2;

bool checkAnimationTimeout(bool);
//bool OnAPlatform(int, int);
void setUpGame(void);
void getPlayerInput(int, int);
void setMetaSprite(int);

uint8_t groundLevel = 135;
bool playerIsJumping = false;
uint8_t playerYVelocity = 0; 
uint8_t startingSprite = 0; 
uint8_t jumpHeight = 13;

int platformPositionX = 1000;

// try to understand this badboy
// dy & dx relative to center point of metasprite.
const metasprite_t platfrom_metasprite[] = {
     {.dy=-8, .dx=-8, .dtile=16, .props=0},
    //  {.dy=0, .dx=8, .dtile=18, .props=0},
     {.dy=8, .dx=-8, .dtile=17, .props=0},
    // {.dy=0, .dx=8, .dtile=19, .props=0},
    METASPR_TERM
};

typedef struct platformObject{
    int platformX;
   int platformY;
   int platformHeight;
   int platformWidth;
} platformObjects;

const platformObjects platform_objects[] = {
     {.platformX=100, .platformY=120, .platformHeight=5, .platformWidth =30},
     {.platformX=120, .platformY=100, .platformHeight=5, .platformWidth =30},
     {.platformX=140, .platformY=80, .platformHeight=5, .platformWidth =30},
     {.platformX=200, .platformY=120, .platformHeight=5, .platformWidth =30},
     {.platformX=220, .platformY=100, .platformHeight=5, .platformWidth =30},
     {.platformX=240, .platformY=80, .platformHeight=5, .platformWidth =30}
};

void main(void)
{
    setUpGame();

    // Loop forever
    while(1) {

        nextPlayerX = playerX, nextPlayerY = playerY;

        getPlayerInput(nextPlayerX, nextPlayerY);

        //move sprite params (sprite index, sprite positionX, sprite positionY)
        move_sprite(0, playerX, playerY);
        move_sprite(1, playerX+8, playerY);

        //animate sprite X axis
        set_sprite_tile(0,(4*roleyXAnimationStage)-4);
        set_sprite_tile(1,(2 + (4*roleyXAnimationStage))-4);

        //0,2
        //4,6
        //8,10,
        //12,14


		// Game main loop processing goes here

		// Done processing, yield CPU and wait for start of next frame
        //wait_vbl_done();
        vsync();
    }

}

void getPlayerInput(int nextPlayerX, int nextPlayerY) {

        //only to stop the warnings for now
        nextPlayerX = nextPlayerY;

        // set previous state
        joypadPrevious = joypadCurrent;

        //get joypad button presses
        joypadCurrent =joypad();

        // Move player position and x animation stage
        if(joypadCurrent & J_RIGHT){
           // playerX+movePixels;
            scroll_bkg(movePixels, 0);  
            platformPositionX = platformPositionX-movePixels/2;
            
            if (checkAnimationTimeout(true)== true) {
                if (roleyXAnimationStage == 4) roleyXAnimationStage = 1;
                else roleyXAnimationStage++;
            }
        }
        if(joypadCurrent & J_LEFT){
            //playerX-movePixels;
            scroll_bkg(-movePixels, 0);
            platformPositionX = platformPositionX+movePixels/2;

            if (checkAnimationTimeout(false) == true) {
                if (roleyXAnimationStage == 1) roleyXAnimationStage = 4;
                else roleyXAnimationStage--;
            }
        }

    
        startingSprite = 0;

        //loop runs too fast for the rendering i think?
        for (int i =0; i < sizeof(platform_objects); i++)
        {
              //  setMetaSprite(i);
                //usleep();
                //delay();
        };

        setMetaSprite(0);
        // setMetaSprite(1);
        // setMetaSprite(2);
        // setMetaSprite(3);
        // setMetaSprite(4);
        // setMetaSprite(5);
        // setMetaSprite(6);
            
       // hide_sprites_range(startingSprite, 40);





        //groundLevel = playerY;

        // vertical movement
        // if(joypadCurrent & J_DOWN){
        //     //playerY++;
        //     //scroll_bkg(0, movePixels);
        // }
        // if(joypadCurrent & J_UP){
        //     //playerY--;
        //     scroll_bkg(0, -movePixels);
        // }

        //check new A button click and not already jumping
        if(joypadCurrent & J_A && joypadCurrent != joypadPrevious && !playerIsJumping){
            playerIsJumping = true;
            playerYVelocity = jumpHeight;       
        }

        //update jump position
        if (playerIsJumping) {
            playerY = playerY-playerYVelocity;
        }
        
        //Update jump velocity
        if(playerIsJumping) {
            if (playerY < groundLevel)
            {
                playerYVelocity--;
            }
            else {

                //TODO Add rebound bounce on first jump
                playerYVelocity = 0;
                playerIsJumping = false;
            }        
        }     
}

void setMetaSprite(int Index) {
     startingSprite += move_metasprite(
        platfrom_metasprite,
        0,
        startingSprite,
        platformPositionX+platform_objects[Index].platformX,
        platform_objects[Index].platformY
    );
}

void setUpGame(void){
// make background visible
    SHOW_BKG;

    // make sprite visible
    SHOW_SPRITES;

    //Use stacked sprites
    SPRITES_8x16;

   // DISPLAY_ON;

    // 1: vram start ,2 how many tiles in vram, 3:tile image code
    //background layer
    set_bkg_data(0, roleyMap1_TILE_COUNT, roleyMap1_tiles);

    //sprite layer
    set_sprite_data(0, roley_TILE_COUNT, roley_tiles);


    //params: start location 1, 2  image size 3, 4 tile map pointer 5
    set_bkg_tiles(0, 0, 32, 18, roleyMap1_map);

    //sprite sheet start location
    set_sprite_tile(0,0);
    set_sprite_tile(1,2);

    //set sprite positions (X default-8 Y default -16)
    move_sprite(0, 80, 80);
    move_sprite(0, 80, 80);
}

bool checkAnimationTimeout(bool movingForward)
{
    if (movingForward == true){
        if (movementDirectionForward == true && animationXDelayTimeout > animationDelay)
        {
            animationXDelayTimeout=0;
            return true;
        }
        else if (movementDirectionForward == false) {
            movementDirectionForward = 1;
            animationXDelayTimeout=1;
            return false;
        }
    }
    else {
        if (movementDirectionForward == false && animationXDelayTimeout > animationDelay)
        {
            animationXDelayTimeout=0;
            return true;
        }
        else if (movementDirectionForward == true) {
            movementDirectionForward = 0;
            animationXDelayTimeout=1;
            return false;
        }
    }
    animationXDelayTimeout++;
    return false;
}