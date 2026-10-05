
#ifndef COMMON_H
#define COMMON_H

// 系统容量
#define MAX_LOCATIONS 50
#define MAX_VEHICLES 10
#define MAX_ORDERS 100
#define MAX_ROUTE_LEN 50

//订单状态
#define ORDER_WAITING 0
#define ORDER_DELIVERING 1
#define ORDER_COMPLETED 2

//车辆状态
#define VEHICLE_IDLE 0
#define VEHICLE_DELIVERING 1 

//边界检查
#define MIN_LOCATION_ID     0
#define MAX_LOCATION_ID     9999

#define MIN_COORD           0
#define MAX_COORD_X         760
#define MAX_COORD_Y         700

#define MIN_ROAD_DISTANCE   1
#define MAX_ROAD_DISTANCE   10000

#define MAX_NAME_LEN        50
//无穷大
#define INF 99999999

#endif // !COMMON_H
