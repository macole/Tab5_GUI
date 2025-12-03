#include "M5UnitSynth.h"
#include <M5Unified.h>
#include <M5GFX.h>
#include "NotoSansBold15.h"
#include "tinyFont.h"
#include "smallFont.h"
#include "midleFont.h"
#include "bigFont.h"
#include "font18.h"

m5::touch_detail_t touchDetail;
static int32_t w;
static int32_t h;

long duration=0;
long bmp=90; //(duration*4)/1000*60;     
M5Canvas canvas(&M5.Display);
M5Canvas blink(&M5.Display);
M5Canvas canvas2(&M5.Display);
M5Canvas playBut(&M5.Display);
M5Canvas displ(&M5.Display);
M5Canvas bmpSpr(&M5.Display);
M5Canvas metSpr(&M5.Display);

const int drumNotes[] = {35, 38, 37, 59, 61};
String instruments[5]= {
  "Bass Drum",  // 35
  "Snare",      // 38
  "Side Stick",          // 37
  "Ride Cymbal",       // 59
  "Low Bongo"            // 61
};

M5UnitSynth synth;

bool playing=0;  // playing or muted
bool notePlayed[5][16]={0};  
bool deb=0; //debounce touch input

unsigned long timeBuff=0; 
unsigned long blinkTime[5]={0}; 
bool blinkOn[5]={0};
int blinkDuration=30;

int n=0;
int posx[16]={0};
int posy[5]={0};

int xoffset=270;
int yoffset=150;

// play buton sprite
int playBW=290;
int playBH=80;
int playBX=450;
int playBY=610;

// beat display sprite
int displW=100;
int displH=70;
int displX=640-displW/2;
int displY=18;

//bmp sprite
int bmpW=470;
int bmpH=80;
int bmpX=765;
int bmpY=610;


unsigned short b1=0x18E3;
unsigned short b2=0x11C8;
//unsigned short b2=0x60C1;
unsigned short b3=0x11C8;
unsigned short cols[16]={b1,b1,b1,b1,b2,b2,b2,b2,b1,b1,b1,b1,b2,b2,b2,b2};
unsigned short background=0x10E4;
unsigned short backgroundInst=0x32EE;
unsigned short playCol=0x322A;
unsigned short metCol=0x1225;
bool metronome=1;

void setup() {

    auto cfg = M5.config();
    M5.begin(cfg);
    M5.Display.setRotation(3);
    M5.Display.fillScreen(background);
    M5.Display.fillRoundRect(displX-50,displY,displW+100,displH,8,BLACK); 
    M5.Display.fillRoundRect(40,100,1200,480,8,backgroundInst); 
    M5.Display.drawRoundRect(40,100,1200,480,8,b1);
    M5.Display.drawRoundRect(41,101,1198,478,8,b1); 
  
    M5.Display.fillRoundRect(40,bmpY,180,bmpH,6,b3);  // SAVE BUTTON
    M5.Display.fillRoundRect(240,bmpY,180,bmpH,6,b3); // LOAD BUTTON
    M5.Display.fillRoundRect(1060,18,180,70,6,RED); // CLEAR BUTTON
   
    M5.Display.loadFont(smallFont);
    M5.Display.setTextColor(GREEN,BLACK);
    M5.Display.drawString("MEASURE",displX-30,displY+10);
    M5.Display.drawString("4/4",displX-30,displY+35);
    M5.Display.unloadFont();
   
    M5.Display.setTextColor(WHITE,b3);
    M5.Display.setTextDatum(5);
    M5.Display.loadFont(tinyFont);
    M5.Display.drawString("SAVE",130,bmpY+bmpH/2);
    M5.Display.drawString("LOAD",330,bmpY+bmpH/2);
    M5.Display.setTextColor(WHITE,RED);
    M5.Display.drawString("CLEAR",1150,18+35);
    M5.Display.unloadFont();

    M5.Display.setTextColor(WHITE,background);
    M5.Display.loadFont(bigFont);
    M5.Display.drawString("DRUM MACHINE",200,60);
    M5.Display.unloadFont();
    

    M5.Display.setBrightness(30);

    blink.createSprite(18,70);
    canvas.createSprite(960, 70);
    canvas2.createSprite(960, 8);
    playBut.createSprite(playBW, playBH);
    displ.createSprite(displW, displH);
    bmpSpr.createSprite(bmpW, bmpH);
    metSpr.createSprite(280,70);
 

    Serial.begin(115200);
    Serial.println("Unit Synth Piano");
    synth.begin(&Serial2, UNIT_SYNTH_BAUD, 54, 53);
    synth.setInstrument(0, 9, SynthDrum);  // synth piano 1


    for(int j=0;j<5;j++)
    {
      posy[j]=j*80;
    for(int i=0;i<16;i++)
      {
      posx[i]=i*60;
      }
    }

   
    M5.Display.setTextColor(WHITE,backgroundInst);
    M5.Display.loadFont(tinyFont);
    for(int i=0;i<5;i++)
    M5.Display.drawString(instruments[i],xoffset-126,posy[i]+yoffset+35);
    M5.Display.unloadFont();

    
    M5.Display.loadFont(midleFont);
    M5.Display.setTextColor(0x738E,background);
    M5.Display.drawString("VOLOS",400,45);
    M5.Display.drawString("SEQUENCER",420,75);
    M5.Display.setTextColor(b1,backgroundInst);
    M5.Display.drawString("M5STACK TAB5",1155,560);
    M5.Display.unloadFont();
    M5.Display.loadFont(smallFont);
    M5.Display.setTextColor(WHITE,RED);
    M5.Display.drawString(" VOLOS PROJECTS 2025 ",126,710);
    M5.Display.unloadFont();
     M5.Display.loadFont(tinyFont);
    M5.Display.setTextColor(b1,backgroundInst);
    M5.Display.drawString("INSTRUMENTS",150,130);
    M5.Display.unloadFont();
    
    for(int i=0;i<5;i++)
    {
    draw(i);
    drawBlink(i,0);
    }
  
    draw2();
    drawDisplay();
    drawPlayBut();
    drawBMP();
    drawMet();
    calculateDuration();
}

void drawMet()
{
     metSpr.fillSprite(background);
     metSpr.fillRoundRect(0,0,280,70,6,0x59A1);
     metSpr.loadFont(tinyFont);
     metSpr.setTextDatum(5);
     metSpr.setTextColor(WHITE,0x59A1);
     if(metronome)
     metSpr.drawString("METRONOME OFF",140,35);
     else
     metSpr.drawString("METRONOME ON",140,35);
     metSpr.pushSprite(760,18);
}

void drawBMP()
{
   bmpSpr.fillSprite(background);
   bmpSpr.fillRoundRect(0,0,150,bmpH,6, metCol);
   bmpSpr.fillRoundRect(bmpW-150,0,150,bmpH,6, metCol);
   bmpSpr.fillRoundRect(bmpW/2-65,0,130,bmpH,6,BLACK);
   bmpSpr.setTextDatum(5);
   bmpSpr.setTextColor(GREEN,BLACK);
   bmpSpr.loadFont(midleFont);
   bmpSpr.drawString("BMP",bmpW/2,18);
   bmpSpr.setTextColor(GREEN, metCol);
   bmpSpr.unloadFont();
   bmpSpr.loadFont(tinyFont);
   bmpSpr.drawString("-5",75,bmpH/2);
   bmpSpr.drawString("+5",bmpW-75,bmpH/2);
  
   bmpSpr.setTextColor(GREEN,BLACK);
  
   bmpSpr.drawString(String(bmp),bmpW/2,bmpH/2+16);
   bmpSpr.unloadFont();
   bmpSpr.pushSprite(bmpX,bmpY);
}

void drawPlayBut()
{
  playBut.fillSprite(background);
  playBut.fillRoundRect(0,0,playBW,playBH,6,playCol);
  playBut.setTextColor(WHITE,playCol);
  playBut.setTextDatum(5);
  playBut.loadFont(tinyFont);
  if(playing){
  playBut.drawString("STOP",playBW/2,playBH/2);
  playBut.fillRect(65,24,34,34,RED);
  }else{
  playBut.drawString("PLAY",playBW/2,playBH/2);
  playBut.fillTriangle(65,24,65,58,90,40,GREEN);
  }
   playBut.unloadFont();
   playBut.pushSprite(playBX,playBY);
}

void drawDisplay()
{
  displ.fillSprite(BLACK);
  displ.setTextColor(GREEN,BLACK);
  displ.setTextDatum(5);

  displ.drawString(String(n+1),displW/2,displH/2,7);
 
  displ.pushSprite(displX+40,displY);
}

void drawBlink(int part, bool on)
{
  blink.fillSprite(backgroundInst);
  if(on)
   {
      blink.fillRoundRect(0,0,18,70,4,background);
      blink.fillRoundRect(3,3,12,64,4,GREEN);
      blinkTime[part]=millis();
      blinkOn[part]=1;
   }
   else
   {
      blink.fillRoundRect(0,0,15,70,4,background);
      blink.fillRoundRect(3,3,12,64,4,playCol);
   }
 
  blink.pushSprite(xoffset-30,yoffset+part*80);
}

void draw(int part)
{
  canvas.fillSprite(backgroundInst);
  
  for(int i=0;i<16;i++){
  if(notePlayed[part][i]==1){
  canvas.fillRoundRect(posx[i],0,50,70,5,SILVER);
  canvas.fillRoundRect(posx[i]+10,8,30,5,2,b1);
  }else{
  canvas.fillRoundRect(posx[i],0,50,70,5,cols[i]);
  canvas.fillRoundRect(posx[i]+10,8,30,5,2,SILVER);
  }
  
}
canvas.pushSprite(xoffset,yoffset+part*80);
}

void draw2()
{
  canvas2.fillSprite(backgroundInst);
  for(int i=0;i<16;i++)
  if(i==n)
  canvas2.fillRect(posx[i],0,50,8,RED);
  else
  canvas2.fillRect(posx[i],0,50,8,b1);
  canvas2.pushSprite(xoffset,yoffset-30);
}

void calculateDuration()
{
duration=(60000/bmp)/4;

}

void loop() {

  M5.update();
  touchDetail = M5.Touch.getDetail();

 
  if (touchDetail.isPressed()) {
   
      for(int j=0;j<5;j++)
      for(int i=0;i<16;i++)
      if(touchDetail.x>posx[i]+xoffset && touchDetail.x<posx[i]+60+xoffset && touchDetail.y>posy[j]+yoffset && touchDetail.y<posy[j]+70+yoffset)
      if(deb==0)
       {
       deb=1;
       notePlayed[j][i]=!notePlayed[j][i];
       draw(j);
    
       }

        // if play buton is pressed
        if(touchDetail.x>playBX && touchDetail.x<playBX+playBW && touchDetail.y>playBY && touchDetail.y<playBY+playBH)
        {
          if(deb==0)
          {
            deb=1;
            playing=!playing;
            drawPlayBut();
          }
        }  

        //if bmpPlus
        if(touchDetail.x>bmpX && touchDetail.x<bmpX+150 && touchDetail.y>bmpY && touchDetail.y<bmpY+bmpH)
        {
          if(deb==0)
          {
            deb=1;
            bmp=bmp-5;
            calculateDuration();
            drawBMP();
          }
        } 
       
        if(touchDetail.x>bmpX+bmpW-150 && touchDetail.x<bmpX+bmpW && touchDetail.y>bmpY && touchDetail.y<bmpY+bmpH)
        {
          if(deb==0)
          {
            deb=1;
            bmp=bmp+5;
            calculateDuration();
            drawBMP();
          }
        }

     //if metronome is touched
     if(touchDetail.x>760 && touchDetail.x<760+280 && touchDetail.y>18 && touchDetail.y<88)
        {
          if(deb==0)
          {
            deb=1;
            metronome=!metronome;
            drawMet();
            drawBMP();
          }
        }

       //if clear is touched
     if(touchDetail.x>1060 && touchDetail.x<1060+180 && touchDetail.y>18 && touchDetail.y<88)
        {
          if(deb==0)
          {
            deb=1;
              for(int j=0;j<5;j++)
              for(int i=0;i<16;i++)
              notePlayed[j][i]=0;
             draw(0);
             draw(1);
             draw(2);
             draw(3);
             draw(4);
            
          }
        }

  
  }else deb=0;

//35,38,37,59,61
 if(playing){
 if(millis()>timeBuff+duration)
 {
    timeBuff=millis(); 
    n++; if(n==16) n=0;

    if(notePlayed[0][n]==1){
    synth.setNoteOn(9, drumNotes[0], 100);
    drawBlink(0,1);
    }

    if(notePlayed[1][n]==1)
    {synth.setNoteOn(9, drumNotes[1],100);
    drawBlink(1,1);
    }

    if(notePlayed[2][n]==1){
      synth.setNoteOn(9, drumNotes[2], 100);
      drawBlink(2,1);
    }
    
    if(notePlayed[3][n]==1)
    {synth.setNoteOn(9, drumNotes[3], 100);
    drawBlink(3,1);
    }

    if(notePlayed[4][n]==1){
    synth.setNoteOn(9, drumNotes[4],100);
    drawBlink(4,1);
    }

    if(metronome && n%4==0)
    synth.setNoteOn(9, 62, 90);
    draw2();
    drawDisplay();
 }

  for(int i=0;i<5;i++)
  if(blinkOn[i])
  if(millis()>blinkTime[i]+blinkDuration)
    {
    blinkOn[i]=0;
    drawBlink(i,false);
    }

 }

}

