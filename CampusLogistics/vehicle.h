#ifndef VEHICLE_H
#define VEHICLE_H

#include "common.h"

//车辆模块
typedef struct
{
    int id;             /* 车辆编号 */
    int position;       /* 当前所在位置（地点 ID） */
    int status;         /* 车辆状态 */
    int currentOrder;   /* 当前正在配送的订单 ID，空闲时为 -1 */
} Vehicle;

/* 初始化车辆数组 */
void initVehicles(Vehicle vehicles[], int* count);

/* 添加车辆 */
int addVehicle(Vehicle vehicles[], int* count, Vehicle vehicle);

/* 查找车辆 */
Vehicle* findVehicle(Vehicle vehicles[], int count, int vehicleId);

/* 获取空闲车辆数量 */
int getIdleVehicleCount(Vehicle vehicles[], int count);

/* 删除车辆 */
int deleteVehicle(Vehicle vehicles[], int* count, int vehicleId);

/* 修改车辆所在位置 */
int updateVehiclePosition(Vehicle vehicles[], int count,
    int vehicleId, int newPositionId);

/* 从 txt 读取车辆信息 */
int LoadVehicles(const char* filename, Vehicle vehicles[], int* count);

/* 把车辆写回 txt */
int SaveVehicles(const char* filename, Vehicle vehicles[], int count);

#endif /* VEHICLE_H */
