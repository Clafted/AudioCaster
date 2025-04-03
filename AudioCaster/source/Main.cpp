#include "raylib.h"
#include "../include/RayListener.hpp"
#include "../include/VertexBuffer.hpp"
#include "../include/LineBuffer.hpp"

#define VERTEX_FILE "resources/vertices.csv"

LineBuffer lB;
VertexBuffer vB;
RayListener player;
Color bg{ 5, 10, 15 };

float pTime = 0.0f;

void displayStats()
{
	std::string t;
	t = "FPS: " + std::to_string((int)(1.0f / ((float)GetTime() - pTime)));
	DrawText(t.c_str(), 10, 10, 30, WHITE);
	DrawText(("Sample size: " + std::to_string((int)player.sampleSize)).c_str(), 10, 50, 30, WHITE);
	DrawText(("Num bounces: " + std::to_string((int)player.maxBounces)).c_str(), 10, 90, 30, WHITE);
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
	// Load vertexx data
	if (vB.loadData(VERTEX_FILE) == -1) return -1;
	lB.loadData(vB.vertices, vB.endOfVertices);

	// Create sound-sources
	LineObject& snd = lB.lines[lB.lineCount++];
	LineObject& snd2 = lB.lines[lB.lineCount++];
	LineObject& head = lB.lines[lB.lineCount++];
	snd = LineObject(Vec2{ 400, 400 }, 8, "resources/snap.mp3");
	snd2 = LineObject(Vec2{ 400, 400 }, 40, "resources/amazing sound.mp3");
	head = LineObject(Vec2(player.pos.x - 10.0f, player.pos.y - 30.0f), Vec2(player.pos.x + 10.0f, player.pos.y - 30.0f));

	// Initialize Raylib
	InitWindow(0, 0, "RayCaster");
	int sWidth = (int)(GetScreenWidth() * 0.7f);
	int sHeight = (int)(GetScreenHeight() * 0.7f);
	SetWindowSize(sWidth, sHeight);
	SetWindowPosition((int)(GetScreenHeight() * 0.15f), (int)(GetScreenWidth() * 0.15f));
	InitAudioDevice();
	
	while (!WindowShouldClose())
	{
		head.move((Vec2)GetMousePosition() - player.pos);
		player.pos = GetMousePosition();
		snd.start = player.pos;
		
		BeginDrawing();
		ClearBackground(bg);

		// Process inputs
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

		snd.deleteOldSounds();
		snd2.deleteOldSounds();
		player.listen(lB, GetTime()-pTime);
		player.playDetectedSounds();

		drawObjects();
		displayStats();
		DrawCircle((int)player.pos.x, (int)player.pos.y, 10, GREEN);

		EndDrawing();
		pTime = (float)GetTime();
	}

	CloseAudioDevice();
	CloseWindow();
	return 0;
}