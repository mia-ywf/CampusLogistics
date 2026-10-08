#include "stats.h"

// 遍历订单，数各种状态；遍历车辆，数各种状态
void ComputeStats(Order orders[], int orderCount,
    Vehicle vehicles[], int vehicleCount,
    Stats* stats)
{
    int i;

    stats->totalOrders = orderCount;
    stats->waitingOrders = 0;
    stats->deliveringOrders = 0;
    stats->completedOrders = 0;
    stats->totalDistance = 0;

    // 数订单
    for (i = 0; i < orderCount; i++)
    {
        if (orders[i].status == ORDER_WAITING)
            stats->waitingOrders++;
        else if (orders[i].status == ORDER_DELIVERING)
            stats->deliveringOrders++;
        else if (orders[i].status == ORDER_COMPLETED)
            stats->completedOrders++;
    }

    // 数车辆
    stats->idleVehicles = 0;
    stats->deliveringVehicles = 0;

    for (i = 0; i < vehicleCount; i++)
    {
        if (vehicles[i].status == VEHICLE_IDLE)
            stats->idleVehicles++;
        else if (vehicles[i].status == VEHICLE_DELIVERING)
            stats->deliveringVehicles++;
    }

    // 平均距离
    if (stats->completedOrders > 0)
        stats->avgDistance = (double)stats->totalDistance / stats->completedOrders;
    else
        stats->avgDistance = 0.0;
}