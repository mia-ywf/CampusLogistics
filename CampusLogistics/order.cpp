#include "order.h"
#include <stddef.h>
#include <stdio.h>
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>

/* 初始化订单 */
void initOrders(Order orders[], int* count)
{
    *count = 0;
}

/* 添加订单 */
int addOrder(Order orders[], int* count, Order order)
{
    int i;
    int nextCreateTime = 0;

    if (orders == NULL || count == NULL)
    {
        return 0;
    }

    if (*count >= MAX_ORDERS)
    {
        return 0;
    }

    /*
     * 自动生成创建顺序：
     * createTime 越小，表示订单创建得越早。
     */
    for (i = 0; i < *count; i++)
    {
        if (orders[i].createTime >= nextCreateTime)
        {
            nextCreateTime = orders[i].createTime + 1;
        }
    }

    order.createTime = nextCreateTime;

    orders[*count] = order;
    (*count)++;

    return 1;
}

/* 删除订单 */
int deleteOrder(Order orders[], int* count, int orderId)
{
    int i;

    for (i = 0; i < *count; i++)
    {
        if (orders[i].id == orderId)
        {
            int j;

            for (j = i; j < *count - 1; j++)
            {
                orders[j] = orders[j + 1];
            }

            (*count)--;

            return 1;
        }
    }

    return 0;
}

/* 查找订单 */
Order* findOrder(Order orders[], int count, int orderId)
{
    int i;

    for (i = 0; i < count; i++)
    {
        if (orders[i].id == orderId)
        {
            return &orders[i];
        }
    }

    return NULL;
}

/* 从 txt 文件读取订单 */
int LoadOrders(const char* filename, Order orders[], int* count)
{
    FILE* fp = fopen(filename, "r");
    int id, pickup, delivery, priority;
    int n = 0;

    if (fp == NULL)
    {
        printf("Cannot open %s\n", filename);
        return 0;
    }

    while (fscanf(fp, "%d %d %d %d",
        &id, &pickup, &delivery, &priority) == 4)
    {
        if (n >= MAX_ORDERS)
        {
            break;
        }

        orders[n].id = id;
        orders[n].pickup = pickup;
        orders[n].delivery = delivery;
        orders[n].priority = priority;
        orders[n].status = ORDER_WAITING;
        orders[n].createTime = n;

        n++;
    }

    fclose(fp);

    *count = n;

    printf("Loaded %d orders\n", n);

    return 1;
}