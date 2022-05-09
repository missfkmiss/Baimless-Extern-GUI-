#pragma once
#include <Windows.h>
#include "globals.hh"


#define EnemyPen 0x000000FF
HBRUSH EnemyBrush = CreateSolidBrush(0x000000FF);
HBRUSH HealthBrush = CreateSolidBrush(0x00c717);
HBRUSH HealthBackgroundBrush = CreateSolidBrush(0x00000000);




void DrawFilledRect(int x, int y, int w, int h)
{
	RECT rect = { x,y,x + w,y + h };
	FillRect(globals.hdc, &rect, EnemyBrush);
}

void DrawHealth(int x, int y, int w, int h)
{
	RECT rect = { x,y,x + w,y + h };
	FillRect(globals.hdc, &rect, HealthBrush);
}

void DrawBackGroundHealth(int x, int y, int w, int h)
{
	RECT rect = { x,y,x + w,y + h };
	FillRect(globals.hdc, &rect, HealthBackgroundBrush);
}