#pragma once

#include "raylib.h"
#include "Buffer.hpp"
#include <fstream>
#include <string>

#define MAX_VERTEX_COUNT 100
#define MAX_FILE_LENGTH 10000

struct VertexBuffer : public Buffer
{
	Vec2 vertices[MAX_VERTEX_COUNT];
	int endOfVertices = 0;

	void parseStringToFloatArray(char str[MAX_FILE_LENGTH], float dest[MAX_VERTEX_COUNT], int& floatCount)
	{
		char numS[1024]; // # chars per float
		int numIt = 0;
		for (int i = 0; i < 10000; i++)
		{
			if (str[i] == ',' || str[i] == '\0') // End of float
			{
				// Reset numS
				numS[numIt] = '\0';
				numIt = 0;
				// Convert numS to float
				dest[floatCount] = std::stof(numS);
				floatCount++;
				if (str[i] == '\0') break;
			}
			else
			{
				numS[numIt] = str[i];
				numIt++;
			}
		}
	}

	void parseCoordsToVector2(float coords[MAX_VERTEX_COUNT], int numCoords)
	{
		Vec2 point;
		int vI = 0;
		for (int i = 0; i < numCoords && vI < MAX_VERTEX_COUNT; i++)
		{
			if (i % 2 == 0) point.x = coords[i];
			else
			{
				point.y = coords[i];
				vertices[vI] = point;
				vI++;
				endOfVertices++;
			}
		}
	}

	int loadData(const char* file)
	{
		std::ifstream iS;
		char val[10000];
		float coords[MAX_VERTEX_COUNT * 2];
		int floatCount = 0;

		iS.open(file);
		for (int i = 0; i < 10000; i++) val[i] = 0;
		if (iS.is_open()) iS.getline(val, 10000);

		// Parse val into float array
		parseStringToFloatArray(val, coords, floatCount);
		parseCoordsToVector2(coords, floatCount);

		return 0;
	}
};