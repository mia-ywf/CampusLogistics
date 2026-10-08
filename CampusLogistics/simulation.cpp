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