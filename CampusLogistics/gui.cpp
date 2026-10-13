#define _CRT_SECURE_NO_WARNINGS
#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include "gui.h"
#include "stats.h"

#define WIN_W 1200
#define WIN_H 800

static char logs[100][256];
static int  logCount = 0;
// 初始化图形界面
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

// 画地图：地点和道路
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

// 画路径
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

// 画订单列表
void DrawOrders(Order orders[], int count, int selectedOrderId)
{
    char buf[256];
    int i;

    outtextxy(800, 20, "Orders");

    for (i = 0; i < count && i < 15; i++)
    {
        int y = 50 + i * 20;

        // 选中的订单高亮
        if (orders[i].id == selectedOrderId)
        {
            setfillcolor(LIGHTBLUE);
            fillrectangle(795, y - 2, 1150, y + 18);
            settextcolor(BLACK);
        }
        else
        {
            settextcolor(WHITE);
        }

        sprintf(buf, "Order%d status%d pri%d",
            orders[i].id,
            orders[i].status,
            orders[i].priority);

        outtextxy(800, y, buf);
    }

    settextcolor(WHITE);
}

// 画车辆列表
void DrawVehicles(Vehicle vehicles[], int count, int selectedVehicleId)
{
    char buf[256];
    int i;

    outtextxy(800, 400, "Vehicles");

    for (i = 0; i < count; i++)
    {
        int y = 430 + i * 20;

        if (vehicles[i].id == selectedVehicleId)
        {
            // 选中车辆的整行高亮
            setfillcolor(LIGHTBLUE);
            fillrectangle(795, y - 2, 1150, y + 18);

            settextcolor(BLACK);
        }
        else
        {
            settextcolor(WHITE);
        }

        sprintf(buf, "V%d pos%d st%d ord%d",
            vehicles[i].id,
            vehicles[i].position,
            vehicles[i].status,
            vehicles[i].currentOrder);

        outtextxy(800, y, buf);
    }

    settextcolor(WHITE);
}

// 画统计信息
void DrawStats(Order orders[], int count ,
    Vehicle vehicles[], int countV)
{
    Stats stats ;
    char buf[256];

    ComputeStats( orders,count,vehicles,countV,&stats);

    outtextxy(800, 600, "Statistics");

    sprintf(buf, "Orders: tot%d wait%d del%d done%d",
        stats.totalOrders,
        stats.waitingOrders,
        stats.deliveringOrders,
        stats.completedOrders);
    outtextxy(800, 630, buf);

    sprintf(buf, "Vehicles: idle%d busy%d",
        stats.idleVehicles,
        stats.deliveringVehicles);
    outtextxy(800, 650, buf);
}

// 记录日志
void AddLog(const char* msg)
{
    if (logCount < 100)
    {
        strncpy(logs[logCount], msg, 255);
        logs[logCount][255] = '\0';
        logCount++;
    }
}
// 画日志
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
int IsButtonClicked(int mx, int my, int x, int y, int w, int h)
{
    return (mx >= x && mx <= x + w && my >= y && my <= y + h);
}



/* 在地图上绘制车辆，配送中的车辆使用实时坐标 */
void DrawVehicleMarkers(
    Graph* g,
    Vehicle vehicles[],
    int vehicleCount,
    double vehicleX[],
    double vehicleY[],
    int moving[])
{
    int i;
    int locationIndex;
    int x, y;
    char label[32];

    if (g == NULL || vehicles == NULL ||
        vehicleX == NULL || vehicleY == NULL || moving == NULL)
        return;

    for (i = 0; i < vehicleCount; i++)
    {
        if (moving[i])
        {
            x = (int)vehicleX[i];
            y = (int)vehicleY[i];
        }
        else
        {
            locationIndex =
                FindLocationIndexById(g, vehicles[i].position);

            if (locationIndex < 0)
                continue;

            x = g->locations[locationIndex].x;
            y = g->locations[locationIndex].y;
        }

        /* 多辆车位于同一点时，稍微错开显示 */
        x += (i % 3) * 18 - 18;
        y += (i / 3) * 18 - 9;

        if (vehicles[i].status == VEHICLE_DELIVERING)
            setfillcolor(RED);
        else
            setfillcolor(GREEN);

        setlinecolor(WHITE);
        fillcircle(x, y, 9);

        sprintf(label, "V%d", vehicles[i].id);
        settextcolor(WHITE);
        outtextxy(x + 10, y - 8, label);
    }
}

