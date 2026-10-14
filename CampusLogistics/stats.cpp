#define _CRT_SECURE_NO_WARNINGS

#include "stats.h"
#include "mapdata.h"

// 统计计算
void ComputeStats(Order orders[], int orderCount,
    Vehicle vehicles[], int vehicleCount,
    Graph* graph, Stats* stats)
{
    int i;

    if (stats == NULL)
    {
        return;
    }

    /* ---- 先全部清零，避免残留上一次的数据 ---- */
    stats->totalOrders = 0;
    stats->waitingOrders = 0;
    stats->deliveringOrders = 0;
    stats->completedOrders = 0;
    stats->idleVehicles = 0;
    stats->deliveringVehicles = 0;
    stats->totalDistance = 0;
    stats->avgDistance = 0.0;
    stats->locationCount = 0;
    stats->roadCount = 0;
    stats->isConnected = 0;

    /* ---- 统计订单 ---- */
    if (orders != NULL && orderCount > 0)
    {
        stats->totalOrders = orderCount;

        for (i = 0; i < orderCount; i++)
        {
            if (orders[i].status == ORDER_WAITING)
            {
                stats->waitingOrders++;
            }
            else if (orders[i].status == ORDER_DELIVERING)
            {
                stats->deliveringOrders++;
            }
            else if (orders[i].status == ORDER_COMPLETED)
            {
                stats->completedOrders++;
                stats->totalDistance += orders[i].distance;
            }
        }
    }

    /* ---- 统计车辆 ---- */
    if (vehicles != NULL && vehicleCount > 0)
    {
        for (i = 0; i < vehicleCount; i++)
        {
            if (vehicles[i].status == VEHICLE_IDLE)
            {
                stats->idleVehicles++;
            }
            else if (vehicles[i].status == VEHICLE_DELIVERING)
            {
                stats->deliveringVehicles++;
            }
        }
    }

    /* ---- 平均配送距离 ---- */
    if (stats->completedOrders > 0)
    {
        stats->avgDistance =
            (double)stats->totalDistance / (double)stats->completedOrders;
    }
    else
    {
        stats->avgDistance = 0.0;
    }

    /* ---- 地图统计 ---- */
    if (graph != NULL)
    {
        stats->locationCount = CountValidLocations(graph);
        stats->roadCount = CountRoads(graph);
        stats->isConnected = IsGraphConnected(graph);
    }
}
