#include "../include/RayListener.hpp"


RayListener::~RayListener()
{
	clearDetected();
	for (auto i = loadedSounds.begin(); i != loadedSounds.end(); i++)
		UnloadSound(i->second);
	loadedSounds.clear();
}

float RayListener::dot(const Vec2& a,const Vec2& b)
{
	return a.x * b.x + a.y * b.y;
}

float RayListener::getLineHit(Vec2& s, Vec2& d, LineObject& line, float t)
{
	Vec2 n{ line.end.y - line.start.y, line.start.x - line.end.x };
	n.normalize();
	float dP = dot(d, n);
	if (dP == 0.0f) return -1.0f;
	float collision = dot(line.end - s, n) / dP;
	if (0.0f < collision && collision <= t) return collision;
	return -1.0f;
}

float RayListener::getSoundHit(Vec2 &s, Vec2 &d, LineObject& sound, float &t)
{
	float dist = LineObject::getLength(s, sound.start);
	if (t < dist) return -1.0f;
	Vec2 p1 = s + d * dist;
	if (LineObject::getLength(p1, sound.start) <= 40.0f)
		return dist;
	return -1.0f;
}

bool RayListener::rayInSound(float soundTime, float rayLength)
{
	return abs((GetTime() - soundTime) - (rayLength / SOUND_SPEED)) < 0.005f;
}

void RayListener::clearDetected()
{
	for (int i = 0; i < numDetected; i++) detPairs[i] = {};
	numDetected = 0;
}

LineObject* RayListener::findClosestObject(Vec2 &s, Vec2 &d, float t, LineBuffer& objects, float& closestDist)
{
	float nCO;
	LineObject* o = nullptr, * closest = nullptr;
	closestDist = -1.0f;
	// Find closest colliding wall
	for (int i = 0; i < objects.lineCount; i++)
	{
		o = &objects.lines[i];
		nCO = (o->type == WALL) ? getLineHit(s, d, *o, t) : getSoundHit(s, d, *o, t);
		// Skip if no collision OR collision is farther than previous
		if (nCO <= 0.1f || (closestDist <= nCO && closestDist != -1.0f)) continue;
		Vec2 p = s + d*nCO;
		if (o->type == WALL && !o->containsPoint(p)) continue;
		closestDist = nCO;
		closest = o;
	}

	return closest;
}

SoundInfo RayListener::castRay(Vec2& s, Vec2& d, float t, float cT, int numBounces, LineBuffer & objects)
{
	if (numBounces > maxBounces) return SoundInfo{};

	float cD;
	LineObject* closest = findClosestObject(s, d, t, objects, cD);

	if (cD == -1.0f || t < cD)
	{
		DrawLine((int)s.x, (int)s.y, (int)s.x + d.x * t, (int)s.y + d.y * t, Color{ 255, 50, 50, 40 });
		return SoundInfo{};
	}

	LineObject cL{ s, s + d*cD };
	// Distance falloff
	float p = std::min(1.0f, 0.01f * (cT + t) * (cT + t) / ((cT + cD) * (cT + cD)) );

	if (closest->type == SOUND)
	{
		DrawLine((int)s.x, (int)s.y, (int)cL.end.x, (int)cL.end.y, Color{ 200, 200, 100, (unsigned char)(p * 255) });
		std::pair<SoundInfo, float> aS;
		// Find active sound
		for (int i = 0; i < closest->numActive; i++)
		{
			aS = closest->activeSounds[i];
			if (aS.first.volume <= 0.0f || !rayInSound(aS.second, cT + cD)) continue;
			aS.first.volume = p;
			DrawLine((int)s.x, (int)s.y, (int)cL.end.x, (int)cL.end.y, ORANGE);
	
			return aS.first;
		}
		return SoundInfo{};
	}

	SoundInfo reflS, refrS;
	Vec2 n{ closest->end.y - closest->start.y, closest->start.x - closest->end.x };
	n.normalize();
	if (dot(d, n) > 0) n = { -n.x, -n.y };	// Flip normal if facing away

	// Reflection
	Vec2 r = d - n * 2 * dot(d, n);	// Calculate reflection
	r.normalize();
	reflS = castRay(cL.end, r, t - cD, cT + cD, numBounces + 1, objects);
	reflS.volume *= 0.9f;

	// Refraction
	refrS = castRay(cL.end, d, t - cD, cT + cD, numBounces + 1, objects);
	refrS.volume *= 0.1f;

	DrawLine((int)s.x, (int)s.y, (int)cL.end.x, (int)cL.end.y, Color{ 40, 140, 250, (unsigned char)(p * 255) });

	if (refrS.file != "") return SoundInfo{ refrS.file, reflS.volume + refrS.volume };
	if (reflS.file != "") return SoundInfo{ reflS.file, reflS.volume + refrS.volume };
	return SoundInfo{};
}

void RayListener::playDetectedSounds()
{
	std::string cFile = "";
	Sound s{};
	std::pair<Vec2, SoundInfo> p;

	for (int i = 0; i < numDetected; i++)
	{
		p = detPairs[i];
		if (p.second.file.c_str()[0] == '\0' || p.second.volume <= 0.0f) continue;

		if (cFile != p.second.file)
		{
			if (loadedSounds.find(p.second.file) == loadedSounds.end())
			{
				cFile = p.second.file;
				s = LoadSound(cFile.c_str());
				loadedSounds[p.second.file] = s;

			}
			s = loadedSounds.find(p.second.file)->second;
		}
		s = LoadSoundAlias(s);

		SetSoundVolume(s, p.second.volume);
		SetSoundPan(s, -p.first.x);
		PlaySound(s);
	}
}

void RayListener::listen(LineBuffer& objects)
{
	SoundInfo s;
	Vec2 d;
	clearDetected();
	for (float i = 0.0f; i <= 6.28f; i += 1.0f/sampleSize)
	{
		d.x = cos(i);
		d.y = sin(i);
		s = castRay(pos, d, 3000.0f, 0.0f, 0, objects);
		if (s.volume <= 0.0f || numDetected >= MAX_DETECTED) continue;
		detPairs[numDetected] = { d, s };
		numDetected++;
	}
}