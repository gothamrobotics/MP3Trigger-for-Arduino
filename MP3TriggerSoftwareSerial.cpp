/*
MP3Trigger.cpp
@author David Wicks, additions by Carl Jensen
@url	sansumbrella.com
*/

#include "MP3TriggerSoftwareSerial.h"

MP3TriggerSS::MP3TriggerSS()
{
	mDoLoop = false;
	mPlaying = false;
}

MP3TriggerSS::~MP3TriggerSS()
{
	s->flush();
	s = NULL;
	quickModeCallback = NULL;
}

//void MP3TriggerSS::setup()
//{
	//setup(&Serial);
//}

//void MP3TriggerSS::setup(HardwareSerial* serial)
void MP3TriggerSS::setup(SoftwareSerial* serial)
{
	s = serial;
	s->begin(38400);
}

// 
// Looping functions
// 
void MP3TriggerSS::setLooping(bool doLoop, byte track)
{
	mDoLoop = doLoop;
	mLoopTrack = track;
	
	if(!mPlaying && mDoLoop)
	{
		loop();
	}
}

void MP3TriggerSS::setLoopingTrack(byte track)
{
	mLoopTrack = track;
}

void MP3TriggerSS::update()
{
	if( s->available() )
	{
		int data = s->read();
		
		if(char(data) == 'X')
		{
			if(mDoLoop)
			{	
				loop();
			} else
			{
				mPlaying = false;
			}
		} else if(char(data) == 'E')
		{
			mPlaying = false;
		} else if(char(data) == 'M')
		{
			byte reads = 0;
			byte retries = 0;
			byte i;
			while(reads < 3 && retries < 10) {
				if(s->available()) {
					data = (byte) s->read();
					if(data) {
	  					for(i = 0; i < 8; i++) {
							if( (data >> i) & B00000001 ) {
								quickModeCallback(i + 8*(2-reads) + 1);
							}
	  					}
					}
  					reads++;
				}
				else {
					retries++;
				}
			}
		}
	}
}

void MP3TriggerSS::loop()
{
	trigger(mLoopTrack);
}

void MP3TriggerSS::stop()
{
	mDoLoop = false;
	
	if(mPlaying)
	{
		play();
	}
}

// 
// Single-byte built-in functions
// 

void MP3TriggerSS::play()
{
	s->write('O');
	mPlaying = !mPlaying;
}

void MP3TriggerSS::forward()
{
	s->write('F');
}

void MP3TriggerSS::reverse()
{
	s->write('R');
}

//
// Built-in two-byte functions
// 

void MP3TriggerSS::trigger(byte track)
{
	s->write('t');
	s->write(track);
	mPlaying = true;
}

void MP3TriggerSS::play(byte track)
{
	s->write('p');
	s->write(track);
	mPlaying = true;
}

void MP3TriggerSS::setVolume(byte level)
{
	// level = level ^ B11111111;	//flip it around, so the higher number > higher volume
	s->write('v');
	s->write(level);
}

 //1 for on, 0 for off, callback function
void MP3TriggerSS::quietMode(boolean onoff, void (*callback)(int))
{
	s->write('Q');
	s->write( '0' + onoff );
	quickModeCallback = callback;
}

// 
// Response functions
// 

void MP3TriggerSS::statusRequest()
{
	s->println("MP3TriggerSS::statusRequest is not yet implemented");
	s->flush();
	s->write('S');
	s->write('1');
	delay(5);
	s->read();
	//will need to work on this one to make it useful
	// if (Serial.available() > 0)
	// {
	// 	// read the incoming byte:
	// 	int incomingByte = Serial.read();
	// 
	// 	// say what you got:
	// 	Serial.print("I received: ");
	// 	Serial.println(incomingByte, DEC);
	// }
}