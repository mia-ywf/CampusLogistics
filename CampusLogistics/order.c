#include "order.h"
#include <stddef.h>

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