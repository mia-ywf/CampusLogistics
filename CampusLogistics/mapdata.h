#pragma once
#ifndef MAPDATA_H
#define MAPDATA_H


 //地图数据管理模块

#include "common.h"
#include "graph.h"

 // 保存地点到 txt，格式：id name x y
int SaveLocations(Graph* g, const char* filename);

// 保存道路到 txt，格式：from_id to_id distance
int SaveRoads(Graph* g, const char* filename);

// 统计无向边的条数（每条边只算一次） 
int CountRoads(Graph* g);

// 统计有效地点的个数 
int CountValidLocations(Graph* g);

#endif /* MAPDATA_H */
