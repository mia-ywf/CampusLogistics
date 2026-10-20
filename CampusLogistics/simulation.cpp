#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stddef.h>
#include <math.h>     
#include "simulation.h"
#include "scheduler.h"
#include "gui.h"      

// 开始配送：把订单和车辆都切到"配送中"
int startDelivery(Order* order, Vehicle* vehicle, Path* path)
{
    if (order == NULL || vehicle == NULL || path == NULL)
    {
        return 0;
    }

    if (order->status != ORDER_WAITING)
    {
        return 0;
    }

    if (vehicle->status != VEHICLE_IDLE)
    {
        return 0;
    }

    order->status = ORDER_DELIVERING;
    vehicle->status = VEHICLE_DELIVERING;
    vehicle->currentOrder = order->id;

    return 1;
}

// 完成配送：订单完成，车辆恢复空闲
int finishDelivery(Order* order, Vehicle* vehicle)
{
    if (order == NULL || vehicle == NULL)
    {
        return 0;
    }

    if (order->status != ORDER_DELIVERING)
    {
        return 0;
    }

    order->status = ORDER_COMPLETED;
    vehicle->status = VEHICLE_IDLE;
    vehicle->currentOrder = -1;

    return 1;
}


 //合并两段路径
int buildDeliveryRoute(Path* pathToPickup, Path* pathToDelivery,
    int routeNodes[], int* routeLength)
{
    int i;
    int count = 0;

    if (pathToPickup == NULL || pathToDelivery == NULL ||
        routeNodes == NULL || routeLength == NULL)
    {
        return 0;
    }

    if (pathToPickup->length <= 0 || pathToDelivery->length <= 0)
    {
        return 0;
    }

    /* 两段必须在取货点衔接 */
    if (pathToPickup->nodes[pathToPickup->length - 1]
        != pathToDelivery->nodes[0])
    {
        return 0;
    }

    for (i = 0; i < pathToPickup->length; i++)
    {
        if (count >= 2 * MAX_LOCATIONS)
        {
            return 0;
        }
        routeNodes[count++] = pathToPickup->nodes[i];
    }

    for (i = 1; i < pathToDelivery->length; i++)
    {
        if (count >= 2 * MAX_LOCATIONS)
        {
            return 0;
        }
        routeNodes[count++] = pathToDelivery->nodes[i];
    }

    *routeLength = count;
    return 1;
}

// 初始化所有配送任务
void initDeliveries(ActiveDelivery deliveries[], int count)
{
    int i;

    if (deliveries == NULL)
    {
        return;
    }

    if (count > MAX_VEHICLES)
    {
        count = MAX_VEHICLES;
    }

    for (i = 0; i < count; i++)
    {
        deliveries[i].active = 0;
        deliveries[i].orderIndex = -1;
        deliveries[i].vehicleIndex = -1;
        deliveries[i].routeLength = 0;
        deliveries[i].currentStep = 0;
        deliveries[i].progress = 0.0;
        deliveries[i].animX = 0.0;
        deliveries[i].animY = 0.0;
        deliveries[i].lastTick = 0;
        deliveries[i].toPickupLength = 0;
        deliveries[i].routeDistance = 0;
    }
}

// 给空闲车辆分配订单
int assignWaitingOrders(Graph* graph, Order orders[], int orderCount,
    Vehicle vehicles[], int vehicleCount,
    ActiveDelivery deliveries[])
{
    PriorityQueue queue;
    Path pathToPickup;
    Path pathToDelivery;

    int orderIndex;
    int vehicleIndex;
    int assigned = 0;
    int attempt;
    char buf[128];

    if (graph == NULL || orders == NULL ||
        vehicles == NULL || deliveries == NULL)
    {
        return 0;
    }

    initPriorityQueue(&queue);

    for (attempt = 0; attempt < vehicleCount; attempt++)
    {
        /* 选最高优先级订单 + 最近的空闲车 + 算两段最短路 */
        if (!prepareNextDelivery(graph, orders, orderCount,
            vehicles, vehicleCount, &queue,
            &orderIndex, &vehicleIndex,
            &pathToPickup, &pathToDelivery))
        {
            break;      /* 没有可分配的订单或车辆，结束 */
        }

        /*  下标越界保护 */
        if (orderIndex < 0 || orderIndex >= orderCount ||
            vehicleIndex < 0 || vehicleIndex >= vehicleCount)
        {
            break;
        }

        /*  这辆车已经有任务 */
        if (deliveries[vehicleIndex].active)
        {
            break;
        }

        /*  合并两段路径 */
        deliveries[vehicleIndex].routeLength = 0;

        if (!buildDeliveryRoute(&pathToPickup, &pathToDelivery,
            deliveries[vehicleIndex].routeNodes,
            &deliveries[vehicleIndex].routeLength))
        {
            AddLog("[调度] 路线合并失败");
            break;
        }

        /* 切换订单和车辆状态 */
        if (!startDelivery(&orders[orderIndex],
            &vehicles[vehicleIndex], &pathToPickup))
        {
            AddLog("[调度] 开始配送失败");
            deliveries[vehicleIndex].routeLength = 0;
            break;
        }

        /* 记录任务信息 */
        deliveries[vehicleIndex].active = 1;
        deliveries[vehicleIndex].orderIndex = orderIndex;
        deliveries[vehicleIndex].vehicleIndex = vehicleIndex;
        deliveries[vehicleIndex].currentStep = 0;
        deliveries[vehicleIndex].progress = 0.0;
        deliveries[vehicleIndex].lastTick = GetTickCount();

        /* 平滑动画的起点 = 车辆当前所在地点 */
        {
            int startIdx = FindLocationIndexById(
                graph, vehicles[vehicleIndex].position);

            if (startIdx >= 0)
            {
                deliveries[vehicleIndex].animX = graph->locations[startIdx].x;
                deliveries[vehicleIndex].animY = graph->locations[startIdx].y;
            }
        }

        deliveries[vehicleIndex].routeDistance =
            pathToPickup.distance + pathToDelivery.distance;

        assigned++;

        sprintf(buf, "[调度] 订单%d -> 车辆%d  路程%d",
            orders[orderIndex].id,
            vehicles[vehicleIndex].id,
            deliveries[vehicleIndex].routeDistance);
        AddLog(buf);
    }

    return assigned;
}

// 按 currentStep / progress 算出车辆此刻该画在哪
static void updateAnimPos(Graph* graph, ActiveDelivery* task)
{
    int fromIdx;
    int toIdx;
    double t;

    fromIdx = task->routeNodes[task->currentStep];

    if (fromIdx < 0 || fromIdx >= graph->locationCount)
    {
        return;                  
    }

    if (task->currentStep >= task->routeLength - 1)
    {
        task->animX = graph->locations[fromIdx].x;
        task->animY = graph->locations[fromIdx].y;
        return;
    }

    toIdx = task->routeNodes[task->currentStep + 1];

    if (toIdx < 0 || toIdx >= graph->locationCount)
    {
        task->animX = graph->locations[fromIdx].x;
        task->animY = graph->locations[fromIdx].y;
        return;
    }

    t = task->progress;
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;

  
    task->animX = graph->locations[fromIdx].x +
        (graph->locations[toIdx].x - graph->locations[fromIdx].x) * t;
    task->animY = graph->locations[fromIdx].y +
        (graph->locations[toIdx].y - graph->locations[fromIdx].y) * t;
}

//推进配送（平滑）
void updateDeliveries(Graph* graph, Order orders[], Vehicle vehicles[],
    ActiveDelivery deliveries[])
{
    int i;
    char buf[128];
    unsigned int now;

    if (graph == NULL || orders == NULL ||
        vehicles == NULL || deliveries == NULL)
    {
        return;
    }

    now = GetTickCount();

    for (i = 0; i < MAX_VEHICLES; i++)
    {
        ActiveDelivery* task = &deliveries[i];
        int fromIdx;
        int toIdx;
        double dx, dy, segLen, dt;

        if (!task->active)
        {
            continue;
        }

        //边界保护
        if (task->vehicleIndex < 0 || task->vehicleIndex >= MAX_VEHICLES ||
            task->orderIndex < 0 || task->orderIndex >= MAX_ORDERS ||
            task->routeLength <= 0 ||
            task->currentStep < 0 || task->currentStep >= task->routeLength)
        {
            task->active = 0;
            task->orderIndex = -1;
            task->vehicleIndex = -1;
            task->routeLength = 0;
            task->currentStep = 0;
            task->progress = 0.0;
            continue;
        }

        //已经站在终点：收尾完成配送 
        if (task->currentStep >= task->routeLength - 1)
        {
            updateAnimPos(graph, task);

            /* 先把距离写进订单，供统计模块求和 */
            orders[task->orderIndex].distance = task->routeDistance;

            if (finishDelivery(&orders[task->orderIndex],
                &vehicles[task->vehicleIndex]))
            {
                sprintf(buf, "[配送] 订单%d 已送达（车辆%d）",
                    orders[task->orderIndex].id,
                    vehicles[task->vehicleIndex].id);
                AddLog(buf);
            }

            task->active = 0;
            task->orderIndex = -1;
            task->vehicleIndex = -1;
            task->routeLength = 0;
            task->currentStep = 0;
            task->progress = 0.0;
            continue;
        }

        /* 本帧经过了多久 */
        dt = (double)(now - task->lastTick) / 1000.0;
        task->lastTick = now;

        if (dt < 0.0) dt = 0.0;
        if (dt > 0.1) dt = 0.1;

        fromIdx = task->routeNodes[task->currentStep];
        toIdx = task->routeNodes[task->currentStep + 1];

        if (fromIdx < 0 || fromIdx >= graph->locationCount ||
            toIdx < 0 || toIdx >= graph->locationCount)
        {
            continue;
        }

        dx = (double)(graph->locations[toIdx].x - graph->locations[fromIdx].x);
        dy = (double)(graph->locations[toIdx].y - graph->locations[fromIdx].y);
        segLen = sqrt(dx * dx + dy * dy);

        if (segLen < 0.5)
        {
            task->progress = 1.0;          
        }
        else
        {
            task->progress += DELIVERY_SPEED * dt / segLen;
        }

        /* 走满一段就换下一段 */
        while (task->progress >= 1.0 &&
            task->currentStep < task->routeLength - 1)
        {
            int nodeIdx;

            task->progress -= 1.0;
            task->currentStep++;

            nodeIdx = task->routeNodes[task->currentStep];

            if (nodeIdx < 0 || nodeIdx >= graph->locationCount)
            {
                break;
            }

            //真正到了节点，更新逻辑位置
            vehicles[task->vehicleIndex].position =
                graph->locations[nodeIdx].id;

            if (task->currentStep < task->routeLength - 1)
            {
                fromIdx = task->routeNodes[task->currentStep];
                toIdx = task->routeNodes[task->currentStep + 1];

                if (fromIdx < 0 || fromIdx >= graph->locationCount ||
                    toIdx < 0 || toIdx >= graph->locationCount)
                {
                    break;
                }

                dx = (double)(graph->locations[toIdx].x
                    - graph->locations[fromIdx].x);
                dy = (double)(graph->locations[toIdx].y
                    - graph->locations[fromIdx].y);
                segLen = sqrt(dx * dx + dy * dy);

                if (segLen < 0.5)
                {
                    task->progress = 1.0;
                }
            }
        }

        if (task->progress < 0.0)
        {
            task->progress = 0.0;
        }

        /*  刷新这一帧车辆要画在哪  */
        updateAnimPos(graph, task);
    }
}
