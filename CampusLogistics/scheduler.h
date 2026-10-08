
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "common.h"
#include "order.h"
#include "vehicle.h"
#include "graph.h"

/* 优先队列 */
typedef struct
{
	int orderIndex[MAX_ORDERS];// 存储订单ID的数组下标
    int size;
} PriorityQueue;

/* 初始化优先队列 */
void initPriorityQueue(PriorityQueue* queue);

/* 插入订单 */
int pushOrder(PriorityQueue* queue, Order orders[], int orderId);

/* 取出最高优先级订单 （最大堆）*/
int popOrder(PriorityQueue* queue, Order orders[]);

/* 查看队首订单 （最大堆）*/
int peekOrder(PriorityQueue* queue);

/* 调度：选择优先级最高的等待订单，并分配空闲车辆 */
int scheduleNextOrder(
    PriorityQueue* queue,
    Order orders[],
    int orderCount,
    Vehicle vehicles[],
    int vehicleCount
);

#endif