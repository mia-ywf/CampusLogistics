#include "scheduler.h"

void initPriorityQueue(PriorityQueue* queue)
{
    queue->size = 0;
}

/* 插入订单到优先队列（最大堆） */
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

/* 取出优先队列中最高优先级订单（最大堆） */
int popOrder(PriorityQueue* queue, Order orders[])
{
    int result;
    int i;
    int left;
    int right;
    int largest;
    int temp;

	if (queue->size == 0)// 队列为空
    {
        return -1;
    }

	result = queue->orderIds[0];// 保存队首订单ID
    queue->size--;

    if (queue->size == 0)
    {
        return result;
    }

	queue->orderIds[0] = queue->orderIds[queue->size]; // 将最后一个订单放到队首
    i = 0;

	// 堆化下沉
    while (1)
    {
        left = 2 * i + 1;
        right = 2 * i + 2;
        largest = i;

        if (left < queue->size &&
            orders[queue->orderIds[left]].priority >
			orders[queue->orderIds[largest]].priority)// 左子节点优先级大于当前节点
        {
            largest = left;
        }

        if (right < queue->size &&
            orders[queue->orderIds[right]].priority >
			orders[queue->orderIds[largest]].priority)// 右子节点优先级大于当前节点
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

// 只查看优先队列中最高优先级订单（最大堆）
int peekOrder(PriorityQueue* queue)
{
    if (queue->size == 0)
    {
        return -1;
    }

    return queue->orderIds[0];
}
