#ifndef RAYLISTENER_H
#define RAYLISTENER_H

#include "LineBuffer.hpp"
#include "Vec2.h"
#include <map>
#include <utility>

#define MAX_DETECTED 12

struct RayListener
{
	Vec2 pos;
	std::pair<Vec2, SoundInfo> detPairs[MAX_DETECTED];
	std::map<std::string, Sound> loadedSounds;

	int numDetected;
	int radius;

	~RayListener();

	float dot(Vec2 a, Vec2 b);

	/* Find the parameter of collision between a parametric line
	* and an implicit line, given tail s, direction d, implicit
	* line line, and max parameter t.
	* Returns a positive parameter for existing collision, 
	* -1.0f otherwise. */
	float getLineHit(Vec2 s, Vec2 d, LineObject& line, float t);

	/* Find the parameter of collision between a parametric line
	* and a sound point, given tail s, direction d, sound-point
	* sound, and max parameter t.
	* Returns a positive parameter for existing collision,
	* -1.0f otherwise. */
	float getSoundHit(Vec2 s, Vec2 d, LineObject& sound, float t);

	/* Return whether or not a ray is approximately in an emitted
	* sound, given the active-time of sound soundTime, and length
	* of ray rayLength.
	* Returns true if ray is within sound, false otherwise. */
	bool rayInSound(float soundTime, float rayLength);
	void clearDetected();

	/* Finds the closest object collision between a parametric
	* line and a buffer of LineObjects, given tail s,
	* direction d, max parameter t, LineBuffer objects,
	* and closestDist, where closestDist is updated to parameter
	* for closest collision.
	* Returns a pointer to the closest object if any, nullptr otherwise. */
	LineObject* findClosestObject(Vec2 s, Vec2 d, float t, LineBuffer& objects, float* closestDist);
	SoundInfo castRay(Vec2 s, Vec2 d, float t, float cT, int numBounces, LineBuffer& objects);
	void playDetectedSounds();
	void listen(LineBuffer& objects);
};

#endif