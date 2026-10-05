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
    if (*count >= MAX_VEHICLES)
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
