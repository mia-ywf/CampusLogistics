
#ifndef VEHICLE_H
#define VEHICLE_H

#include "common.h"

// 车辆结构体
typedef struct {
	int id;                 // 车辆ID
	int current_location;   // 当前所在位置
	int status;             // 车辆状态
	int assigned_order_id;  // 分配的订单ID
} Vehicle;


void initVehicles(Vehicle vehicles[], int* count);

Vehicle* findVehicle(Vehicle vehicles[], int count, int vehicleId);

int getIdleVehicle(Vehicle vehicles[], int count);

#endif // !VEHICLE_H