#include "scheduler.h"

void initPriorityQueue(PriorityQueue* queue)
{
    queue->size = 0;
}

int pushOrder(PriorityQueue* queue, Order orders[], int orderId)
{
    int i;
    int parent;
    int temp;

    if (queue->size >= MAX_ORDERS)
    {
        return 0;
    }

    queue->orderIds[queue->size] = orderId;
    i = queue->size;
    queue->size++;

    /* 上浮：优先级数字越大，优先级越高 */
    while (i > 0)
    {
        parent = (i - 1) / 2;

        if (orders[queue->orderIds[parent]].priority >=
            orders[queue->orderIds[i]].priority)
        {
            break;
        }

        temp = queue->orderIds[parent];
        queue->orderIds[parent] = queue->orderIds[i];
        queue->orderIds[i] = temp;

        i = parent;
    }

    return 1;
}

int popOrder(PriorityQueue* queue, Order orders[])
{
    int result;
    int i;
    int left;
    int right;
    int largest;
    int temp;

    if (queue->size == 0)
    {
        return -1;
    }

    result = queue->orderIds[0];

    queue->size--;

    if (queue->size == 0)
    {
        return result;
    }

    queue->orderIds[0] = queue->orderIds[queue->size];

    /* 下沉 */
    i = 0;

    while (1)
    {
        left = 2 * i + 1;
        right = 2 * i + 2;
        largest = i;

        if (left < queue->size &&
            orders[queue->orderIds[left]].priority >
            orders[queue->orderIds[largest]].priority)
        {
            largest = left;
        }

        if (right < queue->size &&
            orders[queue->orderIds[right]].priority >
            orders[queue->orderIds[largest]].priority)
        {
            largest = right;
        }

        if (largest == i)
        {
            break;
        }

        temp = queue->orderIds[i];
        queue->orderIds[i] = queue->orderIds[largest];
        queue->orderIds[largest] = temp;

        i = largest;
    }

    return result;
}

int peekOrder(PriorityQueue* queue)
{
    if (queue->size == 0)
    {
        return -1;
    }

    return queue->orderIds[0];
}