#include <stdio.h>
#include <unistd.h>

int main() {
    int bucket_size, op_rate, inputs, incoming, stored = 0;

    printf("Enter bucket size: ");
    scanf("%d", &bucket_size);
    
    printf("Enter output rate: ");
    scanf("%d", &op_rate);
    
    printf("Enter number of times to input: ");
    scanf("%d", &inputs);

    // Loop until all inputs are processed AND bucket is empty
    for (int i = 1; i <= inputs || stored > 0; i++) {
        printf("\n--- Time %d ---\n", i);
        
        if (i <= inputs) {
            printf("Enter incoming packets: ");
            scanf("%d", &incoming);
        } else {
            incoming = 0;
            printf("No incoming packets (draining phase).\n");
        }

        // Check for bucket overflow
        if (incoming + stored > bucket_size) {
            int dropped = (incoming + stored) - bucket_size;
            printf("Bucket overflow! %d packets dropped.\n", dropped);
            stored = bucket_size;
        } else {
            stored += incoming;
            printf("0 packets dropped.\n");
        }

        // Determine packets to send
        int sent = (stored < op_rate) ? stored : op_rate;
        stored -= sent;

        printf("Sent: %d packets.\n", sent);
        printf("Remaining in bucket: %d packets.\n", stored);
        
        sleep(1);
    }

    return 0;
}
