// Process accounting CPU-usage, IO, nthreads 
// R. Ansari, Univ. Paris-Sud LAL-IN2P3/CNRS 
//    V 1.0    July 2013
//    V 2.0    Dec  2015   
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

//! information on resource usage by a given process 
class ProcessInfo {
public:
  //! constructor with optional specification of the processId and parent processId
  ProcessInfo(pid_t pid=0, pid_t ppid=0);
  //! Copy constructor 
  ProcessInfo(ProcessInfo const& a) { CopyFrom(a); }
  //! return the process Id 
  inline pid_t getPId() { return pid_; }
  //! return the parent process Id 
  inline pid_t getParentPId() { return ppid_; }

  //! copy operator  
  inline ProcessInfo& operator =  (ProcessInfo const& a)
    { return CopyFrom(a); }
  //! copy member function 
  ProcessInfo& CopyFrom(ProcessInfo const& a);
  //! print the process ressource usage info 
  ostream& Print(ostream& os, bool fglong=true) const;

// --- attributes 
pid_t pid_;   // process id
pid_t ppid_;  // parent process id
time_t start_time_;   // process start-time 
time_t end_time_;   // process end-time 
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
};

//! print operator for ProcessInfo
inline ostream& operator << (ostream& os, ProcessInfo const& pinfo) 
{ return pinfo.Print(os); }

//------------------------------------------------------------------------------------------------
//! consolidated (cumulated) information on resource usage by a given process and its children 
class CumulProcessInfo {
public:
  //! default constructor
  CumulProcessInfo();
  //! constructor from a given ProcessInfo object 
  CumulProcessInfo(ProcessInfo const& a);
  //! copy constructor 
  CumulProcessInfo(CumulProcessInfo const& a)
  { CopyFrom(a); }
  //! copy operator  
  inline CumulProcessInfo& operator =  (CumulProcessInfo const& a)
    { return CopyFrom(a); }
  
  //! copy member function   
  CumulProcessInfo& CopyFrom(CumulProcessInfo const& a);
  
  //! print the cumulated process ressource usage info 
  ostream& Print(ostream& os, bool fglong=true) const;
  
  CumulProcessInfo& Cumul(ProcessInfo const& a);
  CumulProcessInfo& Add(CumulProcessInfo const& a);

  ostream& LogFileOutput(ostream& os, double tmlog) const;
  static ostream& LogFileHeader(ostream& os);
  
  //! Initialize from a ProcessInfo object 
  void InitFrom(ProcessInfo const& a);
  void Reset();
  
  // --- attributes 
pid_t pid_;   // process id
pid_t ppid_;  // parent process id
time_t start_time_;   // start-time of all processes 
time_t end_time_;   // end-time of all processes
long elapsed_time_ms_;  // elapsed time since start of the accounting 
bool fg_running_;   // true : still running  
string command_;  // command of the executed 

long running_cputime_ms_;   // total cpu time in milli-second for all running processes 
long running_utime_ms_;     // total cpu-user time in milli-second for all running processes 
long running_systime_ms_;   // total cpu-system time in milli-second for all running processes 
  
int running_nprocs_;   // total number of running child processes 
int running_nthr_;   // number of threads of running processes
long running_virtmemsz_kb_;   // virtual memory size in kb  (vsz) for all running processes 
long running_resmemsz_kb_;   //  resident memory size in kb   (rss) for all running processes
long running_io_read_bytes_;    //  I/O , bytes read in kb for all running processes
long running_io_write_bytes_;    //  I/O , bytes witten in kb for all running processes

long total_cputime_ms_;   // total cpu time in milli-second for all  processes 
long total_utime_ms_;     // total cpu-user time in milli-second for all  processes 
long total_systime_ms_;   // total cpu-system time in milli-second for all  processes 

int total_nprocs_;   // total number of child processes 
int total_nthr_;   // total number of child processes
long total_io_read_bytes_;    //  I/O , bytes read in kb for all  processes
long total_io_write_bytes_;    //  I/O , bytes witten in kb for all  processes
  
long max_virtmemsz_kb_;   // maximum value of virtual memory size in kb  (vsz) for all  processes 
long max_resmemsz_kb_;   //  maximum value of resident memory size in kb   (rss) for  processes 
  
};

//! print operator for CumulProcessInfo
inline ostream& operator << (ostream& os, CumulProcessInfo const& pinfo) 
{ return pinfo.Print(os); }

//------------------------------------------------------------------------------------------------
//! class to get the ressource usage by all children of the parent process of the currennt process itself  
class MyProcessAccounting {
public:
  //! constructor - initialize the object using the current process id and its parents process id 
  MyProcessAccounting();
  //! update the list of al children processes with corresponding ressource list 
  void update();
  //! return the cumulated ressource usage by all the processes child of the parent of the current process, including itself 
  inline CumulProcessInfo getSummary()
  { return getChildrenSummary(myppid_); }
  //! return the cumulated ressource usage by the the current process
  inline CumulProcessInfo getSelfSummary()
  { return getProcessSummary(mypid_); }
  //! return the cumulated ressource usage by the parent of the current process
  inline CumulProcessInfo getParentSummary()
  { return getProcessSummary(myppid_); }
  //! return the ressource usage by the process pid itself 
  CumulProcessInfo getProcessSummary(pid_t pid);
  //! return the cumulated ressource usage by all children of process pid  
  CumulProcessInfo getChildrenSummary(pid_t pid);
  //! return the cumulated ressource usage by all children of process pid and the process pid itself 
  CumulProcessInfo getFullSummary(pid_t pid);
  
  //---- attributes 
  map<pid_t, ProcessInfo> myprocs_;   // all the children of the parent process 
  uid_t myuid_;   // our real user id
  uid_t myeuid_;  // our effective process id
  pid_t mypid_;   // process id
  pid_t myppid_;  // parent process id

  time_t start_time0_; 

  static long decode_timestr(string &s); 
};

void SplitStringToVString(string const & s,vector<string>& vs,char separator=' ');

#endif
