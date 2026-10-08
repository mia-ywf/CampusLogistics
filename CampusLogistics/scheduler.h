#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "common.h"
#include "order.h"
#include "vehicle.h"
#include "graph.h"

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

#endif