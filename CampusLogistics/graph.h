#ifndef GRAPH_H
#define GRAPH_H

#include "common.h"

typedef struct Location {
	int id;
	char name[MAX_NAME_LEN];
	int x;//横坐标
	int y;
	int valid;//合法性检验
}Location;

typedef struct EdgeNode
{
	int  to;//目标地点的下标                    
	int  distance;
	struct EdgeNode* next;//下一条边     
} EdgeNode;

typedef struct
{
	int      locationCount;
	Location locations[MAX_LOCATIONS];
	EdgeNode* adj[MAX_LOCATIONS];     
} Graph;

//基础操作
void InitGraph(Graph* g);
int  FindLocationIndexById(Graph* g, int id);
int  AddLocation(Graph* g, int id, const char* name, int x, int y);
//修改地点：改名称和坐标（不允许改 ID） 
int  UpdateLocation(Graph* g, int id, const char* newName, int newX, int newY);

int  AddRoad(Graph* g, int fromId, int toId, int distance);
int  RemoveRoad(Graph* g, int fromId, int toId);
int  UpdateRoadDistance(Graph* g, int fromId, int toId, int newDistance);
int  GetRoadDistance(Graph* g, int fromId, int toId, int* outDistance);
void PrintGraph(Graph* g);
void PrintRoads(Graph* g);
void FreeGraph(Graph* g);

//文件加载
int LoadLocations(Graph* g, const char* filename);
int LoadRoads(Graph* g, const char* filename);

//连通性检查
int BFS(Graph* g, int startIdx, int visited[]);
int IsGraphConnected(Graph* g);
int HasPath(Graph* g, int startId, int endId);

#endif