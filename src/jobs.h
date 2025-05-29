#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>

typedef enum  {Stopped,Running,Done} jobs_status;

typedef struct jobs{
    int id;
    pid_t pgid;
    jobs_status status;
    char *cmd;
}jobs_t;


#define MAX_JOBS 69 

void initialize(void);
int  adding_jobs(pid_t pgid, const char *cmd);
void printing_checking_status(void);
jobs_t *find_job_id(int id);

#endif 