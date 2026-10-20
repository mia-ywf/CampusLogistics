#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include "mapdata.h"

// 保存地点
int SaveLocations(Graph* g, const char* filename)
{
    FILE* fp;
    int i;
    int count = 0;

    /*  参数合法性检查  */
    if (g == NULL || filename == NULL)
    {
        return 0;
    }

    fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("Cannot write %s\n", filename);
        return 0;
    }

    for (i = 0; i < g->locationCount; i++)
    {
        if (!g->locations[i].valid)
        {
            continue;
        }

        fprintf(fp, "%d %s %d %d\n",
            g->locations[i].id,
            g->locations[i].name,
            g->locations[i].x,
            g->locations[i].y);

        count++;
    }

    fclose(fp);
    printf("Saved %d locations\n", count);
    return 1;
}

// 保存道路
int SaveRoads(Graph* g, const char* filename)
{
    FILE* fp;
    int i;
    int count = 0;

    if (g == NULL || filename == NULL)
    {
        return 0;
    }

    fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("Cannot write %s\n", filename);
        return 0;
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
            /* 只写 p->to > i 的边，避免重复 */
            if (p->to > i && g->locations[p->to].valid)
            {
                fprintf(fp, "%d %d %d\n",
                    g->locations[i].id,
                    g->locations[p->to].id,
                    p->distance);
                count++;
            }
            p = p->next;
        }
    }

    fclose(fp);
    printf("Saved %d roads\n", count);
    return 1;
}

// 统计道路条数（无向边算一条）
int CountRoads(Graph* g)
{
    int i;
    int count = 0;

    if (g == NULL)
    {
        return 0;
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
                count++;
            }
            p = p->next;
        }
    }

    return count;
}

/* 统计有效地点个数 */
int CountValidLocations(Graph* g)
{
    int i;
    int count = 0;

    if (g == NULL)
    {
        return 0;
    }

    for (i = 0; i < g->locationCount; i++)
    {
        if (g->locations[i].valid)
        {
            count++;
        }
    }

    return count;
}
