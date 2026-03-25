#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

// TEMPLATES FOR QUEUES and QUEUE ITEMS                 //
// YOU WILL LIKELY NEED TO MODIFY THIS
// ==================================================== //

// a struct for the items read from the file
typedef struct {
    int delay_s;
    int value;
} Item;

// a struct for the thread-safe queue node
typedef struct __node_t {
    Item             value;
    struct __node_t *next;
} node_t;

// a struct for the concurrent queue
typedef struct __queue_t {
    node_t         *head;
    node_t         *tail;
} queue_t;

static queue_t qA;  // queue for producers to pass values to workers
static queue_t qB;  // queue for workers to pass values to consumers
static FILE           *input_file;

// ==================================================== //


// ANY VARIABLE DECLARATIONS/INITIALIZATIONS GO HERE //
// ==================================================== //






// COMPLETE the CONCURRENT QUEUE IMPLEMENTATION BELOW       //
// DO NOT CHANGE THE PROVIDED FUNCTION SIGNATURES       //
// YOU MAY ADD MORE QUEUE FUNCTIONS IF NEEDED           //
// ==================================================== //

// queue initialization
void Queue_Init(queue_t *q)
{
    return;
}

// queue destruction
void Queue_Destroy(queue_t *q)
{
    return;
}

// enqueue an item
void Queue_Enqueue(queue_t *q, Item value)
{
    return;
}

int Queue_Dequeue(queue_t *q, Item *value)
{
    return 0;
}

// ==================================================== //

// COMPLETE PRODUCER, WORKER, CONSUMER FUNCTIONS BELOW  //
// DO NOT CHANGE THE PROVIDED FUNCTION SIGNATURES       //
// YOU MAY ADD MORE FUNCTIONS IF NEEDED                 //
// ==================================================== //
// producer threads read the file line by line
// enqueue each read line in qA
// let the workers know when the input is finished
void *producer(void *arg)
{
 
    return;
}

// worker threads dequeue from qA
// sleep for the given amount of time
// multiply the value by 3
// enqueues to qB
// know when to stop
// tell consumers when to stop
void *worker(void *arg)
{
    return NULL;
}

// consumer threads dequeue from qB
// print values in the order they were in the input
// regardless of the added sleep delay 

void *consumer(void *arg)
{

    return NULL;
}


/*                                                              
Usage: ./pipeline <input_file> [P] [W] [C]                      
                                                               
input_file - each line contains <delay_s> <value>     
P          - number of producer threads  (default 2)           
W          - number of worker threads    (default 2)           
C          - number of consumer threads  (default 2)       
*/

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file> [P] [W] [C]\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    int P = argc >= 3 ? atoi(argv[2]) : 2;
    int W = argc >= 4 ? atoi(argv[3]) : 2;
    int C = argc >= 5 ? atoi(argv[4]) : 2;

    input_file  = fopen(filename, "r");
    if (!input_file) { perror(filename); return 1; }
    // DO NOT CHANGE MAIN ABOVE THIS LINE                     //


    // ANY VARIABLE DECLARATIONS/INITIALIZATIONS CAN GO HERE //
    // ==================================================== //

    seq_counter = 0;
    p_remaining = P;
    w_remaining = W;
    out_values  = NULL;
    out_ready   = NULL;
    out_cap     = 0;
    out_next    = 0;

    // ==================================================== //

    // DO NOT CHANGE CODE BELOW THIS LINE

    Queue_Init(&qA);
    Queue_Init(&qB);

    pthread_t *tp = malloc(sizeof(pthread_t) * P);
    pthread_t *tw = malloc(sizeof(pthread_t) * W);
    pthread_t *tc = malloc(sizeof(pthread_t) * C);

    for (int i = 0; i < P; i++)
        pthread_create(&tp[i], NULL, producer, NULL);
    for (int i = 0; i < W; i++)
        pthread_create(&tw[i], NULL, worker, NULL);
    for (int i = 0; i < C; i++)
        pthread_create(&tc[i], NULL, consumer, NULL);

    for (int i = 0; i < P; i++)
        pthread_join(tp[i], NULL);
    for (int i = 0; i < W; i++)
        pthread_join(tw[i], NULL);
    for (int i = 0; i < C; i++)
        pthread_join(tc[i], NULL);

    // DEALLOCATE VARIABLES (IF NEEDED) BELOW THIS LINE
    // ==================================================== //

    free(out_values);
    free(out_ready);

    // ==================================================== //
    // DO NOT CHANGE LINES BELOW
    fclose(input_file);

    free(tp);
    free(tw);
    free(tc);

    Queue_Destroy(&qA);
    Queue_Destroy(&qB);

    return 0;
}