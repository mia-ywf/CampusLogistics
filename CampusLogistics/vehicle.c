#include "vehicle.h"
#include <stddef.h>

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