/*
Arduino Pong based on TVout library
**/

#include <TVout.h>
#include <fontALL.h>
#define PADDLE_W 1
#define PADDLE_H 10
#define CENTERX 60
#define CENTERY 48
#define OFFSETX 10    //Needed for the big PONG in intro()
#define OFFSETY 30    //^^
#define BALLSPEED 20 //The time between ball redrawings


TVout TV;

int playerpointsone=0;
int playerpointstwo=0;

int shiftx=1;
int shifty=1;
int curx=CENTERX;
int cury=CENTERY;

int playerposone=48;
int playerpostwo=48;


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

void gameover() {
  TV.clear_screen();
  TV.select_font(font8x8);
  TV.println("GAME OVER");
  if(playerpointsone==10)
    TV.println("Player 1 wins!");
  else
    TV.println("Player 2 wins!");
}

void startdir() {  //Random start direction
  int decision=random()%3;
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

void drawField() {
  int i=3;
  //Draw the center line
  for(i;i<=96;i+=6) {
    TV.draw_line(60,(i-3),60,i,WHITE);
  }
  TV.draw_line(0,8,120,8,WHITE);
  //Draw the ball
  TV.set_pixel(curx, cury, WHITE);

}

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
  TV.select_font(font6x8);
  TV.println(30,0,playerpointsone,DEC);
  TV.println(90,0,playerpointstwo,DEC);
  startdir();
  TV.delay(1000);

}

//When the player position changes

void posChange(int posy, int player) {
  if(player==1){
    TV.draw_rect(0,0,PADDLE_W+1,96,BLACK,BLACK); //Redraw all the line
    TV.draw_rect(0,posy,PADDLE_W,PADDLE_H,WHITE,WHITE); 
  }
  if(player==2) {
    TV.draw_rect(118-PADDLE_W,0,PADDLE_W+1,96,BLACK,BLACK);
    TV.draw_rect(118-PADDLE_W,posy,PADDLE_W,PADDLE_H,WHITE,WHITE);
  }
  delayMicroseconds(1500); //Wait for the screen to redraw
}

void moveBall(int nextx, int nexty) {
  TV.set_pixel(curx, cury, INVERT);
  TV.set_pixel(nextx, nexty, INVERT);
}

void setup() {
  TV.begin(PAL,120,96);
  TV.select_font(font6x8);
  TV.print(40,0,"Arduino");
  intro();
  TV.delay(2000);
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
    playerpostwo=map(analogRead(1),0,1023,8,(96-PADDLE_H));
    //Draw the two platforms
    posChange(playerposone, 1);
    posChange(playerpostwo, 2);
    

    //Draw the ball
    moveBall(curx+shiftx, cury+shifty);
    curx+=shiftx;
    cury+=shifty;
   //Bouncing ball
    if(cury<=9 || cury>=96) //Points Offset=8
      shifty*=-1;
    
     //Bounce ball when it hits a player platform
   if(curx<=2 || curx>=116) {    //This way the ball doesn't identify itself as a platform
     for(i=0;i<PADDLE_H;i++) {
         if(cury==playerposone+i && curx<10){          //
           plathit=true;                               //
           shifty=((cury-playerposone)-(PADDLE_H/2))/2; //This whole section makes the ball bounce quite realistically
         }                                             //
         if(cury==playerpostwo+i && curx>100) {        //
           plathit=true;                               //
           shifty=((cury-playerpostwo)-(PADDLE_H/2))/2; //
       }    
     }
     if(plathit) {                                //If the platform is on the way
        if(shiftx==1 && curx>=116) //Bounce! --> error-checking so that it won't bounce on the other player's platfoem
          shiftx=-1;
        else if(shiftx==-1 && curx<=2)
          shiftx=1;  
     }
   }
   //Point count
   if(curx==0){     //If the ball is at 0
     addPoint(2);  //Player2 scored a point
   }
   else if(curx==120){   //If the ball is at 120
     addPoint(1);       //Player1 scored a point
   }
}

