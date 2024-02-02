#include <gb/gb.h>
#include <stdint.h>
#include "roleymap1.h"
#include "roley.h"
#include <stdbool.h>
#include <gb/metasprites.h>

uint8_t playerX=80, playerY=135, nextPlayerX =0, nextPlayerY =0;
uint8_t playerWidth = 20;

uint8_t joypadCurrent=0, joypadPrevious=0;
uint8_t roleyXAnimationStage = 1;
uint8_t animationDelay = 4;
uint8_t animationXDelayTimeout = 0;
bool movementDirectionForward = false;
uint8_t movePixels = 2;
//uint8_t moveDelay = 3;


typedef struct collisionObject{
   int positionX;
   int positionY;
   int height;
   int width;
} collisionObjects;

bool checkAnimationTimeout(bool);
void setUpGame(void);
void getPlayerInput(int, int);
void setMetaSprite(int, metasprite_t[]);

bool isColliding(collisionObjects[], int, int);

uint8_t fixedGroundLevel = 135;
uint8_t groundLevel = 135;

bool onPlatform = false;
uint8_t currentPlatformHeight = 0;

bool playerIsJumping = false;
uint8_t playerYVelocity = 0;
uint8_t startingSprite = 0;
uint8_t jumpHeight = 13;

int platformPositionX = -175;
int platformPositionXDefault = -255;

// try to understand this badboy
// dy & dx relative to center point of metasprite.
const metasprite_t platfrom_metasprite[] = {
     {.dy=-8, .dx=0, .dtile=16, .props=0},
     {.dy=0, .dx=8, .dtile=18, .props=0},
    METASPR_TERM
};

const collisionObjects platform_objects[] = {
     {.positionX=100, .positionY=120, .height=10, .width =20},
     {.positionX=120, .positionY=100, .height=10, .width =20},
     {.positionX=140, .positionY=80, .height=10, .width =20},
     {.positionX=200, .positionY=120, .height=10, .width =20},
     {.positionX=220, .positionY=100, .height=10, .width =20},
     {.positionX=240, .positionY=80, .height=10, .width =0}
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
        //nextPlayerX = nextPlayerY;

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
        


        startingSprite = 2;

        //loop runs too fast for the rendering I think?
        for (int i =0; i < sizeof(platform_objects); i++)
        {
              //  setMetaSprite(i);
                //usleep();
                //delay();
        };

        setMetaSprite(0, platfrom_metasprite);
        setMetaSprite(1, platfrom_metasprite);
        setMetaSprite(2, platfrom_metasprite);
        setMetaSprite(3, platfrom_metasprite);
        setMetaSprite(4, platfrom_metasprite);
        setMetaSprite(5, platfrom_metasprite);

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

        if(joypadCurrent & J_B){
            movePixels = 4;
        }
        else{
            movePixels = 2;
        }


        //TODO figure this out
        if (isColliding(platform_objects, nextPlayerX, nextPlayerY)){
            //playerY = 20;
            //playerX = 200;
            //playerYVelocity = 0;
            //playerIsJumping = false;
        }
        else {
            groundLevel = fixedGroundLevel;
            //playerYVelocity = jumpHeight;
            currentPlatformHeight = fixedGroundLevel;
           // playerIsJumping = true;
        }

        //update jump position
        if (playerIsJumping) {
            playerY = playerY-playerYVelocity;
        }

        // if (
        //     playerIsJumping && playerY == groundLevel && !onPlatform ||
        //     playerIsJumping && onPlatform && playerY == currentPlatformHeight
        // ) {
        //     playerY = playerY-playerYVelocity;
        // }

        //Update jump velocity
        //if(playerIsJumping) {
            if (
                playerY < groundLevel 
                //&& !onPlatform ||
               // onPlatform && playerY < currentPlatformHeight
            )
            {
                //playerIsJumping = true;
                playerYVelocity--;
            }
            else {

                //TODO Add rebound bounce on first jump
                playerYVelocity = 0;
                playerIsJumping = false;
            }
        //}

        if (platformPositionX < platformPositionXDefault ){
            platformPositionX = 0;
        }
        else if ( platformPositionX > 0){
            platformPositionX = platformPositionXDefault;
        }
}

void setMetaSprite(int index, metasprite_t metasprite[]) {
     startingSprite += move_metasprite(
        metasprite,
        0,
        startingSprite,
        platformPositionX+platform_objects[index].positionX,
        platform_objects[index].positionY
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

bool isColliding(collisionObjects collision_Objects[], int posX, int posY){

    //collisionObjects all_coliding_Objects[];

    bool isCollidingTemp = false;

    for (int i =0; i < sizeof(collision_Objects); i++)
    {
        if (
            (
                posX > platformPositionX+collision_Objects[i].positionX-8 &&
                posX <  platformPositionX+collision_Objects[i].positionX + collision_Objects[i].width-8
            ) 
            ||
            (
                (posX+playerWidth) > platformPositionX+collision_Objects[i].positionX-8 &&
                (posX+playerWidth) <  platformPositionX+collision_Objects[i].positionX + collision_Objects[i].width-8    
            ) 
            &&
                posY <= collision_Objects[i].positionY-16 &&
                posY >= collision_Objects[i].positionY-(16+collision_Objects[i].height)
            ){

                isCollidingTemp= true;
                if (collision_Objects[i].positionY-16 < currentPlatformHeight){
                    currentPlatformHeight = collision_Objects[i].positionY-(collision_Objects[i].height+16);            
                }

              //  all_coliding_Objects[sizeof(all_coliding_Objects)] = collision_Objects[i];
                
                //all_coliding_Objects = collision_Objects[i];
                //return true;
            }    

             //groundLevel = tempGroundLevel;
  
    };

     onPlatform = isCollidingTemp;
    // if (onPlatform == false){
    //     currentPlatformHeight = fixedGroundLevel;
    // }

    return isCollidingTemp;
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

