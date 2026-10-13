#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "common.h"
#include "order.h"
#include "vehicle.h"
#include "graph.h"
#include "path.h"

/* 优先队列 */
typedef struct
{
    int orderIndex[MAX_ORDERS];
    int size;
} PriorityQueue;

/* 初始化优先队列 */
void initPriorityQueue(PriorityQueue* queue);

/* 插入订单 */
int pushOrder(PriorityQueue* queue, Order orders[], int orderIndex);

/* 取出最高优先级订单（最大堆） */
int popOrder(PriorityQueue* queue, Order orders[]);

/* 查看队首订单（最大堆） */
int peekOrder(PriorityQueue* queue);

//新增
/* 将所有等待中的订单重新加入最大堆 */
int  buildPriorityQueue(PriorityQueue* queue, Order orders[], int orderCount);

/* 选择距离取货点最近的空闲车辆 */
int findNearestIdleVehicle(Vehicle vehicles[], int vehicleCount, Graph* graph, int pickLocationId);

/* 为下一个订单选择车辆并计算两段路线 */
int prepareNextDelivery(
    Graph* graph,
    Order orders[],
    int orderCount,
    Vehicle vehicles[],
    int vehicleCount,
    PriorityQueue* queue,
    int* selectedOrderIndex,
    int* selectedVehicleIndex,
    Path* pathToPickup,
    Path* pathToDelivery
);

#endif