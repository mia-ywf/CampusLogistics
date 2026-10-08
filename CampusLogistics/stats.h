#ifndef STATS_H
#define STATS_H

#include "common.h"
#include "order.h"
#include "vehicle.h"
/* 统计信息结构体 */
typedef struct
{
    int totalOrders;
    int waitingOrders;
    int deliveringOrders;
    int completedOrders;

	int idleVehicles;//空闲车辆数
    int deliveringVehicles;

    int totalDistance;
    double avgDistance;
} Stats;

/* 计算统计信息 */
void ComputeStats(
    Order orders[], 
    int orderCount,
    Vehicle vehicles[],
    int vehicleCount,
    Stats* stats
);

#endif

