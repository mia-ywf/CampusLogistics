#ifndef SIMULATION_H
#define SIMULATION_H

#include "common.h"
#include "order.h"
#include "vehicle.h"
#include "path.h"

/* 开始配送 */
int startDelivery(Order* order, Vehicle* vehicle, Path* path);

/* 完成配送 */
int finishDelivery(Order* order, Vehicle* vehicle);

//新增
/* 合并车辆到取货点、取货点到送货点的两段路径 */
int buildDeliveryRoute(
    Path* pathToPickup,
    Path* pathToDelivery,
    int routeNodes[],
    int* routeLength
);

#endif