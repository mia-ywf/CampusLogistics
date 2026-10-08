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
#include "stats.h"

#define BTN_ADD_ORDER_X  40
#define BTN_ADD_ORDER_Y  620
#define BTN_ADD_ORDER_W  160
#define BTN_ADD_ORDER_H  40

#define BTN_ADD_VEHICLE_X  220
#define BTN_ADD_VEHICLE_Y  620
#define BTN_ADD_VEHICLE_W  160
#define BTN_ADD_VEHICLE_H  40

#define BTN_DELETE_VEHICLE_X  400
#define BTN_DELETE_VEHICLE_Y  620
#define BTN_DELETE_VEHICLE_W  160
#define BTN_DELETE_VEHICLE_H  40

int main(void)
{
    Graph g;
    Order orders[MAX_ORDERS];
    Vehicle vehicles[MAX_VEHICLES];
    int orderCount = 0;
    int vehicleCount = 0;
    int selectedOrderId = -1;
    int selectedVehicleId = -1;
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

    hasPath = 0;

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

        DrawButton(BTN_ADD_VEHICLE_X, BTN_ADD_VEHICLE_Y,
            BTN_ADD_VEHICLE_W, BTN_ADD_VEHICLE_H,
            "Add Vehicle");

        DrawButton(BTN_DELETE_VEHICLE_X, BTN_DELETE_VEHICLE_Y,
            BTN_DELETE_VEHICLE_W, BTN_DELETE_VEHICLE_H,
            "Delete Vehicle");

		settextcolor(WHITE); //设置文本颜色为白色
        setbkmode(TRANSPARENT);

        DrawOrders(orders, orderCount, selectedOrderId);
        DrawVehicles(vehicles, vehicleCount, selectedVehicleId);
        DrawStats(orders, orderCount, vehicles, vehicleCount);
        DrawLogs();

     
        
        FlushBatchDraw();
        Sleep(30);
		// 处理鼠标点击事件
        if (peekmessage(&msg, EX_MOUSE))
        {
            if (msg.message == WM_LBUTTONDOWN)
            {
                // 点击订单列表，选择订单
                if (msg.x >= 800 && msg.x <= 1150 &&
                    msg.y >= 50 &&
                    msg.y < 50 + orderCount * 20 &&
                    orderCount > 0)
                {
                    int index = (msg.y - 50) / 20;

                    if (index >= 0 && index < orderCount && index < 15)
                    {
                        selectedOrderId = orders[index].id;

                        Order* selectedOrder =
                            findOrder(orders, orderCount, selectedOrderId);

                        if (selectedOrder != NULL)
                        {
                            hasPath = dijkstra(
                                &g,
                                selectedOrder->pickup,
                                selectedOrder->delivery,
                                &path
                            );

                            if (hasPath)
                            {
                                AddLog("Order path found");
                            }
                            else
                            {
                                AddLog("Order path unavailable");
                            }
                        }
                    }
                }

                // ① 点击车辆列表，选择车辆
                if (msg.x >= 800 && msg.x <= 1150 &&
                    msg.y >= 430 &&
                    msg.y < 430 + vehicleCount * 20)
                {
                    int index = (msg.y - 430) / 20;

                    if (index >= 0 && index < vehicleCount)
                    {
                        selectedVehicleId = vehicles[index].id;
                        AddLog("Vehicle selected");
                    }
                }

                // ② 添加订单
                else if (IsButtonClicked(msg.x, msg.y,
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

                        if (addOrder(orders, &orderCount, o))
                        {
                            AddLog("Add Order clicked");
                        }
                    }
                }

                // ③ 添加车辆
                else if (IsButtonClicked(msg.x, msg.y,
                    BTN_ADD_VEHICLE_X, BTN_ADD_VEHICLE_Y,
                    BTN_ADD_VEHICLE_W, BTN_ADD_VEHICLE_H))
                {
                    if (vehicleCount < MAX_VEHICLES)
                    {
                        Vehicle v;

                        v.id = vehicleCount + 1;
                        v.position = 0;
                        v.status = VEHICLE_IDLE;
                        v.currentOrder = -1;

                        if (addVehicle(vehicles, &vehicleCount, v))
                        {
                            AddLog("Add Vehicle clicked");
                        }
                        else
                        {
                            AddLog("Add Vehicle failed");
                        }
                    }
                    else
                    {
                        AddLog("Vehicle limit reached");
                    }
                }

                // ④ 删除车辆
                else if (IsButtonClicked(msg.x, msg.y,
                    BTN_DELETE_VEHICLE_X, BTN_DELETE_VEHICLE_Y,
                    BTN_DELETE_VEHICLE_W, BTN_DELETE_VEHICLE_H))
                {
                    if (selectedVehicleId == -1)
                    {
                        AddLog("Please select a vehicle first");
                    }
                    else
                    {
                        if (deleteVehicle(
                            vehicles,
                            &vehicleCount,
                            selectedVehicleId))
                        {
                            AddLog("Delete Vehicle success");
                            selectedVehicleId = -1;
                        }
                        else
                        {
                            AddLog("Cannot delete this vehicle");
                        }
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