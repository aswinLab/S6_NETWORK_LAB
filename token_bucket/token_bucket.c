#include <stdio.h>
#include <unistd.h>

int main()
{
    int bucket_size, token_rate;
    int tokens = 0;
    int time_steps;
    int incoming;

    printf("Enter bucket capacity (max tokens): ");
    scanf("%d",&bucket_size);

    printf("Enter token generation rate: ");
    scanf("%d",&token_rate);

    printf("Enter number of time steps: ");
    scanf("%d",&time_steps);

    for(int i = 1; i <= time_steps; i++)
    {
        printf("\nTime %d\n",i);

        tokens += token_rate;

        if(tokens > bucket_size)
            tokens = bucket_size;

        printf("Tokens available: %d\n",tokens);

        printf("Enter incoming packets: ");
        scanf("%d",&incoming);

        int sent = 0;
        int dropped = 0;

        if(incoming <= tokens)
        {
            sent = incoming;
            tokens -= incoming;
        }
        else
        {
            sent = tokens;
            dropped = incoming - tokens;
            tokens = 0;
        }

        printf("Packets sent: %d\n",sent);
        printf("Packets dropped: %d\n",dropped);
        printf("Tokens remaining: %d\n",tokens);

        sleep(1);
    }

    return 0;
}