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

#endif