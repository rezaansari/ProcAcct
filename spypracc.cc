/* --------------------------------------------------------
   spy process accounting main program 
   LSST software stack checking - 
   R. Ansari (LAL/IN2P3-CNRS , Univ. Paris Sud, Aug 2013
   -------------------------------------------------------- */

#include <unistd.h>
#include <sys/stat.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>  // for using std library sort 
#include <exception>  
#include <fstream>  

#include "spypasighand.h"
#include "procacct.h"
#include "timing.h"
  

//--------------------------------------------
//------------ utility functions -------------
//--------------------------------------------
static double spypra_version = 1.1;   // version number 

//---- Execution control and interaction with signals 
static bool my_fg_run_ = false;
/*-- Fonction --*/
void StopSpyRAcc()
{
  my_fg_run_=false;
}


//---------------------------------------------------------
//------------------- MAIN PROGRAM ------------------------
//---------------------------------------------------------

int main(int narg, char* arg[])
{
  int rc = 0;
  if (narg < 3) {
    cout << "spypracc: resource usage (CPU,I/O ...) statistics and summary (version V= " << spypra_version << ") \n"
	 << "    should be launched as a background process by the process for which \n" 
	 << "    resource statistics is needed \n"
	 << "     Usage: spypracc refresh_interval resusage_log_file [log_modulo=1] \n"
	 << "     - refresh_interval in seconds (float number) \n" 
	 << "     - resusage_log_file (ascii/text format) \n" 
	 << "        specify /dev/null if you want the summary at the end only \n" 
	 << "     - log_modulo:log to file once for log_modulo refresh \n" << endl;
    return 1;
  }
  InitTim();
  SpyPAConfigureSignalhandling(false,false,true,true,true,true);

  /* ---- we create first a lock file containing the spypracc process ID.
     The presence of this file (/tmp/spypracc_PPID.lock  prevents also two 
     or more running spypracc process to be launched from the same parent */
  pid_t myppid=getppid();
  pid_t mypid=getpid();
  char lockfilename[128];
  sprintf(lockfilename,"/tmp/spypracc_%ld.lock",(long)myppid);
  // Checking if a lock file for a spypracc launched from the same parent exists
  struct stat lfst;
  int rclf=stat(lockfilename,&lfst);
  //DBG  cout << " DEBUG stat(" << lockfilename << ") -> Rc=" << rclf << endl;
  if ( rclf == 0 )  {
    pid_t rspid;
    ifstream lockfile(lockfilename);
    lockfile >> rspid;
    cout << " spypracc/ERROR: existing lock file: " << lockfilename << " for spy process with PID=" <<  rspid
	 << " \n Stop corresponding spypracc before launching a new spy ..." << endl;
    cerr << " spypracc/ERROR: existing lock file: " << lockfilename << " for spy process with PID=" <<  rspid
	 << " \n Stop corresponding spypracc before launching a new spy ..." << endl;
    return 9;
  }
  // If NOT, create the lock file which will be used by stopspypracc to terminate the spy process
  {
    ofstream lockfile(lockfilename);
    lockfile << mypid << endl;
  }

  CumulProcessInfo sum;
  try {
    double interval = atof(arg[1]);
    useconds_t slus=(interval*1.e6);
    size_t log_modulo=1;
    if (narg>3) log_modulo=(size_t)atol(arg[3]);
    if (log_modulo<1) log_modulo=1;
    cout << " ------- spyracc(PPID="<<myppid<<",PID="<<mypid
	 << ")  started - program version V= " << spypra_version << " --------- " << endl;
    cout << " --- parameters: interval=" << interval << " s.  LogFile: " << arg[2] << " log_modulo=" << log_modulo << endl;

    // We keep the start time 
    time_t tm0, tmc;
    time(&tm0);

    MyProcessAccounting proca;
    ofstream outlog(arg[2]);

    CumulProcessInfo::LogFileHeader(outlog);

    my_fg_run_=true;
    size_t cnt=0;
    while (my_fg_run_) {
      proca.update();
      if (cnt%log_modulo == 0) {
	time(&tmc);
	double tmlog=difftime(tmc,tm0);
	sum.Reset();
	sum=proca.getSummary();
	sum.LogFileOutput(outlog, tmlog);
      //DBG      cout << sum;
      }
      usleep(slus);   cnt++;
    }
  cout << " ----- spyracc ended: All Parent's process children resource usage summary -------" << endl;
  proca.update();
  CumulProcessInfo sumf=proca.getSummary();
  cout << sumf;
  cout << " ----- spyrac process usage summary -------" << endl;
  sumf=proca.getSelfSummary();
  cout << sumf; 
  cout << "-------------------------------------------------------------------------------" << endl;
  }
  catch (SigCatch & e) {
    cerr << " spypracc.cc: Catched SigCatch "  
         << " - what()= " << e.what() << endl;
    cout << " ----- spyracc ended: Parent process resource usage summary -------" << endl;
    cout << sum;
    cout << "-------------------------------------------------------------------------------" << endl;
    rc = 0;
  }
  catch (std::exception & e) {
    cerr << " spypracc.cc: Catched std::xception "  
         << " - what()= " << e.what() << endl;
    rc = 98;
  }
  catch (...) {
    cerr << " spypracc.cc: some other exception (...) was caught ! " << endl;
    rc = 97;
  }
  PrtTim("End_of_spypracc");
  return rc;
}

