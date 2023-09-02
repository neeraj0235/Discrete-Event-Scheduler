typedef struct Node {
    char* PID;
    double arrival;
    double burst;
    double remaining_burst;
    bool is_complete;
    double first_schedule;
    double completion_time;
} Node;
