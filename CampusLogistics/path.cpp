#include "path.h"
#include <limits.h>

/* Dijkstra：计算 start 到 end 的最短路径，并恢复具体路径 */
int dijkstra(Graph* graph, int start, int end, Path* path)
{
    int dist[MAX_LOCATIONS];
    int prev[MAX_LOCATIONS];
    int visited[MAX_LOCATIONS];

    int i;
    int current;
    int next;
    int newDist;

    /* 初始化 */
    for (i = 0; i < graph->locationCount; i++)
    {
        dist[i] = INT_MAX;
        prev[i] = -1;
        visited[i] = 0;
    }

    dist[start] = 0;

    /* Dijkstra */
    for (i = 0; i < graph->locationCount; i++)
    {
        current = -1;

        /* 找当前距离最小、且未访问的点 */
        for (next = 0; next < graph->locationCount; next++)
        {
            if (!visited[next] &&
                dist[next] != INT_MAX &&
                (current == -1 || dist[next] < dist[current]))
            {
                current = next;
            }
        }

        if (current == -1)
        {
            break;
        }

        if (current == end)
        {
            break;
        }

        visited[current] = 1;

        /* 遍历邻接表 */
        EdgeNode* edge = graph->adj[current];

        while (edge != NULL)
        {
            next = edge->to;
            newDist = dist[current] + edge->distance;

            if (!visited[next] && newDist < dist[next])
            {
                dist[next] = newDist;
                prev[next] = current;
            }

            edge = edge->next;
        }
    }

    /* 不可达 */
    if (dist[end] == INT_MAX)
    {
        return 0;
    }

    /* 填充 Path */
    path->start_location = start;
    path->end_location = end;
    path->distance = dist[end];

    /* 根据 prev[] 从终点反向恢复 */
    path->length = 0;
    current = end;

    while (current != -1)
    {
        path->nodes[path->length] = current;
        path->length++;

        if (current == start)
        {
            break;
        }

        current = prev[current];
    }

    /* 判断是否真的恢复到了起点 */
    if (path->nodes[path->length - 1] != start)
    {
        return 0;
    }

    /* 反转路径 */
    for (i = 0; i < path->length / 2; i++)
    {
        int temp;

        temp = path->nodes[i];
        path->nodes[i] = path->nodes[path->length - 1 - i];
        path->nodes[path->length - 1 - i] = temp;
    }

    return 1;
}