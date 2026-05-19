#include <stdio.h>
#include <unistd.h>

int main()
{
    int bucket_size, output_rate, packets, stored = 0;
    int incoming;

    printf("Enter bucket capacity: ");
    scanf("%d",&bucket_size);

    printf("Enter output rate: ");
    scanf("%d",&output_rate);

    printf("Enter number of time steps: ");
    scanf("%d",&packets);

    for(int i = 1; i <= packets; i++)
    {
        printf("\nTime %d\n",i);

        printf("Enter incoming packets: ");
        scanf("%d",&incoming);

        if(incoming + stored <= bucket_size)
        {
            stored += incoming;
        }
        else
        {
            int dropped = (incoming + stored) - bucket_size;
            stored = bucket_size;

            printf("Bucket overflow! Dropped packets = %d\n",dropped);
        }

        printf("Packets in bucket: %d\n",stored);

        int sent;

        if(stored < output_rate)
            sent = stored;
        else
            sent = output_rate;

        stored -= sent;

        printf("Packets sent: %d\n",sent);
        printf("Packets remaining in bucket: %d\n",stored);

        sleep(1);
    }

    return 0;
}