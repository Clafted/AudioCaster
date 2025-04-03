#ifndef LINE_H
#define LINE_H

#include "Vec2.h"
#include <raylib.h>
#include <cmath>
#include <string>
#include <utility>

#define MAX_SOUND_COUNT 20
#define SOUND_SPEED 2043.0f

enum OBJECT_TYPE { WALL = 0, SOUND = 1 };

struct SoundInfo
{
	std::string file = "";
	float volume = 0.0f;
};

struct LineObject
{
	Vec2 start, end;
	Vec2 normal;
	
	std::pair <SoundInfo, float> activeSounds[MAX_SOUND_COUNT] = {};
	std::string soundFile;
	int numActive = 0;
	int radius = 40;

	OBJECT_TYPE type = WALL;

	LineObject() : type(WALL) {}
	LineObject(Vec2 start, Vec2 end, OBJECT_TYPE type = WALL) : start(start), end(end), type(type)
	{
		normal= Vec2{ end.y - start.y, start.x - end.x };
		normal.normalize();
	}
	LineObject(Vec2 start, int radius, const char* soundFile) : start(start), end(0), radius(radius), type(SOUND)
	{
		this->soundFile = soundFile;
	}

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
		int currentActive = numActive;
		for (int i = 0; i < currentActive; i++)
		{
			if (firstEmpty == -1										// No empty spot
				&& (activeSounds[i].first.volume <= 0.0f					// No sound
					|| (float)GetTime() - activeSounds[i].second > 5.0f))	// Inactive
			{
				firstEmpty = i;
				numActive--;
			}
			else if (firstEmpty != -1										// Has empty spot
					&& activeSounds[i].first.volume > 0.0f						// Has sound
					&& (float)GetTime() - activeSounds[i].second <= 5.0f)	// Active									// There's an empty spot
			{
				// Move active sound to empty spot
				activeSounds[firstEmpty] = activeSounds[i];
				firstEmpty = i;
			}
		}
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
		return sqrt( (end.x-start.x)*(end.x-start.x) + (end.y-start.y)*(end.y-start.y) );
	}
	 
	static float getLength(const Vec2& p1, const Vec2& p2)
	{
		return sqrt( (p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y) );
	}

	float containsPoint(const Vec2 &p)
	{
		return abs(getLength(start, p) + getLength(end, p) - getLength()) < 0.5f;
	}


	void move(Vec2 displacement)
	{
		start += displacement;
		end += displacement;
	}
};

#endif 