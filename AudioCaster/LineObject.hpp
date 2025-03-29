#ifndef LINE_H
#define LINE_H

#include "Vec2.h"
#include <raylib.h>
#include <cmath>
#include <string>
#include <utility>

#define MAX_SOUND_COUNT 10
#define SOUND_SPEED 3043.0f

enum OBJECT_TYPE { WALL = 0, SOUND = 1 };

struct SoundInfo
{
	std::string file = "";
	float volume = 0.0f;
};

struct LineObject
{
	Vec2 start, end;
	
	std::pair <SoundInfo, float> activeSounds[MAX_SOUND_COUNT] = {};
	int numActive = 0;
	std::string soundFile;

	OBJECT_TYPE type;

	LineObject() : start(-1.0f), end(-1.0f), type(WALL) {}
	LineObject(Vec2 start, Vec2 end, OBJECT_TYPE type = WALL) : start(start), end(end), type(type) {}

	void addSound(const char* soundFile)
	{
		this->soundFile = soundFile;
	}

	void playSound()
	{
		if (numActive == 10) return;
		activeSounds[numActive] = { SoundInfo{soundFile, 1.0f},  GetTime()};
		numActive++;
	}

	void deleteOldSounds()
	{
		int firstEmpty = -1;
		for (int i = 0; i < MAX_SOUND_COUNT; i++)
		{
			if ((activeSounds[i].first.volume <= 0.0f					// No sound
				|| (float)GetTime() - activeSounds[i].second > 5.0f)	// Deprecated
				&& firstEmpty == -1)
			{
				firstEmpty = i;
			}
			else if (firstEmpty != -1)
			{
				activeSounds[firstEmpty] = activeSounds[i];
				firstEmpty = -1;
			}
			 
		}
		numActive = firstEmpty;
	}

	float getSlope()
	{	
		return (end.y - start.y) / (end.x - start.x); 
	}

	float getNormal()
	{
		return -1 / getSlope();
	}

	float getLength()
	{ 
		return sqrt(pow(end.x - start.x, 2) + pow(end.y - start.y, 2)); 
	}

	static float getLength(Vec2 &p1, Vec2 &p2)
	{
		return sqrt(pow((p1-p2).x, 2) + pow((p1 - p2).y, 2));
	}

	float containsPoint(Vec2 &p)
	{
		return getLength(start, p) + getLength(end, p) == getLength();
	}

	bool operator<(LineObject other)
	{
		return getLength() < other.getLength();
	}

	bool operator>(LineObject other)
	{
		return getLength() > other.getLength();
	}
};

#endif 