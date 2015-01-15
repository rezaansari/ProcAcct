#include <stdio.h> 
#include <stdlib.h> 
#include <fstream> 
#include <string.h>

#include "procacct.h"

/*--Methode--*/
ProcessInfo::ProcessInfo(pid_t pid,  pid_t ppid)
{
  pid_=pid; ppid_=ppid;
  start_time_=end_time_=time(NULL);
  cputime_ms_=utime_ms_=systime_ms_=0;
  elapsed_time_ms_=0;
  resmemsz_kb_=virtmemsz_kb_=0; 
  nthr_=0;
  io_read_bytes_=io_write_bytes_=0;
  fg_running_=0;
  tot_nchildren_=running_nchildren_=0;
  running_nthr_=0;
}

/*--Methode--*/
ProcessInfo::ProcessInfo(ProcessInfo const & a) 
{
  CopyFrom(a);
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
  tot_nchildren_=a.tot_nchildren_;  running_nchildren_=a.running_nchildren_;
  running_nthr_=a.running_nthr_;

  return (*this);
}

/*--Methode--*/
ProcessInfo& ProcessInfo::Cumul(ProcessInfo const & a) 
{
  cputime_ms_ += a.cputime_ms_;  
  utime_ms_ += a.utime_ms_; systime_ms_ += a.systime_ms_;
  if (a.elapsed_time_ms_ > elapsed_time_ms_)   elapsed_time_ms_ = a.elapsed_time_ms_;
  io_read_bytes_ += a.io_read_bytes_; io_write_bytes_ += a.io_write_bytes_;
  if (a.fg_running_) {
    resmemsz_kb_ += a.resmemsz_kb_;  virtmemsz_kb_ += a.virtmemsz_kb_; 
    nthr_ += a.nthr_;
  }
  tot_nchildren_+=a.tot_nchildren_;
  running_nchildren_+=a.running_nchildren_;
  running_nthr_+=a.running_nthr_;

  return (*this);
}

/*--Methode--*/
ostream& ProcessInfo::Print(ostream& os)  const 
{
  struct tm gtime;
  char starttime[48];
  char endtime[48];
  gmtime_r(&start_time_,&gtime);
  asctime_r(&gtime,starttime);
  gmtime_r(&end_time_,&gtime);
  asctime_r(&gtime,endtime);
  os << pid_ << "," << ppid_ << " " << cputime_ms_ << "," << utime_ms_ << "," << systime_ms_
     << " " << elapsed_time_ms_ << " " << virtmemsz_kb_ << "," << resmemsz_kb_
     << " " << nthr_ << " " << io_read_bytes_ << "," << io_write_bytes_ << " " << command_ << endl;
  return os;
}


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
  map<pid_t, ProcessInfo>::iterator it;
  for(it=myprocs_.begin(); it!=myprocs_.end(); it++) {
    (*it).second.children_.clear();
    (*it).second.end_time_=curtime;
    (*it).second.fg_running_=false;
  }

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
    if (it != myprocs_.end()) {  // update the process info 
      pinfo.start_time_=(*it).second.start_time_;
      (*it).second = pinfo;
    }
    else myprocs_[pid]=pinfo;      // insert in the process list   
  }

  // --- updating children list 
  vector< pair<pid_t,pid_t> > vpids;  // pair of process id and parent process id
  
  for(it=myprocs_.begin(); it!=myprocs_.end(); it++) {
    vpids.push_back( pair<pid_t,pid_t>((*it).second.pid_, (*it).second.ppid_) );
  }
  
  for(size_t k=0; k<vpids.size(); k++) {
    it =  myprocs_.find(vpids[k].second);
    if (it != myprocs_.end())  (*it).second.children_.push_back(vpids[k].first); 
  }
  //  cout << "*DBG* myprocs_.size()= " << myprocs_.size() << endl;

}

/* methode */
ProcessInfo MyProcessAccounting::getSummary(pid_t pid)
{
  ProcessInfo sum;
  sum.start_time_=start_time0_;
  sum.end_time_=clock();
  map<pid_t, ProcessInfo>::iterator it = it =  myprocs_.find(pid);
  if (it == myprocs_.end())  return sum;
  sum.Cumul((*it).second);
  sum.tot_nchildren_=1;  // the process itself 
  if ((*it).second.fg_running_)  {
    sum.running_nchildren_=1;
    sum.running_nthr_=(*it).second.nthr_;
  }
  for(size_t k=0; k<(*it).second.children_.size(); k++) {
    sum.Cumul( getSummary((*it).second.children_[k]) );
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
