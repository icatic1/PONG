#include "mbed.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"
#include "C12832.h"

C12832 lcd(SPI_MOSI, SPI_SCK, SPI_MISO, p8, p11);

TS_StateTypeDef TS_State = { 0 };
InterruptIn btn1(p8);
InterruptIn btn2(p9);


//Faculty of Electrical Engineering Sarajevo

//Embedded systems project in Mbed, June 2021

//PONG

//Samra Behić and Irvin Ćatić








































































































// Classes
//--------------------------------------------------------
class Result{
    int player = 0, computer = 0;

    public:
    void setUp(){ // refreshing the result
        BSP_LCD_SetTextColor (LCD_COLOR_WHITE);
        BSP_LCD_SetFont(&Font16);
        char* rez = ":";
        char textString[16];
        int length = snprintf(textString, 16, "%d", player);
        char textString2[16];
        int length2 = snprintf(textString2, 16, "%d", computer);
        
        BSP_LCD_DisplayStringAt(82, 10, (uint8_t *)textString, LEFT_MODE);
        BSP_LCD_DisplayStringAt(142, 10, (uint8_t *)textString2, LEFT_MODE);
        BSP_LCD_DisplayStringAt(112, 10, (uint8_t *)rez, LEFT_MODE);
    }

    Result(){
        setUp();
    }
    void playerScores(){
        player++;
        setUp();
    }
    void computerScores(){
        computer++;
        setUp();
    }
    
    int getPlayerScore(){
        return player;
    }
    int getComputerScore(){
        return computer;
    }
};

enum Dir{ // easier to add more directions and makes the code readable
    Right=1,
    Left=2,
    Up=3,
    Down=4,
    UpRight=5,
    UpLeft=6,
    DownRight=7,
    DownLeft=8
};

class Direction{
    int x=0, y=0;

    void setAxis(Dir direction){ // sets unidirection for movement
        switch(direction){

            case Right:
            x=1;
            y=0;
            break;

            case Left:
            x=-1;
            y=0;
            break;

            case Up:
            x=0;
            y=-1;
            break;

            case Down:
            x=0;
            y=1;
            break;

            case UpRight:
            x=1;
            y=-1;
            break;

            case UpLeft:
            x=-1;
            y=-1;
            break;

            case DownRight:
            x=1;
            y=1;
            break;

            case DownLeft:
            x=-1;
            y=1;
            break;
        }

    }
public:
    Direction(Dir direction){
        setAxis(direction);
    }

    Direction getDirection(Dir direction){
        setAxis(direction);
        return *this;
    }

    int getX(){
        return x;
    }

    int getY(){
        return y;
    }

};

enum Level { // number needed for speed of ball and paddle
    EASY=3, MEDIUM=5, HARD=7
};

class Ball{
    int x,y,r=5;
    uint32_t color = LCD_COLOR_YELLOW;
    Level lev;
public:
    Ball(uint32_t newColor, Level setLevel){
        lev = setLevel;
        x=120;
        y=120;
        color=newColor;
        BSP_LCD_SetTextColor (color); // draw ball in the middle
        BSP_LCD_FillCircle (x, y, r);
    }

    void resetBall(){
        BSP_LCD_SetTextColor (LCD_COLOR_DARKGRAY); // erase old ball
        BSP_LCD_FillCircle (x, y, r);
        x=120;
        y=120;
        BSP_LCD_SetTextColor (color); // draw new ball in the middle
        BSP_LCD_FillCircle (x, y, r);
    }

    int getX(){
        return x;
    }

    int getY(){
        return y;
    }
    
    void moveBall(Direction dir){
        BSP_LCD_SetTextColor (LCD_COLOR_DARKGRAY); // erase old ball
        BSP_LCD_FillCircle (x, y, r);
        
        x=x+dir.getX()*lev;
        y=y+dir.getY()*lev;
        BSP_LCD_SetTextColor (color); // draw new ball
        BSP_LCD_FillCircle (x, y, r);
    }
    
};

class Paddle{
    int x,y,width=5,height=45;
    uint32_t color=LCD_COLOR_RED;
public:
    Paddle(int x, int y, uint32_t color){
        this->x = x;
        this->y = y;
        this->color = color;
        BSP_LCD_SetTextColor (color);
        BSP_LCD_FillRect (x, y, width, height);
    }

    void resetPaddle(int setX, int setY){
        BSP_LCD_SetTextColor (LCD_COLOR_DARKGRAY); // erase old paddle
        BSP_LCD_FillRect (x, y, width, height);
        
        x=setX;
        y=setY;

        BSP_LCD_SetTextColor (color); // draw new paddle
        BSP_LCD_FillRect (x, y, width, height);
    }

    void movePaddle(int moveY){
        BSP_LCD_SetTextColor (LCD_COLOR_DARKGRAY); // erase old paddle
        BSP_LCD_FillRect (x, y, width, height);
        
        y=y+moveY;
        BSP_LCD_SetTextColor (color); // draw new paddle
        BSP_LCD_FillRect (x, y, width, height);
    }

    int getX(){
        return x;
    }

    int getY(){
        return y;
    }


};

//--------------------------------------------------------------------------

void frontPage() {
    BSP_LCD_Clear(LCD_COLOR_LIGHTGRAY);
    
    BSP_LCD_SetFont(&Font24);
    char* text="PONG";
    char* text2 = "Start Game";
    char* exit = "Exit";
    
    BSP_LCD_SetTextColor (LCD_COLOR_RED);
    BSP_LCD_SetBackColor(LCD_COLOR_LIGHTGRAY);
    BSP_LCD_DisplayStringAt(85, 50, (uint8_t *)text, LEFT_MODE);
    
    
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_SetTextColor (LCD_COLOR_DARKGRAY);
    BSP_LCD_SetBackColor(LCD_COLOR_LIGHTGRAY);
    BSP_LCD_DisplayStringAt(65, 110, (uint8_t *)text2, LEFT_MODE);
    
    BSP_LCD_SetTextColor (LCD_COLOR_DARKGRAY);
    BSP_LCD_SetBackColor(LCD_COLOR_LIGHTGRAY);
    BSP_LCD_DisplayStringAt(95, 140, (uint8_t *)exit, LEFT_MODE);
}

void displayStay(){ // lcd screen
        lcd.cls();
        lcd.locate(1, 3);
        lcd.printf("Pong");
        lcd.locate(1, 20);
        lcd.printf("Samra i Irvin | US ");
}

void paddleOptions(){
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_FillRect(20,130,25,8);
    BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);
    BSP_LCD_FillRect(80,130,25,8);
    BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
    BSP_LCD_FillRect(20,170,25,8);
    BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
    BSP_LCD_FillRect(80,170,25,8);
}

void ballOptions(){
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_FillCircle(150,130,10);
    BSP_LCD_SetTextColor(LCD_COLOR_DARKGREEN);
    BSP_LCD_FillCircle(210,130,10);
    BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
    BSP_LCD_FillCircle(150,170,10);
    BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
    BSP_LCD_FillCircle(210,170,10);
}

void chooseAColorPage() {
    BSP_LCD_Clear(LCD_COLOR_LIGHTGRAY);
    
    BSP_LCD_SetFont(&Font16);
    char* text="Choose a color:";
    char* text1 = "PADDLE";
    char* text2 = "BALL";
   
    
    BSP_LCD_SetTextColor (LCD_COLOR_DARKGRAY);
    BSP_LCD_SetBackColor(LCD_COLOR_LIGHTGRAY);
    BSP_LCD_DisplayStringAt(40, 20, (uint8_t *)text, LEFT_MODE);
    
    BSP_LCD_DrawRect(1,60,120,178);
    BSP_LCD_DrawRect(120, 60,117,178);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_SetTextColor (LCD_COLOR_DARKGRAY);
    BSP_LCD_SetBackColor(LCD_COLOR_LIGHTGRAY);
    BSP_LCD_DisplayStringAt(30, 75, (uint8_t *)text1, LEFT_MODE);
    BSP_LCD_SetTextColor (LCD_COLOR_DARKGRAY);
    BSP_LCD_SetBackColor(LCD_COLOR_LIGHTGRAY);
    BSP_LCD_DisplayStringAt(160, 75, (uint8_t *)text2, LEFT_MODE);
    
    paddleOptions();
    ballOptions();

    BSP_LCD_SetTextColor (LCD_COLOR_LIGHTGRAY);
    BSP_LCD_FillRect(95,200,50,25); 
    BSP_LCD_SetTextColor (LCD_COLOR_DARKGRAY);
    BSP_LCD_DrawRect(95,200,50,25); 

    char* textOK = "OK";
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_SetTextColor (LCD_COLOR_DARKGRAY);
    BSP_LCD_SetBackColor(LCD_COLOR_LIGHTGRAY);
    BSP_LCD_DisplayStringAt(110, 207, (uint8_t *)textOK, LEFT_MODE);
    
}



void chooseALevelPage() {
    BSP_LCD_Clear(LCD_COLOR_LIGHTGRAY);
    
    BSP_LCD_SetFont(&Font16);
    char* text="Choose a level:";
    char* text1 = "EASY";
    char* text2 = "MEDIUM";
    char* text3 = "HARD";
    
    BSP_LCD_SetTextColor (LCD_COLOR_RED);
    BSP_LCD_SetBackColor(LCD_COLOR_LIGHTGRAY);
    BSP_LCD_DisplayStringAt(40, 50, (uint8_t *)text, LEFT_MODE);
    
    
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_SetTextColor (LCD_COLOR_DARKGRAY);
    BSP_LCD_SetBackColor(LCD_COLOR_LIGHTGRAY);
    BSP_LCD_DisplayStringAt(95, 100, (uint8_t *)text1, LEFT_MODE);
    BSP_LCD_SetTextColor (LCD_COLOR_DARKGRAY);
    BSP_LCD_SetBackColor(LCD_COLOR_LIGHTGRAY);
    BSP_LCD_DisplayStringAt(85, 125, (uint8_t *)text2, LEFT_MODE);
    BSP_LCD_SetTextColor (LCD_COLOR_DARKGRAY);
    BSP_LCD_SetBackColor(LCD_COLOR_LIGHTGRAY);
    BSP_LCD_DisplayStringAt(95, 150, (uint8_t *)text3, LEFT_MODE);
}

void setNet(){
    BSP_LCD_SetTextColor (LCD_COLOR_LIGHTGRAY );
    BSP_LCD_SetBackColor(LCD_COLOR_DARKGRAY);
    char linija[1][2]={"|"};
    for(int i = 3; i<240; i+=4){
            BSP_LCD_DisplayStringAt(112, i, (uint8_t *)linija[0], LEFT_MODE);
    }
}

void gameScreen () {
    BSP_LCD_Clear(LCD_COLOR_DARKGRAY);
    setNet();
}

void winPage() {
    BSP_LCD_SetTextColor (LCD_COLOR_LIGHTGRAY);
    BSP_LCD_FillRect (3, 40, 234, 150);
    
    BSP_LCD_SetFont(&Font24);
    char* text="YOU";
    char* text2 = "WIN";
    char* textAgain = "Play Again";
    char* back = "Exit";
    
    BSP_LCD_SetTextColor (LCD_COLOR_DARKGRAY );
    BSP_LCD_SetBackColor(LCD_COLOR_LIGHTGRAY);
    BSP_LCD_DisplayStringAt(60, 60, (uint8_t *)text, LEFT_MODE);
    BSP_LCD_DisplayStringAt(125, 60, (uint8_t *)text2, LEFT_MODE);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(20, 130, (uint8_t *)textAgain, LEFT_MODE);
    BSP_LCD_DisplayStringAt(175, 130, (uint8_t *)back, LEFT_MODE);
}

void gameOverPage() {
    BSP_LCD_SetTextColor (LCD_COLOR_LIGHTGRAY);
    BSP_LCD_FillRect (3, 40, 234, 150);
    
    BSP_LCD_SetFont(&Font24);
    char* text="GAME";
    char* text2 = "OVER";
    char* textAgain = "Play Again";
    char* back = "Exit";
    
    BSP_LCD_SetTextColor (LCD_COLOR_RED);
    BSP_LCD_SetBackColor(LCD_COLOR_LIGHTGRAY);
    BSP_LCD_DisplayStringAt(45, 60, (uint8_t *)text, LEFT_MODE);
    BSP_LCD_DisplayStringAt(125, 60, (uint8_t *)text2, LEFT_MODE);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_SetTextColor (LCD_COLOR_DARKGRAY);
    BSP_LCD_DisplayStringAt(20, 130, (uint8_t *)textAgain, LEFT_MODE);
    BSP_LCD_DisplayStringAt(175, 130, (uint8_t *)back, LEFT_MODE);
    
}


// color chosen functions
void selectedPaddle(int x, int y){
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DrawHLine(x,y,25);
    BSP_LCD_DrawHLine(x,y+8,25);
    BSP_LCD_DrawVLine(x,y,8);
    BSP_LCD_DrawVLine(x+25,y,8);
}
void selectedBall(int x, int y){
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DrawCircle(x,y,10);
}


//Interrupt functions
Paddle player=Paddle(0,0,LCD_COLOR_BLACK);
Level level=EASY;
void moveDown(){
    player.movePaddle(2*level);
}
void moveUp(){
    player.movePaddle(-2*level);
}

int main() {

    //setting up 
    BSP_LCD_Init();
    if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) == TS_ERROR) {
        printf("BSP_TS_Init error\n");
    }
    BSP_LCD_Clear(LCD_COLOR_WHITE);

  while(1){
    frontPage();
    displayStay();
    
   
   
   bool startGame=false, exitGame = false;
    while (1) { // main page
        BSP_TS_GetState(&TS_State);
        if(TS_State.touchDetected) {
            
            uint16_t x1 = TS_State.touchX[0];
            uint16_t y1 = TS_State.touchY[0];
            
           if(x1 >= 65 && x1 <= 170 && y1 >= 110 && y1 <= 122) { //  start the game
               wait_ms(10);
               startGame=true;
               break;
            }
            else if(x1 >= 95 && x1 <= 135 && y1 >= 140 && y1 <= 152){ // turn off
                wait_ms(10);
                exitGame=true;
                break;
            }
            wait_ms(10);
        }
    }
    wait_ms(50);
    if(startGame){ // choosing difficulty
        chooseALevelPage();
        while(1){  
        BSP_TS_GetState(&TS_State);
        if(TS_State.touchDetected) {
            uint16_t x1 = TS_State.touchX[0];
            uint16_t y1 = TS_State.touchY[0];
            
           if(x1 >= 95 && x1 <= 135 && y1 >= 100 && y1 <= 112) {
               wait_ms(10);
               level = EASY;
               break;
           }
           else if(x1 >= 85 && x1 <= 150 && y1 >= 125 && y1 <= 137) {
               wait_ms(10);
               level = MEDIUM;
               break;
           }
           else if(x1 >= 95 && x1 <= 135 && y1 >= 150 && y1 <= 162) {
               wait_ms(10);
               level = HARD;
               break;
           }
        }
         wait_ms(10);  
        }
    }

    if(exitGame){ // turn off
        wait_ms(20);
        BSP_LCD_Clear(LCD_COLOR_BLACK);
        break;
    }
    wait_ms(50);
    
    uint32_t paddleColor = LCD_COLOR_BLACK; // default colors
    uint32_t ballColor = LCD_COLOR_BLACK;

    if(startGame){  // choosing colors for paddle and ball
        chooseAColorPage();
        while(1){  
        BSP_TS_GetState(&TS_State);
        if(TS_State.touchDetected) {
            uint16_t x1 = TS_State.touchX[0];
            uint16_t y1 = TS_State.touchY[0];
            
           if(x1 >= 20 && x1 <= 45 && y1 >= 130 && y1 <= 138) { // red paddle
               wait_ms(10);
               paddleColor = LCD_COLOR_RED;
               paddleOptions(); // refresh all options again
               selectedPaddle(20,130);
           }
           else if(x1 >= 80 && x1 <= 105 && y1 >= 130 && y1 <= 138) { // green paddle
               wait_ms(10);
               paddleColor = LCD_COLOR_DARKGREEN;
               paddleOptions(); // refresh all options again
               selectedPaddle(80,130);
           }
           else if(x1 >= 20 && x1 <= 45 && y1 >= 170 && y1 <= 178) { // blue paddle
               wait_ms(10);
               paddleColor = LCD_COLOR_BLUE;
               paddleOptions(); // refresh all options again
               selectedPaddle(20,170);
           }
           else if(x1 >= 80 && x1 <= 105 && y1 >= 170 && y1 <= 178) { // yellow paddle
               wait_ms(10);
               paddleColor = LCD_COLOR_YELLOW;
               paddleOptions(); // refresh all options again
               selectedPaddle(80,170);
           }
           else if(x1 >= 142 && x1 <= 158 && y1 >= 122 && y1 <= 138) { // red ball
               wait_ms(10);
               ballColor = LCD_COLOR_RED;
               ballOptions(); // refresh all options again
               selectedBall(150,130);
           }
           else if(x1 >= 202 && x1 <= 218 && y1 >= 122 && y1 <= 138) { // green ball
               wait_ms(10);
               ballColor = LCD_COLOR_DARKGREEN;
               ballOptions(); // refresh all options again
               selectedBall(210,130);
           }
           else if(x1 >= 142 && x1 <= 158 && y1 >= 162 && y1 <= 178) { // blue ball
               wait_ms(10);
               ballColor = LCD_COLOR_BLUE;
               ballOptions(); // refresh all options again
               selectedBall(150,170);
           }
           else if(x1 >= 202 && x1 <= 218 && y1 >= 162 && y1 <= 178) { // yellow ball
               wait_ms(10);
               ballColor = LCD_COLOR_YELLOW;
               ballOptions(); // refresh all options again
               selectedBall(210,170);
           }
           else if(x1 >= 95 && x1 <= 145 && y1 >= 200 && y1 <= 225) { // button OK
               wait_ms(10);
               if(paddleColor != LCD_COLOR_BLACK && ballColor != LCD_COLOR_BLACK)break;
           }

        }       
         wait_ms(10);  
        }

    }

    // gameplay
    if(startGame){
        gameScreen();
        player = Paddle(3,90,paddleColor);
        Paddle computer = Paddle(232,90,LCD_COLOR_LIGHTGRAY);
        Ball ball = Ball(ballColor,level);
        Result result = Result();
        srand(static_cast <unsigned int> (time(0)));
        int broj = rand() % 8 + 1;
        Direction dir = Direction(Dir(broj));
        int compTimer=2;
        btn2.rise(&moveDown); //button p9
        btn1.rise(&moveUp); //button p8
        wait_ms(1500);
        while(1){
            ball.moveBall(dir); // ball movement
            player.movePaddle(0); // refreshing the paddles incase the ball is near them
            computer.movePaddle(0);

            // the ball hits the floor
            if(ball.getY() >= 240){
                if(dir.getX() > 0 ) dir = Direction(UpRight);
                else dir = Direction(UpLeft);
            }
            // the ball hits the roof
            if(ball.getY() <= 0){
                if(dir.getX() > 0 ) dir = Direction(DownRight);
                else dir = Direction(DownLeft);
            }
            //the player hits the ball
            if(ball.getX() <= player.getX()+5 && ball.getY() >= player.getY() && ball.getY() <= player.getY()+45){
                if(ball.getY() == player.getY()+22 || ball.getY() == player.getY()+23){
                    dir=Direction(Right);
                }
                else if(ball.getY() < player.getY()+22){
                    dir=Direction(UpRight);
                }
                else dir=Direction(DownRight);

            }
            //the computer hits the ball
            if(ball.getX() >= computer.getX()-5 && ball.getY() >= computer.getY() && ball.getY() <= computer.getY()+45){
                if(ball.getY() == computer.getY()+22 || ball.getY() == computer.getY()+23){
                    dir=Direction(Left);
                }
                else if(ball.getY() < computer.getY()+22){
                    dir=Direction(UpLeft);
                }
                else dir=Direction(DownLeft);
            }

            if(ball.getX() <= 0){ // computer scores point
                result.computerScores();
                player.resetPaddle(3,90);
                computer.resetPaddle(232,90);
                ball.resetBall();
                broj = rand() % 8 + 1;
                dir = Direction(Dir(broj));
                wait_ms(400);
            }
            if(ball.getX() >= 240){ // player scores point
                result.playerScores();
                player.resetPaddle(3,90);
                computer.resetPaddle(232,90);
                ball.resetBall();
                broj = rand() % 8 + 1;
                dir = Direction(Dir(broj));
                wait_ms(400);
            }

            if(computer.getY()<=0) compTimer*=(-1);    // boundaries for computer
            if(computer.getY()>=195) compTimer*=(-1);

            
            if(level==HARD){
                computer.movePaddle(dir.getY()*level);
            }
            else computer.movePaddle(compTimer*level);

            setNet();
            result.setUp();

            if(result.getPlayerScore() == 11){
                winPage();
                break;
            }
            if(result.getComputerScore() == 11){
                gameOverPage();
                break;
            }
            wait_ms(50);
        }
    }
    wait_ms(50);
    while(1){  // final screen (either win or loss)
        BSP_TS_GetState(&TS_State);
        if(TS_State.touchDetected) {
            uint16_t x1 = TS_State.touchX[0];
            uint16_t y1 = TS_State.touchY[0];        
            if(x1 >= 20 && x1 <= 130 && y1 >= 130 && y1 <= 142) {
                wait_ms(10);
                exitGame = false;
                break;
            }
            else if(x1 >= 175 && x1 <= 200 && y1 >= 130 && y1 <= 142) {
                wait_ms(10);
                exitGame=true;  
                break;
            }
        }
        wait_ms(10);  
    }
    if(exitGame){
        wait_ms(20);
        BSP_LCD_Clear(LCD_COLOR_BLACK);
        break;
    }
    wait_ms(10);
    }

}