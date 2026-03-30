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
    int seq;
} Item;

// a struct for the thread-safe queue node
typedef struct __node_t {
    Item             value;
    struct __node_t *next;
} node_t;

// a struct for the concurrent queue
typedef struct __queue_t {
    node_t *head;
    node_t *tail;

    pthread_mutex_t lock;
    pthread_cond_t  cond;

    int closed;
} queue_t;

static queue_t qA;  // queue for producers to pass values to workers
static queue_t qB;  // queue for workers to pass values to consumers
static FILE           *input_file;

// ==================================================== //


// ANY VARIABLE DECLARATIONS/INITIALIZATIONS GO HERE //
// ==================================================== //

//Protect so only 1 producer can read at a time
static pthread_mutex_t file_lock = PTHREAD_MUTEX_INITIALIZER;

//Protect so remaining p and w isnt updated twice
static pthread_mutex_t done_lock = PTHREAD_MUTEX_INITIALIZER;

//Protect output
static pthread_mutex_t out_lock = PTHREAD_MUTEX_INITIALIZER;

//Set counters to default
static int seq_counter = 0;
static int p_remaining = 0;
static int w_remaining = 0;
static int *out_values = NULL;
static int *out_ready  = NULL;
static int out_cap     = 0;
static int out_next    = 0;

// COMPLETE the CONCURRENT QUEUE IMPLEMENTATION BELOW       //
// DO NOT CHANGE THE PROVIDED FUNCTION SIGNATURES       //
// YOU MAY ADD MORE QUEUE FUNCTIONS IF NEEDED           //
// ==================================================== //

// queue initialization
void Queue_Init(queue_t *q){
    q->head = NULL;
    q->tail = NULL;
    q->closed = 0;

    //Set mutex and condition
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->cond, NULL);
}

// queue destruction
void Queue_Destroy(queue_t *q){
    node_t *cur = q->head;

    //Loop through linked list and free all nodes
    while (cur != NULL){
        node_t *temp = cur;
        cur = cur->next;
        free(temp);
    }

    //Free mutex and condition
    pthread_mutex_destroy(&q->lock);
    pthread_cond_destroy(&q->cond);
}

// enqueue an item
void Queue_Enqueue(queue_t *q, Item value){

    //Allocate space for new node
    node_t *new_node = malloc(sizeof(node_t));

    //Exit if null
    if (new_node == NULL){
        printf("Memory allocation failed\n");
        exit(1);
    }

    //Set new node
    new_node->value = value;
    new_node->next = NULL;

    //Lock queue for protection
    pthread_mutex_lock(&q->lock);

    //Check if empty
    if (q->tail == NULL){
        //Add at head and set tail
        q->head = new_node;
        q->tail = new_node;
    } 
    else{
        //Add to tail
        q->tail->next = new_node;
        q->tail = new_node;
    }

    //Wake up thread if waiting
    pthread_cond_signal(&q->cond);

    //Unlock
    pthread_mutex_unlock(&q->lock);
}

int Queue_Dequeue(queue_t *q, Item *value){

    //Lock queue
    pthread_mutex_lock(&q->lock);

    // //Wait if empty but not closed
    // if (q->head == NULL && !q->closed){
    //     pthread_cond_wait(&q->cond, &q->lock);
    // }

    //Wait if empty but not closed
    while (q->head == NULL && !q->closed){
        pthread_cond_wait(&q->cond, &q->lock);
    }

    //Check if queue is empty and closed to stop
    if (q->head == NULL && q->closed){
        pthread_mutex_unlock(&q->lock);
        return 0;
    }

    //Remove node FIFO
    node_t *temp = q->head;
    *value = temp->value;
    q->head = temp->next;

    //Set empty if empty
    if (q->head == NULL){
        q->tail = NULL;
    }

    //Free old head
    free(temp);

    //Unlock and return success
    pthread_mutex_unlock(&q->lock);
    return 1;
}

//HELPER set finised queues to complete
void Queue_Finish(queue_t *q){

    //Lock
    pthread_mutex_lock(&q->lock);

    //Set to closed
    q->closed = 1;

    //Wake up all waiting threads and unlock
    pthread_cond_broadcast(&q->cond);
    pthread_mutex_unlock(&q->lock);
}

// ==================================================== //

// COMPLETE PRODUCER, WORKER, CONSUMER FUNCTIONS BELOW  //
// DO NOT CHANGE THE PROVIDED FUNCTION SIGNATURES       //
// YOU MAY ADD MORE FUNCTIONS IF NEEDED                 //
// ==================================================== //

//HELPER dynamically set size of output array
void set_output_size(int seq){

    //Return if size is fine
    if (seq < out_cap){
        return;
    }

    //Create variable to hold new max size
    int new_max;

    //Set to smallest new size if 0
    if (out_cap == 0){
        new_max = 16;
    } 
    else{
        //Set to outcap
        new_max = out_cap;
    }

    //Double size if too small
    while (seq >= new_max){
        new_max *= 2;
    }

    //Realloc to new size
    int *temp_values = realloc(out_values, new_max * sizeof(int));
    if (temp_values == NULL){
        printf("Memory allocation failed\n");
        exit(1);
    }

    //Resize the ready array
    int *temp_ready = realloc(out_ready, new_max * sizeof(int));
    if (temp_ready == NULL){
        printf("Memory allocation failed\n");
        exit(1);
    }

    //Set new sizes
    out_values = temp_values;
    out_ready = temp_ready;

    //Loop through and initialize
    for (int i = out_cap; i < new_max; i++){
        out_values[i] = 0;
        out_ready[i] = 0;
    }

    //Set new max
    out_cap = new_max;
}

// producer threads read the file line by line
// enqueue each read line in qA
// let the workers know when the input is finished
void *producer(void *arg)
{
    

    //Inf loop
    while (1){
        int delay;
        int value;

        //Lock file
        pthread_mutex_lock(&file_lock);

        //Read file lines
        if (fscanf(input_file, "%d %d", &delay, &value) != 2){
            //Unlick and stop the loop
            pthread_mutex_unlock(&file_lock);
            break;
        }

        //Create item and store delay, val and sequence counter
        Item item;
        item.delay_s = delay;
        item.value = value;
        item.seq = seq_counter;
        seq_counter++;

        //Unlock
        pthread_mutex_unlock(&file_lock);

        //Add item to the first queue
        Queue_Enqueue(&qA, item);
    }

    //Set producer to finished
    pthread_mutex_lock(&done_lock);

    //Decrement producers and close queue if last producer is done
    p_remaining--;
    if (p_remaining == 0){
        Queue_Finish(&qA);
    }

    //Unlock
    pthread_mutex_unlock(&done_lock);

    //Use arg so no wsrnings
    (void)arg;

    return NULL;
}

// worker threads dequeue from qA
// sleep for the given amount of time
// multiply the value by 3
// enqueues to qB
// know when to stop
// tell consumers when to stop
void *worker(void *arg)
{
    //Create variable for the cirrent item
    Item item;

    //Loop while aueue A can dequeue
    while (Queue_Dequeue(&qA, &item)){

        //Sleep for mili seconds
        usleep((useconds_t)item.delay_s * 1000);

        //Process by trippling value
        item.value = item.value * 3;

        //Add to queue B
        Queue_Enqueue(&qB, item);
    }

    //Set worker to finished
    pthread_mutex_lock(&done_lock);

    //Decrement workers and close queue if last worker is done
    w_remaining--;
    if (w_remaining == 0){
        Queue_Finish(&qB);
    }

    //Unlock
    pthread_mutex_unlock(&done_lock);

    //Use arg so no wsrnings
    (void)arg;

    return NULL;
}

// consumer threads dequeue from qB
// print values in the order they were in the input
// regardless of the added sleep delay 
void *consumer(void *arg)
{
    //Create variable for the cirrent item
    Item item;

    //Loop while queue B can be dequeued
    while (Queue_Dequeue(&qB, &item)){

        //Lock output for protection
        pthread_mutex_lock(&out_lock);

        //Set output size with helper
        set_output_size(item.seq);

        //Store processed items to be outputted whenever ready in sequence
        out_values[item.seq] = item.value;
        out_ready[item.seq] = 1;

        //Print if next in sequence is ready
        while (out_next < out_cap && out_ready[out_next]){
            printf("%d\n", out_values[out_next]);
            out_next++;
        }

        //Unlock
        pthread_mutex_unlock(&out_lock);
    }

    //Use arg so no wsrnings
    (void)arg;

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
