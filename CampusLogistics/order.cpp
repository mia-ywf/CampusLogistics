#include "order.h"
#include <stddef.h>
#include <stdio.h>
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>

void initOrders(Order orders[], int* count)
{
    *count = 0;
}

int addOrder(Order orders[], int* count, Order order)
{
    if (*count >= MAX_ORDERS)
    {
        return 0;
    }

    orders[*count] = order;
    (*count)++;

    return 1;
}

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

//从txt里面读取订单
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

    while (fscanf(fp, "%d %d %d %d", &id, &pickup, &delivery, &priority) == 4)
    {
        if (n >= MAX_ORDERS) break;
        orders[n].id = id;
        orders[n].pickup = pickup;
        orders[n].delivery = delivery;
        orders[n].priority = priority;
        orders[n].status = ORDER_WAITING;
        orders[n].createTime = 0;
        n++;
    }

    fclose(fp);
    *count = n;
    printf("Loaded %d orders\n", n);
    return 1;
}