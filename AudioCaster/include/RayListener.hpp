#ifndef RAYLISTENER_H
#define RAYLISTENER_H

#include "LineBuffer.hpp"
#include "Vec2.h"
#include <unordered_map>
#include <utility>

#define MAX_DETECTED 200	// miniaudio supports up to 254 channels

struct RayListener
{
	Vec2 pos;
	std::pair<Vec2, SoundInfo> detPairs[MAX_DETECTED];
	std::unordered_map<std::string, Sound> loadedSounds;

	float sampleSize = 25;
	int maxBounces = 4;
	int numDetected = 0;
	int radius = 0;

	~RayListener();

	inline float dot(const Vec2& a, const Vec2& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	/* Find the parameter of collision between a parametric line
	* and an implicit line, given tail s, direction d, implicit
	* line line, and max parameter t.
	* Returns a positive parameter for existing collision, 
	* -1.0f otherwise. */
	float getLineHit(Vec2& s, Vec2& d, LineObject& line, float t);

	/* Find the parameter of collision between a parametric line
	* and a sound point, given tail s, direction d, sound-point
	* sound, and max parameter t.
	* Returns a positive parameter for existing collision,
	* -1.0f otherwise. */
	float getSoundHit(Vec2& s, Vec2& d, LineObject& sound, float &t);

	/* Return whether or not a ray is approximately in an emitted
	* sound, given the active-time of sound soundTime, and length
	* of ray rayLength.
	* Returns true if ray is within sound, false otherwise. */
	inline bool rayInSound(float soundTime, float rayLength)
	{
		return abs((GetTime() - soundTime) - (rayLength / SOUND_SPEED)) < 0.01f;
	}
	void clearDetected();

	/* Finds the closest object collision between a parametric
	* line and a buffer of LineObjects, given tail s,
	* direction d, max parameter t, LineBuffer objects,
	* and closestDist, where closestDist is updated to parameter
	* for closest collision.
	* Returns a pointer to the closest object if any, nullptr otherwise. */
	LineObject* findClosestObject(Vec2 &s, Vec2 &d, float t, LineBuffer& objects, float& closestDist);
	SoundInfo castRay(Vec2& s, Vec2& d, float t, float cT, int numBounces, LineBuffer& objects);
	void playDetectedSounds();
	void listen(LineBuffer& objects);
};

#endif