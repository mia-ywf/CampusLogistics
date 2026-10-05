#ifndef VEHICLE_H
#define VEHICLE_H

#include "common.h"

// 车辆结构体
typedef struct {
	int id;                 // 车辆ID
	int position;           // 当前所在位置
	int status;             // 车辆状态
	int currentOrder;       // 分配的订单ID
} Vehicle;

// 初始化车辆
void initVehicles(Vehicle vehicles[], int* count);
// 添加车辆
int addVehicle(Vehicle vehicles[], int* count, Vehicle vehicle);
// 查找车辆
Vehicle* findVehicle(Vehicle vehicles[], int count, int vehicleId);
// 获取空闲车辆数量
int getIdleVehicle(Vehicle vehicles[], int count);

// 从文件加载车辆
int LoadVehicles(const char* filename, Vehicle vehicles[], int* count);//新增

#endif // !VEHICLE_H