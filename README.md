# ProcAcct

Spy program (spypracc) to determine resource (memory, CPU, I/O) for a set of processes  sharing a common parent process.
Developed for tests of LSST stack at CC-IN2P3    (July 2013)   

R. Ansari ,  Univ. Paris Sud , LAL IN2P3/CNRS  
   - Version V 1.0 : July 2013 
   - Version V 1.1 : August 2013 


spypracc should be launched by the process for which one wants to determine the 
resource usage. A log file is created with periodic logging of the resource 
usage of spypracc parent process.
The spypracc should be lauched as a background process, and then stopped using 
the stopspypracc command. When succesfully started, spypracc creates a lock 
file identified by the parent process PID in the /tmp directory. This lock 
file prevents a second spypracc to be lauched for the same parent process. 
The lock file is removed by stopspypracc, after being used to identify the 
spypracc process to be terminated. 

------------------------------------------------------------------------------------
spypracc: resource usage (CPU,I/O ...) statistics and summary (version V= 1.1) 
    should be launched as a background process by the process for which 
    resource statistics is needed 
     Usage: spypracc refresh_interval resusage_log_file [log_modulo=1] 
     - refresh_interval in seconds (float number) 
     - resusage_log_file (ascii/text format) 
        specify /dev/null if you want the summary at the end only 
     - log_modulo:log to file once for log_modulo refresh 
------------------------------------------------------------------------------------

* File list (some functions extracted / adapted from SOPHYA , http://www.sophya.org )
Objs		makefile	procacct.h	spypasighand.h	stopspypracc.cc	timing.h
README		procacct.cc	spypasighand.cc	spypracc.cc	timing.c

* Notes , compilation
 - the present version is adapted to Linux or Darwin (Mac OS X) and uses the ps 
 program to gather information about running processes. On Linux, the 
 /proc/<PID>/io file is used to get the related input/output volume (rchar,wchar)  
 
 - make sure that the directory Objs exists (or create it) 
 - check that the correct flag (Darwin, or Linux) is specified in the makefile (CPPFLAGS)
 - type make 
