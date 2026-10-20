#define _CRT_SECURE_NO_WARNINGS

#include <graphics.h>
#include <stdio.h>
#include <string.h>
#include "gui.h"

 //日志存储
static char g_logs[MAX_LOGS][LOG_TEXT_LEN];
static int  g_logCount = 0;

 //按钮表
typedef struct
{
    int x;
    int y;
    int w;
    int h;
    const char* text;
    COLORREF color;
} ButtonDef;

static ButtonDef g_btn[BTN_COUNT] =
{
    /* ---- 第一行：数据与调度 ---- */
    { BTN_START_X + 0 * (BTN_W + BTN_GAP), BTN_ROW1_Y, BTN_W, BTN_H,
      "新建订单", RGB(37, 99, 235) },
    { BTN_START_X + 1 * (BTN_W + BTN_GAP), BTN_ROW1_Y, BTN_W, BTN_H,
      "新建车辆", RGB(37, 99, 235) },
    { BTN_START_X + 2 * (BTN_W + BTN_GAP), BTN_ROW1_Y, BTN_W, BTN_H,
      "删除车辆", RGB(146, 64, 14) },
    { BTN_START_X + 3 * (BTN_W + BTN_GAP), BTN_ROW1_Y, BTN_W, BTN_H,
      "开始调度", RGB(5, 150, 105) },
    { BTN_START_X + 4 * (BTN_W + BTN_GAP), BTN_ROW1_Y, BTN_W, BTN_H,
      "暂停调度", RGB(51, 65, 85) },
    { BTN_START_X + 5 * (BTN_W + BTN_GAP), BTN_ROW1_Y, BTN_W, BTN_H,
      "保存数据", RGB(6, 95, 70) },
    { BTN_START_X + 6 * (BTN_W + BTN_GAP), BTN_ROW1_Y, BTN_W, BTN_H,
      "重置系统", RGB(146, 64, 14) },

      /* ---- 第二行：地图编辑 ---- */
      { BTN_START_X + 0 * (BTN_W + BTN_GAP), BTN_ROW2_Y, BTN_W, BTN_H,
        "道路信息", RGB(51, 65, 85) },
      { BTN_START_X + 1 * (BTN_W + BTN_GAP), BTN_ROW2_Y, BTN_W, BTN_H,
        "添加地点", RGB(51, 65, 85) },
      { BTN_START_X + 2 * (BTN_W + BTN_GAP), BTN_ROW2_Y, BTN_W, BTN_H,
        "修改地点", RGB(51, 65, 85) },
      { BTN_START_X + 3 * (BTN_W + BTN_GAP), BTN_ROW2_Y, BTN_W, BTN_H,
        "添加道路", RGB(51, 65, 85) },
      { BTN_START_X + 4 * (BTN_W + BTN_GAP), BTN_ROW2_Y, BTN_W, BTN_H,
        "修改距离", RGB(51, 65, 85) },
      { BTN_START_X + 5 * (BTN_W + BTN_GAP), BTN_ROW2_Y, BTN_W, BTN_H,
        "删除道路", RGB(146, 64, 14) }
};

 // 通用绘制小工具
 /* 画文字：size 是字号，实际使用的是 size + FONT_BOOST */
void DrawTextXY(int x, int y, const char* text, COLORREF color, int size)
{
    if (text == NULL)
    {
        return;
    }
    setbkmode(TRANSPARENT);
    settextstyle(size + FONT_BOOST, 0, (char*)"微软雅黑");
    settextcolor(color);
    outtextxy(x, y, (char*)text);
}

/* 在宽度为 w 的区域内水平居中画文字 */
void DrawTextCenter(int x, int y, int w, const char* text,
    COLORREF color, int size)
{
    int tw;

    if (text == NULL)
    {
        return;
    }
    setbkmode(TRANSPARENT);
    settextstyle(size + FONT_BOOST, 0, (char*)"微软雅黑");
    settextcolor(color);
    tw = textwidth((char*)text);
    outtextxy(x + (w - tw) / 2, y, (char*)text);
}

/* 带阴影的圆角面板 */
void PanelFill(int x, int y, int w, int h, COLORREF fill, COLORREF border)
{
    /* 阴影：向右下偏移 3 像素 */
    setfillcolor(RGB(8, 12, 20));
    setlinecolor(RGB(8, 12, 20));
    solidroundrect(x + 3, y + 3, x + w + 3, y + h + 3, 12, 12);

    /* 面板本体 */
    setfillcolor(fill);
    setlinecolor(border);
    solidroundrect(x, y, x + w, y + h, 12, 12);
}

/* 面板标题：左侧一根竖条 + 标题文字 */
void PanelTitle(int x, int y, const char* title)
{
    setfillcolor(C_ACCENT);
    setlinecolor(C_ACCENT);
    solidrectangle(x, y + 3, x + 4, y + 19);

    DrawTextXY(x + 12, y, title, C_TEXT, 17);
}

// 初始化 / 关闭
void InitGUI(void)
{
    initgraph(WIN_W, WIN_H);
    SetWindowText(GetHWnd(), "校园智能车辆调度与物流配送规划系统");
    BeginBatchDraw();

    setbkcolor(C_BG);
    cleardevice();
    FlushBatchDraw();
}

void CloseGUI(void)
{
    EndBatchDraw();
    closegraph();
}

 // 窗口底色 + 顶部标题栏
void DrawBackground(void)
{
    setbkcolor(C_BG);
    cleardevice();

    /* 顶栏底色 */
    setfillcolor(RGB(22, 30, 45));
    setlinecolor(RGB(22, 30, 45));
    solidrectangle(0, 0, WIN_W, 48);

    /* 顶栏左侧强调条 */
    setfillcolor(C_ACCENT);
    setlinecolor(C_ACCENT);
    solidrectangle(0, 0, 5, 48);

    DrawTextXY(22, 12, "校园智能车辆调度与物流配送规划系统", C_TEXT, 22);

    /* 顶栏右侧一条分割线 + 副标题 */
    setlinecolor(C_BORDER);
    line(770, 12, 770, 36);
    DrawTextXY(786, 16, "课程设计", C_DIM, 14);
}


//画地图：道路 + 地点
void DrawMap(Graph* g)
{
    int i;

    if (g == NULL)
    {
        return;
    }

    /* ---- 先画道路（细线，在最底层） ---- */
    setlinestyle(PS_SOLID, 2);
    setlinecolor(C_ROAD);

    for (i = 0; i < g->locationCount; i++)
    {
        EdgeNode* p;

        if (!g->locations[i].valid)
        {
            continue;
        }

        p = g->adj[i];
        while (p != NULL)
        {
            /* 每条边只画一次 */
            if (p->to > i && g->locations[p->to].valid)
            {
                line(MAP_OX + g->locations[i].x,
                    MAP_OY + g->locations[i].y,
                    MAP_OX + g->locations[p->to].x,
                    MAP_OY + g->locations[p->to].y);
            }
            p = p->next;
        }
    }
    setlinestyle(PS_SOLID, 1);

    /* ---- 再画地点（圆点 + 名称，在上层） ---- */
    for (i = 0; i < g->locationCount; i++)
    {
        int x, y;

        if (!g->locations[i].valid)
        {
            continue;
        }

        x = MAP_OX + g->locations[i].x;
        y = MAP_OY + g->locations[i].y;

        // 外圈：
        setfillcolor(RGB(48, 42, 12));
        setlinecolor(C_NODE);
        fillcircle(x, y, 12);

        //内芯
        setfillcolor(C_NODE);
        setlinecolor(C_NODE);
        fillcircle(x, y, 6);

        DrawTextXY(x + 15, y - 9, g->locations[i].name, C_TEXT, 15);
    }
}

 // 道路长度标注
void DrawRoadInfo(Graph* g)
{
    int i;
    char buf[32];

    if (g == NULL)
    {
        return;
    }

    for (i = 0; i < g->locationCount; i++)
    {
        EdgeNode* p;

        if (!g->locations[i].valid)
        {
            continue;
        }

        p = g->adj[i];
        while (p != NULL)
        {
            if (p->to > i && g->locations[p->to].valid)
            {
                int mx = MAP_OX +
                    (g->locations[i].x + g->locations[p->to].x) / 2;
                int my = MAP_OY +
                    (g->locations[i].y + g->locations[p->to].y) / 2;

                sprintf(buf, "%d", p->distance);

                /* 先用底色盖住线（底衬比字宽一点，字号变大也放得下） */
                setfillcolor(C_MAPBG);
                setlinecolor(C_MAPBG);
                fillrectangle(mx - 18, my - 9, mx + 18, my + 9);

                setbkmode(TRANSPARENT);
                settextstyle(14 + FONT_BOOST, 0, (char*)"微软雅黑");
                settextcolor(RGB(150, 190, 230));
                outtextxy(mx - 14, my - 8, buf);
            }
            p = p->next;
        }
    }
}

// 画"选中订单"的预览路径（红色加粗）
void DrawPath(Graph* g, Path* path)
{
    int i;

    if (g == NULL || path == NULL || path->length < 2)
    {
        return;
    }

    setlinestyle(PS_SOLID, 4);
    setlinecolor(C_BUSY);

    for (i = 0; i < path->length - 1; i++)
    {
        int a = path->nodes[i];
        int b = path->nodes[i + 1];

        if (a < 0 || a >= g->locationCount) continue;
        if (b < 0 || b >= g->locationCount) continue;

        line(MAP_OX + g->locations[a].x, MAP_OY + g->locations[a].y,
            MAP_OX + g->locations[b].x, MAP_OY + g->locations[b].y);
    }

    setlinestyle(PS_SOLID, 1);
}


 //画所有正在配送的路线
void DrawDeliveryRoutes(Graph* g, ActiveDelivery deliveries[], int count)
{
    int k, i;

    if (g == NULL || deliveries == NULL)
    {
        return;
    }

    if (count > MAX_VEHICLES)
    {
        count = MAX_VEHICLES;
    }

    for (k = 0; k < count; k++)
    {
        if (!deliveries[k].active)
        {
            continue;
        }
        if (deliveries[k].routeLength < 2)
        {
            continue;
        }

        setlinestyle(PS_SOLID, 4);

        for (i = 0; i < deliveries[k].routeLength - 1; i++)
        {
            int a = deliveries[k].routeNodes[i];
            int b = deliveries[k].routeNodes[i + 1];

            if (a < 0 || a >= g->locationCount) continue;
            if (b < 0 || b >= g->locationCount) continue;

            if (i < deliveries[k].currentStep)
            {
                setlinecolor(C_ROUTEDONE);      /* 已走过 */
            }
            else
            {
                setlinecolor(C_ROUTE);          /* 还没走 */
            }

            line(MAP_OX + g->locations[a].x, MAP_OY + g->locations[a].y,
                MAP_OX + g->locations[b].x, MAP_OY + g->locations[b].y);
        }
    }

    setlinestyle(PS_SOLID, 1);
}

 //画车辆图标
 //用"方块"表示车、"圆点"表示地点
void DrawVehicleMarkers(Graph* g, Vehicle vehicles[], int vehicleCount,
    ActiveDelivery deliveries[])
{
    int i;
    char label[32];

    if (g == NULL || vehicles == NULL)
    {
        return;
    }

    if (vehicleCount > MAX_VEHICLES)
    {
        vehicleCount = MAX_VEHICLES;
    }

    for (i = 0; i < vehicleCount; i++)
    {
        int idx;
        int x, y;
        int moving = 0;
        COLORREF c;

        //正在配送的车
        if (deliveries != NULL &&
            deliveries[i].active && deliveries[i].vehicleIndex == i)
        {
            x = MAP_OX + (int)(deliveries[i].animX + 0.5);
            y = MAP_OY + (int)(deliveries[i].animY + 0.5);
            moving = 1;
        }
        else
        {
            idx = FindLocationIndexById(g, vehicles[i].position);

            if (idx < 0)
            {
                continue;
            }

            x = MAP_OX + g->locations[idx].x;
            y = MAP_OY + g->locations[idx].y;
        }

        /* 停着的车错开一点，避免多辆车停在同一地点完全重叠；
           行驶中的车不错开，老老实实贴着路线走 */
        if (!moving)
        {
            x += (i % 3) * 18 - 18;
            y += (i / 3) * 18 - 18;
        }

        if (vehicles[i].status == VEHICLE_DELIVERING)
        {
            c = C_BUSY;         /* 配送中：红 */
        }
        else
        {
            c = C_OK;           /* 空闲：绿 */
        }

        /* 外圈深色底 + 彩色描边 */
        setfillcolor(RGB(14, 22, 34));
        setlinecolor(c);
        fillcircle(x, y, 11);

        /* 车身方块 */
        setfillcolor(c);
        setlinecolor(c);
        solidrectangle(x - 5, y - 5, x + 5, y + 5);

        sprintf(label, "V%d", vehicles[i].id);
        DrawTextXY(x + 12, y - 8, label, c, 15);
    }
}

 //地图面板总装：面板 → 道路信息 → 预览路径 → 配送路线
void DrawMapPanel(Graph* g, ActiveDelivery deliveries[], int deliveryCount,
    Path* highlight, int hasHighlight, int showRoadInfo)
{
    PanelFill(MAP_PANEL_X, MAP_PANEL_Y, MAP_PANEL_W, MAP_PANEL_H,
        C_MAPBG, C_BORDER);

    DrawMap(g);

    if (showRoadInfo)
    {
        DrawRoadInfo(g);
    }

    if (hasHighlight)
    {
        DrawPath(g, highlight);
    }

    DrawDeliveryRoutes(g, deliveries, deliveryCount);
}


 // 订单列表
static const char* OrderStatusText(int s)
{
    if (s == ORDER_WAITING)    return "等待";
    if (s == ORDER_DELIVERING) return "配送中";
    if (s == ORDER_COMPLETED)  return "已完成";
    return "未知";
}

static COLORREF OrderStatusColor(int s)
{
    if (s == ORDER_WAITING)    return C_DIM;
    if (s == ORDER_DELIVERING) return C_ACCENT;
    if (s == ORDER_COMPLETED)  return C_OK;
    return C_BUSY;
}

void DrawOrderPanel(Order orders[], int count, int selectedOrderId)
{
    char buf[64];
    int i;
    int x = PANEL_X;
    int y = ORDER_PANEL_Y;
    int w = PANEL_W;

    PanelFill(x, y, w, ORDER_PANEL_H, C_PANEL, C_BORDER);
    PanelTitle(x + 16, y + 12, "订单列表");

    sprintf(buf, "共 %d 单", count);
    DrawTextXY(x + w - 96, y + 15, buf, C_DIM, 14);

    /* 表头 */
    DrawTextXY(x + 16, y + 44, "编号", C_DIM, 13);
    DrawTextXY(x + 66, y + 44, "取货 -> 送货", C_DIM, 13);
    DrawTextXY(x + 176, y + 44, "优先级", C_DIM, 13);
    DrawTextXY(x + 252, y + 44, "状态", C_DIM, 13);

    setlinecolor(C_BORDER);
    line(x + 14, y + 64, x + w - 14, y + 64);

    if (orders == NULL || count <= 0)
    {
        DrawTextXY(x + 16, ORDER_ROW_Y + 10, "（暂无订单）", C_DIM, 14);
        return;
    }

    for (i = 0; i < count && i < ORDER_ROW_MAX; i++)
    {
        int ry = ORDER_ROW_Y + i * ORDER_ROW_H;
        int sel = (orders[i].id == selectedOrderId);

        if (sel)
        {
            setfillcolor(C_SEL);
            setlinecolor(C_SEL);
            fillrectangle(x + 10, ry - 3, x + w - 10, ry + 17);
        }

        sprintf(buf, "%d", orders[i].id);
        DrawTextXY(x + 16, ry, buf, sel ? C_ACCENT2 : C_TEXT, 14);

        sprintf(buf, "%d -> %d", orders[i].pickup, orders[i].delivery);
        DrawTextXY(x + 66, ry, buf, sel ? C_ACCENT2 : RGB(190, 202, 220), 14);

        sprintf(buf, "P%d", orders[i].priority);
        DrawTextXY(x + 186, ry, buf, C_ACCENT2, 14);

        DrawTextXY(x + 252, ry, OrderStatusText(orders[i].status),
            OrderStatusColor(orders[i].status), 14);
    }

    if (count > ORDER_ROW_MAX)
    {
        sprintf(buf, "…… 还有 %d 单未显示", count - ORDER_ROW_MAX);
        DrawTextXY(x + 16, ORDER_ROW_Y + ORDER_ROW_MAX * ORDER_ROW_H + 4,
            buf, C_DIM, 13);
    }
}


 //车辆列表
void DrawVehiclePanel(Vehicle vehicles[], int count, int selectedVehicleId)
{
    char buf[64];
    int i;
    int x = PANEL_X;
    int y = VEH_PANEL_Y;
    int w = PANEL_W;

    PanelFill(x, y, w, VEH_PANEL_H, C_PANEL, C_BORDER);
    PanelTitle(x + 16, y + 12, "车辆列表");

    sprintf(buf, "共 %d 辆", count);
    DrawTextXY(x + w - 96, y + 15, buf, C_DIM, 14);

    DrawTextXY(x + 16, y + 44, "车辆", C_DIM, 13);
    DrawTextXY(x + 78, y + 44, "当前位置", C_DIM, 13);
    DrawTextXY(x + 176, y + 44, "状态", C_DIM, 13);
    DrawTextXY(x + 252, y + 44, "当前订单", C_DIM, 13);

    setlinecolor(C_BORDER);
    line(x + 14, y + 64, x + w - 14, y + 64);

    if (vehicles == NULL || count <= 0)
    {
        DrawTextXY(x + 16, VEH_ROW_Y + 10, "（暂无车辆）", C_DIM, 14);
        return;
    }

    for (i = 0; i < count && i < VEH_ROW_MAX; i++)
    {
        int ry = VEH_ROW_Y + i * VEH_ROW_H;
        int sel = (vehicles[i].id == selectedVehicleId);
        int busy = (vehicles[i].status == VEHICLE_DELIVERING);

        if (sel)
        {
            setfillcolor(C_SEL);
            setlinecolor(C_SEL);
            fillrectangle(x + 10, ry - 3, x + w - 10, ry + 15);
        }

        sprintf(buf, "V%d", vehicles[i].id);
        DrawTextXY(x + 16, ry, buf, sel ? C_ACCENT2 : C_TEXT, 14);

        sprintf(buf, "地点 %d", vehicles[i].position);
        DrawTextXY(x + 78, ry, buf, sel ? C_ACCENT2 : RGB(190, 202, 220), 14);

        DrawTextXY(x + 176, ry, busy ? "配送中" : "空闲",
            busy ? C_BUSY : C_OK, 14);

        if (vehicles[i].currentOrder < 0)
        {
            DrawTextXY(x + 262, ry, "-", C_DIM, 14);
        }
        else
        {
            sprintf(buf, "订单%d", vehicles[i].currentOrder);
            DrawTextXY(x + 252, ry, buf, C_ACCENT, 14);
        }
    }
}

//系统监控面板 把配送距离显示出来
void DrawStatsPanel(Graph* g, Order orders[], int orderCount,
    Vehicle vehicles[], int vehicleCount)
{
    Stats st;
    char buf[128];
    int x = PANEL_X;
    int y = STAT_PANEL_Y;
    int ry;

    /* 每帧重算一次，保证数据实时 */
    ComputeStats(orders, orderCount, vehicles, vehicleCount, g, &st);

    PanelFill(x, y, PANEL_W, STAT_PANEL_H, C_PANEL, C_BORDER);
    PanelTitle(x + 16, y + 12, "系统监控");

    /*  订单  */
    ry = y + 40;
    sprintf(buf, "订单  总 %d   等待 %d", st.totalOrders, st.waitingOrders);
    DrawTextXY(x + 18, ry, buf, C_TEXT, 14);

    sprintf(buf, "配送中 %d   完成 %d",
        st.deliveringOrders, st.completedOrders);
    DrawTextXY(x + 196, ry, buf, C_ACCENT, 14);

    /*  车辆  */
    ry += 20;
    sprintf(buf, "车辆  空闲 %d   配送中 %d",
        st.idleVehicles, st.deliveringVehicles);
    DrawTextXY(x + 18, ry, buf, C_TEXT, 14);

    /*  地图状态（连通性判断）  */
    ry += 20;
    sprintf(buf, "地图  地点 %d  道路 %d", st.locationCount, st.roadCount);
    DrawTextXY(x + 18, ry, buf, C_TEXT, 14);
    DrawTextXY(x + 250, ry, st.isConnected ? "连通" : "不连通",
        st.isConnected ? C_OK : C_BUSY, 14);
}

// 图例 / 操作提示
void DrawLegendPanel(void)
{
    int x = PANEL_X;
    int y = LEGEND_PANEL_Y;

    PanelFill(x, y, PANEL_W, LEGEND_PANEL_H, C_PANEL, C_BORDER);
    PanelTitle(x + 16, y + 12, "图例与操作提示");

    /* 第 1 行图例 */
    setfillcolor(C_NODE);
    setlinecolor(C_NODE);
    fillcircle(x + 30, y + 56, 7);
    DrawTextXY(x + 44, y + 48, "校园地点", C_TEXT, 14);

    setfillcolor(C_OK);
    setlinecolor(C_OK);
    solidrectangle(x + 134, y + 50, x + 146, y + 62);
    DrawTextXY(x + 154, y + 48, "空闲车辆", C_TEXT, 14);

    setfillcolor(C_BUSY);
    setlinecolor(C_BUSY);
    solidrectangle(x + 244, y + 50, x + 256, y + 62);
    DrawTextXY(x + 264, y + 48, "配送中", C_TEXT, 14);

    /* 第 2 行图例：线路颜色 */
    setlinestyle(PS_SOLID, 4);
    setlinecolor(C_ROUTE);
    line(x + 24, y + 78, x + 62, y + 78);
    setlinecolor(C_ROUTEDONE);
    line(x + 24, y + 96, x + 62, y + 96);
    setlinecolor(C_BUSY);
    line(x + 190, y + 78, x + 228, y + 78);
    setlinestyle(PS_SOLID, 1);

    DrawTextXY(x + 70, y + 70, "待走路线", C_TEXT, 14);
    DrawTextXY(x + 70, y + 88, "已走路线", C_TEXT, 14);
    DrawTextXY(x + 236, y + 70, "选中订单路径", C_TEXT, 14);

    DrawTextXY(x + 24, y + 104, "提示：点击右侧列表可选中订单 / 车辆", C_DIM, 13);
}


 //日志栏
void DrawLogPanel(void)
{
    int i;
    int start;
    int x = LOG_PANEL_X;
    int y = LOG_PANEL_Y;

    PanelFill(x, y, LOG_PANEL_W, LOG_PANEL_H, C_PANEL, C_BORDER);
    PanelTitle(x + 16, y + 6, "运行日志");

    /* “清空”小按钮 */
    setfillcolor(C_PANEL2);
    setlinecolor(C_BORDER);
    solidroundrect(LOG_CLEAR_X, LOG_CLEAR_Y,
        LOG_CLEAR_X + LOG_CLEAR_W, LOG_CLEAR_Y + LOG_CLEAR_H, 8, 8);
    DrawTextCenter(LOG_CLEAR_X, LOG_CLEAR_Y + 2, LOG_CLEAR_W, "清空",
        C_DIM, 13);

    if (g_logCount <= 0)
    {
        DrawTextXY(x + 20, y + 30, "（暂无日志）", C_DIM, 14);
        return;
    }

    /* 只显示最后 LOG_LINES 条 */
    start = (g_logCount > LOG_LINES) ? (g_logCount - LOG_LINES) : 0;

    for (i = start; i < g_logCount; i++)
    {
        /* 最后一条用亮色，其余用暗色 */
        COLORREF c = (i == g_logCount - 1) ? C_TEXT : C_DIM;
        DrawTextXY(x + 20, y + 30 + (i - start) * LOG_LINE_H, g_logs[i], c, 14);
    }
}

// 按钮
void DrawButtons(int mouseX, int mouseY, int dispatchRunning, int showRoadInfo)
{
    int i;

    for (i = 0; i < BTN_COUNT; i++)
    {
        ButtonDef* b = &g_btn[i];

        int hover = (mouseX >= b->x && mouseX <= b->x + b->w &&
            mouseY >= b->y && mouseY <= b->y + b->h);

        COLORREF fill = b->color;
        COLORREF border = RGB(84, 104, 134);

        /* 状态高亮 */
        if (i == BTN_DISPATCH && dispatchRunning)
        {
            fill = RGB(20, 60, 48);
            border = C_OK;
        }
        if (i == BTN_ROAD_INFO && showRoadInfo)
        {
            fill = RGB(26, 58, 92);
            border = C_ACCENT;
        }

		// 鼠标悬停高亮
        if (hover)
        {
            int r = GetRValue(fill) + 34;
            int gg = GetGValue(fill) + 34;
            int bb = GetBValue(fill) + 34;

            if (r > 255) r = 255;
            if (gg > 255) gg = 255;
            if (bb > 255) bb = 255;

            fill = RGB(r, gg, bb);
            border = C_ACCENT;
        }

        /* 阴影 */
        setfillcolor(RGB(9, 13, 21));
        setlinecolor(RGB(9, 13, 21));
        solidroundrect(b->x + 2, b->y + 3, b->x + b->w + 2,
            b->y + b->h + 3, 10, 10);

        /* 按钮本体 */
        setfillcolor(fill);
        setlinecolor(border);
        solidroundrect(b->x, b->y, b->x + b->w, b->y + b->h, 10, 10);

        DrawTextCenter(b->x, b->y + (b->h - 16) / 2, b->w, b->text,
            C_TEXT, 15);
    }
}

int HitButton(int mx, int my)
{
    int i;

    for (i = 0; i < BTN_COUNT; i++)
    {
        ButtonDef* b = &g_btn[i];

        if (mx >= b->x && mx <= b->x + b->w &&
            my >= b->y && my <= b->y + b->h)
        {
            return i;
        }
    }
    return -1;
}

int HitLogClear(int mx, int my)
{
    if (mx >= LOG_CLEAR_X && mx <= LOG_CLEAR_X + LOG_CLEAR_W &&
        my >= LOG_CLEAR_Y && my <= LOG_CLEAR_Y + LOG_CLEAR_H)
    {
        return 1;
    }
    return 0;
}

// 日志
void AddLog(const char* msg)
{
    int i;

    if (msg == NULL)
    {
        return;
    }

    if (g_logCount < MAX_LOGS)
    {
        strncpy(g_logs[g_logCount], msg, LOG_TEXT_LEN - 1);
        g_logs[g_logCount][LOG_TEXT_LEN - 1] = '\0';
        g_logCount++;
    }
    else
    {
        for (i = 1; i < MAX_LOGS; i++)
        {
            strcpy(g_logs[i - 1], g_logs[i]);
        }
        strncpy(g_logs[MAX_LOGS - 1], msg, LOG_TEXT_LEN - 1);
        g_logs[MAX_LOGS - 1][LOG_TEXT_LEN - 1] = '\0';
    }
}

void ClearLogs(void)
{
    g_logCount = 0;
}

// 简易输入框
#ifndef EX_CHAR
#define EX_CHAR 0x0004
#endif

int InputText(const char* title, const char* prompt, char* out, int maxLen)
{
    int len;
    int boxW = 460;
    int boxH = 180;
    int bx = (WIN_W - boxW) / 2;
    int by = (WIN_H - boxH) / 2;
    int done = 0;
    int result = 0;
    ExMessage msg;

    if (out == NULL || maxLen <= 1)
    {
        return 0;
    }

    len = (int)strlen(out);
    if (len > maxLen - 1)
    {
        len = maxLen - 1;
        out[len] = '\0';
    }

    while (peekmessage(&msg, EX_KEY | EX_CHAR))
    {
        /* 只丢弃 */
    }

    while (!done)
    {
        PanelFill(bx, by, boxW, boxH, C_PANEL2, C_ACCENT);

        DrawTextXY(bx + 24, by + 18, title, C_TEXT, 19);
        DrawTextXY(bx + 24, by + 52, prompt, C_DIM, 15);

        /* 输入框：每次重画会清掉上一次的文字，避免残影 */
        setfillcolor(RGB(14, 20, 30));
        setlinecolor(C_BORDER);
        fillrectangle(bx + 24, by + 84, bx + boxW - 24, by + 118);

        setbkmode(TRANSPARENT);
        settextstyle(18, 0, (char*)"Consolas");
        settextcolor(C_ACCENT2);
        outtextxy(bx + 32, by + 92, out);

        /* 光标 */
        {
            int cw;
            settextstyle(18, 0, (char*)"Consolas");
            cw = textwidth(out);
            setfillcolor(C_ACCENT2);
            setlinecolor(C_ACCENT2);
            fillrectangle(bx + 34 + cw, by + 92, bx + 36 + cw, by + 114);
        }

        DrawTextXY(bx + 24, by + 140, "回车 = 确定    ESC = 取消", C_DIM, 13);

        FlushBatchDraw();

        /*  读键盘  */
        if (!peekmessage(&msg, EX_KEY | EX_CHAR))
        {
            Sleep(10);          
            continue;
        }

        if (msg.message == WM_CHAR)
        {
            char c = (char)msg.ch;

            /* 只接受可打印的 ASCII：数字、字母、空格等
             * 这样会自动忽略退格(8)、回车(13) 这些控制字符 */
            if (c >= 32 && c < 127 && len < maxLen - 1)
            {
                out[len] = c;
                len++;
                out[len] = '\0';
            }
        }
        else if (msg.message == WM_KEYDOWN)
        {
            if (msg.vkcode == VK_RETURN)
            {
                result = 1;
                done = 1;
            }
            else if (msg.vkcode == VK_ESCAPE)
            {
                out[0] = '\0';
                result = 0;
                done = 1;
            }
            else if (msg.vkcode == VK_BACK)
            {
                if (len > 0)
                {
                    len--;
                    out[len] = '\0';
                }
            }
        }
    }

    /* 清掉这次对话框残留的按键 */
    while (peekmessage(&msg, EX_KEY | EX_CHAR))
    {
        /* 只丢弃 */
    }

    return result;
}
