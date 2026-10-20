#ifndef STATS_H
#define STATS_H


 //系统监控与统计模块
#include "common.h"
#include "order.h"
#include "vehicle.h"
#include "graph.h"

typedef struct
{
    /*订单*/
    int totalOrders;        /* 订单总数 */
    int waitingOrders;      /* 待处理 */
    int deliveringOrders;   /* 配送中 */
    int completedOrders;    /* 已完成 */

    /*车辆*/
    int idleVehicles;       /* 空闲车辆 */
    int deliveringVehicles; /* 配送中车辆 */

    /*距离*/
    int totalDistance;      /* 已完成订单的配送总距离 */
    double avgDistance;     /* 平均配送距离 */

    /*地图*/
    int locationCount;      /* 地点个数 */
    int roadCount;          /* 道路条数 */
    int isConnected;        /* 地图是否连通：1 连通 / 0 不连通 */
} Stats;


void ComputeStats(Order orders[], int orderCount,
    Vehicle vehicles[], int vehicleCount,
    Graph* graph, Stats* stats);

#endif /* STATS_H */
