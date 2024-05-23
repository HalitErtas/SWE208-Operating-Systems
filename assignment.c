#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma region Type Definitions
// Process structure
typedef struct
{
    char process_number[10];
    int arrival_time;
    int priority;
    int burst_time;
    int ram;
    int cpu_rate;
    int waiting_time;
} Process;

// queue structure
typedef struct
{
    Process items[20];
    int front;
    int rear;
} Queue;

typedef struct
{
    int cpu_rate;
    int cpu_ram;
    Queue processes[10];
} CPU;
#pragma endregion
#pragma region Queue Implementation
// starting queue
void initQueue(Queue *q)
{
    q->front = -1;
    q->rear = -1;
}

// queue lenght
int lenght(Queue *q)
{
    return q->rear;
}

// checking whether queue is full or not
int isEmpty(Queue *q)
{
    return q->front == -1;
}

// checking queue is full
int isFull(Queue *q)
{
    return q->rear == 99;
}

// enqueue
void enqueue(Queue *q, Process value)
{
    if (isFull(q))
    {
        printf("Kuyruk dolu. Eleman eklenemiyor.\n");
        return;
    }
    if (isEmpty(q))
    {
        q->front = 0;
    }
    q->rear++;
    q->items[q->rear] = value;
}

// taking first front element in the queue
Process front(Queue *q)
{
    return q->items[q->front];
}

// dequeue
Process dequeue(Queue *q)
{
    Process item;
    if (isEmpty(q))
    {
        // printf("Kuyruk boş. Eleman çıkarılamıyor.\n");
        return item;
    }
    item = q->items[q->front];
    q->front++;
    if (q->front > q->rear)
    {
        q->front = q->rear = -1;
    }
    return item;
}
#pragma endregion
// CPU-1 için FIFO algoritmasını uygula
void cpuScheduleFIFO(Queue *q, CPU *cpu)
{
    while (!isEmpty(q))
    {
        Process currentProcess = dequeue(q);
        if ((currentProcess.ram <= cpu->cpu_ram))
        {
            printf("Process %s is queued to be assigned to CPU-1.\n", currentProcess.process_number);
            cpu->cpu_ram -= currentProcess.ram;
            if (currentProcess.cpu_rate <= cpu->cpu_rate)
            {
                cpu->cpu_rate -= currentProcess.cpu_rate;
                printf("Process %s is assigned to CPU-1.\n", currentProcess.process_number);
                printf("Process %s is completed and terminated.\n", currentProcess.process_number);
                cpu->cpu_rate += currentProcess.cpu_rate;
            }
            cpu->cpu_ram += currentProcess.ram;
        }
        else
        {
            printf("Process %s could not be assigned to CPU-1 due to insufficient resourcese.\n", currentProcess.process_number);
        }
    }
}

// Round Robin algorithm
char *roundRobin(Queue *q, CPU *cpu, int quantum_time)
{

    // timer start from first process
    int time = front(q).arrival_time;
    char processString[100];
    processString[0] = '\0';
    // this queue is used for process that came back when quantum time runs out
    Queue readyQueue;
    initQueue(&readyQueue);
    while (!isEmpty(&readyQueue) || !isEmpty(q))
    {
        while (time >= front(q).arrival_time)
        {
            enqueue(&readyQueue, dequeue(q));
        }
        // if there is a gap between two process
        if (isEmpty(&readyQueue) && !isEmpty(q))
        {
            enqueue(&readyQueue, dequeue(q));
            time = front(&readyQueue).arrival_time;
        }

        Process current_process = dequeue(&readyQueue);
        char tempProcess[15];
        strcpy(tempProcess, current_process.process_number);

        // RAM ve CPU check
        if (current_process.ram > cpu->cpu_ram || current_process.cpu_rate > cpu->cpu_rate)
        {
            printf("time %d: process %s unsufficient resources . enqueue the que.\n", time, current_process.process_number);
            // printf("%d : %d - %d : %d\n", current_process.ram, cpu->cpu_ram, current_process.cpu_rate, cpu->cpu_rate);
            enqueue(&readyQueue, current_process);
            continue;
        }

        // allocationg resources
        cpu->cpu_ram -= current_process.ram;
        cpu->cpu_rate -= current_process.cpu_rate;
        printf("time %d: process %s is assigned to CPU-2.\n", time, current_process.process_number);

        if (current_process.burst_time > quantum_time)
        {
            // quantum time is up
            time += quantum_time;
            current_process.burst_time -= quantum_time;
            strcat(tempProcess, "->");
            strcat(processString, tempProcess);

            // add processes if they're time and waiting
            while (time >= front(q).arrival_time)
            {
                enqueue(&readyQueue, dequeue(q));
            }
            // add queue again to run later
            enqueue(&readyQueue, current_process);
            printf("time %d: process %s quantum time is up, remaining burst time: %d\n", time, current_process.process_number, current_process.burst_time);
        }
        else
        {
            // time up
            time += current_process.burst_time;
            //  in order to print Grantt Chart.
            strcat(tempProcess, "->");
            strcat(processString, tempProcess);
            printf("time %d: process %s is completed.\n", time, current_process.process_number);
        }

        // deallocating resources
        cpu->cpu_ram += current_process.ram;
        cpu->cpu_rate += current_process.cpu_rate;
    }
    printf("All processes are done. whole time: %d\n", time);
    char *result = processString;
    return result;
}

// CPU-2 sort by Short Job First Algorithm
// this algorithm is for sorting the queue according to burst time
// before implementing cpu sheduling
void sortByBurstTimeSJF(Queue *q)
{
    for (int i = q->front; i <= q->rear; i++)
    {
        for (int j = i + 1; j <= q->rear; j++)
        {
            if (q->items[i].burst_time > q->items[j].burst_time)
            {
                // burst time yüksekse queue sonuna atar.
                Process temp = q->items[i];
                q->items[i] = q->items[j];
                q->items[j] = temp;
            }
        }
    }
}

// short job first arrival time short
void sortByArrivalTimeSJF(Queue *q)
{
    for (int i = q->front; i <= q->rear; i++)
    {
        for (int j = i + 1; j <= q->rear; j++)
        {
            if (q->items[i].arrival_time > q->items[j].arrival_time)
            {
                // arrival time yüksekse queue sonuna atar.
                Process temp = q->items[i];
                q->items[i] = q->items[j];
                q->items[j] = temp;
            }
        }
    }
}

// CPU-2  Shortest Job First (SJF) algorithm
void cpuScheduleSJF(Queue *q, CPU *cpu)
{
    int time = front(q).arrival_time;
    Process currentProcess = dequeue(q);
    Process tempCurrentProcess = currentProcess;

    while (!isEmpty(q) || currentProcess.burst_time > 0)
    {
        // RAM ve CPU check
        if (currentProcess.ram > cpu->cpu_ram || currentProcess.cpu_rate > cpu->cpu_rate)
        {
            printf("time %d: process %s unsufficient resources . enqueue the que.\n", time, currentProcess.process_number);
            enqueue(q, currentProcess);
            currentProcess = dequeue(q);
            continue;
        }

        cpu->cpu_ram -= currentProcess.ram;
        cpu->cpu_rate -= currentProcess.cpu_rate;
        // Simulate the execution of the current process
        while (currentProcess.burst_time > 0)
        {
            printf("time: %d, processing %s (Remaining Burst Time: %d)\n", time, currentProcess.process_number, currentProcess.burst_time);
            currentProcess.burst_time--;

            if (currentProcess.burst_time == 0)
            {
                printf("time: %d, process %s completed\n", time + 1, currentProcess.process_number);
                cpu->cpu_ram += currentProcess.ram;
                cpu->cpu_rate += currentProcess.cpu_rate;

                if (!isEmpty(q))
                {
                    currentProcess = dequeue(q);
                }
                time = currentProcess.arrival_time - 1;
                time++;
                break;
            }
            time++;
        }
    }
}

// CPU-2  Round Robin algorithm
// void cpuScheduleRoundRobin(Queue *q, int quantum, CPU *cpu)
// {
//     while (!isEmpty(q))
//     {
//         Process currentProcess = dequeue(q);
//         if (currentProcess.ram <= cpu->cpu_ram)
//         {
//             cpu->cpu_ram -= currentProcess.ram;
//             printf("Process %s is placed in the queue to be assigned to CPU-2.\n", currentProcess.process_number);
//             printf("Process %s is assigned to CPU-2.\n", currentProcess.process_number);
//             if (currentProcess.burst_time <= quantum)
//             {
//                 printf("Process %s run until the defined quantum time and is queued again because the process is not completed.\n", currentProcess.process_number);
//                 cpu->cpu_ram += currentProcess.ram;
//             }
//             else
//             {
//                 printf("Process %s run until the defined quantum time.\n", currentProcess.process_number);
//                 currentProcess.burst_time -= quantum;
//                 enqueue(q, currentProcess);
//             }
//         }
//         else
//         {
//             printf("Process %s could not be assigned to CPU-2 due to insufficient RAM.\n", currentProcess.process_number);
//         }
//     }
// }

// printing queue processes
void printQueue(Queue *q, char cpuType[], char queType[], char algorithmType[])
{
    if (isEmpty(q))
    {
        printf("Kuyruk boş.\n");
        return;
    }

    printf("%s %s %s: ", cpuType, queType, algorithmType);
    for (int i = q->front; i <= q->rear; i++)
    {
        Process p = q->items[i];
        printf("%s->", p.process_number);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    FILE *file;
    char line[256];
    Queue cpu1_queue, cpu2_queue1, cpu2_queue2, cpu2_queue3;

    // initiating queues
    initQueue(&cpu1_queue);
    initQueue(&cpu2_queue1);
    initQueue(&cpu2_queue2);
    initQueue(&cpu2_queue3);

    // reading file
    file = fopen(argv[1], "r");

    // if you cannot open file, show a message
    if (file == NULL)
    {
        printf("Dosya açılamadı.\n");
        return 1;
    }

    // reading file line by line
    while (fgets(line, sizeof(line), file) != NULL)
    {
        Process process;
        char *token;

        // Process.number
        token = strtok(line, ",");
        strcpy(process.process_number, token);

        // Process.arrival_time
        token = strtok(NULL, ",");
        process.arrival_time = atoi(token);

        // Process.priority
        token = strtok(NULL, ",");
        process.priority = atoi(token);

        // Process.burst_time
        token = strtok(NULL, ",");
        process.burst_time = atoi(token);

        // Processç.ram
        token = strtok(NULL, ",");
        process.ram = atoi(token);

        // Process.cpu_rate
        token = strtok(NULL, ",");
        process.cpu_rate = atoi(token);

        // queue processes for CPU-1
        if (process.priority == 0)
        {
            enqueue(&cpu1_queue, process);
        }
        // queue processes for CPU-2
        else
        {
            if (process.priority == 1)
            {
                enqueue(&cpu2_queue1, process);
            }
            else if (process.priority == 2)
            {
                enqueue(&cpu2_queue2, process);
            }
            else if (process.priority == 3)
            {
                enqueue(&cpu2_queue3, process);
            }
        }
    }

    // rearranging Processes according to Short Job First Algorithm
    // sortByBurstTimeSJF(&cpu2_queue1);
    sortByArrivalTimeSJF(&cpu2_queue2);
    sortByArrivalTimeSJF(&cpu2_queue3);
    sortByArrivalTimeSJF(&cpu2_queue1);

    printQueue(&cpu1_queue, "cpu1", "que", "(FCFS)");
    printQueue(&cpu2_queue1, "cpu2", "que1", "(SJF)");
    printQueue(&cpu2_queue2, "cpu2", "que2", "Round Robin");
    printQueue(&cpu2_queue3, "cpu2", "que3", "Round Robin");

    // closing file
    fclose(file);

    CPU cpu1;
    CPU cpu2;

    cpu1.cpu_ram = 512;
    cpu1.cpu_rate = 100;
    cpu2.cpu_ram = 1536;
    cpu2.cpu_rate = 100;

    printf("----CPU-1 FIFO Algorithm----\n");
    cpuScheduleFIFO(&cpu1_queue, &cpu1);

    printf("\n----CPU-2 Shortest Job First (SJF) Algorithm----\n");
    cpuScheduleSJF(&cpu2_queue1, &cpu2);

    // CPU2 Queue2 printing
    char *q2print = roundRobin(&cpu2_queue2, &cpu2, 8);
    printf("Cpu2 que2 Round Robin :%s\n", q2print);
    q2print[0] = '\0';
    // CPU2 Queue3 printing
    char *q3print = roundRobin(&cpu2_queue3, &cpu2, 16);
    printf("Cpu2 que3 Round Robin%s", q3print);

    return 0;
}
