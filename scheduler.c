#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
// #include "Node.h"
// #include "q.h"
#include "pq.h"

#define min(a, b) (a > b ? b : a)
#define max(a, b) (a > b ? a : b)

#define enqueue(obj, node) _Generic(obj, Queue* : enqueue_q, PriorityQueue* : enqueue_pq) (obj, node)
#define dequeue(obj) _Generic(obj, Queue* : dequeue_q, PriorityQueue* : dequeue_pq) (obj)
#define isEmpty(obj) _Generic(obj, Queue* : isEmpty_q, PriorityQueue* : isEmpty_pq) (obj)


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
    return a->arrival < b->arrival;
}

bool cmp_burst(Node* a, Node* b) {
    return a->remaining_burst < b->remaining_burst;
}

void sjf_scheduling(Node** jobs, int N) {
    int current_time = 1;
    int jobs_completed = 0;


    while(jobs_completed != N) {
        int next_arrival = current_time;
        for(int i = 0; i < N; i++) {
            if(!jobs[i]->is_complete) {
                next_arrival = min(next_arrival, jobs[i]->arrival);
            }
        }
       
        current_time = max(next_arrival, current_time);

        int burst_time = __INT32_MAX__, shortest_idx;
        for(int i = 0; i < N; i++) {
            if(!jobs[i]->is_complete && jobs[i]->arrival <= current_time && jobs[i]->burst < burst_time) {
                burst_time = jobs[i]->burst;
                shortest_idx = i;
            }
        }

        current_time += burst_time;
        jobs[shortest_idx]->is_complete = true;
        jobs_completed++;

        printf("%s started executing at %d and completed at %d\n", jobs[shortest_idx]->PID, current_time - burst_time, current_time);
    }



    for(int i = 0; i < N; i++) {
        jobs[i]->is_complete = false;
    }

}

void print_report_and_reset(Node** jobs, int N) {
    for(int i = 0; i < N; i++) {
        printf("%s : \n", jobs[i]->PID);
        printf("Arrival = %lf, first_schedule = %lf, completion_time = %lf\n", jobs[i]->arrival, jobs[i]->first_schedule, jobs[i]->completion_time);
    }

    for(int i = 0; i < N; i++) {
        jobs[i]->remaining_burst = jobs[i]->burst;
        jobs[i]->first_schedule = -1;
        jobs[i]->completion_time = -1;
    }
}

void stcf_scheduling(Node** jobs, int N) {
    // sort(jobs, 0, N, cmp_arrival);

    // for(int i = 0; i < N; i ++) {
    //     printf("%s\n", jobs[i]->PID);
    // }

    int job_ct = 0;
    double current_time = jobs[0]->arrival;
    // printf("current_time = %f\n", current_time);
    PriorityQueue *pq = createPriorityQueue(N, cmp_burst);

        // for(int i = 0; i < N; i++) {
        //     enqueue(pq, jobs[i]);
        // }        
        // for(int i = 1; i <= pq->size; i++) {
        //     printf("PID --> %s,    rem_burst = %lf\n", pq->heap[i]->PID, pq->heap[i]->remaining_burst);
        // }
        Node* current_job = dequeue(pq);
        // current_job->remaining_burst -= 90;
        // enqueue(pq, current_job);
        // printf("IN PQ");
        // for(int i = 1; i <= pq->size; i++) {
        //     printf("PID --> %s,    rem_burst = %lf\n", pq->heap[i]->PID, pq->heap[i]->remaining_burst);
        // }
        // dequeue(pq);

    while(job_ct != N) {

        while(job_ct < N && jobs[job_ct]->arrival == current_time){
            // printf("Inserting %s\n", jobs[job_ct]->PID);
            enqueue(pq, jobs[job_ct]);
            job_ct++;
        }
        // printf("%d\n", job_ct);
        // printf("IN PQ\n");
        // for(int i = 1; i <= pq->size; i++) {
        //     printf("PID --> %s,    rem_burst = %lf\n", pq->heap[i]->PID, pq->heap[i]->remaining_burst);
        // }
        double next_arrival = (job_ct == N ? 1000000 : jobs[job_ct]->arrival);
        // printf("%d\n", pq->size);
        // printf("current_time = %f\n", current_time);
        // printf("current_job.priority = %f\n", current_job->remaining_burst);
        // printf("next_arrival = %f\n", next_arrival);
        // printf("Popping %s\n", current_job.data);
        while(!isEmpty(pq) && current_time < next_arrival) {
            Node* current_job = dequeue(pq);
            if(current_job->first_schedule == -1) {
                current_job->first_schedule = current_time;
            }

            if(current_job->remaining_burst > (next_arrival - current_time) ){
                current_job->remaining_burst -= (next_arrival - current_time);
                // printf("%s --> %lf\n", current_job->PID, current_job->completion_time);
                // printf("%s --> %lf\n", current_job->PID, current_job->remaining_burst);
                enqueue(pq, current_job);
                break;
            }
            // printf("%s started at %f and ended at %f\n", current_job->remaining_burst, current_time, current_time + current_job->remaining_burst);
            
            // printf("current_time = %f\n", current_time);
            current_time += current_job->remaining_burst;
            // printf("current_time = %f\n", current_time);
            current_job->remaining_burst = 0;
            current_job->completion_time = current_time;
            // printf("%s --> %lf\n", current_job->PID, current_job->completion_time);
        }
        current_time = next_arrival;
    }
    print_report_and_reset(jobs, N);

}

void rr_scheduling(Node** jobs, int N, double ts) {
    double current_time = jobs[0]->arrival;
    int jobs_ct = 0;

    Queue *q = createQueue(N);
    while(jobs_ct < N) {
        while(jobs_ct < N && jobs[jobs_ct]->arrival <= current_time) {
            enqueue(q, jobs[jobs_ct]);
            // printf("%s inserted in q at %lf\n", jobs[jobs_ct]->PID, current_time);
            jobs_ct++;
        }

        double next_arrival = (jobs_ct == N ? 100000 : jobs[jobs_ct]->arrival);
        while(!isEmpty(q)) {
            Node* current_job = dequeue(q);
            if(current_job->first_schedule == -1) {
                current_job->first_schedule = current_time;
            }
            double time_reqd = min(ts, current_job->remaining_burst);
            // printf("Time required for %s at %lf = %lf\n ", jobs[jobs_ct]->PID, current_time, time_reqd);
            current_time += time_reqd;
            
            while(jobs_ct < N && jobs[jobs_ct]->arrival <= current_time) {
                enqueue(q, jobs[jobs_ct]);
                // printf("%s inserted in q at %lf\n", jobs[jobs_ct]->PID, current_time);
                jobs_ct++;
            }

            next_arrival = (jobs_ct == N ? 100000 : jobs[jobs_ct]->arrival);

            // printf("%lf\n", current_time);
            current_job->remaining_burst -= time_reqd;
            if(current_job->remaining_burst == 0) {
                current_job->completion_time = current_time;
            } else {
                enqueue(q, current_job);
                // printf("%s re-inserted in q at %lf\n", jobs[jobs_ct]->PID, current_time);
            }
        }
        current_time = next_arrival;
    }

    print_report_and_reset(jobs, N);
}

void mlfq_scheduling(Node** jobs, int N, double ts1, double ts2, double ts3, double b) {
    double current_time = jobs[0]->arrival;
    double next_boost = b;
    int job_ct = 0;

    Queue* q1 = createQueue(N);
    Queue* q2 = createQueue(N);
    Queue* q3 = createQueue(N);

    while(jobs[job_ct]->arrival <= current_time) {
        enqueue(q1, jobs[job_ct]);
        job_ct++;
    }

    printf("%d  %d  %d\n", q1->size, q2->size, q3->size);
    
    double next_arrival = (job_ct == N ? 100000 : jobs[job_ct]->arrival);

    printf("%ld\n", next_arrival);


    while(!isEmpty(q1) || !isEmpty(q2) || !isEmpty(q3)) {
        double time_allowed = min(next_arrival, next_boost);

        while(!isEmpty(q1)) {
            // printf("Entered in q1\n");
            Node* current_job = dequeue(q1);
            if(current_job->first_schedule == -1) {
                current_job->first_schedule = current_time;
            }

            double time_reqd = min(ts1, current_job->remaining_burst);
            current_time += time_reqd;

            current_job->remaining_burst -= time_reqd;

            current_job->remaining_burst == 0 ? current_job->completion_time = current_time : enqueue(q2, current_job);

            if(current_time >= time_allowed) {
                // printf("Entered in if-else\n");
                if(next_boost == time_allowed) {
                    while(!isEmpty(q2)) {
                        Node* temp = dequeue(q2);
                        enqueue(q1, temp);
                    }
                    while(!isEmpty(q3)) {
                        Node* temp = dequeue(q3);
                        enqueue(q1, temp);
                    }
                    next_boost += b;
                } else {
                    while(job_ct < N && jobs[job_ct]->arrival == next_arrival) {
                        enqueue(q1, jobs[job_ct]);
                        job_ct++;
                    }
                    next_arrival = (job_ct == N ? 100000 : jobs[job_ct]->arrival);
                }
            }
        }

        while(!isEmpty(q2)) {
            // printf("Entered in q2\n");
            Node* current_job = dequeue(q2);

            double time_reqd = min(ts2, current_job->remaining_burst);
            if(current_time + time_reqd > time_allowed) {
                enqueue(q1, current_job);
                if(time_allowed == next_boost) {
                    current_time = time_allowed;
                    while(!isEmpty(q2)) {
                        Node* temp = dequeue(q2);
                        enqueue(q1, temp);
                    }
                } 
                break;
            }

            current_time += time_reqd;

            current_job->remaining_burst -= time_reqd;
            current_job->remaining_burst == 0 ? current_job->completion_time = current_time : enqueue(q3, current_job);
        }

        while (!isEmpty(q3)) {
            // printf("Entered in q3\n");
            Node* current_job = dequeue(q3);

            double time_reqd = min(ts3, current_job->remaining_burst);

            if(current_time + time_reqd > time_allowed) {
                if(time_allowed == next_boost) {
                    current_time = time_allowed;
                    enqueue(q1, current_job);
                    while(!isEmpty(q2)) {
                        Node* temp = dequeue(q2);
                        enqueue(q1, temp);
                    }
                    next_boost += b;
                } 
                break;
            }

            current_time += time_reqd;

            current_job->remaining_burst -= time_reqd;
            current_job->remaining_burst == 0 ? current_job->completion_time = current_time : enqueue(q3, current_job);
        }
    }

    for(int i = 0; i < N; i++) {
        printf("%s : \n", jobs[i]->PID);
        printf("Arrival = %lf, first_schedule = %lf, completion_time = %lf\n", jobs[i]->arrival, jobs[i]->first_schedule, jobs[i]->completion_time);
    }

    for(int i = 0; i < N; i++) {
        jobs[i]->remaining_burst = jobs[i]->burst;
        jobs[i]->first_schedule = -1;
        jobs[i]->completion_time = -1;
    }

}

int main () {
    // freopen("output.txt", "a+", stdout); 
    // freopen("inpTestcase1.txt", "r", stdin);
    int N;
    scanf("%d", &N);

    Node** jobs = (Node**)malloc(N * sizeof(Node*));
    
    for(int i = 0; i < N; i++) {  
        char PID[10];
        double t1, t2;
        scanf("%s %lf %lf", PID, &t1, &t2);
        
        Node* job_i = (Node*)malloc(sizeof(Node));

        job_i->PID = strdup(PID);
        job_i->arrival = t1;
        job_i->burst = t2;
        job_i->remaining_burst = t2;
        job_i->is_complete = false;
        job_i->first_schedule = -1;
        job_i->completion_time = -1;

        jobs[i] = job_i;
        // printf("%d --> %s\n", i, jobs[i]->PID);
    }

    sort(jobs, 0, N, cmp_arrival);
    // for(int i = 0; i < N; i ++) {
    //     printf("%s\n", jobs[i]->PID);
    // }

    printf("\n");

    // sjf_scheduling(jobs, N);
    // rr_scheduling(jobs, N, 6);
    // stcf_scheduling(jobs, N);
    // mlfq_scheduling(jobs, N, 5, 3, 2, 8);
    return 0;
    
}
