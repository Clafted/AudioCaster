#pragma once

#include "LineObject.hpp"
#include "Buffer.hpp"

#define MAX_LINE_COUNT MAX_VERTEX_COUNT/2

struct LineBuffer : public Buffer
{
	LineObject lines[MAX_LINE_COUNT] = {};
	int lineCount = 0;

	int loadData(const Vec2 vertices[MAX_VERTEX_COUNT], int vertexCount)
	{
		LineObject line;
		for (int i = 0; i < MAX_LINE_COUNT; i++) lines[i] = LineObject{};
		for (int i = 0; i < vertexCount - 1; i += 2)
		{
			line = LineObject(vertices[i], vertices[i+1]);
			lines[lineCount] = line;
			lineCount++;
		}
		return 0;
	}
};