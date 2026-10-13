#include "scheduler.h"
#include "path.h"
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

        if (!higherPriority(
            orders,
            queue->orderIndex[i],
            queue->orderIndex[parent]))
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

//新增
/* 将所有等待中的订单重新加入最大堆 */
int buildPriorityQueue(
    PriorityQueue* queue,
    Order orders[],
    int orderCount)
{
    int i;

    if (queue == NULL || orders == NULL ||
        orderCount < 0 || orderCount > MAX_ORDERS)
    {
        return 0;
    }

    /* 清空旧队列，避免重复调度 */
    initPriorityQueue(queue);

    for (i = 0; i < orderCount; i++)
    {
        if (orders[i].status == ORDER_WAITING)
        {
            if (!pushOrder(queue, orders, i))
            {
                initPriorityQueue(queue);
                return 0;
            }
        }
    }

    return 1;
}

/* 选择距离取货点最近的空闲车辆 */
int findNearestIdleVehicle(
    Graph* graph,
    Vehicle vehicles[],
    int vehicleCount,
    int pickupLocationId)
{
    int pickupIndex;
    int startIndex;
    int bestIndex = -1;
    int bestDistance = INF;
    int i;
    Path path;

    /* 检查输入是否合法 */
    if (graph == NULL || vehicles == NULL ||
        vehicleCount < 0 || vehicleCount > MAX_VEHICLES)
    {
        return -1;
    }

    /* 将取货地点 ID 转换为地图数组下标 */
    pickupIndex = FindLocationIndexById(
        graph, pickupLocationId);

    if (pickupIndex < 0)
    {
        return -1;
    }

    /* 遍历所有车辆 */
    for (i = 0; i < vehicleCount; i++)
    {
        /* 忙碌车辆不能被分配 */
        if (vehicles[i].status != VEHICLE_IDLE)
        {
            continue;
        }

        /* 将车辆当前位置 ID 转换为地图数组下标 */
        startIndex = FindLocationIndexById(
            graph, vehicles[i].position);

        if (startIndex < 0)
        {
            continue;
        }

        /* 计算车辆到取货点的最短路径 */
        if (!dijkstra(graph, startIndex, pickupIndex, &path))
        {
            continue;
        }

        /* 记录距离最近的空闲车辆 */
        if (path.distance < bestDistance)
        {
            bestDistance = path.distance;
            bestIndex = i;
        }
    }

    /* 返回车辆数组下标；-1 表示没有合适车辆 */
    return bestIndex;
}


/* 为下一个订单选择车辆并计算两段路线 */
int prepareNextDelivery(
    Graph* graph,
    Order orders[],
    int orderCount,
    Vehicle vehicles[],
    int vehicleCount,
    PriorityQueue* queue,
    int* selectedOrderIndex,
    int* selectedVehicleIndex,
    Path* pathToPickup,
    Path* pathToDelivery)
{
    int orderIndex;
    int vehicleIndex;
    int pickupIndex;
    int deliveryIndex;
    int vehiclePositionIndex;

    if (graph == NULL || orders == NULL ||
        vehicles == NULL || queue == NULL ||
        selectedOrderIndex == NULL ||
        selectedVehicleIndex == NULL ||
        pathToPickup == NULL || pathToDelivery == NULL)
    {
        return 0;
    }

    if (orderCount < 0 || orderCount > MAX_ORDERS ||
        vehicleCount < 0 || vehicleCount > MAX_VEHICLES)
    {
        return 0;
    }

    /* 重新收集所有等待订单 */
    if (!buildPriorityQueue(queue, orders, orderCount))
    {
        return 0;
    }

    /* 查看最高优先级订单，不立即弹出 */
    orderIndex = peekOrder(queue);

    if (orderIndex < 0 || orderIndex >= orderCount)
    {
        return 0;
    }

    /* 选择距离取货点最近的空闲车辆 */
    vehicleIndex = findNearestIdleVehicle(
        graph,
        vehicles,
        vehicleCount,
        orders[orderIndex].pickup
    );

    if (vehicleIndex < 0)
    {
        return 0;
    }

    /* 转换地点 ID 为地图数组下标 */
    vehiclePositionIndex = FindLocationIndexById(
        graph, vehicles[vehicleIndex].position);

    pickupIndex = FindLocationIndexById(
        graph, orders[orderIndex].pickup);

    deliveryIndex = FindLocationIndexById(
        graph, orders[orderIndex].delivery);

    if (vehiclePositionIndex < 0 ||
        pickupIndex < 0 || deliveryIndex < 0)
    {
        return 0;
    }

    /* 第一段：车辆当前位置到取货点 */
    if (!dijkstra(
        graph,
        vehiclePositionIndex,
        pickupIndex,
        pathToPickup))
    {
        return 0;
    }

    /* 第二段：取货点到送货点 */
    if (!dijkstra(
        graph,
        pickupIndex,
        deliveryIndex,
        pathToDelivery))
    {
        return 0;
    }

    /* 路线全部计算成功后，才正式从队列取出订单 */
    if (popOrder(queue, orders) != orderIndex)
    {
        return 0;
    }

    *selectedOrderIndex = orderIndex;
    *selectedVehicleIndex = vehicleIndex;

    return 1;
}
