#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// Round Robin algoritması
void roundRobin(Queue *q, CPU *cpu, int quantum_time)
{
    int time = 0;
    char processString[100];

    while (!isEmpty(q))
    {
        Process current_process = dequeue(q);
        char tempProcess[15];
        strcpy(tempProcess, current_process.process_number);

        // RAM ve CPU check
        if (current_process.ram > cpu->cpu_ram || current_process.cpu_rate > cpu->cpu_ram)
        {
            printf("time %d: process %s unsufficient resources . enqueue the que.\n", time, current_process.process_number);
            enqueue(q, current_process);
            continue;
        }

        // allocationg resources
        cpu->cpu_ram -= current_process.ram;
        cpu->cpu_ram -= current_process.cpu_rate;
        printf("time %d: process %s is assigned to CPU-2.\n", time, current_process.process_number);

        if (current_process.burst_time > quantum_time)
        {
            // quantum time is up
            time += quantum_time;
            current_process.burst_time -= quantum_time;
            strcat(tempProcess, "->");
            strcat(processString, tempProcess);
            //  Kuyruğa tekrar ekle
            enqueue(q, current_process);
            printf("time %d: process %s quantum time is up, remaining burst time: %d\n", time, current_process.process_number, current_process.burst_time);
        }
        else
        {
            // time up
            time += current_process.burst_time;
            //  in order to print Grantt Chart.
            strcat(tempProcess, "->");
            strcat(processString, tempProcess);
            //
            printf("time %d: process %s is completed.\n", time, current_process.process_number);
        }

        // deallocating resources
        cpu->cpu_ram += current_process.ram;
        cpu->cpu_ram += current_process.cpu_rate;
    }
    printf("All processes are done. whole time: %d\n", time);
    printf("%s", processString);
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
    int time = 1;
    Process currentProcess = dequeue(q);
    Process tempCurrentProcess = currentProcess;

    while (!isEmpty(q) || !isEmpty(cpu->processes) || currentProcess.burst_time > 0)
    {

        // load process to CPU queue
        while (!isEmpty(q))
        {
            Process process = dequeue(q);

            if (process.arrival_time <= time)
            {
                if (process.ram <= cpu->cpu_ram)
                {
                    cpu->cpu_ram -= process.ram;
                    enqueue(cpu->processes, process);
                }
                else
                {
                    printf("%s", "not sufficient cpu ram.");
                    enqueue(q, process);
                    break;
                }
            }
            else
            {
                enqueue(q, process);
                break;
            }
        }

        // If no process is currently running, take the next one from the CPU queue
        if (currentProcess.burst_time == 0 && !isEmpty(cpu->processes))
        {
            currentProcess = dequeue(cpu->processes);
            if (currentProcess.burst_time == 0)
                continue;
            printf("Time: %d, Process %s is loaded to cpu\n", time, currentProcess.process_number);
        }

        // Simulate the execution of the current process
        if (currentProcess.burst_time > 0)
        {
            printf("Time: %d, Processing %s (Remaining Burst Time: %d)\n", time, currentProcess.process_number, currentProcess.burst_time);
            currentProcess.burst_time--;

            if (currentProcess.burst_time == 0)
            {
                printf("Process %s completed at time %d\n", currentProcess.process_number, time);
                time -= tempCurrentProcess.burst_time;
                cpu->cpu_ram += currentProcess.ram;
                // currentProcess = (Process){0, 0, 0}; // Reset the current process
            }
        }

        time++;
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

int main()
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
    file = fopen("input.txt", "r");

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

    printf("\n----CPU-2 Round Robin Algorithm (queue 2, Quantum time: 8)----\n");
    roundRobin(&cpu2_queue2, &cpu2, 8);

    printf("\n----CPU-2 Round Robin Algorithm (queue 3, Quantum time: 16)----\n");
    roundRobin(&cpu2_queue3, &cpu2, 16);

    return 0;
}
