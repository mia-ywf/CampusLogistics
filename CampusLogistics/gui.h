#ifndef GUI_H
#define GUI_H

#include <graphics.h>    

#ifndef EX_KEY
#define EX_KEY  0x0002
#endif
#ifndef EX_CHAR
#define EX_CHAR 0x0004
#endif

#include "common.h"
#include "graph.h"
#include "order.h"
#include "vehicle.h"
#include "path.h"
#include "simulation.h"
#include "stats.h"

 //窗口
#define WIN_W 1200
#define WIN_H 800

// 全局字号增量
#define FONT_BOOST 4

 //布局
 /* 地图面板 */
#define MAP_PANEL_X 20
#define MAP_PANEL_Y 56
#define MAP_PANEL_W 776
#define MAP_PANEL_H 538
#define MAP_OX      (MAP_PANEL_X + 14)   /* 地点坐标的绘制原点偏移 */
#define MAP_OY      (MAP_PANEL_Y + 20)

/* 按钮区：两行，每行 7 个 */
#define BTN_W       100
#define BTN_H       34
#define BTN_ROW1_Y  604
#define BTN_ROW2_Y  644
#define BTN_START_X 30
#define BTN_GAP     10

/* 日志面板 */
#define LOG_PANEL_X 20
#define LOG_PANEL_Y 688
#define LOG_PANEL_W 776
#define LOG_PANEL_H 104
#define LOG_LINES   4
#define LOG_LINE_H  17
#define LOG_CLEAR_X 716
#define LOG_CLEAR_Y 694
#define LOG_CLEAR_W 64
#define LOG_CLEAR_H 20

/* 右侧栏 */
#define PANEL_X 812
#define PANEL_W 372

#define ORDER_PANEL_Y  56
#define ORDER_PANEL_H  250
#define ORDER_ROW_Y    128
#define ORDER_ROW_H    22
#define ORDER_ROW_MAX  8

#define VEH_PANEL_Y    314
#define VEH_PANEL_H    200
#define VEH_ROW_Y      380
#define VEH_ROW_H      20
#define VEH_ROW_MAX    6

#define STAT_PANEL_Y   522
#define STAT_PANEL_H   140

#define LEGEND_PANEL_Y 670
#define LEGEND_PANEL_H 122

//配色
#define C_BG        RGB(16, 22, 34)     /* 窗口底色 */
#define C_PANEL     RGB(26, 35, 51)     /* 面板底色 */
#define C_PANEL2    RGB(36, 47, 66)     /* 面板内的次级块 */
#define C_MAPBG     RGB(18, 25, 38)     /* 地图区底色 */
#define C_BORDER    RGB(58, 76, 104)    /* 面板描边 */
#define C_TEXT      RGB(228, 234, 244)  /* 主文字 */
#define C_DIM       RGB(140, 156, 180)  /* 次要文字 */
#define C_ACCENT    RGB(56, 189, 248)   /* 强调色（青） */
#define C_ACCENT2   RGB(250, 204, 21)   /* 强调色（黄） */
#define C_OK        RGB(52, 211, 153)   /* 空闲/成功 */
#define C_BUSY      RGB(248, 113, 113)  /* 配送中/错误 */
#define C_ROUTE     RGB(96, 165, 250)   /* 待走路线（亮蓝） */
#define C_ROUTEDONE RGB(52, 84, 130)    /* 已走路线（暗蓝） */
#define C_SEL       RGB(51, 70, 100)    /* 列表选中行底色 */
#define C_NODE      RGB(250, 204, 21)   /* 地点圆点 */
#define C_ROAD      RGB(90, 108, 136)   /* 道路线 */

//按钮 ID
enum
{
    /* 数据与调度 */
    BTN_ADD_ORDER = 0,
    BTN_ADD_VEHICLE,
    BTN_DEL_VEHICLE,
    BTN_DISPATCH,
    BTN_PAUSE,
    BTN_SAVE,
    BTN_RESET,

    /* 地图编辑 */
    BTN_ROAD_INFO,
    BTN_ADD_LOC,
    BTN_EDIT_LOC,
    BTN_ADD_ROAD,
    BTN_EDIT_ROAD,
    BTN_DEL_ROAD,

    BTN_COUNT
};

//初始化 / 收尾 
void InitGUI(void);
void CloseGUI(void);

//绘制 
void DrawBackground(void);          /* 窗口底色 + 顶部标题栏 */

void DrawMapPanel(Graph* g, ActiveDelivery deliveries[], int deliveryCount,
    Path* highlight, int hasHighlight, int showRoadInfo);

void DrawMap(Graph* g);             /* 道路 + 地点 */
void DrawRoadInfo(Graph* g);        /* 道路长度标注 */
void DrawPath(Graph* g, Path* path);/* 选中订单的预览路径（红） */
void DrawDeliveryRoutes(Graph* g, ActiveDelivery deliveries[], int count);
//平滑动画
void DrawVehicleMarkers(Graph* g, Vehicle vehicles[], int vehicleCount,
    ActiveDelivery deliveries[]);

void DrawOrderPanel(Order orders[], int count, int selectedOrderId);
void DrawVehiclePanel(Vehicle vehicles[], int count, int selectedVehicleId);
void DrawStatsPanel(Graph* g, Order orders[], int orderCount,
    Vehicle vehicles[], int vehicleCount);
void DrawLegendPanel(void);
void DrawLogPanel(void);

void DrawButtons(int mouseX, int mouseY, int dispatchRunning, int showRoadInfo);
int  HitButton(int mx, int my);
int  HitLogClear(int mx, int my);

// 日志 
void AddLog(const char* msg);
void ClearLogs(void);

// 通用小工具
void DrawTextXY(int x, int y, const char* text, COLORREF color, int size);
void DrawTextCenter(int x, int y, int w, const char* text,
    COLORREF color, int size);
void PanelFill(int x, int y, int w, int h, COLORREF fill, COLORREF border);
void PanelTitle(int x, int y, const char* title);

// 简易输入框  返回值：1 = 确定，0 = 取消
int InputText(const char* title, const char* prompt, char* out, int maxLen);

#endif /* GUI_H */
