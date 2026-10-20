#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include "vehicle.h"

//初始化车辆数组
void initVehicles(Vehicle vehicles[], int* count)
{
    if (count == NULL)
    {
        return;
    }
    *count = 0;
}

// 添加车辆
int addVehicle(Vehicle vehicles[], int* count, Vehicle vehicle)
{
    int i;

    if (vehicles == NULL || count == NULL)
    {
        return 0;
    }

    if (*count < 0 || *count >= MAX_VEHICLES)
    {
        return 0;
    }

    /* 编号不能重复 */
    for (i = 0; i < *count; i++)
    {
        if (vehicles[i].id == vehicle.id)
        {
            return 0;
        }
    }

    /* 状态值必须在合法范围内 */
    if (vehicle.status != VEHICLE_IDLE &&
        vehicle.status != VEHICLE_DELIVERING)
    {
        return 0;
    }

    /* 空闲车不应该关联任何订单；配送中的车必须有订单 */
    if (vehicle.status == VEHICLE_IDLE && vehicle.currentOrder != -1)
    {
        return 0;
    }
    if (vehicle.status == VEHICLE_DELIVERING && vehicle.currentOrder == -1)
    {
        return 0;
    }

    vehicles[*count] = vehicle;
    (*count)++;

    return 1;
}

// 查找车辆
Vehicle* findVehicle(Vehicle vehicles[], int count, int vehicleId)
{
    int i;

    if (vehicles == NULL || count <= 0)
    {
        return NULL;
    }

    for (i = 0; i < count; i++)
    {
        if (vehicles[i].id == vehicleId)
        {
            return &vehicles[i];
        }
    }

    return NULL;
}

//删除车辆
int deleteVehicle(Vehicle vehicles[], int* count, int vehicleId)
{
    int i;
    int j;

    if (vehicles == NULL || count == NULL || *count <= 0)
    {
        return 0;
    }

    for (i = 0; i < *count; i++)
    {
        if (vehicles[i].id == vehicleId)
        {
            /* 配送中的车不能删，否则配送任务会指向不存在的车 */
            if (vehicles[i].status != VEHICLE_IDLE)
            {
                return 0;
            }

            for (j = i; j < *count - 1; j++)
            {
                vehicles[j] = vehicles[j + 1];
            }

            (*count)--;
            return 1;
        }
    }

    return 0;
}

//修改车辆位置
int updateVehiclePosition(Vehicle vehicles[], int count,
    int vehicleId, int newPositionId)
{
    Vehicle* target = findVehicle(vehicles, count, vehicleId);

    if (target == NULL)
    {
        return 0;
    }

    if (target->status != VEHICLE_IDLE)
    {
        return 0;
    }

    target->position = newPositionId;
    return 1;
}

// 获取空闲车辆数量
int getIdleVehicleCount(Vehicle vehicles[], int count)
{
    int i;
    int idleCount = 0;

    if (vehicles == NULL || count <= 0)
    {
        return 0;
    }

    for (i = 0; i < count; i++)
    {
        if (vehicles[i].status == VEHICLE_IDLE)
        {
            idleCount++;
        }
    }

    return idleCount;
}

//从 txt 读取车辆
int LoadVehicles(const char* filename, Vehicle vehicles[], int* count)
{
    FILE* fp;
    char line[256];
    int n = 0;

    if (filename == NULL || vehicles == NULL || count == NULL)
    {
        return 0;
    }

    *count = 0;

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Cannot open %s\n", filename);
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL && n < MAX_VEHICLES)
    {
        int id, position, status, currentOrder;
        int got;
        char* p = line;

        while (*p == ' ' || *p == '\t')
        {
            p++;
        }
        if (*p == '\0' || *p == '\n' || *p == '\r' || *p == '#')
        {
            continue;
        }

        got = sscanf(p, "%d %d %d %d", &id, &position, &status, &currentOrder);
        if (got < 3)
        {
            printf("Skip invalid vehicle line: %s", line);
            continue;
        }
        if (got < 4) currentOrder = -1;

        /* 合法性校验 */
        if (status != VEHICLE_IDLE && status != VEHICLE_DELIVERING)
        {
            status = VEHICLE_IDLE;
        }
        if (status == VEHICLE_IDLE)
        {
            currentOrder = -1;
        }

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


//保存车辆
int SaveVehicles(const char* filename, Vehicle vehicles[], int count)
{
    FILE* fp;
    int i;

    if (filename == NULL || vehicles == NULL || count < 0)
    {
        return 0;
    }

    fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("Cannot write %s\n", filename);
        return 0;
    }

    fprintf(fp, "# id position status currentOrder\n");

    for (i = 0; i < count; i++)
    {
        fprintf(fp, "%d %d %d %d\n",
            vehicles[i].id,
            vehicles[i].position,
            vehicles[i].status,
            vehicles[i].currentOrder);
    }

    fclose(fp);
    printf("Saved %d vehicles\n", count);
    return 1;
}
