#include "raylib.h"
#include "../include/RayListener.hpp"
#include "../include/VertexBuffer.hpp"
#include "../include/LineBuffer.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <vector>

LineBuffer lB;
VertexBuffer vB;
RayListener player;

float pTime = 0.0f;
std::string t;

void displayStats()
{
	t = "FPS: " + std::to_string((int)(1.0f / ((float)GetTime() - pTime)));
	DrawText(t.c_str(), 10, 10, 30, WHITE);
	DrawText(("Sample size: " + std::to_string((int)player.sampleSize)).c_str(), 10, 50, 30, WHITE);

	pTime = (float)GetTime();
}

void drawObjects()
{
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
			DrawCircle(lB.lines[i].start.x, lB.lines[i].start.y, lB.lines[i].radius, YELLOW);
			for (int j = 0; j < lB.lines[i].numActive; j++)
			{
				if (lB.lines[i].activeSounds[j].first.volume <= 0.0f) continue;
				DrawCircleLines(lB.lines[i].start.x, lB.lines[i].start.y, SOUND_SPEED * ((float)GetTime() - lB.lines[i].activeSounds[j].second), YELLOW);
			}
		}
	}
}

int main()
{
	if (vB.loadData("resources/vertices.csv") == -1)
	{
		return -1;
	}

	lB.loadData(vB.vertices, vB.endOfVertices);
	LineObject obj{ Vec2{400, 400}, Vec2{0,0}, SOUND };
	obj.addSound("resources/snap.mp3");
	lB.lines[lB.lineCount] = obj;
	LineObject& snd = lB.lines[lB.lineCount++];
	obj = LineObject{ Vec2{400, 400}, Vec2{0,0}, SOUND };
	obj.addSound("resources/amazing sound.mp3");
	lB.lines[lB.lineCount] = obj;
	LineObject& snd2 = lB.lines[lB.lineCount++];

	InitWindow(0, 0, "RayCaster");
	int sWidth = (int)(GetScreenWidth() * 0.7f);
	int sHeight = (int)(GetScreenHeight() * 0.7f);
	SetWindowSize(sWidth, sHeight);
	SetWindowPosition((int)(GetScreenHeight() * 0.15f), (int)(GetScreenWidth() * 0.15f));

	InitAudioDevice();
	
	player.radius = 10;
	
	snd.radius = player.radius-2;
	snd2.radius = 40;
	lB.lines[lB.lineCount] = LineObject(Vec2(player.pos.x - 10.0f, player.pos.y - 30.0f), Vec2(player.pos.x + 10.0f, player.pos.y - 30.0f));
	LineObject& head = lB.lines[lB.lineCount++];
	Color bg{ 20, 20, 30 };

	while (!WindowShouldClose())
	{
		player.pos.x = (float)GetMouseX();
		player.pos.y = (float)GetMouseY();
		snd.start = player.pos;
		head.end = Vec2(player.pos.x + 10.0f, player.pos.y + 25.0f);
		head.start = Vec2(player.pos.x -10.0f, player.pos.y + 25.0f);
		
		BeginDrawing();
		ClearBackground(bg);

		snd.deleteOldSounds();
		snd2.deleteOldSounds();
		if (IsKeyPressed(KEY_SPACE)) 
			snd.playSound();
		if (IsKeyPressed(KEY_W))
			snd2.playSound();
		if (IsKeyPressed(KEY_UP))
			player.sampleSize += 20;
		if (IsKeyPressed(KEY_DOWN) && player.sampleSize > 5)
			player.sampleSize -= 20;
		if (IsKeyPressed(KEY_LEFT) && player.maxBounces > 1)
			player.maxBounces--;
		if (IsKeyPressed(KEY_RIGHT) && player.maxBounces < 8)
			player.maxBounces++;

		player.listen(lB);
		player.playDetectedSounds();
		drawObjects();
		
		DrawCircle((int)player.pos.x, (int)player.pos.y, player.radius, GREEN);
		displayStats();
		EndDrawing();
	}
	CloseAudioDevice();
	CloseWindow();
	return 0;
}