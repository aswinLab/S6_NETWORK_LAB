#include <stdio.h>

#define INF 999

// Structure to represent a router's routing table
struct Router {
    int dist[20];
    int next[20];
} rt[20];

int main() {
    int cost[20][20];
    int n, i, j, k, updated;

    printf("Enter the number of routers: ");
    scanf("%d", &n);

    printf("Enter the cost matrix (use %d for infinity):\n", INF);
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            scanf("%d", &cost[i][j]);
            // Initially, distance is direct cost and next hop is the destination itself
            rt[i].dist[j] = cost[i][j];
            rt[i].next[j] = j;
        }
    }

    // Distance Vector Routing Simulation (Bellman-Ford approach)
    do {
        updated = 0;
        for (i = 0; i < n; i++) {          // For each router i
            for (j = 0; j < n; j++) {      // For each destination j
                for (k = 0; k < n; k++) {  // Evaluate paths via neighbor k
                    if (cost[i][k] != INF && rt[k].dist[j] != INF) {
                        if (rt[i].dist[j] > cost[i][k] + rt[k].dist[j]) {
                            rt[i].dist[j] = cost[i][k] + rt[k].dist[j];
                            rt[i].next[j] = k; // Update to go via router k
                            updated = 1;       // Table was updated, keep simulating
                        }
                    }
                }
            }
        }
    } while (updated); // Loop until no router updates its table

    // Print final routing tables
    printf("\n--- Final Routing Tables ---\n");
    for (i = 0; i < n; i++) {
        printf("\nRouter %d\n", i);
        printf("Dest\tCost\tNext Hop\n");
        for (j = 0; j < n; j++) {
            printf("%d\t%d\t", j, rt[i].dist[j]);
            
            // If it's itself or unreachable, print '-'
            if (i == j || rt[i].dist[j] == INF) {
                printf("-\n");
            } else {
                printf("%d\n", rt[i].next[j]);
            }
        }
    }

    return 0;
}
