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
    int distance;           //本订单实际配送距离，未完成时为 0 

} Order;

/* 初始化订单 */
void initOrders(Order orders[], int* count);//count记录当前订单数

/* 添加订单 */
int addOrder(Order orders[], int* count, Order order);

/* 删除订单 */
int deleteOrder(Order orders[], int* count, int orderId);

/* 查找订单 */
Order* findOrder(Order orders[], int count, int orderId);

/* 修改订单*/
int updateOrder(Order orders[], int count, int orderId,
    int newPickup, int newDelivery, int newPriority);

/* 从 txt 文件读取订单 */
int LoadOrders(const char* filename, Order orders[], int* count);   

/* 把订单写回 txt */
int SaveOrders(const char* filename, Order orders[], int count);

#endif