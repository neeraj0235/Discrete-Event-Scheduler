#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
// #include "Node.h"
// #include "q.h"
#include "pq.h"

#define min(a, b) (a > b ? b : a)
#define max(a, b) (a > b ? a : b)

#define add_jobs(obj, ...) _Generic(obj, Queue* : add_jobs_q, PriorityQueue* : add_jobs_pq)(obj, __VA_ARGS__)
#define enqueue(obj, ...) _Generic(obj, Queue* : enqueue_q, PriorityQueue* : enqueue_pq) (obj, __VA_ARGS__)
#define dequeue(obj) _Generic(obj, Queue* : dequeue_q, PriorityQueue* : dequeue_pq) (obj)
#define isEmpty(obj) _Generic(obj, Queue* : isEmpty_q, PriorityQueue* : isEmpty_pq) (obj)

#define MAX_JOBS 32768
void sort(Node** jobs, int lo, int hi, bool(*cmp)(Node*, Node*)) {
    if(lo + 1 >= hi) 
        return;

    int mid = (lo + hi)/2;

    sort(jobs, lo, mid, cmp);
    sort(jobs, mid, hi, cmp);

    int n1 = mid - lo, n2 = hi - mid;

    Node* L[n1];
    Node* R[n2];

    for(int i = 0; i < n1; i++) {
        L[i] = jobs[lo + i];
    }
    for(int i = 0; i < n2; i++) {
        R[i] = jobs[mid + i];
    }

    int i = 0, j = 0, k = lo;

    while(i < n1 && j < n2) {
        if(cmp(L[i], R[j])) {
            jobs[k++] = L[i++];
        } else {
            jobs[k++] = R[j++];
        }
    }

    while(i < n1) {
        jobs[k++] = L[i++];
    }
    while(j < n2) {
        jobs[k++] = R[j++];
    }
}

bool cmp_arrival(Node* a, Node* b) {
    if(a->arrival == b->arrival) {
        return strcmp(a->PID, b->PID) == -1;
    }
    return a->arrival < b->arrival;
}

bool cmp_burst(Node* a, Node* b) {
    // if(a->remaining_burst == b->remaining_burst) {
    //     return strcmp(a->PID, b->PID) == -1;
    // }
    return a->remaining_burst < b->remaining_burst;
}

void print(Node** jobs, int N, char** exec_IDs, float exec_period[][2], int exec_ct) {
    float turnaround = 0, response = 0;
    for(int i = 0; i < N; i++) {
        turnaround += jobs[i]->completion_time - jobs[i]->arrival;
        response += jobs[i]->first_schedule - jobs[i]->arrival;
        // printf("%s : ", jobs[i]->PID);
        // printf("Arrival = %f, first_schedule = %f, completion_time = %f\n", jobs[i]->arrival, jobs[i]->first_schedule, jobs[i]->completion_time);

        // printf("PID %s \n", jobs[i]->PID);
        // printf("response time %f \n",jobs[i]->first_schedule - jobs[i]->arrival);
        // printf("turnaround time %f \n", jobs[i]->completion_time - jobs[i]->arrival);
    }

    char* current_job = strdup(exec_IDs[0]);
    float start = exec_period[0][0], end = exec_period[0][1];
    for(int i = 0; i < exec_ct; i++) {
        if(strcmp(current_job, exec_IDs[i])) {
            printf("%s %.3f %.3f ", current_job, start, end);
            current_job = strdup(exec_IDs[i]);
            start = exec_period[i][0];
            end = exec_period[i][1];
        } else {
            end = exec_period[i][1];
        }
    }
    printf("%s %.3f %.3f ", current_job, start, end);
    printf("\n");

        // printf("Avg turnaround time = %f\nAvg. response time = %f\n", turnaround/N, response/N);
        printf("%.3f %.3f\n", turnaround/N, response/N);

        // printf("Turnaround time = %f\nResponse time = %f\n", turnaround, response);

    for(int i = 0; i < N; i++) {
        jobs[i]->remaining_burst = jobs[i]->burst;
        jobs[i]->first_schedule = -1;
        jobs[i]->completion_time = -1;
    }
    for(int i = 0; i < exec_ct; i++) {
        free(exec_IDs[i]);
    }
    free(exec_IDs);
}

float add_jobs_q(Queue* q, Node** jobs, int N, float current_time, int* job_ct) {
    while(*job_ct < N && jobs[*job_ct]->arrival <= current_time) {
        enqueue(q, jobs[*job_ct]);
        (*job_ct)++;
    }
    return (*job_ct == N ? __FLT_MAX__ : jobs[*job_ct]->arrival);
}

float add_jobs_pq(PriorityQueue* pq, Node** jobs, int N, float current_time, int* job_ct) {
    while(*job_ct < N && jobs[*job_ct]->arrival <= current_time) {
        enqueue(pq, jobs[*job_ct]);
        (*job_ct)++;
    }
    return (*job_ct == N ? __FLT_MAX__ : jobs[*job_ct]->arrival);
}

void fcfs_scheduling(Node** jobs, int lenght) {
    char** exec_IDs = malloc(100000 * sizeof(char*));
    float exec_period[100000][2];
    int exec_idx = 0;

    float current_time = jobs[0]->arrival;
    int job_ct = 0;

    while (job_ct < lenght){
        jobs[job_ct]->first_schedule = max(current_time, jobs[job_ct]->arrival);
        jobs[job_ct]->completion_time = jobs[job_ct]->first_schedule + jobs[job_ct]->burst;
        current_time = jobs[job_ct]->completion_time;
        

        exec_IDs[exec_idx] = strdup(jobs[job_ct]->PID);
        exec_period[exec_idx][0] = jobs[job_ct]->first_schedule;
        exec_period[exec_idx][1] = jobs[job_ct]->completion_time;
        exec_idx++;

        job_ct++;
    }

    print(jobs, lenght, exec_IDs, exec_period, exec_idx);
}

void sjf_scheduling(Node** jobs, int N) {
    int job_ct = 0;
    float current_time = jobs[0]->arrival, temp = current_time;
    

    char** exec_IDs = malloc(100000 * sizeof(char*));
    float exec_period[100000][2];
    int exec_idx = 0;

    PriorityQueue* pq = createPriorityQueue(N, cmp_burst);
    while(job_ct < N) {
        // while(job_ct < N && jobs[job_ct]->arrival <= current_time){
        //     enqueue(pq, jobs[job_ct]);
        //     job_ct++;
        // }

        float next_arrival = add_jobs(pq, jobs, N, current_time, &job_ct);

        while(!isEmpty(pq)) {
            Node* current_job = dequeue(pq);

            current_job->first_schedule = current_time;
            current_job->completion_time = current_time + current_job->remaining_burst;

            current_time += current_job->remaining_burst;

            
            exec_IDs[exec_idx] = strdup(current_job->PID);
            exec_period[exec_idx][0] = current_job->first_schedule;
            exec_period[exec_idx][1] = current_job->completion_time;
            exec_idx++;

            next_arrival = add_jobs(pq, jobs, N, current_time, &job_ct);
            
        }
        current_time = next_arrival;
    }

    print(jobs, N, exec_IDs, exec_period, exec_idx);

}



void stcf_scheduling(Node** jobs, int N) {
    // printf("STCF Scheduling :\n");

    int job_ct = 0;
    float current_time = jobs[0]->arrival;

    char** exec_IDs = malloc(100000 * sizeof(char*));
    float exec_period[100000][2];
    int exec_idx = 0;

    PriorityQueue *pq = createPriorityQueue(N, cmp_burst);

    while(job_ct < N) {
        // while(job_ct < N && jobs[job_ct]->arrival <= current_time){
        //     // printf("Inserting %s\n", jobs[job_ct]->PID);
        //     enqueue(pq, jobs[job_ct]);
        //     job_ct++;
        // }

        float next_arrival = add_jobs(pq, jobs, N, current_time, &job_ct);

        while(!isEmpty(pq)) {
            Node* current_job = dequeue(pq);

            if(current_job->first_schedule == -1) {
                current_job->first_schedule = current_time;
            }

            exec_IDs[exec_idx] = strdup(current_job->PID);
            exec_period[exec_idx][0] = current_time;

            if(current_job->remaining_burst > (next_arrival - current_time) ){
                current_job->remaining_burst -= (next_arrival - current_time);
                // printf("%s --> %f\n", current_job->PID, current_job->completion_time);
                // printf("%s --> %f\n", current_job->PID, current_job->remaining_burst);
                enqueue(pq, current_job);
                current_time = next_arrival;
            } else {
                current_time += current_job->remaining_burst;

                current_job->remaining_burst = 0;
                current_job->completion_time = current_time;
                
            }
            exec_period[exec_idx][1] = current_time;
            exec_idx++;

            // while(job_ct < N && jobs[job_ct]->arrival <= current_time){
            //     // printf("Inserting %s\n", jobs[job_ct]->PID);
            //     enqueue(pq, jobs[job_ct]);
            //     job_ct++;
            // }
            next_arrival = add_jobs(pq, jobs, N, current_time, &job_ct);

        }
        current_time = next_arrival;
    }
    print(jobs, N, exec_IDs, exec_period, exec_idx);

}

void rr_scheduling(Node** jobs, int N, float ts) {
    // printf("RR scheduling :\n");
    int job_ct = 0;
    float current_time = jobs[0]->arrival;

    char** exec_IDs = malloc(100000 * sizeof(char*));
    float exec_period[100000][2];
    int exec_idx = 0;

    Queue *q = createQueue(N);
    while(job_ct < N) {
        // printf("%f <= %f\n", jobs[job_ct]->arrival, current_time);
        // while(job_ct < N && jobs[job_ct]->arrival <= current_time) {
        //     enqueue(q, jobs[job_ct]);
        //     // printf("%s inserted in q at %f\n", jobs[job_ct]->PID, current_time);
        //     job_ct++;
        // }

        float next_arrival = add_jobs(q, jobs, N, current_time, &job_ct);
        
        while(!isEmpty(q)) {
            Node* current_job = dequeue(q);
            if(current_job->first_schedule == -1) {
                current_job->first_schedule = current_time;
            }
            float time_reqd = min(ts, current_job->remaining_burst);
            // printf("Time required for %s at %f = %f\n ", jobs[job_ct]->PID, current_time, time_reqd);

            exec_IDs[exec_idx] = strdup(current_job->PID);
            exec_period[exec_idx][0] = current_time;
            exec_period[exec_idx][1] = current_time + time_reqd;
            exec_idx++;

            current_time += time_reqd;

            // printf("%f <= %f\n", jobs[job_ct]->arrival, current_time);
            // while(job_ct < N && jobs[job_ct]->arrival <= current_time) {
            //     enqueue(q, jobs[job_ct]);
            //     // printf("%s inserted in q at %f\n", jobs[job_ct]->PID, current_time);
            //     job_ct++;
            // }

            next_arrival = add_jobs(q, jobs, N, current_time, &job_ct);

            current_job->remaining_burst -= time_reqd;
            current_job->remaining_burst == 0 ? current_job->completion_time = current_time : enqueue(q, current_job);
        }
        current_time = next_arrival;
    }

    print(jobs, N, exec_IDs, exec_period, exec_idx);
}

void empty_queues(Queue* q0, Queue* q1, Queue* q2) {
    while(!isEmpty(q1)) {
        Node* temp = dequeue(q1);
        enqueue(q0, temp);
    }
    while(!isEmpty(q2)) {
        Node* temp = dequeue(q2);
        enqueue(q2, temp);
    }
}
void mlfq_scheduling(Node** jobs, int N, float ts0, float ts1, float ts2, float b) {
    // printf("MLFQ");
    float current_time = jobs[0]->arrival;
    float next_boost = b;
    float temp_ts[3] = {ts0, ts1, ts2};
    int job_ct = 0;

    char** exec_IDs = malloc(100000 * sizeof(char*));
    float exec_period[100000][2];
    int exec_idx = 0;

    Queue* q0 = createQueue(N);
    Queue* q1 = createQueue(N);
    Queue* q2 = createQueue(N);


    while(job_ct < N) {
        // while(job_ct < N && jobs[job_ct]->arrival <= current_time) {
        //     enqueue(q0, jobs[job_ct]);
        //     job_ct++;
        // }
        float next_arrival = add_jobs(q0, jobs, N, current_time, &job_ct);

        while(!isEmpty(q0) || !isEmpty(q1) || !isEmpty(q2)) {
            while(!isEmpty(q0)) {
                Node* current_job = dequeue(q0);
                if(current_job->first_schedule == -1) {
                    current_job->first_schedule = current_time;
                }

                float actual_burst = min(ts0, current_job->remaining_burst);

                current_job->remaining_burst -= actual_burst;
                current_job->remaining_burst == 0 ? current_job->completion_time = current_time : enqueue(q1, current_job);


                exec_IDs[exec_idx] = strdup(current_job->PID);
                exec_period[exec_idx][0] = current_time;
                exec_period[exec_idx++][1] = current_time + actual_burst;

                current_time += actual_burst;

                
                if(next_boost < next_arrival) {
                    if(next_boost <= current_time) {
                        empty_queues(q0, q1, q2);
                        next_boost += b;
                    } 
                    if(next_arrival <= current_time) {
                        // while(job_ct < N && jobs[job_ct]->arrival <= current_time) {
                        //     enqueue(q0, jobs[job_ct]);
                        //     job_ct++;
                        // }
                        next_arrival = add_jobs(q0, jobs, N, current_time, &job_ct);
                    }
                } else {
                    if(next_arrival <= current_time) {
                        // while(job_ct < N && jobs[job_ct]->arrival <= current_time) {
                        //     enqueue(q0, jobs[job_ct]);
                        //     job_ct++;
                        // }
                        next_arrival = add_jobs(q0, jobs, N, current_time, &job_ct);
                    }
                    if(next_boost <= current_time) {
                        // while(!isEmpty(q1)) {
                        //     Node* temp = dequeue(q1);
                        //     enqueue(q0, temp);
                        // }
                        // while(!isEmpty(q2)) {
                        //     Node* temp = dequeue(q2);
                        //     enqueue(q0, temp);
                        // }
                        empty_queues(q0, q1, q2);
                        next_boost += b;
                    }
                }
            }

            while(isEmpty(q0) && !isEmpty(q1)) {
                Node* current_job = front(q1);

                float expected_burst = min(ts1, current_job->remaining_burst);
                float actual_burst = min(expected_burst, next_boost - current_time);

                current_job->remaining_burst -= actual_burst;

                if(current_job->remaining_burst == 0) {
                    current_job->completion_time = current_time + actual_burst;
                    dequeue(q1);
                } else if(actual_burst == expected_burst) {
                    current_job = dequeue(q1);
                    enqueue(q2, current_job);
                }

                exec_IDs[exec_idx] = strdup(current_job->PID);
                exec_period[exec_idx][0] = current_time;
                exec_period[exec_idx++][1] = current_time + actual_burst;

                current_time += actual_burst;

                while(job_ct < N && jobs[job_ct]->arrival <= current_time) {
                    enqueue(q0, jobs[job_ct++]);
                }
                next_arrival = add_jobs(q0, jobs, N, current_time, &job_ct);

                if(next_boost == current_time) {   
                    // while(!isEmpty(q1)) {
                    //     Node* temp = dequeue(q1);
                    //     enqueue(q0, temp);
                    // }
                    // while(!isEmpty(q2)) {
                    //     Node* temp = dequeue(q2);
                    //     enqueue(q0, temp);
                    // }
                    empty_queues(q0, q1, q2);
                    next_boost += b;
                }
            }

            while (isEmpty(q0) && isEmpty(q1) && !isEmpty(q2)) {
                // printf("Entered in q2\n");
                Node* current_job = front(q2);

                float expected_burst = min(ts2, current_job->remaining_burst);
                float actual_burst = min(expected_burst, next_boost - current_time);

                current_job->remaining_burst -= actual_burst;

                if(current_job->remaining_burst == 0) {
                    current_job->completion_time = current_time + actual_burst;
                    dequeue(q2);
                } else if(actual_burst == expected_burst) {
                    current_job = dequeue(q2);
                    enqueue(q2, current_job);
                }
            
                exec_IDs[exec_idx] = strdup(current_job->PID);
                exec_period[exec_idx][0] = current_time;
                exec_period[exec_idx++][1] = current_time + actual_burst;

                current_time += actual_burst;

                // while(job_ct < N && jobs[job_ct]->arrival <= current_time) {
                //     enqueue(q0, jobs[job_ct++]);
                // }
                next_arrival = add_jobs(q0, jobs, N, current_time, &job_ct);

                if(next_boost == current_time) {   
                    // while(!isEmpty(q2)) {
                    //     Node* temp = dequeue(q2);
                    //     enqueue(q0, temp);
                    // }
                    empty_queues(q0, q1, q2);
                    next_boost += b;
                }
            }
        }
        current_time = next_arrival;
        // printf("current_time = %f, next_arrival = %f\n", current_time, next_arrival);
    }
    // printf("%d\n", exec_idx);
    // printf("current_time = %f, next_arrival = \n", current_time);
    print(jobs, N, exec_IDs, exec_period, exec_idx);

}

int main (int argc, char* argv[]) {
    if(freopen(argv[1], "r", stdin) == NULL) {
        perror("Couldn't find input.txt");
        return EXIT_FAILURE;
    }
    if(freopen(argv[2], "w", stdout) == NULL) {
        perror("Couldn't find output.txt");
        return EXIT_FAILURE;
    } 
    
    int N = 0;

    Node** jobs = (Node**)malloc(MAX_JOBS * sizeof(Node*));

    while(true) {  
        char PID[10];
        float t1, t2;
        if(scanf("%s %f %f", PID, &t1, &t2) == EOF) {
            break;
        }
        
        Node* job_i = (Node*)malloc(sizeof(Node));

        job_i->PID = strdup(PID);
        job_i->arrival = t1;
        job_i->burst = t2;
        job_i->remaining_burst = t2;
        job_i->is_complete = false;
        job_i->first_schedule = -1;
        job_i->completion_time = -1;

        jobs[N++] = job_i;
    }

    for(int i = N; i < MAX_JOBS; i++) {
        free(jobs[i]);
    }

    sort(jobs, 0, N, cmp_arrival);

    float TsRR = atoi(argv[3]);
    float TsMLFQ1 = atoi(argv[4]), TsMLFQ2 = atoi(argv[5]), TsMLFQ3 = atoi(argv[6]), BMLFQ = atoi(argv[7]);

    fcfs_scheduling(jobs, N);
    // printf("\n");
    rr_scheduling(jobs, N, TsRR);
    // printf("\n");
    sjf_scheduling(jobs, N);
    // printf("\n");
    stcf_scheduling(jobs, N);
    // printf("\n");
    mlfq_scheduling(jobs, N, TsMLFQ1, TsMLFQ2, TsMLFQ3, BMLFQ);
    return 0;
    
}
