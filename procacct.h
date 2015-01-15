// Process accounting CPU-usage, IO, nthreads 
// R. Ansari, Univ. Paris-Sud LAL-IN2P3/CNRS 
//    V 1.0    July 2013 
#ifndef PROCACCT_H_SEEN
#define PROCACCT_H_SEEN

#include <iostream>
#include <string>
#include <vector>
#include <map> 

#include <time.h> 
#include <sys/types.h>
#include <unistd.h>

using namespace std;

class ProcessInfo {
public: 
  ProcessInfo(pid_t pid=0, pid_t ppid=0);
  ProcessInfo(ProcessInfo const& a); 
  inline pid_t getPId() { return pid_; }
  inline pid_t getParentPId() { return ppid_; }
  
  inline ProcessInfo& operator =  (ProcessInfo const& a)
    { return CopyFrom(a); }
  ProcessInfo& CopyFrom(ProcessInfo const& a);
  ostream& Print(ostream& os) const;
  
  ProcessInfo& Cumul(ProcessInfo const& a);

pid_t pid_;   // process id
pid_t ppid_;  // parent process id
time_t start_time_;   // process start-time 
time_t end_time_;   // process start-time 
long cputime_ms_;   // total cpu time in milli-second   
long utime_ms_;   // total cpu-user time in milli-second   
long systime_ms_;   // total cpu-system time in milli-second 
long elapsed_time_ms_;  // total elapsed time
long virtmemsz_kb_;   // virtual memory size in kb  (vsz)
long resmemsz_kb_;   //  resident memory size in kb   (rss)
int  nthr_;   // number of threads 
long io_read_bytes_;    // total I/O , bytes read 
long io_write_bytes_;    // total I/O , bytes witten 
bool fg_running_;   // true : still running  
string command_;  // command being executed 
vector<pid_t> children_;  // children process pid's
// les valeurs suivantes ne sont utilisées que par cumul 
int tot_nchildren_;   // total number of child processes (used by Cumul only) 
int running_nchildren_;   // total number of running child processes (used by Cumul only) 
int running_nthr_;   // number of threads of running processes 
};

inline ostream& operator << (ostream& os, ProcessInfo const& pinfo) 
{ return pinfo.Print(os); }

class MyProcessAccounting {
public:
  MyProcessAccounting();
  void update();
  inline ProcessInfo getParentSummary()
  { return getSummary(myppid_); }
  inline ProcessInfo getSelfSummary()
  { return getSummary(mypid_); }
  ProcessInfo getSummary(pid_t pid);
  //

  map<pid_t, ProcessInfo> myprocs_;
  uid_t myuid_;   // our real user id
  uid_t myeuid_;  // our effective process id
  pid_t mypid_;   // process id
  pid_t myppid_;  // parent process id

  time_t start_time0_; 

  static long decode_timestr(string &s); 
};

void SplitStringToVString(string const & s,vector<string>& vs,char separator=' ');

#endif
