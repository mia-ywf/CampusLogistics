#ifndef SIMULATION_H
#define SIMULATION_H

#include "common.h"
#include "graph.h"
#include "order.h"
#include "vehicle.h"
#include "path.h"

 /* 一辆配送中的车辆 = 一个独立的配送任务 */
typedef struct
{
    int active;                         /* 是否正在执行 */
    int orderIndex;                     /* 订单在数组中的下标 */
    int vehicleIndex;                   /* 车辆在数组中的下标 */

    int routeNodes[2 * MAX_LOCATIONS];  /* 完整路线（车→取货点→送货点）的节点下标 */
    int routeLength;                    /* 路线节点个数 */

    /*平滑动画状态*/
    int currentStep;                    /* 当前所在路段的起点节点下标 */
    double progress;                    /* 当前路段的进度 */
    double animX, animY;                /* 当前应绘制的地图坐标 */
    unsigned int lastTick;              /* 上一次推进的时间戳*/

    int toPickupLength;                 /* 第一段路径的节点数 */
    int routeDistance;                  /* 本次配送总距离 */
} ActiveDelivery;

/* 基础操作*/
int startDelivery(Order* order, Vehicle* vehicle, Path* path);
int finishDelivery(Order* order, Vehicle* vehicle);

/* 把"车→取货点"和"取货点→送货点"两段路径拼成一条路线 */
int buildDeliveryRoute(Path* pathToPickup, Path* pathToDelivery,
    int routeNodes[], int* routeLength);

/*配送任务管理*/
void initDeliveries(ActiveDelivery deliveries[], int count);

/* 给所有空闲车辆分配等待中的订单，返回成功分配的数量 */
int assignWaitingOrders(Graph* graph, Order orders[], int orderCount,
    Vehicle vehicles[], int vehicleCount,
    ActiveDelivery deliveries[]);

/* 推进所有正在配送的车辆 */
void updateDeliveries(Graph* graph, Order orders[], Vehicle vehicles[],
    ActiveDelivery deliveries[]);

#endif /* SIMULATION_H */
