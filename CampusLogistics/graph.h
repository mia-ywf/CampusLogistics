#ifndef GRAPH_H
#define GRAPH_H

#include "common.h"

typedef struct
{
    int id;
    char name[50];
    int x;
    int y;
} Location;

typedef struct EdgeNode
{
    int to;
    int distance;
    struct EdgeNode* next;
} EdgeNode;

typedef struct
{
    int locationCount;
    Location locations[MAX_LOCATIONS];
    EdgeNode* adj[MAX_LOCATIONS];
} Graph;

#endif
