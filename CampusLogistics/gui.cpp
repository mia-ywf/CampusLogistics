#define _CRT_SECURE_NO_WARNINGS
#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include "gui.h"

#define WIN_W 1200
#define WIN_H 800

static char logs[100][256];
static int  logCount = 0;

void InitGUI(void)
{
    initgraph(WIN_W, WIN_H);
    BeginBatchDraw();
}

void CloseGUI(void)
{
    EndBatchDraw();
    closegraph();
}

void DrawMap(Graph* g)
{
    int i;

    setlinecolor(LIGHTGRAY);
    for (i = 0; i < g->locationCount; i++)
    {
        EdgeNode* p;
        if (!g->locations[i].valid) continue;
        p = g->adj[i];
        while (p != NULL)
        {
            if (p->to > i && g->locations[p->to].valid)
            {
                line(g->locations[i].x, g->locations[i].y,
                    g->locations[p->to].x, g->locations[p->to].y);
            }
            p = p->next;
        }
    }

    for (i = 0; i < g->locationCount; i++)
    {
        if (!g->locations[i].valid) continue;
        setfillcolor(YELLOW);
        setlinecolor(BLACK);
        fillcircle(g->locations[i].x, g->locations[i].y, 12);
        outtextxy(g->locations[i].x - 20,
            g->locations[i].y - 30,
            g->locations[i].name);
    }
}

void DrawPath(Graph* g, Path* path)
{
    int i;
    if (path == NULL || path->length < 2) return;

    setlinecolor(RED);
    setlinestyle(PS_SOLID, 3);

    for (i = 0; i < path->length - 1; i++)
    {
        line(g->locations[path->nodes[i]].x,
            g->locations[path->nodes[i]].y,
            g->locations[path->nodes[i + 1]].x,
            g->locations[path->nodes[i + 1]].y);
    }

    setlinestyle(PS_SOLID, 1);
    setlinecolor(BLACK);
}

void DrawOrders(Order orders[], int count)
{
    char buf[256];
    int i;

    outtextxy(800, 20, "Orders");
    for (i = 0; i < count && i < 15; i++)
    {
        sprintf(buf, "Order%d status%d pri%d",
            orders[i].id, orders[i].status, orders[i].priority);
        outtextxy(800, 50 + i * 20, buf);
    }
}

void DrawVehicles(Vehicle vehicles[], int count)
{
    char buf[256];
    int i;

    outtextxy(800, 400, "Vehicles");
    for (i = 0; i < count; i++)
    {
        sprintf(buf, "V%d pos%d st%d ord%d",
            vehicles[i].id,
            vehicles[i].position,
            vehicles[i].status,
            vehicles[i].currentOrder);
        outtextxy(800, 430 + i * 20, buf);
    }
}

void DrawStats(Order orders[], int count,
    Vehicle vehicles[], int countV)
{
    int i;
    int waiting = 0, delivering = 0, completed = 0;
    int idle = 0, busy = 0;
    char buf[256];

    for (i = 0; i < count; i++)
    {
        if (orders[i].status == ORDER_WAITING) waiting++;
        else if (orders[i].status == ORDER_DELIVERING) delivering++;
        else if (orders[i].status == ORDER_COMPLETED) completed++;
    }
    for (i = 0; i < countV; i++)
    {
        if (vehicles[i].status == VEHICLE_IDLE) idle++;
        else busy++;
    }

    outtextxy(800, 600, "Statistics");
    sprintf(buf, "Orders: tot%d wait%d del%d done%d",
        count, waiting, delivering, completed);
    outtextxy(800, 630, buf);
    sprintf(buf, "Vehicles: idle%d busy%d", idle, busy);
    outtextxy(800, 650, buf);
}

void AddLog(const char* msg)
{
    if (logCount < 100)
    {
        strncpy(logs[logCount], msg, 255);
        logs[logCount][255] = '\0';
        logCount++;
    }
}

void DrawLogs(void)
{
    int i;
    int start = logCount > 5 ? logCount - 5 : 0;

    outtextxy(50, 700, "Log:");
    for (i = start; i < logCount; i++)
    {
        outtextxy(100, 700 + (i - start) * 18, logs[i]);
    }
}

// 画一个按钮
void DrawButton(int x, int y, int w, int h, const char* text)
{
    setfillcolor(LIGHTGRAY);
    setlinecolor(WHITE);
    fillrectangle(x, y, x + w, y + h);

    setbkmode(TRANSPARENT);
    settextcolor(BLACK);
    outtextxy(x + 10, y + (h - 16) / 2, (char*)text);
}

// 判断鼠标点击是否落在按钮内
bool IsButtonClicked(int mx, int my, int x, int y, int w, int h)
{
    return (mx >= x && mx <= x + w && my >= y && my <= y + h);
}