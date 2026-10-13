#include "simulation.h"
#include <stddef.h>

/* 开始配送 */
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

    /* 车辆开始配送 */
    order->status = ORDER_DELIVERING;
    vehicle->status = VEHICLE_DELIVERING;
    vehicle->currentOrder = order->id;

    return 1;
}
/* 完成配送 */
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

    /* 配送完成 */
    order->status = ORDER_COMPLETED;
    vehicle->status = VEHICLE_IDLE;
    vehicle->currentOrder = -1;

    return 1;
}

//新增
/* 合并两段路径，取货点只保留一次 */
int buildDeliveryRoute(
    Path* pathToPickup,
    Path* pathToDelivery,
    int routeNodes[],
    int* routeLength)
{
    int i;
    int count = 0;

    if (pathToPickup == NULL ||
        pathToDelivery == NULL ||
        routeNodes == NULL ||
        routeLength == NULL)
    {
        return 0;
    }

    if (pathToPickup->length <= 0 ||
        pathToDelivery->length <= 0)
    {
        return 0;
    }

    /* 两段路径必须在取货点衔接 */
    if (pathToPickup->nodes[pathToPickup->length - 1]
        != pathToDelivery->nodes[0])
    {
        return 0;
    }

    /* 复制第一段：车辆当前位置到取货点 */
    for (i = 0; i < pathToPickup->length; i++)
    {
        if (count >= 2 * MAX_LOCATIONS)
        {
            return 0;
        }

        routeNodes[count++] = pathToPickup->nodes[i];
    }

    /* 复制第二段，从下标 1 开始，跳过重复的取货点 */
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

