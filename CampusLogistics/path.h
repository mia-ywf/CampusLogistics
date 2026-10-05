#ifndef PATH_H
#define PATH_H

#include "common.h"
#include "graph.h"

// 路径结构体
typedef struct {
	int id;                 // 路径ID
	int start_location;     // 起点位置
	int end_location;       // 终点位置
	int distance;           // 距离

	int nodes[MAX_LOCATIONS]; // 路径经过的节点
	int length;               // 路径长度
} Path;

int dijkstra(Graph* graph, int start, int end, Path* path);

#endif // !PATH_H