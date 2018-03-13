// Jambon nano sync USBMIDI
// Super simple LSDJ TO USBMIDI-SYNC (just what i need)
// Use LSDJ Nano Sync
// Send midi Start/Stop and Ticks
// Plus a CC (31) on every beats

#include "MIDIUSB.h"

int beat_pin=8;
int led_pin=9;
int analog_pin=18;//LSDJ NANO INPUT

void setup(){   
  pinMode(beat_pin, OUTPUT);//blink
  pinMode(led_pin, OUTPUT);//blink  
  
  digitalWrite(beat_pin,1);
  digitalWrite(led_pin,1);
  delay(100);
  digitalWrite(beat_pin,0);
  digitalWrite(led_pin,0);
  delay(100);
}

int val = 0; 
bool play=0;
bool last=0;//last state//
bool tick=0;//clock up
byte ticks=0;
int steps=0;
int beats=0;
int idle=0;

void loop() {

  val = analogRead(analog_pin);     // read the input pin
  
  if (val>10) {
    //digitalWrite(led_pin,1);
    tick=1;
  } else {
    //digitalWrite(led_pin,0);
    tick=0;
  }
  
  if(tick!=last){
    ticks++;
    
    if(play==0){
      play=1;
      start();
    }
    
    tickMidi();
    
    idle=0;
    
  }else{
    //no change
    idle++;
  }

  if(ticks>=24){// Regular 4/4
  //if(ticks>=18){//CrazyQ ;]
    beatMidi();
    ticks=0;
    steps++;
  }

  if(play&&ticks<2){
    digitalWrite(beat_pin,1);
  }else{
    digitalWrite(beat_pin,0);
  }

  last=tick;  

  if(idle>2000){
    stop();
  }
}


void start(){
  play=1;//playing
  //Serial.println("START");
  startMidi();
}

void beatMidi(){
  byte a=0xC0;
  byte b=31;//CCNUM (arbitrary)
  byte c=127;//MAX CCVAL 
  // https://arduino.stackexchange.com/questions/41684/midiusb-why-is-the-command-put-twice
  midiEventPacket_t event = {0|a>>4, a, b, c};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();//send NOW
}

void tickMidi(){
  byte a=248;
  midiEventPacket_t event = {0|a>>4, a, 0, 0};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();//send NOW
}

void startMidi(){
  byte a=250;
  midiEventPacket_t event = {0|a>>4, a, 0, 0};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();//send NOW
}


void stopMidi(){
  byte a=252;
  midiEventPacket_t event = {0|a>>4, a, 0, 0};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();//send NOW
}

void stop(){
  if(play==0)return;//already stoped
  ticks=0;
  steps=0;
  beats=0;
  idle=0;
  play=0;
  
  stopMidi();
}
