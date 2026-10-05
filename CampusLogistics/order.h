#ifndef ORDER_H
#define ORDER_H

#include "common.h"

/* 订单 */
typedef struct
{
    int id;                 // 订单编号
    int pickup;             // 取货地点
    int delivery;           // 配送地点
    int priority;           // 优先级
    int status;             // 订单状态
    int createTime;         // 创建时间
} Order;

/* 初始化订单 */
void initOrders(Order orders[], int* count);

/* 添加订单 */
int addOrder(Order orders[], int* count, Order order);

/* 删除订单 */
int deleteOrder(Order orders[], int* count, int orderId);

/* 查找订单 */
Order* findOrder(Order orders[], int count, int orderId);

#endif