#include "jobs.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#ifndef WCONTINUED
#define WCONTINUED 0
#endif


// i need a list to put the processes in 
#define MAX_JOBS 69  // assuming the jobs will not go over this
static jobs_t job_list[MAX_JOBS];
static int number_of_jobs = 0; // this is to track the process id
static int next = 1;

void initialize(void){
    number_of_jobs = 0;
    next = 1;
}

 int adding_jobs ( pid_t pgid, const char *cmd){
    jobs_t *j = &job_list[number_of_jobs++];
    j->id    = next++;
    j->pgid  = pgid;
    j->cmd   = strdup(cmd);            
    j->status = Running;
    return j->id;
 }

//* Jobs status check the process to see the status
static void job_check_status(jobs_t *j){
    int status; 
    pid_t process = waitpid(-j->pgid, &status, WNOHANG | WUNTRACED | WCONTINUED);
    if (process > 0) {
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            j->status = Done;
        } else if (WIFSTOPPED(status)) {
            j->status = Stopped;
        } else if (WIFCONTINUED(status)) {
            j->status = Running;
        }
    }else{
        return;
    }
}

//*Remove jobs when they are done 
static void remove_job_index(int index){
    free(job_list[index].cmd);
    if(index != number_of_jobs - 1){
        job_list[index] = job_list[number_of_jobs - 1];
    }
    number_of_jobs--;
}

static void removing_done(void){
    for (int i = 0; i < number_of_jobs; ++i) { // here i am checking if the jobs are Done yet or not 
        job_check_status(&job_list[i]);
        if (job_list[i].status == Done) {
            remove_job_index(i);
            i--;  // re-checking 
        }
    }
}
//*Print the jobs after checking if it's done or not 
void printing_checking_status(void){ 
    removing_done();
    for (int i = 0; i < number_of_jobs; i++) {
        const char *status_str;
        if (job_list[i].status == Running) {
            status_str = "Running";
        }
        else if (job_list[i].status == Stopped) {
            status_str = "Stopped";
        }
        else {  
            status_str = "Done";
        }
        
        printf("[%d] %-8s %s &\n",
            job_list[i].id, 
            status_str,       
            job_list[i].cmd);    
    }
}

//* im starting the bg and fg part in milestone 6
jobs_t *find_job_id(int id){
    for (int i = 0; i < number_of_jobs; ++i) {
        if (job_list[i].id == id)
            return &job_list[i];
    }
    return NULL;
}

//*this is for the fg to remove the job by id 
void remove_job(int job_id) {
  for (int i = 0; i < number_of_jobs; i++) {
    if (job_list[i].id == job_id) {
      remove_job_index(i);
      return;
    }
  }
}