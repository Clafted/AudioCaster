#include "../include/RayListener.hpp"


RayListener::~RayListener()
{
	clearDetected();
	for (auto i = loadedSounds.begin(); i != loadedSounds.end(); i++)
		UnloadSound(i->second);
	loadedSounds.clear();
}


float RayListener::getLineHit(Vec2& s, Vec2& d, LineObject& line, float t)
{
	float dP = Vec2::dot(d, line.normal);
	if (dP == 0.0f) return -1.0f;
	float collision = Vec2::dot(line.end - s, line.normal) / dP;
	if (0.0f < collision && collision <= t && line.containsPoint(s + d*collision)) return collision;
	return -1.0f;
}

float RayListener::getSoundHit(Vec2 &s, Vec2 &d, LineObject& sound, float t)
{
	float dist = LineObject::getLength(s, sound.start);
	if (t < dist || LineObject::getLength(s + d * dist, sound.start) > sound.radius) return -1.0f;
	return dist;
}

void RayListener::clearDetected()
{
	for (int i = 0; i < MAX_DETECTED; i++) detPairs[i] = {};
	numDetected = 0;
}

LineObject* RayListener::findClosestObject(Vec2 &s, Vec2 &d, float t, LineBuffer& objects, float& closestDist)
{
	float nCO;
	LineObject* o = nullptr, *closest = nullptr;
	closestDist = -1.0f;

	// Find closest colliding wall
	for (int i = 0; i < objects.lineCount; i++)
	{
		o = &objects.lines[i];
		nCO = (o->type == WALL) ? getLineHit(s, d, *o, t) : getSoundHit(s, d, *o, t);

		// Skip if no collision OR collision is farther than previous
		if (nCO <= 0.1f || (closestDist != -1.0f && closestDist <= nCO)) continue;
		closestDist = nCO;
		closest = o;
	}

	return closest;
}

SoundInfo RayListener::castRay(Vec2& s, Vec2& d, float t, float cT, int numBounces, LineBuffer & objects)
{
	float cD;
	LineObject* cO = findClosestObject(s, d, t, objects, cD);

	if (cO == nullptr || cD == -1.0f || t < cD)
	{
		DrawLine((int)s.x, (int)s.y, (int)s.x + d.x * t, (int)s.y + d.y * t, Color{ 255, 50, 50, 40 });
		return SoundInfo{};
	}

	LineObject cL{ s, s + d*cD };

	// Distance falloff (from closest object)
	float p = std::min(1.0f, 0.01f * (cT + t) * (cT + t) / ((cT + cD) * (cT + cD)) );
	float curTime = GetTime();

	if (cO->type == SOUND)
	{
		DrawLine((int)s.x, (int)s.y, (int)cL.end.x, (int)cL.end.y, Color{ 200, 200, 100, (unsigned char)(p * 255) });
		std::pair<SoundInfo, float>* aS;
		// Find active sound
		for (int i = 0; i < cO->numActive; i++)
		{
			aS = &cO->activeSounds[i];
			// Skip if sound is silent OR ray is not within sound proximity
			if (aS->first.volume <= 0.0f || !rayInSound(curTime - aS->second, (cT+cD)/SOUND_SPEED)) continue;
			aS->first.volume = p;
			DrawLine((int)s.x, (int)s.y, (int)cL.end.x, (int)cL.end.y, ORANGE);

			return aS->first;
		}
		return SoundInfo{};
	}
	if (numBounces == maxBounces) return SoundInfo{};

	Vec2 n = cO->normal;
	float dP = Vec2::dot(d, n);
	if (dP > 0)	// Flip normal if facing away
	{
		n *= -1;
		dP *= -1;
	}

	Vec2 r = d - n * (2*dP);	// Calculate reflection (assumes d and n are unit vectors)
	SoundInfo reflS = castRay(cL.end, r, t - cD, cT + cD, numBounces + 1, objects); // Reflection
	SoundInfo refrS = castRay(cL.end, d, t - cD, cT + cD, numBounces + 1, objects); // Refraction (currently in original direction)

	DrawLine((int)s.x, (int)s.y, (int)cL.end.x, (int)cL.end.y, Color{ 40, 140, 250, 
		(unsigned char)(p * cO->soundReflection*255) });

	if (refrS.file.c_str()[0] != '\0') return SoundInfo{refrS.file, reflS.volume * cO->soundReflection + refrS.volume * (1.0f - cO->soundAbsorption - cO->soundReflection) };
	if (reflS.file.c_str()[0] != '\0') return SoundInfo{reflS.file, reflS.volume * cO->soundReflection + refrS.volume * (1.0f - cO->soundAbsorption - cO->soundReflection) };
	return SoundInfo{}; 
}

void RayListener::playDetectedSounds()
{
	std::string cFile = "";
	std::pair<Vec2, SoundInfo> *p;
	Sound s{};

	for (int i = 0; i < numDetected; i++)
	{
		p = &detPairs[i];
		if (p->second.file.c_str()[0] == '\0' || p->second.volume <= 0.0f) continue;

		// Different sound
		if (cFile != p->second.file)
		{
			// Not yet loaded
			if (loadedSounds.find(p->second.file) == loadedSounds.end())
				loadedSounds[p->second.file] = LoadSound(p->second.file.c_str());
			// Retrieve loaded sound
			s = loadedSounds[p->second.file];
		}

		s = LoadSoundAlias(loadedSounds[p->second.file]);
		SetSoundVolume(s, p->second.volume);
		SetSoundPan(s, -0.5f * (p->first.x - 1.0f));
		PlaySound(s);
	}
	numDetected = 0;
}

void RayListener::listen(LineBuffer& objects, float dTime)
{
	SoundInfo s;
	Vec2 d, p;
	for (float i = 0.0f; i <= 6.28f; i += 6.28f/sampleSize)
	{
		d = Vec2{ cos(i), sin(i) };
		p = pos + d * 10.0f;
		s = castRay(p, d, 3000.0f, 0.0f, 0, objects);
		if (s.volume <= 0.0f || numDetected >= MAX_DETECTED) continue;
		detPairs[numDetected++] = { d, s };
	}
}