#include <stdio.h>

#define MAX 10
#define INF 999

int main()
{
    int cost[MAX][MAX];
    int dist[MAX][MAX];
    int next[MAX][MAX];

    int n;
    int i, j, k;

    printf("Enter number of routers: ");
    scanf("%d", &n);

    printf("Enter cost matrix:\n");

    // Input and initialization
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            scanf("%d", &cost[i][j]);

            dist[i][j] = cost[i][j];

            if(i == j)
            {
                next[i][j] = -1;
            }
            else if(cost[i][j] == INF)
            {
                next[i][j] = -1;
            }
            else
            {
                next[i][j] = j;
            }
        }
    }

    // Floyd-Warshall style shortest path update
    for(k = 0; k < n; k++)
    {
        for(i = 0; i < n; i++)
        {
            for(j = 0; j < n; j++)
            {
                // Avoid invalid INF additions
                if(dist[i][k] != INF && dist[k][j] != INF)
                {
                    if(dist[i][j] > dist[i][k] + dist[k][j])
                    {
                        dist[i][j] = dist[i][k] + dist[k][j];

                        // Correct next hop
                        next[i][j] = next[i][k];
                    }
                }
            }
        }
    }

    // Print routing tables
    printf("\nRouting Tables:\n");

    for(i = 0; i < n; i++)
    {
        printf("\nRouter %d\n", i);

        printf("Destination\tCost\tNext Hop\n");

        for(j = 0; j < n; j++)
        {
            printf("%d\t\t%d\t", j, dist[i][j]);

            if(next[i][j] == -1)
                printf("-\n");
            else
                printf("%d\n", next[i][j]);
        }
    }

    return 0;
}