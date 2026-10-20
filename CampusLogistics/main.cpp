#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <graphics.h>

#include "common.h"
#include "graph.h"
#include "mapdata.h"
#include "order.h"
#include "vehicle.h"
#include "scheduler.h"
#include "path.h"
#include "simulation.h"
#include "gui.h"
#include "stats.h"

// 全局应用状态
typedef struct
{
    Graph g;                            /* 校园地图 */
    Order orders[MAX_ORDERS];
    int orderCount;
    Vehicle vehicles[MAX_VEHICLES];
    int vehicleCount;
    ActiveDelivery deliveries[MAX_VEHICLES];

    int selectedOrderId;                /* 当前选中的订单 */
    int selectedVehicleId;              /* 当前选中的车辆 */
    int nextOrderId;                    /* 下一个可用订单号 */
    int nextVehicleId;                  /* 下一个可用车辆号 */

    int dispatchRunning;                /* 调度是否在运行 */
    int showRoadInfo;                   /* 是否显示道路长度 */

    Path previewPath;                   /* 选中订单的预览路径 */
    int hasPreview;
} App;


//计算下一个可用编号
static int NextOrderId(Order orders[], int count)
{
    int i;
    int maxId = 0;

    for (i = 0; i < count; i++)
    {
        if (orders[i].id > maxId)
        {
            maxId = orders[i].id;
        }
    }
    return maxId + 1;
}
static int NextVehicleId(Vehicle vehicles[], int count)
{
    int i;
    int maxId = 0;

    for (i = 0; i < count; i++)
    {
        if (vehicles[i].id > maxId)
        {
            maxId = vehicles[i].id;
        }
    }
    return maxId + 1;
}

/* 丢弃鼠标消息队列 */
static void FlushMouseQueue(void)
{
    ExMessage m;

    while (peekmessage(&m, EX_MOUSE))
    {
        /* 只丢弃，不处理 */
    }
}

// 载入全部数据
static int LoadAll(App* app)
{
    int ok = 1;

    InitGraph(&app->g);
    initOrders(app->orders, &app->orderCount);
    initVehicles(app->vehicles, &app->vehicleCount);
    initDeliveries(app->deliveries, MAX_VEHICLES);

    if (!LoadLocations(&app->g, "locations.txt"))
    {
        AddLog("[错误] locations.txt 打开失败");
        ok = 0;
    }
    if (!LoadRoads(&app->g, "roads.txt"))
    {
        AddLog("[错误] roads.txt 打开失败");
        ok = 0;
    }
    if (!LoadOrders("orders.txt", app->orders, &app->orderCount))
    {
        AddLog("[错误] orders.txt 打开失败");
        ok = 0;
    }
    if (!LoadVehicles("vehicles.txt", app->vehicles, &app->vehicleCount))
    {
        AddLog("[错误] vehicles.txt 打开失败");
        ok = 0;
    }

    app->nextOrderId = NextOrderId(app->orders, app->orderCount);
    app->nextVehicleId = NextVehicleId(app->vehicles, app->vehicleCount);
    app->selectedOrderId = -1;
    app->selectedVehicleId = -1;
    app->dispatchRunning = 0;
    app->hasPreview = 0;

    return ok;
}

//重置系统
static void DoReset(App* app)
{
    FreeGraph(&app->g);       /* 先把旧的邻接表内存释放掉，否则会泄漏 */
	ClearLogs();             /* 清空日志 */
	LoadAll(app);            /* 重新载入数据 */
    AddLog("[系统] 已重置，数据重新载入");
}

// 新建订单（带完整合法性校验）
static void DoAddOrder(App* app)
{
    char buf[64];
    char log[160];
    int pickup;
    int delivery;
    int priority;
    Order o;

    if (app->orderCount >= MAX_ORDERS)
    {
        AddLog("[错误] 订单数量已达上限");
        return;
    }

    strcpy(buf, "0");
    if (!InputText("新建订单 (1/3)", "请输入【取货地点 ID】：",
        buf, sizeof(buf)))
    {
        return;
    }
    pickup = atoi(buf);

    strcpy(buf, "5");
    if (!InputText("新建订单 (2/3)", "请输入【配送地点 ID】：",
        buf, sizeof(buf)))
    {
        return;
    }
    delivery = atoi(buf);

    strcpy(buf, "2");
    if (!InputText("新建订单 (3/3)", "请输入【优先级】1-3（越大越优先）：",
        buf, sizeof(buf)))
    {
        return;
    }
    priority = atoi(buf);

    /*  合法性校验  */
    if (FindLocationIndexById(&app->g, pickup) < 0)
    {
        AddLog("[错误] 取货地点不存在，订单未创建");
        return;
    }
    if (FindLocationIndexById(&app->g, delivery) < 0)
    {
        AddLog("[错误] 配送地点不存在，订单未创建");
        return;
    }
    if (pickup == delivery)
    {
        AddLog("[错误] 取货点与配送点不能相同");
        return;
    }
    if (priority < MIN_PRIORITY || priority > MAX_PRIORITY)
    {
        AddLog("[错误] 优先级必须在 1-3 之间");
        return;
    }

    o.id = app->nextOrderId;
    o.pickup = pickup;
    o.delivery = delivery;
    o.priority = priority;
    o.status = ORDER_WAITING;
    o.createTime = 0;
    o.distance = 0;

    if (addOrder(app->orders, &app->orderCount, o))
    {
        sprintf(log, "[订单] 新建成功：订单%d  %d -> %d  优先级P%d",
            o.id, pickup, delivery, priority);
        AddLog(log);
        app->nextOrderId++;
    }
    else
    {
        AddLog("[错误] 新建订单失败");
    }
}

// 新建车辆
static void DoAddVehicle(App* app)
{
    char buf[64];
    char log[128];
    int position;
    Vehicle v;

    if (app->vehicleCount >= MAX_VEHICLES)
    {
        AddLog("[错误] 车辆数量已达上限");
        return;
    }

    strcpy(buf, "0");
    if (!InputText("新建车辆", "请输入车辆【初始位置】的地点 ID：",
        buf, sizeof(buf)))
    {
        return;
    }
    position = atoi(buf);

    if (FindLocationIndexById(&app->g, position) < 0)
    {
        AddLog("[错误] 该地点不存在，车辆未创建");
        return;
    }

    v.id = app->nextVehicleId;
    v.position = position;
    v.status = VEHICLE_IDLE;
    v.currentOrder = -1;

    if (addVehicle(app->vehicles, &app->vehicleCount, v))
    {
        sprintf(log, "[车辆] 新建成功：车辆%d 初始位置 地点%d",
            v.id, position);
        AddLog(log);
        app->nextVehicleId++;
    }
    else
    {
        AddLog("[错误] 新建车辆失败");
    }
}

// 删除车辆
static void DoDeleteVehicle(App* app)
{
    int i;
    int idx = -1;
    Vehicle* v;

    if (app->selectedVehicleId < 0)
    {
        AddLog("[提示] 请先在右侧【车辆列表】中点击选中一辆车");
        return;
    }

    v = findVehicle(app->vehicles, app->vehicleCount, app->selectedVehicleId);
    if (v == NULL)
    {
        AddLog("[错误] 未找到该车辆");
        return;
    }
    if (v->status != VEHICLE_IDLE)
    {
        AddLog("[错误] 配送中的车辆不能删除");
        return;
    }

    for (i = 0; i < app->vehicleCount; i++)
    {
        if (app->vehicles[i].id == app->selectedVehicleId)
        {
            idx = i;
            break;
        }
    }

    if (idx < 0)
    {
        AddLog("[错误] 未找到该车辆");
        return;
    }

    if (deleteVehicle(app->vehicles, &app->vehicleCount,
        app->selectedVehicleId))
    {
        /* 配送任务数组同步前移 */
        for (i = idx; i < MAX_VEHICLES - 1; i++)
        {
            app->deliveries[i] = app->deliveries[i + 1];
        }
        initDeliveries(&app->deliveries[MAX_VEHICLES - 1], 1);

        AddLog("[车辆] 删除成功");
        app->selectedVehicleId = -1;
    }
    else
    {
        AddLog("[错误] 删除车辆失败");
    }
}

//保存全部数据
static void DoSaveAll(App* app)
{
    int ok = 1;

    if (!SaveLocations(&app->g, "locations.txt")) ok = 0;
    if (!SaveRoads(&app->g, "roads.txt"))         ok = 0;
    if (!SaveOrders("orders.txt", app->orders, app->orderCount)) ok = 0;
    if (!SaveVehicles("vehicles.txt", app->vehicles, app->vehicleCount)) ok = 0;

    if (ok)
    {
        AddLog("[保存] 已写入 locations / roads / orders / vehicles");
    }
    else
    {
        AddLog("[错误] 保存失败，请确认 txt 未被其它程序占用");
    }
}

//地图编辑：添加地点 / 添加道路 / 删除道路 / 修改距离
static int CanEditMap(App* app)
{
    if (app->dispatchRunning)
    {
        AddLog("[提示] 请先【暂停调度】再编辑地图");
        return 0;
    }

    app->hasPreview = 0;
    app->selectedOrderId = -1;
    return 1;
}
//添加地点
static void DoAddLocation(App* app)
{
    char buf[64];
    char name[MAX_NAME_LEN];
    char log[160];
    int id;
    int x;
    int y;

    if (!CanEditMap(app))
    {
        return;
    }

    strcpy(buf, "9");
    if (!InputText("添加地点 (1/4)", "请输入地点【ID】（数字）：",
        buf, sizeof(buf)))
    {
        return;
    }
    id = atoi(buf);

    strcpy(buf, "NewSpot");
    if (!InputText("添加地点 (2/4)", "请输入地点【名称】（英文或拼音）：",
        buf, sizeof(buf)))
    {
        return;
    }
    strncpy(name, buf, MAX_NAME_LEN - 1);
    name[MAX_NAME_LEN - 1] = '\0';

    strcpy(buf, "400");
    if (!InputText("添加地点 (3/4)", "请输入【X 坐标】0-720：",
        buf, sizeof(buf)))
    {
        return;
    }
    x = atoi(buf);

    strcpy(buf, "300");
    if (!InputText("添加地点 (4/4)", "请输入【Y 坐标】0-510：",
        buf, sizeof(buf)))
    {
        return;
    }
    y = atoi(buf);

    if (AddLocation(&app->g, id, name, x, y) < 0)
    {
        AddLog("[错误] 添加地点失败：ID 重复 / 坐标越界 / 已达容量");
        return;
    }

    sprintf(log, "[地图] 新增地点  %d %s (%d,%d)", id, name, x, y);
    AddLog(log);
}

//修改地点
static void DoEditLocation(App* app)
{
    char buf[64];
    char name[MAX_NAME_LEN];
    char log[192];
    int id;
    int x;
    int y;
    int idx;

    if (!CanEditMap(app))
    {
        return;
    }

    strcpy(buf, "0");
    if (!InputText("修改地点 (1/4)", "要修改的【地点 ID】：",
        buf, sizeof(buf)))
    {
        return;
    }
    id = atoi(buf);

    idx = FindLocationIndexById(&app->g, id);
    if (idx < 0)
    {
        AddLog("[错误] 该地点不存在");
        return;
    }

    /* 预填当前名称 */
    strncpy(name, app->g.locations[idx].name, MAX_NAME_LEN - 1);
    name[MAX_NAME_LEN - 1] = '\0';
    if (!InputText("修改地点 (2/4)", "新的【名称】（英文或拼音）：",
        name, sizeof(name)))
    {
        return;
    }

    /* 预填当前 X 坐标 */
    sprintf(buf, "%d", app->g.locations[idx].x);
    if (!InputText("修改地点 (3/4)", "新的【X 坐标】0-690：",
        buf, sizeof(buf)))
    {
        return;
    }
    x = atoi(buf);

    /* 预填当前 Y 坐标 */
    sprintf(buf, "%d", app->g.locations[idx].y);
    if (!InputText("修改地点 (4/4)", "新的【Y 坐标】0-510：",
        buf, sizeof(buf)))
    {
        return;
    }
    y = atoi(buf);

    if (UpdateLocation(&app->g, id, name, x, y))
    {
        sprintf(log, "[地图] 地点%d 已修改为  %s (%d,%d)", id, name, x, y);
        AddLog(log);
    }
    else
    {
        AddLog("[错误] 修改失败：名称为空、名称过长或坐标越界");
    }
}

// 添加道路 / 删除道路 / 修改距离
static void DoAddRoad(App* app)
{
    char buf[64];
    char log[160];
    int from;
    int to;
    int dist;

    if (!CanEditMap(app))
    {
        return;
    }

    strcpy(buf, "0");
    if (!InputText("添加道路 (1/3)", "起点地点 ID：", buf, sizeof(buf)))
    {
        return;
    }
    from = atoi(buf);

    strcpy(buf, "1");
    if (!InputText("添加道路 (2/3)", "终点地点 ID：", buf, sizeof(buf)))
    {
        return;
    }
    to = atoi(buf);

    strcpy(buf, "100");
    if (!InputText("添加道路 (3/3)", "道路长度 1-10000：", buf, sizeof(buf)))
    {
        return;
    }
    dist = atoi(buf);

    /* ---- 合法性校验 ---- */
    if (from == to)
    {
        AddLog("[错误] 起点和终点不能相同");
        return;
    }
    if (FindLocationIndexById(&app->g, from) < 0)
    {
        AddLog("[错误] 起点地点不存在");
        return;
    }
    if (FindLocationIndexById(&app->g, to) < 0)
    {
        AddLog("[错误] 终点地点不存在");
        return;
    }
    if (dist < MIN_ROAD_DISTANCE || dist > MAX_ROAD_DISTANCE)
    {
        AddLog("[错误] 道路长度越界");
        return;
    }

    if (AddRoad(&app->g, from, to, dist))
    {
        sprintf(log, "[地图] 新增道路  %d - %d  长度 %d", from, to, dist);
        AddLog(log);
    }
    else
    {
        AddLog("[错误] 添加道路失败：两点间道路已存在");
    }
}
// 删除道路
static void DoDeleteRoad(App* app)
{
    char buf[64];
    char log[160];
    int from;
    int to;

    if (!CanEditMap(app))
    {
        return;
    }

    strcpy(buf, "0");
    if (!InputText("删除道路 (1/2)", "起点地点 ID：", buf, sizeof(buf)))
    {
        return;
    }
    from = atoi(buf);

    strcpy(buf, "1");
    if (!InputText("删除道路 (2/2)", "终点地点 ID：", buf, sizeof(buf)))
    {
        return;
    }
    to = atoi(buf);

    if (RemoveRoad(&app->g, from, to))
    {
        sprintf(log, "[地图] 已删除道路  %d - %d", from, to);
        AddLog(log);
    }
    else
    {
        AddLog("[错误] 删除道路失败：地点不存在");
    }
}
// 修改道路长度
static void DoEditRoadDistance(App* app)
{
    char buf[64];
    char log[160];
    int from;
    int to;
    int dist;
    int oldDist = 0;

    if (!CanEditMap(app))
    {
        return;
    }

    strcpy(buf, "0");
    if (!InputText("修改距离 (1/3)", "起点地点 ID：", buf, sizeof(buf)))
    {
        return;
    }
    from = atoi(buf);

    strcpy(buf, "1");
    if (!InputText("修改距离 (2/3)", "终点地点 ID：", buf, sizeof(buf)))
    {
        return;
    }
    to = atoi(buf);

    if (!GetRoadDistance(&app->g, from, to, &oldDist))
    {
        AddLog("[错误] 这两点之间没有道路");
        return;
    }

    sprintf(buf, "%d", oldDist);
    if (!InputText("修改距离 (3/3)", "新的道路长度 1-10000：",
        buf, sizeof(buf)))
    {
        return;
    }
    dist = atoi(buf);

    if (UpdateRoadDistance(&app->g, from, to, dist))
    {
        sprintf(log, "[地图] 道路 %d - %d 长度：%d -> %d",
            from, to, oldDist, dist);
        AddLog(log);
    }
    else
    {
        AddLog("[错误] 修改失败：地点不存在或长度越界");
    }
}
//选中订单 → 计算并预览最短路径
static void SelectOrder(App* app, int index)
{
    Order* o;
    int pIdx;
    int dIdx;
    char log[160];

    if (index < 0 || index >= app->orderCount)
    {
        return;
    }

    o = &app->orders[index];
    app->selectedOrderId = o->id;

    pIdx = FindLocationIndexById(&app->g, o->pickup);
    dIdx = FindLocationIndexById(&app->g, o->delivery);

    if (pIdx < 0 || dIdx < 0)
    {
        app->hasPreview = 0;
        AddLog("[错误] 订单中的地点不存在，无法计算路径");
        return;
    }

    app->hasPreview = dijkstra(&app->g, pIdx, dIdx, &app->previewPath);

    if (app->hasPreview)
    {
        sprintf(log, "[路径] 订单%d： %d -> %d   最短距离 %d",
            o->id, o->pickup, o->delivery, app->previewPath.distance);
        AddLog(log);
    }
    else
    {
        AddLog("[路径] 两点之间不连通，无可用路径");
    }
}

// 鼠标点击分发
static void HandleClick(App* app, int mx, int my)
{
    int btn;

    /* 日志栏的"清空"小按钮 */
    if (HitLogClear(mx, my))
    {
        ClearLogs();
        AddLog("[系统] 日志已清空");
        return;
    }

    btn = HitButton(mx, my);

    if (btn == BTN_ADD_ORDER)
    {
        DoAddOrder(app);
    }
    else if (btn == BTN_ADD_VEHICLE)
    {
        DoAddVehicle(app);
    }
    else if (btn == BTN_DEL_VEHICLE)
    {
        DoDeleteVehicle(app);
    }
    else if (btn == BTN_ROAD_INFO)
    {
        app->showRoadInfo = !app->showRoadInfo;
        AddLog(app->showRoadInfo ? "[地图] 已显示道路长度"
            : "[地图] 已隐藏道路长度");
    }
    else if (btn == BTN_SAVE)
    {
        DoSaveAll(app);
    }
    else if (btn == BTN_RESET)
    {
        DoReset(app);
    }
    else if (btn == BTN_DISPATCH)
    {
        if (!app->dispatchRunning)
        {
            app->dispatchRunning = 1;
            AddLog("[调度] 调度已启动");

            if (assignWaitingOrders(&app->g, app->orders, app->orderCount,
                app->vehicles, app->vehicleCount, app->deliveries) == 0)
            {
                AddLog("[调度] 当前没有可分配的订单或空闲车辆");
            }
        }
        else
        {
            AddLog("[提示] 调度已在运行中");
        }
    }
    else if (btn == BTN_PAUSE)
    {
        if (app->dispatchRunning)
        {
            app->dispatchRunning = 0;
            AddLog("[调度] 调度已暂停");
        }
    }
    else if (btn == BTN_ADD_LOC)
    {
        DoAddLocation(app);
    }
    else if (btn == BTN_EDIT_LOC)
    {
        DoEditLocation(app);
    }
    else if (btn == BTN_ADD_ROAD)
    {
        DoAddRoad(app);
    }
    else if (btn == BTN_DEL_ROAD)
    {
        DoDeleteRoad(app);
    }
    else if (btn == BTN_EDIT_ROAD)
    {
        DoEditRoadDistance(app);
    }
    else if (mx >= PANEL_X + 10 && mx <= PANEL_X + PANEL_W - 10)
    {
        /* 订单列表 */
        if (my >= ORDER_ROW_Y &&
            my < ORDER_ROW_Y + ORDER_ROW_MAX * ORDER_ROW_H)
        {
            int idx = (my - ORDER_ROW_Y) / ORDER_ROW_H;
            SelectOrder(app, idx);
        }
        /* 车辆列表 */
        else if (my >= VEH_ROW_Y &&
            my < VEH_ROW_Y + VEH_ROW_MAX * VEH_ROW_H)
        {
            int idx = (my - VEH_ROW_Y) / VEH_ROW_H;

            if (idx >= 0 && idx < app->vehicleCount)
            {
                app->selectedVehicleId = app->vehicles[idx].id;
                AddLog("[选择] 已选中车辆");
            }
        }
    }
}

//主函数
int main(void)
{
    static App app;             /* 静态存储，避免占用栈空间 */
    ExMessage msg;
    int mouseX = -1;
    int mouseY = -1;
    int running = 1;            /* 主循环开关，按 ESC 会置 0 */

    if (!LoadAll(&app))
    {
        printf("数据文件加载失败：请确认 txt 文件与程序在同一目录\n");
        return 1;
    }

    InitGUI();
    AddLog("[系统] 数据加载完成，点击【开始调度】运行配送模拟");
    
    while (running)
    {
        //逻辑更新 
        if (app.dispatchRunning)
        {
            /* 先推进已有任务，再给新空闲的车分配订单 */
            updateDeliveries(&app.g, app.orders, app.vehicles, app.deliveries);
            assignWaitingOrders(&app.g, app.orders, app.orderCount,
                app.vehicles, app.vehicleCount, app.deliveries);
        }

        //绘制
        DrawBackground();

        /* 地图面板：面板 → 道路 → 道路长度 → 预览路径 → 配送路线 */
        DrawMapPanel(&app.g, app.deliveries, MAX_VEHICLES,
            &app.previewPath, app.hasPreview, app.showRoadInfo);

        /* 车辆图标画在最上层，不会被路线盖住 */
        DrawVehicleMarkers(&app.g, app.vehicles, app.vehicleCount,
            app.deliveries);

        DrawOrderPanel(app.orders, app.orderCount, app.selectedOrderId);
        DrawVehiclePanel(app.vehicles, app.vehicleCount, app.selectedVehicleId);
        DrawStatsPanel(&app.g, app.orders, app.orderCount,
            app.vehicles, app.vehicleCount);
        DrawLegendPanel();
        DrawLogPanel();
        DrawButtons(mouseX, mouseY, app.dispatchRunning, app.showRoadInfo);

        FlushBatchDraw();
        Sleep(20);

        //鼠标事件
        while (peekmessage(&msg, EX_MOUSE))
        {
            mouseX = msg.x;
            mouseY = msg.y;

            if (msg.message == WM_LBUTTONDOWN)
            {
                HandleClick(&app, msg.x, msg.y);
                FlushMouseQueue();
            }
        }
        //键盘：ESC 退出 
        while (peekmessage(&msg, EX_KEY))
        {
            if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE)
            {
                running = 0;
            }
        }
    }

    CloseGUI();
    FreeGraph(&app.g);
    return 0;
}
