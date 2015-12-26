#include <stdio.h> 
#include <stdlib.h> 
#include <fstream>
#include <iomanip>
#include <string.h>

#include "procacct.h"

/*--Methode--*/
ProcessInfo::ProcessInfo(pid_t pid,  pid_t ppid)
  : command_("")
{
  pid_=pid; ppid_=ppid;
  start_time_=end_time_=time(NULL);
  cputime_ms_=utime_ms_=systime_ms_=0;
  elapsed_time_ms_=0;
  resmemsz_kb_=virtmemsz_kb_=0; 
  nthr_=0;
  io_read_bytes_=io_write_bytes_=0;
  fg_running_=false;
}


/*--Methode--*/
ProcessInfo& ProcessInfo::CopyFrom(ProcessInfo const & a) 
{
  pid_=a.pid_; ppid_=a.ppid_;
  start_time_ = a.start_time_; end_time_ = a.end_time_;
  cputime_ms_ = a.cputime_ms_;  
  utime_ms_ = a.utime_ms_; systime_ms_ = a.systime_ms_;
  elapsed_time_ms_ = a.elapsed_time_ms_;
  resmemsz_kb_ = a.resmemsz_kb_;  virtmemsz_kb_ = a.virtmemsz_kb_; 
  nthr_=a.nthr_;
  io_read_bytes_ = a.io_read_bytes_; io_write_bytes_ = a.io_write_bytes_;
  fg_running_ = a.fg_running_;
  command_ = a.command_;
  children_ = a.children_;

  return (*this);
}


/*--Methode--*/
ostream& ProcessInfo::Print(ostream& os, bool fglong)  const 
{
  struct tm gtime;
  char starttime[48];
  char endtime[48];
  gmtime_r(&start_time_,&gtime);
  asctime_r(&gtime,starttime);
  gmtime_r(&end_time_,&gtime);
  asctime_r(&gtime,endtime);
  if (fglong) {
  // single process info
  double telapsed=((elapsed_time_ms_>0)?(double)elapsed_time_ms_:1.);
  os << "---pid,ppid=" << pid_ << "," << ppid_ << " command="<< command_
     << "\n elapsed (s)= " << (double)elapsed_time_ms_/1000. << " NThr=" << nthr_ 
     << "\n CPU (s) total,user,sys=" << (double)cputime_ms_/1000. << "," << (double)utime_ms_/1000. << "," << (double)systime_ms_/1000.
     << " %CPU=" << 100.*cputime_ms_/telapsed << "\n  mem (kb) (vsz,rss)= " << virtmemsz_kb_ << "," << resmemsz_kb_
     << " IO R/W (kb)= " << io_read_bytes_/1024 << "," << io_write_bytes_/1024 << endl;
  }
  else {
    os << pid_ << "," << ppid_ << "  " << elapsed_time_ms_ << nthr_ << " " << "  " << cputime_ms_ << "," << utime_ms_ << "," << systime_ms_
       << " " << virtmemsz_kb_ << "," << resmemsz_kb_ << " " 
       << io_read_bytes_ << "," << io_write_bytes_ << " " << command_ << endl;
  }
  return os;
}

//------------------------------------------------------------------------------------------------
//------------------------------------  class CumulProcessInfo -----------------------------------
//------------------------------------------------------------------------------------------------
/*--Methode--*/
CumulProcessInfo::CumulProcessInfo()
{
  Reset();
}

/*--Methode--*/
CumulProcessInfo::CumulProcessInfo(ProcessInfo const& a)
{
  InitFrom(a);
}

/*--Methode--*/
void CumulProcessInfo::Reset()
{
  pid_=0; ppid_=0;
  start_time_=end_time_=time(NULL);  
  elapsed_time_ms_ = 0;
  fg_running_ = false;
  command_ = "";

  total_cputime_ms_ = 0;  
  total_utime_ms_ = 0;
  total_systime_ms_ = 0;
  
  total_nprocs_ = 0;
  total_nthr_=0;
  total_io_read_bytes_ = 0;
  total_io_write_bytes_ = 0;

  max_virtmemsz_kb_ = 0; 
  max_resmemsz_kb_ = 0;
  
  running_cputime_ms_ = 0;  
  running_utime_ms_ = 0;
  running_systime_ms_ = 0;
  
  running_virtmemsz_kb_ = 0; 
  running_resmemsz_kb_ = 0;  
  running_nthr_ = 0;
  running_io_read_bytes_ = 0;
  running_io_write_bytes_ = 0;
  running_nprocs_ = 0;

  return;
}

/*--Methode--*/
void CumulProcessInfo::InitFrom(ProcessInfo const& a)
{
  pid_=a.pid_; ppid_=a.ppid_;
  start_time_ = a.start_time_; end_time_ = a.end_time_;
  
  elapsed_time_ms_ = a.elapsed_time_ms_;
  fg_running_ = a.fg_running_;
  command_ = a.command_;

  total_cputime_ms_ = a.cputime_ms_;  
  total_utime_ms_ = a.utime_ms_;
  total_systime_ms_ = a.systime_ms_;
  
  total_nprocs_ = 1;
  total_nthr_=a.nthr_;
  total_io_read_bytes_ = a.io_read_bytes_;
  total_io_write_bytes_ = a.io_write_bytes_;

  max_virtmemsz_kb_ = a.virtmemsz_kb_; 
  max_resmemsz_kb_ = a.resmemsz_kb_;  

  if (a.fg_running_) {
    running_cputime_ms_ = a.cputime_ms_;  
    running_utime_ms_ = a.utime_ms_;
    running_systime_ms_ = a.systime_ms_;
    
    running_virtmemsz_kb_ = a.virtmemsz_kb_; 
    running_resmemsz_kb_ = a.resmemsz_kb_;  
    running_nthr_ = a.nthr_;
    running_io_read_bytes_ = a.io_read_bytes_;
    running_io_write_bytes_ = a.io_write_bytes_;
    running_nprocs_ = 1;
  }
  else {
    running_cputime_ms_ = 0;  
    running_utime_ms_ = 0;
    running_systime_ms_ = 0;
    
    running_virtmemsz_kb_ = 0; 
    running_resmemsz_kb_ = 0;  
    running_nthr_ = 0;
    running_io_read_bytes_ = 0;
    running_io_write_bytes_ = 0;
    running_nprocs_ = 0;
  }

}

/*--Methode--*/
CumulProcessInfo& CumulProcessInfo::CopyFrom(CumulProcessInfo const& a)
{
  pid_=a.pid_; ppid_=a.ppid_;
  start_time_ = a.start_time_; end_time_ = a.end_time_;
  
  elapsed_time_ms_ = a.elapsed_time_ms_;
  fg_running_ = a.fg_running_;
  command_ = a.command_;

  total_cputime_ms_ = a.total_cputime_ms_;  
  total_utime_ms_ = a.total_utime_ms_;
  total_systime_ms_ = a.total_systime_ms_;
  
  total_nprocs_ = a.total_nprocs_;
  total_nthr_=a.total_nthr_;
  total_io_read_bytes_ = a.total_io_read_bytes_;
  total_io_write_bytes_ = a.total_io_write_bytes_;

  max_virtmemsz_kb_ = a.max_virtmemsz_kb_; 
  max_resmemsz_kb_ = a.max_resmemsz_kb_;  

  running_cputime_ms_ = a.running_cputime_ms_;  
  running_utime_ms_ = a.running_utime_ms_;
  running_systime_ms_ = a.running_systime_ms_;
  
  running_virtmemsz_kb_ = a.running_virtmemsz_kb_; 
  running_resmemsz_kb_ = a.running_resmemsz_kb_;  
  running_nthr_=a.running_nthr_;
  running_io_read_bytes_ = a.running_io_read_bytes_;
  running_io_write_bytes_ = a.running_io_write_bytes_;
  running_nprocs_ = a.running_nprocs_;
  return (*this);
}


/*--Methode--*/
CumulProcessInfo& CumulProcessInfo::Cumul(ProcessInfo const & a) 
{
  if (a.elapsed_time_ms_ > elapsed_time_ms_)   elapsed_time_ms_ = a.elapsed_time_ms_;
  total_cputime_ms_ += a.cputime_ms_;  
  total_utime_ms_ += a.utime_ms_;
  total_systime_ms_ += a.systime_ms_;

  total_nprocs_ ++;
  total_nthr_ += a.nthr_;
  total_io_read_bytes_ += a.io_read_bytes_;
  total_io_write_bytes_ += a.io_write_bytes_;

  if (a.virtmemsz_kb_ > max_virtmemsz_kb_)  max_virtmemsz_kb_=a.virtmemsz_kb_;
  if (a.resmemsz_kb_ > max_resmemsz_kb_)  max_resmemsz_kb_=a.resmemsz_kb_;

  if (a.fg_running_) {   // ressources for running processes 
    running_cputime_ms_ += a.cputime_ms_;  
    running_utime_ms_ += a.utime_ms_;
    running_systime_ms_ += a.systime_ms_;
    
    running_virtmemsz_kb_ += a.virtmemsz_kb_; 
    running_resmemsz_kb_ += a.resmemsz_kb_;  
    running_nthr_ += a.nthr_;
    running_io_read_bytes_ += a.io_read_bytes_;
    running_io_write_bytes_ += a.io_write_bytes_;
    running_nprocs_ ++;
  }  
  return (*this);
}

/*--Methode--*/
CumulProcessInfo& CumulProcessInfo::Add(CumulProcessInfo const& a)
{
  if (a.elapsed_time_ms_ > elapsed_time_ms_)   elapsed_time_ms_ = a.elapsed_time_ms_;

  total_cputime_ms_ += a.total_cputime_ms_;  
  total_utime_ms_ += a.total_utime_ms_;
  total_systime_ms_ += a.total_systime_ms_;
  
  total_nprocs_ += a.total_nprocs_;
  total_nthr_ += a.total_nthr_;
  total_io_read_bytes_ += a.total_io_read_bytes_;
  total_io_write_bytes_ += a.total_io_write_bytes_;

  if (a.max_virtmemsz_kb_ > max_virtmemsz_kb_)  max_virtmemsz_kb_=a.max_virtmemsz_kb_;
  if (a.max_resmemsz_kb_ > max_resmemsz_kb_)  max_resmemsz_kb_=a.max_resmemsz_kb_;

  running_cputime_ms_ += a.running_cputime_ms_;  
  running_utime_ms_ += a.running_utime_ms_;
  running_systime_ms_ += a.running_systime_ms_;
  
  running_virtmemsz_kb_ += a.running_virtmemsz_kb_; 
  running_resmemsz_kb_ += a.running_resmemsz_kb_;  
  running_nthr_ += a.running_nthr_;
  running_io_read_bytes_ += a.running_io_read_bytes_;
  running_io_write_bytes_ += a.running_io_write_bytes_;
  running_nprocs_ += a.running_nprocs_;
  return (*this);
}

/*--Methode--*/
ostream& CumulProcessInfo::Print(ostream& os, bool fglong)  const 
{
  struct tm gtime;
  char starttime[48];
  char endtime[48];
  gmtime_r(&start_time_,&gtime);
  asctime_r(&gtime,starttime);
  gmtime_r(&end_time_,&gtime);
  asctime_r(&gtime,endtime);
  double telapsed=((elapsed_time_ms_>0)?(double)elapsed_time_ms_:1.);
  if (fglong) {
    os << "CUMUL(pid=" << pid_ << ",ppid=" << ppid_ << ") elapsed (s)="<< (double)elapsed_time_ms_/1000. <<
      " Running CPU (s) total,user,sys=" << (double)running_cputime_ms_/1000. << "," << (double)running_utime_ms_/1000.
       << "," << (double)running_systime_ms_/1000. << " %CPU=" << 100.*running_cputime_ms_/telapsed
       << "\n Running mem(kb) (vsz,rss)= " << running_virtmemsz_kb_ << "," << running_resmemsz_kb_
       << " ioR/W(kb)= " << running_io_read_bytes_/1024 << "," << running_io_write_bytes_/1024
       << " ioR/W-rate(kb/s)= " << (double)running_io_read_bytes_/telapsed << "," << (double)running_io_write_bytes_/telapsed
       << " nproc,nthr= " << running_nprocs_ << "," << running_nthr_<< endl
       << "... CPU (s) total,user,sys=" << (double)total_cputime_ms_/1000. << "," << (double)total_utime_ms_/1000.
       << "," << (double)total_systime_ms_/1000. << " %CPU=" << 100.*total_cputime_ms_/telapsed
       << "\n Max mem(kb) (vsz,rss)= " << max_virtmemsz_kb_ << "," << max_resmemsz_kb_
       << " ioR/W(kb)= " << total_io_read_bytes_/1024 << "," << total_io_write_bytes_/1024
       << " ioR/W-rate(kb/s)= " << (double)total_io_read_bytes_/telapsed << "," << (double)total_io_write_bytes_/telapsed
       << " nproc,nthr= " << total_nprocs_ << "," << total_nthr_ << endl;
  }
  else {
    os << "CUMUL(" << pid_ << "," << ppid_ << ") "<< elapsed_time_ms_<< "  " 
       << running_cputime_ms_<< "," << running_utime_ms_ << "," << running_systime_ms_ << "  " 
       << running_virtmemsz_kb_ << "," << running_resmemsz_kb_ << "  "
       << running_io_read_bytes_ << "," << running_io_write_bytes_ << "  " 
       << total_cputime_ms_ << "," << total_utime_ms_/1000. << "," << total_systime_ms_ <<  "  "
       << running_nprocs_ << "," << running_nthr_ << "  " 
       << max_virtmemsz_kb_ << "," << max_resmemsz_kb_ << "  " 
       << total_io_read_bytes_/1024 << "," << total_io_write_bytes_/1024
       << total_nprocs_ << "," << total_nthr_
       << endl;
  }
  return os;
}

ostream& CumulProcessInfo::LogFileHeader(ostream& os) 
{
  //    os << "# log_time elapsed_time Running:: cpu_time user_time sys_time  %CPU mem_vsz  mem_rss io_read_kb io_write_kb io_read_rate io_write_rate nproc nthread  Total:: cpu_time user_time sys_time  %CPU mem_vsz  mem_rss io_read_kb io_write_kb io_read_rate io_write_rate nproc nthread" << endl;
  os << "# log_time elapsed_time tot_cpu_time tot_cpu_p100  max_mem_vsz max_mem_rss tot_io_read_kb tot_io_write_kb tot_io_read_rate tot_io_write_rate tot_nproc tot_nthread  r_cpu_time r_cpu_100 r_sum_mem_vsz r_sum_mem_rss r_io_read_kb r_io_write_kb r_io_read_rate r_io_write_rate r_nproc r_nthread" << endl;

    os << "#  elapsed,cpu time in seconds, memory & IO in kb (kilo-bytes) - IO rate kb/s (kilo-bytes/sec) " << endl;
    os << "#  tot_ : total for all processes , r_ : total for running processes " << endl;
    os << "#  max_mem : maximum memory usage , r_sum_mem : sum of memory usage for running processes " << endl;
    os << "#  field separator character ; " << endl;
    return os;
}

//--- classe pour faciliter la sortie / formattage de CumulProcessInfo pour log-file
class LGF_SEP {
public:
  LGF_SEP(int w, char sepc)
  { w_=w;   sepc_[0]=sepc_[2]=' '; sepc_[1]=sepc; sepc_[3]='\0'; }
  int w_;
  char sepc_[4];
};
inline ostream& operator << (ostream& os, LGF_SEP const& sep) 
{  os<<std::setw(3)<<sep.sepc_<<std::setw(sep.w_);  return os; }

ostream& CumulProcessInfo::LogFileOutput(ostream& os, double tmlog)  const 
{
  int w=8, ws=2;
  char sepc=';';
  LGF_SEP sep(w,sepc);
  double telapsed=((elapsed_time_ms_>0)?(double)elapsed_time_ms_:1.);
  os << setw(w) << tmlog << sep << (double)elapsed_time_ms_/1000. << sep
     << (double)total_cputime_ms_/1000.<< sep
    //     << (double)total_utime_ms_/1000. << sep  << (double)total_systime_ms_/1000. << sep
     << 100.*(double)total_cputime_ms_/telapsed << sep
     << max_virtmemsz_kb_ << sep << max_resmemsz_kb_ << sep
     << total_io_read_bytes_/1024 << sep << total_io_write_bytes_/1024 << sep
     << (double)total_io_read_bytes_/telapsed << sep << (double)total_io_write_bytes_/telapsed << sep
     << total_nprocs_ << sep << total_nthr_ << sep
    
     << (double)running_cputime_ms_/1000.<< sep
    //     << (double)running_utime_ms_/1000. << sep << (double)running_systime_ms_/1000. << sep
     << 100.*(double)running_cputime_ms_/telapsed << sep
     << running_virtmemsz_kb_ << sep << running_resmemsz_kb_ << sep
     << running_io_read_bytes_/1024 << sep << running_io_write_bytes_/1024 << sep
     << (double)running_io_read_bytes_/telapsed << sep << (double)running_io_write_bytes_/telapsed << sep
     << running_nprocs_ << sep << running_nthr_ << sep
     << endl;
  return os; 
}
//------------------------------------------------------------------------------------------------
//------------------------------------  class MyProcessAccounting --------------------------------
//------------------------------------------------------------------------------------------------

/*--Methode--*/
MyProcessAccounting::MyProcessAccounting()
{
  myuid_ = getuid();
  myeuid_ = geteuid();
  mypid_ = getpid();
  myppid_ = getppid();
  time(&start_time0_); 
  struct tm gtime;
  char starttime[48];
  gmtime_r(&start_time0_,&gtime);
  asctime_r(&gtime,starttime);
  cout << " MyProcessAccounting() " << starttime << " uid=" << myuid_ << " euid=" << myeuid_
       << " pid=" << mypid_ << " ppid=" << myppid_ << endl;

}

/*--Methode--*/
void MyProcessAccounting::update()
{
  time_t curtime;
  time(&curtime);
  // Prepare the process list first 
  map<pid_t, ProcessInfo>::iterator it, itp;
  for(it=myprocs_.begin(); it!=myprocs_.end(); it++) {
    //    (*it).second.children_.clear();   this is the full list of children, running or not 
    (*it).second.end_time_=curtime;
    (*it).second.fg_running_=false;
  }

  // pair of process id and parent process id  to store (pid, parent_pid) for processes inserted in the list, before their parents 
  vector< pair<pid_t,pid_t> > vpids;  

  char cmdstr[256];
  char filename[92];
  sprintf(filename,"/tmp/prac%ld_ATMP.res",(long)myuid_);
#ifdef Darwin
  sprintf(cmdstr,"ps -U %ld -c -o pid,ppid,time,utime,stime,etime,vsz,rss,wq,command > %s",
	  (long)myuid_,filename);
#else 
#if defined(Linux)  || defined(linux)
  sprintf(cmdstr,"ps -u %ld -o pid,ppid,bsdtime,cutime,cstime,etime,vsz,rss,thcount,comm > %s",
	  (long)myuid_,filename);
  
#else
#error "procacct.cc : ONLY linux and macos are currently supported"
#endif
#endif
  int rc = system(cmdstr);
  char line[256];
  size_t kl=0;
  ifstream ifs(filename);  // std::ifstream::in);
  while (ifs.is_open() && (!ifs.eof())) {
    ifs.getline(line,256);  line[255]='\0';
    if (ifs.eof())  break;
    // cout << " *DBG*Line= " << line << endl;
    kl++;   if (kl==1)  continue;
    string sline=line;
    vector<string> vs;
    SplitStringToVString(sline,vs);
    //DBG cout << " DBG*X = vs.size()= " << vs.size() << " vs[0] vs[1]=" << vs[0] << " " << vs[1] << endl;
    if (vs.size() < 10)  {
      cerr << " MyProcessAccounting::update() - ERROR ? vs.size()= " << vs.size() << " < 10" << endl; 
      continue;
    }
    pid_t pid=atol(vs[0].c_str());
    pid_t ppid=atol(vs[1].c_str());
    //DBG    cout << " *DBG* pid,ppid=" << pid << "," << ppid << endl;
    ProcessInfo pinfo(pid,ppid);
    pinfo.cputime_ms_=decode_timestr(vs[2]);
    pinfo.utime_ms_=decode_timestr(vs[3]);
    pinfo.systime_ms_=decode_timestr(vs[4]);
    pinfo.elapsed_time_ms_=decode_timestr(vs[5]); 
    /*
    if ((pid==18286)||(ppid==18286)) 
      cout << " DBG* elapsed: pid,ppid=" << pid << "," << ppid << " vs=" << vs[5] << " -> " << pinfo.elapsed_time_ms_ << " ms" << endl;
    */
    if (vs[6]!="-")  pinfo.virtmemsz_kb_=atol(vs[6].c_str());
    if (vs[7]!="-")  pinfo.resmemsz_kb_=atol(vs[7].c_str());

    if (vs[8]!="-")  pinfo.nthr_=atol(vs[8].c_str());
    else pinfo.nthr_=1;

    pinfo.io_read_bytes_=pinfo.io_write_bytes_=0;
#if defined(Linux)  || defined(linux)
    {
      sprintf(filename,"/proc/%ld/io",(long)pid);
      ifstream iof(filename);  // std::ifstream::in);
      while (iof.is_open() && !iof.eof()) {
	iof.getline(line,256);  line[255]='\0';
	if (strncmp(line,"rchar:",6)==0)  pinfo.io_read_bytes_=atol(line+6); 
	if (strncmp(line,"wchar:",6)==0)  pinfo.io_write_bytes_=atol(line+6); 
	/*
	if (strncmp(line,"read_bytes:",11)==0)  pinfo.io_read_bytes_=atol(line+12); 
	if (strncmp(line,"write_bytes:",12)==0)  pinfo.io_write_bytes_=atol(line+13); 
	*/
      }
    }
#endif
    pinfo.command_ = vs[9];
    pinfo.fg_running_=true;
    // for debugging 
    //DBG if ((pid==45626)||(ppid==45626))   
    //DBG cout << pinfo;
    it = myprocs_.find(pid);
    if (it != myprocs_.end()) {  // Process already in the list - update the process info 
      pinfo.start_time_=(*it).second.start_time_;
      pinfo.children_=(*it).second.children_;
      (*it).second = pinfo;
    }
    else {   // insert the new process in the list and update its parent child list
      myprocs_[pid]=pinfo;      
      itp=myprocs_.find(ppid);
      if (itp != myprocs_.end()) {
	(*itp).second.children_.push_back(pid);
      }
      else {  // we keep the (pid, parent_pid) for later insertion 
	vpids.push_back( pair<pid_t,pid_t>(pid, ppid) );
      }
    }
  }
  // We finalize the children list update for processes inserted before their parent 
  for(size_t k=0; k<vpids.size(); k++) {
    it =  myprocs_.find(vpids[k].second);  // the parent process Id
    if (it != myprocs_.end()) {
      (*it).second.children_.push_back(vpids[k].first);
    }
  }
  
}

/* methode */
CumulProcessInfo MyProcessAccounting::getProcessSummary(pid_t pid)
{
  map<pid_t, ProcessInfo>::iterator it=myprocs_.find(pid);
  if (it == myprocs_.end())  return CumulProcessInfo(ProcessInfo());
  CumulProcessInfo sum((*it).second);
  return sum;
}

/* methode */
CumulProcessInfo MyProcessAccounting::getChildrenSummary(pid_t pid)
{
  map<pid_t, ProcessInfo>::iterator it=myprocs_.find(pid);
  if (it == myprocs_.end())  return CumulProcessInfo(ProcessInfo());
  CumulProcessInfo sum;
  for(size_t k=0; k<(*it).second.children_.size(); k++) {
    CumulProcessInfo cchd = getFullSummary((*it).second.children_[k]);
    sum.Add(cchd);
  }
  return sum;
}

/* methode */
CumulProcessInfo MyProcessAccounting::getFullSummary(pid_t pid)
{
  map<pid_t, ProcessInfo>::iterator it=myprocs_.find(pid);
  if (it == myprocs_.end())  return CumulProcessInfo(ProcessInfo());
  CumulProcessInfo sum((*it).second);
    for(size_t k=0; k<(*it).second.children_.size(); k++) {
    CumulProcessInfo cchd = getFullSummary((*it).second.children_[k]);
    sum.Add(cchd);
  }
  return sum;
}

/* methode */
long MyProcessAccounting::decode_timestr(string &s)
{
  if (s == "-") return 0;
  long rt=0;
  vector<string> hms;
  SplitStringToVString(s,hms,':');
  if (hms.size()==0)  return 0;
  else if (hms.size()==1)  { return (long)(atof(hms[0].c_str())*1000.); }
  else if (hms.size()==2) {
    rt=atol(hms[0].c_str())*60000;
    rt+=(long)(atof(hms[1].c_str())*1000.);
    return rt;
  }
  else {
    size_t posc=hms[0].find('-');
    if (posc > hms[0].length())  posc=0;
    else posc++;
    rt=atol(hms[0].c_str()+posc)*3600*1000;
    rt+=atol(hms[1].c_str())*60000;
    rt+=(long)(atof(hms[2].c_str())*1000.);
    return rt;
  }
  return 0;
}

/* methode */
void SplitStringToVString(string const & s,vector<string>& vs,char separator)
// Split the string "s" into a string vector "vs"
// - Separator is "separator"
// - If the separator is not a space (' '), the space is considered
//   as a dummy character:
// - If ',' is the separator, "1, 2  ,3 ,    4  " == "1,2,3,4"
// - If ',' is the separator, "1, 2 3 ,    4  " == "1,23,4"
// - Fill "vs" at the end: NO RESET IS DONE
// - If ',' is the separator, ",,," is c
// That routine is much more rapid than FillVStringFrString although less general
//           (ex no '\' gestion is done)
{
 size_t ls = s.size();
 if(ls<=0) return;
 //bool sep_non_blanc = (separator==' ')? false: true;
 char *str = new char[ls+2];

 size_t i=0, lstr=0;
 while(i<ls) {
   // le caractere n'est ni un blanc ni un separateur
   if(s[i]!=separator && s[i]!=' ') {
     str[lstr] = s[i]; lstr++;
   }
   // Condition de remplissage du vecteur
   if(s[i]==separator || i==ls-1) {
     if(lstr>0) {str[lstr]='\0'; vs.push_back(str); lstr=0;}
   }
   i++;
 }

 delete [] str;
 return;
}
