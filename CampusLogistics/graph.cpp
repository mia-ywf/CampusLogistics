#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

// 初始化图
void InitGraph(Graph* g)
{
    int i;
    g->locationCount = 0;
    for (i = 0; i < MAX_LOCATIONS; i++)
    {
        g->locations[i].valid = 0;
        g->adj[i] = NULL;
    }
}

// 按 id 查找地点下标
int FindLocationIndexById(Graph* g, int id)
{
    int i;
    for (i = 0; i < g->locationCount; i++)
    {
        if (g->locations[i].valid && g->locations[i].id == id)
        {
            return i;
        }
    }
    return -1;
}

// 添加地点
int AddLocation(Graph* g, int id, const char* name, int x, int y)
{
    int idx;

    // 校验：容量
    if (g->locationCount >= MAX_LOCATIONS)               return -1;
    // 校验：id 重复
    if (FindLocationIndexById(g, id) != -1)              return -1;
    // 校验：id 范围
    if (id < MIN_LOCATION_ID || id > MAX_LOCATION_ID)    return -1;
    // 校验：坐标范围
    if (x < MIN_COORD || x > MAX_COORD_X ||
        y < MIN_COORD || y > MAX_COORD_Y)                return -1;
    // 校验：名字非空
    if (name == NULL || name[0] == '\0')                 return -1;

    idx = g->locationCount;
    g->locations[idx].id = id;
    strncpy(g->locations[idx].name, name, MAX_NAME_LEN - 1);
    g->locations[idx].name[MAX_NAME_LEN - 1] = '\0';
    g->locations[idx].x = x;
    g->locations[idx].y = y;
    g->locations[idx].valid = 1;
    g->locationCount++;

    return idx;
}

// 添加道路（无向图，双向挂边）
int AddRoad(Graph* g, int fromId, int toId, int distance)
{
    int fromIdx, toIdx;
    EdgeNode* node1, * node2;

    fromIdx = FindLocationIndexById(g, fromId);
    toIdx = FindLocationIndexById(g, toId);

    if (fromIdx == -1 || toIdx == -1) return 0;
    if (fromIdx == toIdx)             return 0;
    if (distance < MIN_ROAD_DISTANCE ||
        distance > MAX_ROAD_DISTANCE) return 0;

    // 查重：from 的邻接表里是否已有 to
    {
        EdgeNode* p = g->adj[fromIdx];
        while (p != NULL)
        {
            if (p->to == toIdx) return 0;
            p = p->next;
        }
    }

    // 正向边 from -> to
    node1 = (EdgeNode*)malloc(sizeof(EdgeNode));
    if (node1 == NULL) return 0;
    node1->to = toIdx;
    node1->distance = distance;
    node1->next = g->adj[fromIdx];
    g->adj[fromIdx] = node1;

    // 反向边 to -> from
    node2 = (EdgeNode*)malloc(sizeof(EdgeNode));
    if (node2 == NULL)
    {
        g->adj[fromIdx] = node1->next;
        free(node1);
        return 0;
    }
    node2->to = fromIdx;
    node2->distance = distance;
    node2->next = g->adj[toIdx];
    g->adj[toIdx] = node2;

    return 1;
}

// 查询两点间道路距离
int GetRoadDistance(Graph* g, int fromId, int toId, int* outDistance)
{
    int fromIdx = FindLocationIndexById(g, fromId);
    int toIdx = FindLocationIndexById(g, toId);
    EdgeNode* p;

    if (fromIdx == -1 || toIdx == -1 || outDistance == NULL)
        return 0;

    p = g->adj[fromIdx];
    while (p != NULL)
    {
        if (p->to == toIdx)
        {
            *outDistance = p->distance;
            return 1;
        }
        p = p->next;
    }
    return 0;
}

// 修改道路距离
int UpdateRoadDistance(Graph* g, int fromId, int toId, int newDistance)
{
    int fromIdx = FindLocationIndexById(g, fromId);
    int toIdx = FindLocationIndexById(g, toId);
    EdgeNode* p;

    if (fromIdx == -1 || toIdx == -1) return 0;
    if (newDistance < MIN_ROAD_DISTANCE ||
        newDistance > MAX_ROAD_DISTANCE) return 0;

    p = g->adj[fromIdx];
    while (p != NULL)
    {
        if (p->to == toIdx) { p->distance = newDistance; break; }
        p = p->next;
    }

    p = g->adj[toIdx];
    while (p != NULL)
    {
        if (p->to == fromIdx) { p->distance = newDistance; break; }
        p = p->next;
    }

    return 1;
}

// 删除道路
int RemoveRoad(Graph* g, int fromId, int toId)
{
    int fromIdx = FindLocationIndexById(g, fromId);
    int toIdx = FindLocationIndexById(g, toId);
    EdgeNode** pp;
    EdgeNode* del;

    if (fromIdx == -1 || toIdx == -1) return 0;

    // 删 from -> to
    pp = &g->adj[fromIdx];
    while (*pp != NULL)
    {
        if ((*pp)->to == toIdx)
        {
            del = *pp;
            *pp = del->next;
            free(del);
            break;
        }
        pp = &(*pp)->next;
    }

    // 删 to -> from
    pp = &g->adj[toIdx];
    while (*pp != NULL)
    {
        if ((*pp)->to == fromIdx)
        {
            del = *pp;
            *pp = del->next;
            free(del);
            break;
        }
        pp = &(*pp)->next;
    }

    return 1;
}

// 打印邻接表（调试用）
void PrintGraph(Graph* g)
{
    int i;
    for (i = 0; i < g->locationCount; i++)
    {
        EdgeNode* p;
        if (!g->locations[i].valid) continue;
        printf("[%d] %s(id=%d) : ",
            i, g->locations[i].name, g->locations[i].id);
        p = g->adj[i];
        while (p != NULL)
        {
            printf("-> [%d]%s(%d) ",
                p->to, g->locations[p->to].name, p->distance);
            p = p->next;
        }
        printf("\n");
    }
}

// 打印道路列表
void PrintRoads(Graph* g)
{
    int i;
    for (i = 0; i < g->locationCount; i++)
    {
        EdgeNode* p;
        if (!g->locations[i].valid) continue;
        p = g->adj[i];
        while (p != NULL)
        {
            if (p->to > i)
            {
                printf("%s -- %s : %d\n",
                    g->locations[i].name,
                    g->locations[p->to].name,
                    p->distance);
            }
            p = p->next;
        }
    }
}

// 释放图内存
void FreeGraph(Graph* g)
{
    int i;
    for (i = 0; i < g->locationCount; i++)
    {
        EdgeNode* p = g->adj[i];
        while (p != NULL)
        {
            EdgeNode* q = p->next;
            free(p);
            p = q;
        }
        g->adj[i] = NULL;
    }
    g->locationCount = 0;
}

// 从文件加载地点
// 文件格式：id name x y
int LoadLocations(Graph* g, const char* filename)
{
    FILE* fp = fopen(filename, "r");
    int id, x, y;
    char name[MAX_NAME_LEN];
    int count = 0;

    if (fp == NULL)
    {
        printf("Cannot open %s\n", filename);
        return 0;
    }

    while (fscanf(fp, "%d %49s %d %d", &id, name, &x, &y) == 4)
    {
        if (AddLocation(g, id, name, x, y) >= 0)
            count++;
        else
            printf("Skip invalid location: id=%d name=%s (%d,%d)\n",
                id, name, x, y);
    }

    fclose(fp);
    printf("Loaded %d locations\n", count);
    return 1;
}

// 从文件加载道路
// 文件格式：from to distance
int LoadRoads(Graph* g, const char* filename)
{
    FILE* fp = fopen(filename, "r");
    int from, to, dist;
    int count = 0;

    if (fp == NULL)
    {
        printf("Cannot open %s\n", filename);
        return 0;
    }

    while (fscanf(fp, "%d %d %d", &from, &to, &dist) == 3)
    {
        if (AddRoad(g, from, to, dist))
            count++;
        else
            printf("Skip invalid road: %d-%d (%d)\n", from, to, dist);
    }

    fclose(fp);
    printf("Loaded %d roads\n", count);
    return 1;
}

// 广度优先搜索 BFS
// visited 数组大小应为 MAX_LOCATIONS
int BFS(Graph* g, int startIdx, int visited[])
{
    int queue[MAX_LOCATIONS];
    int front = 0, rear = 0;
    int i;
    EdgeNode* p;

    for (i = 0; i < g->locationCount; i++) visited[i] = 0;

    if (startIdx < 0 || startIdx >= g->locationCount) return 0;
    if (!g->locations[startIdx].valid) return 0;

    visited[startIdx] = 1;
    queue[rear++] = startIdx;

    while (front < rear)
    {
        int u = queue[front++];
        p = g->adj[u];
        while (p != NULL)
        {
            if (!visited[p->to] && g->locations[p->to].valid)
            {
                visited[p->to] = 1;
                queue[rear++] = p->to;
            }
            p = p->next;
        }
    }
    return 1;
}

// 判断整图是否连通
int IsGraphConnected(Graph* g)
{
    int visited[MAX_LOCATIONS];
    int i, first = -1;

    for (i = 0; i < g->locationCount; i++)
    {
        if (g->locations[i].valid)
        {
            first = i;
            break;
        }
    }
    if (first == -1) return 1;

    BFS(g, first, visited);

    for (i = 0; i < g->locationCount; i++)
    {
        if (g->locations[i].valid && !visited[i])
            return 0;
    }
    return 1;
}

// 判断两点之间是否有路径
int HasPath(Graph* g, int startId, int endId)
{
    int visited[MAX_LOCATIONS];
    int startIdx = FindLocationIndexById(g, startId);
    int endIdx = FindLocationIndexById(g, endId);

    if (startIdx == -1 || endIdx == -1) return 0;

    BFS(g, startIdx, visited);
    return visited[endIdx];
}