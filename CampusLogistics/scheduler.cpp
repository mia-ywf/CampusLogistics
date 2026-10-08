#include "scheduler.h"
#include <cstddef>

/* 比较两个订单的优先级 */
int higherPriority(Order orders[], int a, int b)
{
    /* 优先级不同：优先级高的优先 */
    if (orders[a].priority != orders[b].priority)
    {
        return orders[a].priority > orders[b].priority;
    }

    /* 优先级相同：创建时间早的优先 */
    return orders[a].createTime < orders[b].createTime;
}

/* 初始化优先队列 */
void initPriorityQueue(PriorityQueue* queue)
{
    queue->size = 0;
}

/* 插入订单到优先队列（最大堆）
   queue 中保存的是订单在 orders[] 中的下标 */
int pushOrder(PriorityQueue* queue, Order orders[], int orderIndex)
{
    int i;
    int parent;
    int temp;

    if (queue == NULL || orders == NULL)
    {
        return 0;
    }

    if (queue->size >= MAX_ORDERS)
    {
        return 0;
    }

    if (orderIndex < 0 || orderIndex >= MAX_ORDERS)
    {
        return 0;
    }

    /* 将订单下标放到堆的末尾 */
    queue->orderIndex[queue->size] = orderIndex;

    i = queue->size;
    queue->size++;

    /* 向上调整，维护最大堆 */
    while (i > 0)
    {
        parent = (i - 1) / 2;

        if (higherPriority(orders,
            queue->orderIndex[parent],
            queue->orderIndex[i]))
        {
            break;
        }

        temp = queue->orderIndex[parent];
        queue->orderIndex[parent] = queue->orderIndex[i];
        queue->orderIndex[i] = temp;

        i = parent;
    }

    return 1;
}

/* 取出优先队列中最高优先级订单
   返回订单在 orders[] 中的下标 */
int popOrder(PriorityQueue* queue, Order orders[])
{
    int result;
    int i;
    int left;
    int right;
    int largest;
    int temp;

    if (queue == NULL || orders == NULL)
    {
        return -1;
    }

    if (queue->size == 0)
    {
        return -1;
    }

    /* 保存堆顶 */
    result = queue->orderIndex[0];

    /* 最后一个元素移动到堆顶 */
    queue->size--;

    if (queue->size == 0)
    {
        return result;
    }

    queue->orderIndex[0] = queue->orderIndex[queue->size];

    i = 0;

    /* 向下调整，维护最大堆 */
    while (1)
    {
        left = 2 * i + 1;
        right = 2 * i + 2;
        largest = i;

        if (left < queue->size &&
            higherPriority(orders,
                queue->orderIndex[left],
                queue->orderIndex[largest]))
        {
            largest = left;
        }

        if (right < queue->size &&
            higherPriority(orders,
                queue->orderIndex[right],
                queue->orderIndex[largest]))
        {
            largest = right;
        }

        if (largest == i)
        {
            break;
        }

        temp = queue->orderIndex[i];
        queue->orderIndex[i] = queue->orderIndex[largest];
        queue->orderIndex[largest] = temp;

        i = largest;
    }

    return result;
}

/* 查看优先队列中最高优先级订单
   返回订单在 orders[] 中的下标 */
int peekOrder(PriorityQueue* queue)
{
    if (queue == NULL || queue->size == 0)
    {
        return -1;
    }

    return queue->orderIndex[0];
}