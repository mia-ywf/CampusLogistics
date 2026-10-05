#ifndef GUI_H
#define GUI_H
#include <graphics.h>

void DrawButton(int x, int y, int w, int h, const char* text);
bool IsButtonClicked(int mouseX, int mouseY, int x1, int y1, int x2, int y2);

#include "common.h"
#include "graph.h"
#include "order.h"
#include "vehicle.h"
#include "path.h"

void InitGUI(void);
void CloseGUI(void);
void DrawMap(Graph* g);
void DrawPath(Graph* g, Path* path);
void DrawOrders(Order orders[], int count);
void DrawVehicles(Vehicle vehicles[], int count);
void DrawStats(Order orders[], int count,
    Vehicle vehicles[], int countV);
void DrawLogs(void);
void AddLog(const char* msg);

#endif
#pragma once
