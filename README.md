# CIS\*3110 A4 Producer-consumer

### Full Name: Matthew Allen
### Student id: 1313528

## Overview

This program implements a multi-threaded pipeline using producers, workers, and consumers to create an efficient solution to process a stream of data using multiple threads and preserve the correct output order.

### Producers

Producers read pairs of integers from the input file to store their delay and value. These pairs are stored in Item structs and then added to a queue.

Producers share a file pointer and use a lock for the file to ensure only one thread reads at a time. Each item stores a sequence number when read to maintain order for the consumers later. When all producers finish, the last one closes the first queue to signal that the file is finished.

### Workers

The workers sleep for the designated delay time and then simulate processing the value by multiplying it by 3.

Workers terminate when Queue A is empty and closed. The final worker closes the second queue to notify consumers.

### Consumers

Consumers print the new processed values in the original order from the input. 

Consumers dequeue processed items from the second queue and store them indexed by sequence number. Consumers print values only when the next expected sequence is ready, ensuring correct ordering while still allowing concurrent processing.

### Queues

In between the stages, queues are used to store the ordered items. The queues are implemented through linked lists with mutex and condition protection and a closed flag. Multiple threads can safely use the queues concurrently since each queue is protected by a mutex so only one thread can modify it at a time. The queue also supports concurrent execution by using a condition variable. When a thread is ready to dequeue from an empty queue, it waits with ```pthread_cond_wait```, which locks the queue until an item is enqueued to optimize CPU usage. The closed flag and ```pthread_cond_broadcast``` allow all waiting threads to exit when no more data is arriving. My queue implementation is similar and based on the example from the Chapter 29.3. 

## Installation

Linux SOCS server

Compilation: 
```make```

This creates the executable: ```pipeline```

## Usage

Run Code: 
```./pipeline <input_file> [P] [W] [C]```

### Parameters

+ ```<input_file>```: Text file storing one item per line 
+ ```[P]```: Number of producer threads
+ ```[W]```: Number of worker threads
+ ```[C]```: Number of consumer threads

[P], [W], and [C] are all optional with their default values all being 2. Thread counts should all be positive integers.


### Input File Format
```<delay> <value>``` 

### Invalid Usage

+ ```./pipeline```: No input file

+ ```./pipeline input.txt 0 -1 -2```: Number of threads must all be positive integers

### Output

The program outputs the processed values in the sequence that they were read from the input file. Each processed value is on its own line and is 3 times the initial value that was read in. These values are printed by the consumer.

## Examples

input1.txt
```
0 5
10 10
15 15
```

input2.txt
```
10 1
0 2
0 3
0 4
0 5
```

### Example 1

```
make
./pipeline input1.txt
```

Output:
```
15
30
45
```

### Example 2

```
make
./pipeline input1.txt 5 2 4
```

```
15
30
45
```

### Example 3

```
make
./pipeline input2.txt 3 3 3
```

Output:
```
3
6
9
12
15
```
