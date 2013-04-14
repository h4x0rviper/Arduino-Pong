/*
Arduino Pong based on TVout library. Now with AI support !!!
*/

#include <TVout.h>
#include <fontALL.h>
#define PADDLE_W 1
#define PADDLE_H 10
#define CENTERX 60
#define CENTERY 48
#define OFFSETX 10                                            //Needed for the big PONG in intro()
#define OFFSETY 20                                            //^^
#define BALLSPEED 20                                          //The time between ball redrawings
#define MAX_FOOLNESS 5
#define POINTS_LINE 8
#define SINGLE_PLAYER_OFFSET 0
#define MULTIPLAYER_OFFSET 60
#define MENUY 90


TVout TV;

int playerpointsone=0;
int playerpointstwo=0;

int shiftx=1;
int shifty=1;
int curx=CENTERX;
int cury=CENTERY;

int playerposone=48;
int playerpostwo=48;

int currentstep=0;
int ai_shift=1;
int actualfoolness=0;
int selected_speed=1;

int selection;    //1 means single player, 0 means multiplayer
/* 
Start menu (select between single player and multiplayer mode)
*/
void startMenu() {
  unsigned int startMillis=millis();
  TV.print(SINGLE_PLAYER_OFFSET+5,MENUY,"Single player");
  TV.print(MULTIPLAYER_OFFSET+5,MENUY,"Multiplayer");
  
  while((millis()-startMillis)<=3000) {           //Some time to select the playing mode
  selection = map(analogRead(0), 0, 1023, 0, 1);  //Player 1 pot will select the playing mode
  
  if(selection){
    TV.draw_rect(SINGLE_PLAYER_OFFSET,MENUY,3,1,WHITE);
    TV.draw_rect(MULTIPLAYER_OFFSET,MENUY,3,1,BLACK);
  }else{
    TV.draw_rect(SINGLE_PLAYER_OFFSET,MENUY,3,1,BLACK);
    TV.draw_rect(MULTIPLAYER_OFFSET,MENUY,3,1,WHITE);
    
  }
  delay(10);  //No overloads, please!
  }
}


/* 
A function to select the AI foolness level
*/
void fool_select() {
  unsigned int startMillis=millis();
  TV.clear_screen();
  TV.println(" Select the CPU skill level:\n");
  TV.print(5,10,"Easy Peasy");;
  TV.print(5,20,"Quite Simple");
  TV.print(5,30,"Challenging");
  TV.print(5,50,"God Mode (A.K.a Impossible)");       
  while((millis()-startMillis)<=3000) {           //Some time to select the playing skill lvl
  selection = map(analogRead(0), 0, 1023, 1, 4);  //Player 1 pot will select the playing difficulty
  TV.draw_rect(0,0,3,96,BLACK,BLACK);
  switch (selection) {
    case 1:
    TV.draw_rect(0,10,3,1,WHITE,WHITE);
    actualfoolness=2;
    break;
    case 2:
    TV.draw_rect(0,20,3,1,WHITE,WHITE);
    actualfoolness=1;
    break;
    case 3:
    TV.draw_rect(0,30,3,1,WHITE,WHITE);
    actualfoolness=0;
    break;
    case 4:
    TV.draw_rect(0,50,3,1,WHITE,WHITE);
    break;
  }
  TV.delay(10);
  }
  if(selection==4)  selected_speed=2;    //God mode setup
}

/*
The core of the AI in single-player mode
*/
void ai() {
  if(actualfoolness==MAX_FOOLNESS){
                                                              //Move up and down one step a time
   if(playerpostwo==POINTS_LINE || playerpostwo==96-PADDLE_W) //If end of line is reached
    ai_shift*=-1;
   playerpostwo+=ai_shift;                                    //Move platform
  }else{
    if(currentstep==actualfoolness) {                               //Foolness control
      if(playerpostwo>=cury)
        ai_shift=-1*(selected_speed);
      else if(playerpostwo<=cury && playerpostwo<=96-PADDLE_W)
        ai_shift=selected_speed;
      playerpostwo+=ai_shift;                                 //Track ball
      currentstep=0;
    }else{
      currentstep++;                                          //Wait...
    }
  }
}

/*
The intro big title
*/    
void intro() {
  //P
  TV.draw_rect(0+OFFSETX,10+OFFSETY,5,20,WHITE,WHITE);
  TV.draw_rect(5+OFFSETX,5+OFFSETY,10,5,WHITE,WHITE);
  TV.draw_rect(5+OFFSETX,15+OFFSETY,10,5,WHITE,WHITE);
  TV.draw_rect(15+OFFSETX,10+OFFSETY,5,5,WHITE,WHITE);
  //O
  TV.draw_rect(25+OFFSETX,10+OFFSETY,5,15,WHITE,WHITE);
  TV.draw_rect(30+OFFSETX,5+OFFSETY,10,5,WHITE,WHITE);
  TV.draw_rect(30+OFFSETX,25+OFFSETY,10,5,WHITE,WHITE);
  TV.draw_rect(40+OFFSETX,10+OFFSETY,5,15,WHITE,WHITE);
  //N
  TV.draw_rect(50+OFFSETX,5+OFFSETY,5,25,WHITE,WHITE);
  TV.draw_rect(55+OFFSETX,10+OFFSETY,5,5,WHITE,WHITE);
  TV.draw_rect(60+OFFSETX,15+OFFSETY,5,5,WHITE,WHITE);
  TV.draw_rect(65+OFFSETX,20+OFFSETY,5,5,WHITE,WHITE);
  TV.draw_rect(70+OFFSETX,5+OFFSETY,5,25,WHITE,WHITE);
  //G
  TV.draw_rect(80+OFFSETX,10+OFFSETY,5,15,WHITE,WHITE);
  TV.draw_rect(85+OFFSETX,5+OFFSETY,10,5,WHITE,WHITE);
  TV.draw_rect(85+OFFSETX,25+OFFSETY,10,5,WHITE,WHITE);
  TV.draw_rect(95+OFFSETX,20+OFFSETY,5,5,WHITE,WHITE);
  TV.draw_rect(90+OFFSETX,15+OFFSETY,10,5,WHITE,WHITE);
}

/*
The game over routine
*/
void gameover() {
  TV.clear_screen();
  TV.select_font(font8x8);
  TV.println("GAME OVER");
  if(playerpointsone==10)
    TV.println("Player 1 wins!");
  else
    TV.println("Player 2 wins!");
}

/*
Random starting direction
*/

void startdir() {
  int decision=random()%4;
  switch (decision) {
    case 0:
      shiftx=-1;
      shifty=1;
      break;
    case 1:
      shiftx=-1;
      shifty=-1;
      break;
    case 2:
      shiftx=1;
      shifty=-1;
    default:
      shiftx=1;
      shifty=1;
      break;
  }
  
}

/*
Draw the field (e.g. the center line, the points line, the ball at its initial position...)
*/

void drawField() {
  int i=3;
                                                              //Draw the center line
  for(i;i<=96;i+=6) {
    TV.draw_line(60,(i-3),60,i,WHITE);
  }
  TV.draw_line(0,POINTS_LINE,120,POINTS_LINE,WHITE);
                                                              //Draw the ball
  TV.set_pixel(curx, cury, WHITE);

}

/*
Add points to a player
*/

void addPoint(int player) {
  if(player==1)
    playerpointsone++;
  else if(player==2)
    playerpointstwo++;
                                                              //If somebody reached the win threshold, congratulate!
   if(playerpointsone==10 || playerpointstwo==10) {
     gameover();
     TV.delay(10000);
     playerpointsone=0;
     playerpointstwo=0;
   }
                                                              //Redo the whole field
  TV.clear_screen();
  curx=CENTERX;
  cury=CENTERY;
  drawField();
                                                              //Rewrite updated points
  TV.select_font(font4x6);
  TV.println(30,0,playerpointsone,DEC);
  TV.println(90,0,playerpointstwo,DEC);
  startdir();
  TV.delay(1000);

}

/*
Paddle position updating routine
*/

void posChange(int posy, int player) {
  if(player==1){
    TV.draw_rect(0,0,PADDLE_W+1,96,BLACK,BLACK);              //Redraw all the line
    TV.draw_rect(0,posy,PADDLE_W,PADDLE_H,WHITE,WHITE); 
  }
  if(player==2) {
    TV.draw_rect(118-PADDLE_W,0,PADDLE_W+1,96,BLACK,BLACK);
    TV.draw_rect(118-PADDLE_W,posy,PADDLE_W,PADDLE_H,WHITE,WHITE);
  }
  delayMicroseconds(1500);                                    //Wait for the screen to redraw
}

/*
Ball position updating routine
*/
void moveBall(int nextx, int nexty) {
  TV.set_pixel(curx, cury, INVERT);
  TV.set_pixel(nextx, nexty, INVERT);
}

void setup() {
  TV.begin(PAL,120,96);
  TV.select_font(font4x6);
  TV.print(40,0,"Arduino");
  intro();
  startMenu();
  if(selection)
        fool_select();
  TV.clear_screen();
  TV.print(30,0,playerpointsone,WHITE);
  TV.print(90,0,playerpointstwo,WHITE);
  drawField();
  randomSeed(analogRead(2));
  startdir();
}
void loop() {
    int i=0;
    boolean plathit=false;
    TV.delay(BALLSPEED);
                                                              //Where are the players? Here we are!
    playerposone=map(analogRead(0),0,1023,8,(96-PADDLE_H));
    if(selection) {                     //Single player selected
      ai();
    }else{
      playerpostwo=map(analogRead(1),0,1023,8,(96-PADDLE_H));
    }
                                                              //Draw the two platforms
    posChange(playerposone, 1);
    posChange(playerpostwo, 2);
                                                              //Draw the ball
    moveBall(curx+shiftx, cury+shifty);
    curx+=shiftx;
    cury+=shifty;
                                                              //Bouncing ball
    if(cury<=POINTS_LINE+1 || cury>=96)                       //Points Offset=8
      shifty*=-1;
                                                              //Bounce ball when it hits a player platform
   if(curx<=2 || curx>=116) {                                 //This way the ball doesn't identify itself as a platform
     for(i=0;i<PADDLE_H;i++) {
         if(cury==playerposone+i && curx<10){                 //
           plathit=true;                                      //
           shifty=((cury-playerposone)-(PADDLE_H/2))/2;       //This whole section makes the ball bounce quite realistically
         }                                                    //
         if(cury==playerpostwo+i && curx>100) {               //
           plathit=true;                                      //
           shifty=((cury-playerpostwo)-(PADDLE_H/2))/2;       //
       }    
     }
     if(plathit) {                                            //If the platform is on the way
        if(shiftx==1 && curx>=116)                            //Bounce! --> error-checking so that it won't bounce on the other player's platfoem
          shiftx=-1;
        else if(shiftx==-1 && curx<=2)
          shiftx=1;  
     }
   }
                                                              //Point count
   if(curx==0){                                               //If the ball is at 0
     addPoint(2);                                             //Player2 scored a point
   }
   else if(curx==120){                                        //If the ball is at 120
     addPoint(1);                                             //Player1 scored a point
   }
}

