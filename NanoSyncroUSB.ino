// Jambon nano sync USBMIDI
// dead simple LSDJ TO USBMIDI-SYNC (just what i need)
// Use LSDJ Nano Sync on Arduino Pro micro / Leonardo / etc
// Send midi Start/Stop and Ticks
// Plus a CC (31) on every beats

#include "MIDIUSB.h"

int beat_pin=8;//blink
//int led_pin=9;
int analog_pin=18;//LSDJ NANO INPUT

void setup(){   
  //hi
}

bool play=0;
bool last=0;//last state
bool tick=0;//clock state
byte ticks=0;
byte beats=0;
int idle=0;

void loop() {

  int val = analogRead(analog_pin);
  tick=0;
  if (val>10) {
    tick=1;
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
    idle++;//no change
  }

  if(ticks>=24){// Regular 4/4
  //if(ticks>=36){//BIG SWING
    beatMidi();
    ticks=0;
    beats++;
  }
  
  if(beats>=4){
    barMidi();
    beats=0;
  }

  // blink to the beat
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
  startMidi();
}

void beatMidi(){
  byte a=0xB0;// Control change
  byte b=30;// CCNUM (i like this one)
  byte c=127;// MAXIMUM POWER !!!  
  midiEventPacket_t event = {0|a>>4, a, b, c};// https://arduino.stackexchange.com/questions/41684/midiusb-why-is-the-command-put-twice
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();//send NOW
}

void barMidi(){//every 4 beats...
  byte a=0xB0;// Control change
  byte b=31;// CCNUM (i like this one)
  byte c=127;// MAXIMUM POWER !!!  
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
  beats=0;
  idle=0;
  play=0;  
  stopMidi();
}
