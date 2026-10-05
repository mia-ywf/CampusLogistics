#include <stdio.h>
#include <graphics.h>
#include <conio.h>

#include "common.h"
#include "graph.h"
#include "order.h"
#include "vehicle.h"
#include "scheduler.h"
#include "path.h"
#include "simulation.h"
#include "gui.h"

#define BTN_ADD_ORDER_X  40
#define BTN_ADD_ORDER_Y  620
#define BTN_ADD_ORDER_W  160
#define BTN_ADD_ORDER_H  40

int main(void)
{
    Graph g;
    Order orders[MAX_ORDERS];
    Vehicle vehicles[MAX_VEHICLES];
    int orderCount = 0;
    int vehicleCount = 0;
    int nextOrderId = 7;

    Path path;
    int hasPath = 0;

    InitGraph(&g);
    initOrders(orders, &orderCount);
    initVehicles(vehicles, &vehicleCount);

    if (!LoadLocations(&g, "locations.txt")) return 1;
    if (!LoadRoads(&g, "roads.txt")) return 1;
    LoadOrders("orders.txt", orders, &orderCount);
    LoadVehicles("vehicles.txt", vehicles, &vehicleCount);

    hasPath = dijkstra(&g, 0, 5, &path);
    if (hasPath) AddLog("Path found");
    else         AddLog("No path");

    InitGUI();

    ExMessage msg;

    while (1)
    {
        cleardevice();

        DrawMap(&g);
        if (hasPath) DrawPath(&g, &path);

        DrawButton(BTN_ADD_ORDER_X, BTN_ADD_ORDER_Y,
            BTN_ADD_ORDER_W, BTN_ADD_ORDER_H,
            "Add Order");
        settextcolor(WHITE); //閬垮厤鎸夐挳鐨勯粦鑹查儴鍒嗗奖鍝嶅叾浠栧湴鏂?  
        setbkmode(TRANSPARENT);

        DrawOrders(orders, orderCount);
        DrawVehicles(vehicles, vehicleCount);
        DrawStats(orders, orderCount, vehicles, vehicleCount);
        DrawLogs();

     

        FlushBatchDraw();
        Sleep(30);

        if (peekmessage(&msg, EX_MOUSE))
        {
            if (msg.message == WM_LBUTTONDOWN)
            {
                if (IsButtonClicked(msg.x, msg.y,
                    BTN_ADD_ORDER_X, BTN_ADD_ORDER_Y,
                    BTN_ADD_ORDER_W, BTN_ADD_ORDER_H))
                {
                    if (orderCount < MAX_ORDERS)
                    {
                        Order o;
                        o.id = nextOrderId++;
                        o.pickup = 0;
                        o.delivery = 5;
                        o.priority = 2;
                        o.status = ORDER_WAITING;
                        o.createTime = 0;

                        orders[orderCount++] = o;
                        AddLog("Add Order clicked");
                    }
                }
            }
        }

        if (_kbhit())
        {
            if (_getch() == 27) break;
        }
    }

    CloseGUI();
    FreeGraph(&g);
    return 0;
}