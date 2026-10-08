#include "vehicle.h"
#include <stddef.h>
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>

void initVehicles(Vehicle vehicles[], int* count)
{
    *count = 0;
}

int addVehicle(Vehicle vehicles[], int* count, Vehicle vehicle)
{
    int i;

    if (vehicles == NULL || count == NULL)
    {
        return 0;
    }

    if (*count >= MAX_VEHICLES)
    {
        return 0;
    }

    // 检查车辆 ID 是否重复
    for (i = 0; i < *count; i++)
    {
        if (vehicles[i].id == vehicle.id)
        {
            return 0;
        }
    }

    // 检查初始状态是否合法
    if (vehicle.status != VEHICLE_IDLE &&
        vehicle.status != VEHICLE_DELIVERING)
    {
        return 0;
    }

    // 空闲车辆不能关联正在处理的订单
    if (vehicle.status == VEHICLE_IDLE &&
        vehicle.currentOrder != -1)
    {
        return 0;
    }

    vehicles[*count] = vehicle;
    (*count)++;

    return 1;
}

Vehicle* findVehicle(Vehicle vehicles[], int count, int vehicleId)
{
    int i;

    for (i = 0; i < count; i++)
    {
        if (vehicles[i].id == vehicleId)
        {
            return &vehicles[i];
        }
    }

    return NULL;
}


/* 删除车辆：成功返回 1，失败返回 0 */
int deleteVehicle(Vehicle vehicles[], int* count, int vehicleId)
{
    int i;
    int j;

    if (vehicles == NULL || count == NULL)
    {
        return 0;
    }

    for (i = 0; i < *count; i++)
    {
        if (vehicles[i].id == vehicleId)
        {
            // 配送中的车辆不能删除
            if (vehicles[i].status != VEHICLE_IDLE)
            {
                return 0;
            }

            // 后面的车辆依次向前移动
            for (j = i; j < *count - 1; j++)
            {
                vehicles[j] = vehicles[j + 1];
            }

            (*count)--;

            return 1;
        }
    }

    // 没找到对应车辆
    return 0;
}

int getIdleVehicleCount(Vehicle vehicles[], int count)
{
    int i;
    int idleCount = 0;

    for (i = 0; i < count; i++)
    {
        if (vehicles[i].status == VEHICLE_IDLE)
        {
            idleCount++;
        }
    }

    return idleCount;
}

//从txt里面读取车辆信息
int LoadVehicles(const char* filename, Vehicle vehicles[], int* count)
{
    FILE* fp = fopen(filename, "r");
    int id, position, status, currentOrder;
    int n = 0;

    if (fp == NULL)
    {
        printf("Cannot open %s\n", filename);
        return 0;
    }

    while (fscanf(fp, "%d %d %d %d", &id, &position, &status, &currentOrder) == 4)
    {
        if (n >= MAX_VEHICLES) break;
        vehicles[n].id = id;
        vehicles[n].position = position;
        vehicles[n].status = status;
        vehicles[n].currentOrder = currentOrder;
        n++;
    }

    fclose(fp);
    *count = n;
    printf("Loaded %d vehicles\n", n);
    return 1;
}
