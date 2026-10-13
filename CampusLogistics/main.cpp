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

//新增
#define BTN_DISPATCH_X  580
#define BTN_DISPATCH_Y  620
#define BTN_DISPATCH_W  160
#define BTN_DISPATCH_H  40     
 
//新增
/* 一辆配送中的车辆对应一个独立任务 */
typedef struct
{
    int active;                         // 任务是否正在执行
    int orderIndex;                     // 订单在数组中的下标
    int vehicleIndex;                   // 车辆在数组中的下标

    int routeNodes[2 * MAX_LOCATIONS];  // 完整路线的节点下标
    int routeLength;                    // 路线节点数量
    int currentStep;                    // 当前路线节点
    int frameCounter;                   // 帧计数器（后续不再用于控制移动）
    double progress;                    // 当前路段的移动进度：0.0～1.0
    double drawX;   // 车辆当前屏幕 X 坐标
    double drawY;   // 车辆当前屏幕 Y 坐标
} ActiveDelivery;

//新增
/* 给空闲车辆分配等待订单 */
int assignWaitingOrders(
    Graph* graph,
    Order orders[],
    int orderCount,
    Vehicle vehicles[],
    int vehicleCount,
    ActiveDelivery deliveries[])
{
    PriorityQueue queue;
    Path pathToPickup;
    Path pathToDelivery;

    int orderIndex;
    int vehicleIndex;
    int assigned = 0;
    int attempt;

    initPriorityQueue(&queue);

    /* 最多给每辆车分配一个新任务 */
    for (attempt = 0; attempt < vehicleCount; attempt++)
    {
        /* 选择订单、车辆，并计算两段最短路径 */
        if (!prepareNextDelivery(
            graph,
            orders,
            orderCount,
            vehicles,
            vehicleCount,
            &queue,
            &orderIndex,
            &vehicleIndex,
            &pathToPickup,
            &pathToDelivery))
        {
            break;
        }

        /* 防止覆盖正在执行的任务 */
        if (deliveries[vehicleIndex].active)
        {
            break;
        }

        /* 合并两段路径 */
        if (!buildDeliveryRoute(
            &pathToPickup,
            &pathToDelivery,
            deliveries[vehicleIndex].routeNodes,
            &deliveries[vehicleIndex].routeLength))
        {
            AddLog("Build delivery route failed");
            break;
        }

        /* 正式切换订单和车辆状态 */
        if (!startDelivery(
            &orders[orderIndex],
            &vehicles[vehicleIndex],
            &pathToPickup))
        {
            AddLog("Start delivery failed");
            break;
        }

        /* 初始化这辆车的配送进度 */
        deliveries[vehicleIndex].active = 1;
        deliveries[vehicleIndex].orderIndex = orderIndex;
        deliveries[vehicleIndex].vehicleIndex = vehicleIndex;
        deliveries[vehicleIndex].currentStep = 0;
        deliveries[vehicleIndex].frameCounter = 0;
        deliveries[vehicleIndex].progress = 0.0;
        deliveries[vehicleIndex].drawX = 0.0;
        deliveries[vehicleIndex].drawY = 0.0;

        assigned++;

        AddLog("Order assigned to vehicle");
    }

    return assigned;
}


 /* 平滑更新所有正在配送的车辆 */
    void updateDeliveries(
        Graph* graph,
        Order orders[],
        Vehicle vehicles[],
        ActiveDelivery deliveries[])
{
    int i;
    int currentNode;
    int nextNode;
    int vehicleIndex;
    double startX, startY;
    double endX, endY;
    double speed = 0.025;
    ActiveDelivery* task;

    for (i = 0; i < MAX_VEHICLES; i++)
    {
        task = &deliveries[i];

        if (!task->active)
            continue;

        if (task->routeLength <= 0 ||
            task->currentStep < 0 ||
            task->currentStep >= task->routeLength)
            continue;

        vehicleIndex = task->vehicleIndex;

        if (vehicleIndex < 0 || vehicleIndex >= MAX_VEHICLES)
            continue;

        /* 已经到达路线终点 */
        if (task->currentStep >= task->routeLength - 1)
        {
            if (finishDelivery(
                &orders[task->orderIndex],
                &vehicles[vehicleIndex]))
            {
                AddLog("Delivery completed");
            }

            task->active = 0;
            task->orderIndex = -1;
            task->vehicleIndex = -1;
            task->routeLength = 0;
            task->currentStep = 0;
            task->frameCounter = 0;
            task->progress = 0.0;
            continue;
        }

        currentNode = task->routeNodes[task->currentStep];
        nextNode = task->routeNodes[task->currentStep + 1];

        if (currentNode < 0 || currentNode >= graph->locationCount ||
            nextNode < 0 || nextNode >= graph->locationCount)
            continue;

        startX = graph->locations[currentNode].x;
        startY = graph->locations[currentNode].y;
        endX = graph->locations[nextNode].x;
        endY = graph->locations[nextNode].y;

        /* 每帧前进一小段 */
        task->progress += speed;

        if (task->progress >= 1.0)
        {
            task->progress = 0.0;
            task->currentStep++;

            /* 更新车辆逻辑位置 */
            vehicles[vehicleIndex].position =
                graph->locations[nextNode].id;

            /* 到达最后一个节点后，在下一轮完成订单 */
        }

        /* 根据当前路段进度计算屏幕坐标 */
        task->drawX = startX +
            (endX - startX) * task->progress;

        task->drawY = startY +
            (endY - startY) * task->progress;
    }
}



int main(void)
{
    Graph g;
    Order orders[MAX_ORDERS];
    Vehicle vehicles[MAX_VEHICLES];
    int orderCount = 0;
    int vehicleCount = 0;

    //新增
    /* 每辆车最多对应一个正在执行的配送任务 */
    ActiveDelivery deliveries[MAX_VEHICLES];
    double vehicleX[MAX_VEHICLES];
    double vehicleY[MAX_VEHICLES];
    int moving[MAX_VEHICLES];
    int dispatchStarted = 0;
    int i;

    int selectedOrderId = -1;
    int selectedVehicleId = -1;
	//新增
    int nextVehicleId = 1;
    int nextOrderId = 7;

    Path path;
    int hasPath = 0;

    InitGraph(&g);
    initOrders(orders, &orderCount);
    initVehicles(vehicles, &vehicleCount);


    //新增
    /* 初始化所有配送任务 */
    for (i = 0; i < MAX_VEHICLES; i++)
    {
        deliveries[i].active = 0;
        deliveries[i].orderIndex = -1;
        deliveries[i].vehicleIndex = -1;
        deliveries[i].routeLength = 0;
        deliveries[i].currentStep = 0;
        deliveries[i].frameCounter = 0;
        deliveries[i].progress = 0.0;
        deliveries[i].drawX = 0.0;
        deliveries[i].drawY = 0.0;
    }

    if (!LoadLocations(&g, "locations.txt")) return 1;
    if (!LoadRoads(&g, "roads.txt")) return 1;
    LoadOrders("orders.txt", orders, &orderCount);
    LoadVehicles("vehicles.txt", vehicles, &vehicleCount);
	// 确定下一个车辆 ID
    for (int i = 0; i < vehicleCount; i++)
	{
		if (vehicles[i].id >= nextVehicleId)
			nextVehicleId = vehicles[i].id + 1;
	}

    hasPath = 0;

    InitGUI();

    ExMessage msg;
	
    while (1)
    {
        //新增
        /* 更新配送进度 */
        if (dispatchStarted)
        {
            updateDeliveries(&g, orders, vehicles, deliveries);
        }

        /* 配送过程中持续为新空闲车辆分配等待订单 */
        if (dispatchStarted)
        {
            assignWaitingOrders(
                &g,
                orders,
                orderCount,
                vehicles,
                vehicleCount,
                deliveries);
        }

        cleardevice();

        DrawMap(&g);

        if (hasPath)
        {
            DrawPath(&g, &path);
        }

        /* 在地图和路线绘制完成后，再绘制车辆 */
        /* 准备每辆车的实时绘制坐标 */
        for (i = 0; i < vehicleCount; i++)
        {
            moving[i] = 0;
            vehicleX[i] = 0.0;
            vehicleY[i] = 0.0;

            if (i < MAX_VEHICLES && deliveries[i].active)
            {
                moving[i] = 1;
                vehicleX[i] = deliveries[i].drawX;
                vehicleY[i] = deliveries[i].drawY;
            }
        }

        DrawVehicleMarkers(
            &g, vehicles, vehicleCount,
            vehicleX, vehicleY, moving);

        DrawButton(BTN_ADD_ORDER_X, BTN_ADD_ORDER_Y,
            BTN_ADD_ORDER_W, BTN_ADD_ORDER_H,
            "Add Order");

        DrawButton(BTN_ADD_VEHICLE_X, BTN_ADD_VEHICLE_Y,
            BTN_ADD_VEHICLE_W, BTN_ADD_VEHICLE_H,
            "Add Vehicle");

        DrawButton(BTN_DELETE_VEHICLE_X, BTN_DELETE_VEHICLE_Y,
            BTN_DELETE_VEHICLE_W, BTN_DELETE_VEHICLE_H,
            "Delete Vehicle");
        //新增
        DrawButton(BTN_DISPATCH_X, BTN_DISPATCH_Y,
            BTN_DISPATCH_W, BTN_DISPATCH_H,
            "Start Dispatch");

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

                        v.id = nextVehicleId;
                        v.position = 0;
                        v.status = VEHICLE_IDLE;
                        v.currentOrder = -1;

                        if (addVehicle(vehicles, &vehicleCount, v))
                        {
                            AddLog("Add Vehicle clicked");
                            nextVehicleId++;
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
                //新增
                // ⑤ 开始调度
                else if (IsButtonClicked(msg.x, msg.y,
                    BTN_DISPATCH_X, BTN_DISPATCH_Y,
                    BTN_DISPATCH_W, BTN_DISPATCH_H))
                {

                    if (!dispatchStarted)
                    {
                        dispatchStarted = 1;
                        AddLog("Dispatch started");

                        if (assignWaitingOrders(
                            &g,
                            orders,
                            orderCount,
                            vehicles,
                            vehicleCount,
                            deliveries) == 0)
                        {
                            AddLog("No order assigned");
                        }
                    }

                    else
                    {
                        AddLog("Dispatch already started");
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