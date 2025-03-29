#include "raylib.h"
#include "../include/RayListener.hpp"
#include "../include/VertexBuffer.hpp"
#include "../include/LineBuffer.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <vector>

LineBuffer lB;
VertexBuffer vB;

float pTime = 0.0f;
std::string t;

void displayStats()
{
	t = "FPS: " + std::to_string((int)(1.0f / ((float)GetTime() - pTime)));
	DrawText(t.c_str(), 10, 10, 30, WHITE);
	pTime = (float)GetTime();
}

int main()
{
	if (vB.loadData("resources/vertices.csv") == -1)
	{
		return -1;
	}

	lB.loadData(vB.vertices, vB.endOfVertices);
	LineObject sound{ Vec2{400, 400}, Vec2{0,0}, SOUND };
	sound.addSound("resources/bottle.mp3");
	lB.lines[lB.lineCount] = sound;
	lB.lineCount++;

	InitWindow(0, 0, "RayCaster");
	int sWidth = (int)(GetScreenWidth() * 0.7f);
	int sHeight = (int)(GetScreenHeight() * 0.7f);
	SetWindowSize(sWidth, sHeight);
	SetWindowPosition((int)(GetScreenHeight() * 0.15f), (int)(GetScreenWidth() * 0.15f));

	InitAudioDevice();


	RayListener player;
	player.radius = 5;
	Color bg{ 20, 20, 30 };

	while (!WindowShouldClose())
	{
		player.pos.x = (float)GetMouseX();
		player.pos.y = (float)GetMouseY();

		BeginDrawing();

		ClearBackground(bg);
		
		// Draw lines
		for (int i = 0; i < lB.lineCount; i++)
		{
			if (lB.lines[i].type == WALL)
			{
				DrawLine(lB.lines[i].start.x, lB.lines[i].start.y,
					lB.lines[i].end.x, lB.lines[i].end.y,
					WHITE);
			}
			else
			{
				DrawCircle(lB.lines[i].start.x, lB.lines[i].start.y, 40, YELLOW);
				for (int j = 0; j < MAX_SOUND_COUNT; j++)
				{
					if (lB.lines[i].activeSounds[j].first.volume <= 0.0f) continue;
					DrawCircleLines(lB.lines[i].start.x, lB.lines[i].start.y, SOUND_SPEED * ((float)GetTime() - lB.lines[i].activeSounds[j].second), YELLOW);
				}
			}
			
		}

		sound.deleteOldSounds();
		if (IsKeyPressed(KEY_SPACE)) 
			lB.lines[lB.lineCount-1].playSound();
		if (IsKeyPressed(KEY_UP))
			player.sampleSize++;
		if (IsKeyPressed(KEY_DOWN) && player.sampleSize > 1)
			player.sampleSize--;
		if (IsKeyPressed(KEY_LEFT) && player.maxBounces > 1)
			player.maxBounces--;
		if (IsKeyPressed(KEY_RIGHT) && player.maxBounces < 8)
			player.maxBounces++;

		player.listen(lB);
		player.playDetectedSounds();
		
		DrawCircle((int)player.pos.x, (int)player.pos.y, player.radius, GREEN);
		displayStats();
		EndDrawing();
	}
	CloseAudioDevice();
	CloseWindow();
	return 0;
}