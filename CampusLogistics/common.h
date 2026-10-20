#ifndef COMMON_H
#define COMMON_H
#include <stddef.h>

  /*全局常量：容量、状态、数据边界*/

 //系统容量
#define MAX_LOCATIONS      50      /* 最多地点数 */
#define MAX_VEHICLES       10      /* 最多车辆数 */
#define MAX_ORDERS         100     /* 最多订单数 */
#define MAX_ROUTE_LEN      50      /* 单段路径最长节点数 */

// 订单状态 
#define ORDER_WAITING      0
#define ORDER_DELIVERING   1
#define ORDER_COMPLETED    2

//车辆状态 
#define VEHICLE_IDLE       0
#define VEHICLE_DELIVERING 1

// 订单优先级范围 
#define MIN_PRIORITY       1
#define MAX_PRIORITY       3

// 数据合法性边界 
#define MIN_LOCATION_ID    0
#define MAX_LOCATION_ID    9999

#define MIN_COORD          0
#define MAX_COORD_X        690
#define MAX_COORD_Y        510

#define MIN_ROAD_DISTANCE  1
#define MAX_ROAD_DISTANCE  10000

#define MAX_NAME_LEN       50      

 // 日志容量 
#define MAX_LOGS           200     /* 最多保留多少条日志 */
#define LOG_TEXT_LEN       192     /* 单条日志最大字符数 */

//配送模拟：平滑动画 
 //调小 = 更慢 调大 = 更快
#define DELIVERY_SPEED      150.0  /* 车辆移动速度 */

//无穷大 
#define INF                99999999

#endif 
