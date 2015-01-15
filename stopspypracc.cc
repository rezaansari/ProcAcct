/* --------------------------------------------------------
   spy process accounting 
   LSST software stack checking - 
   Utility program to stop the spypracc process
   R. Ansari (LAL/IN2P3-CNRS , Univ. Paris Sud, Aug 2013
   -------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>

#include <iostream>
#include <fstream>  

using namespace std; 

//---------------------------------------------------------
//------------------- MAIN PROGRAM ------------------------
//---------------------------------------------------------
int main(int narg, char* arg[])
{
  pid_t myppid=getppid();
  char lockfilename[128];
  sprintf(lockfilename,"/tmp/spypracc_%ld.lock",(long)myppid);
  // Checking if the lock file exists ...
  struct stat lfst;
  int rclf=stat(lockfilename,&lfst);
  if ( rclf == 0 )  {
    pid_t rspid;
    ifstream lockfile(lockfilename);
    lockfile >> rspid;
    cout << " stopspypracc: stopping spy process PID=" << rspid << " and removing lock file " 
	 << lockfilename << "..." << endl;
    int rck = kill(rspid,SIGQUIT);
    remove(lockfilename);
    return 0;
  }
  else {
    cout << " stopspypracc/ERROR: No lock file found (lock file name:"<<lockfilename<<endl;
    cerr << " stopspypracc/ERROR: No lock file found (lock file name:"<<lockfilename<<endl;
    return 9;
  }
  return 0;
}
