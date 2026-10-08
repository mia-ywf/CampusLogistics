#ifndef STATS_H
#define STATS_H

#include "common.h"
#include "order.h"
#include "vehicle.h"

typedef struct
{
    int totalOrders;
    int waitingOrders;
    int deliveringOrders;
    int completedOrders;

    int idleVehicles;
    int deliveringVehicles;

    int totalDistance;
    double avgDistance;
} Stats;

void ComputeStats(
    Order orders[], 
    int orderCount,
    Vehicle vehicles[],
    int vehicleCount,
    Stats* stats
);

#endif

