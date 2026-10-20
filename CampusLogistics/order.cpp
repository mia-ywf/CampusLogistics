#define _CRT_SECURE_NO_WARNINGS

#include "order.h"
#include <stddef.h>
#include <stdio.h>
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>

// 初始化订单 
void initOrders(Order orders[], int* count)
{
    if (count == NULL)
    {
        return;
    }
    *count = 0;
}

//添加订单 
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

    /* 生成创建顺序：createTime 越小，表示订单创建得越早*/
    for (i = 0; i < *count; i++)
    {
        if (orders[i].createTime >= nextCreateTime)
        {
            nextCreateTime = orders[i].createTime + 1;
        }
    }

    order.createTime = nextCreateTime;
    order.createTime = nextCreateTime;
    order.status = ORDER_WAITING;   /* 新订单一律是"等待" */
    order.distance = 0;             /* 还没配送，距离为 0 */

    orders[*count] = order;
    (*count)++;

    return 1;
}

// 删除订单 
int deleteOrder(Order orders[], int* count, int orderId)
{
    int i;

    if (orders == NULL || count == NULL || *count <= 0)
    {
        return 0;
    }

    for (i = 0; i < *count; i++)
    {
        if (orders[i].id == orderId)
        {
            int j;

            /* 正在配送的订单不能删 */
            if (orders[i].status == ORDER_DELIVERING)
            {
                return 0;
            }

            /* 后面的订单整体前移，覆盖掉被删的那一个 */
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

//查找订单
Order* findOrder(Order orders[], int count, int orderId)
{
    int i;
    if (orders == NULL || count <= 0)
    {
        return NULL;
    }

    for (i = 0; i < count; i++)
    {
        if (orders[i].id == orderId)
        {
            return &orders[i];
        }
    }

    return NULL;
}


//修改订单
int updateOrder(Order orders[], int count, int orderId,
    int newPickup, int newDelivery, int newPriority)
{
    Order* target;

    /* 参数合法性 */
    if (newPickup == newDelivery)
    {
        return 0;
    }

    if (newPriority < MIN_PRIORITY || newPriority > MAX_PRIORITY)
    {
        return 0;
    }

    target = findOrder(orders, count, orderId);
    if (target == NULL)
    {
        return 0;
    }

    if (target->status != ORDER_WAITING)
    {
        return 0;
    }

    target->pickup = newPickup;
    target->delivery = newDelivery;
    target->priority = newPriority;

    return 1;
}

//读取订单txt文件
int LoadOrders(const char* filename, Order orders[], int* count)
{
    FILE* fp;
    char line[256];
    int n = 0;

    if (filename == NULL || orders == NULL || count == NULL)
    {
        return 0;
    }

    *count = 0;

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Cannot open %s\n", filename);
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL && n < MAX_ORDERS)
    {
        int id, pickup, delivery, priority;
        int status = ORDER_WAITING;
        int createTime = n;
        int distance = 0;
        int got;
        char* p = line;

        
        while (*p == ' ' || *p == '\t')
        {
            p++;
        }

       
        if (*p == '\0' || *p == '\n' || *p == '\r' || *p == '#')
        {
            continue;
        }

        got = sscanf(p, "%d %d %d %d %d %d %d",
            &id, &pickup, &delivery, &priority,
            &status, &createTime, &distance);

        if (got < 4)
        {
            printf("Skip invalid order line: %s", line);
            continue;                 /* 至少要有 4 个字段 */
        }

        /* 缺的字段用默认值补上 */
        if (got < 5) status = ORDER_WAITING;
        if (got < 6) createTime = n;
        if (got < 7) distance = 0;

        /*合法性校验 */
        if (status < ORDER_WAITING || status > ORDER_COMPLETED)
        {
            status = ORDER_WAITING;
        }
        if (priority < MIN_PRIORITY || priority > MAX_PRIORITY)
        {
            priority = MIN_PRIORITY;
        }
        if (distance < 0)
        {
            distance = 0;
        }

        orders[n].id = id;
        orders[n].pickup = pickup;
        orders[n].delivery = delivery;
        orders[n].priority = priority;
        orders[n].status = status;
        orders[n].createTime = createTime;
        orders[n].distance = distance;
        n++;
    }

    fclose(fp);
    *count = n;
    printf("Loaded %d orders\n", n);
    return 1;
}

// 保存订单
int SaveOrders(const char* filename, Order orders[], int count)
{
    FILE* fp;
    int i;

    if (filename == NULL || orders == NULL || count < 0)
    {
        return 0;
    }

    fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("Cannot write %s\n", filename);
        return 0;
    }

    fprintf(fp, "# id pickup delivery priority status createTime distance\n");

    for (i = 0; i < count; i++)
    {
        fprintf(fp, "%d %d %d %d %d %d %d\n",
            orders[i].id,
            orders[i].pickup,
            orders[i].delivery,
            orders[i].priority,
            orders[i].status,
            orders[i].createTime,
            orders[i].distance);
    }

    fclose(fp);
    printf("Saved %d orders\n", count);
    return 1;
}
