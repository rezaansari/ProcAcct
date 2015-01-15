//  Pour la gestion des signaux a travers des exceptions
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <exception>

#include "spypasighand.h"

SigCatch::SigCatch(const char* m, int ident) throw()
{
  id_ = ident;
  if (m!=NULL) { 
    strncpy(msg_, m, 127);
    msg_[127] = '\0';
  }
  else msg_[0] = '\0';
}


const char* SigCatch::what() const throw()
{
  return msg_;
}


static bool sigprstate[6] = {false, false, false, false, false, false};

void SpyPAProcessSignal(int s);
void SpyPAProcessSignalException(int s);

/* Nouvelle-Fonction */
/*!\ingroup  SysTools 
   \brief Utility function for signal handling configuration.

   Configures signal handling. When the flag is true, an exception 
   \b SigCatch is thrown with the excpetion id set to the 
   signal id. If the flag is false, the default signal handling
   is restored.
   \param cfpe : Floating point exception \c SIGFPE
   \param csegv : Segmentation fault \c SIGSEGV
   \param cint : Interrupt signal \c SIGINT
   \param cquit : Quit signal \c SIGINT
   \param cusr1 : User signal 1 \c SIGUSR1
   \param cusr2 : User signal 1 \c SIGUSR2
*/
void SpyPAConfigureSignalhandling(bool cfpe, bool csegv, bool cint, bool cquit, bool cusr1, bool cusr2) 
{
  struct sigaction ae, ad;

  ae.sa_handler = SpyPAProcessSignal;
  ad.sa_handler = SIG_DFL;
  memset( &(ae.sa_mask), 0, sizeof(sigset_t) ); 
  ae.sa_flags = 0;
  memset( &(ad.sa_mask), 0, sizeof(sigset_t) ); 
  ad.sa_flags = 0;
#ifdef  OSF1
  ae.sa_flags = SA_RESTART;
  ad.sa_mask = 0;
#endif

// SIGFPE 
  if (sigprstate[0] != cfpe) {
    if (cfpe)   { 
      sigaction(SIGFPE, &ae, NULL);
      puts("SpyPAConfigureSignalhandling(): Activating SIGFPE handling...");
      }
    else  { 
      sigaction(SIGFPE, &ad, NULL);
      puts("SpyPAConfigureSignalhandling(): Back to default SIGFPE handling ...");
      }
    sigprstate[0] = cfpe;
    }
// SIGSEGV 
  if (sigprstate[1] != csegv) {
    if (csegv)   { 
      sigaction(SIGSEGV, &ae, NULL);
      puts("SpyPAConfigureSignalhandling(): Activating SIGSEGV handling...");
      }
    else  { 
      sigaction(SIGSEGV, &ad, NULL);
      puts("SpyPAConfigureSignalhandling(): Back to default SIGSEGV handling ...");
      }
    sigprstate[1] = csegv;
    }

// SIGINT
  if (sigprstate[2] != cint) {
    if (cint)   { 
      sigaction(SIGINT, &ae, NULL);
      puts("SpyPAConfigureSignalhandling(): Activating SIGINT handling...");
      }
    else  { 
      sigaction(SIGINT, &ad, NULL);
      puts("SpyPAConfigureSignalhandling(): Back to default SIGINT handling ...");
      }
    sigprstate[2] = cint; 
    }
  
// SIGQUIT
  if (sigprstate[3] != cquit) {
    if (cquit)   { 
      sigaction(SIGQUIT, &ae, NULL);
      puts("SpyPAConfigureSignalhandling(): Activating SIGQUIT handling...");
      }
    else  { 
      sigaction(SIGQUIT, &ad, NULL);
      puts("SpyPAConfigureSignalhandling(): Back to default SIGQUIT handling ...");
      }
    sigprstate[3] = cquit; 
    }
  
// SIGUSR1
  if (sigprstate[4] != cusr1) {
    if (cusr1)   { 
      sigaction(SIGUSR1, &ae, NULL);
      puts("SpyPAConfigureSignalhandling(): Activating SIGUSR1 handling...");
      }
    else  { 
      sigaction(SIGUSR1, &ad, NULL);
      puts("SpyPAConfigureSignalhandling(): Back to default SIGUSR1 handling ...");
      }
    sigprstate[4] = cusr1;
    }
// SIGUSR2
  if (sigprstate[5] != cusr2) {
    if (cusr2)   { 
      sigaction(SIGUSR2, &ae, NULL);
      puts("SpyPAConfigureSignalhandling(): Activating SIGUSR2 handling...");
      }
    else  { 
      sigaction(SIGUSR2, &ad, NULL);
      puts("SpyPAConfigureSignalhandling(): Back to default SIGUSR2 handling ...");
      }
    sigprstate[5] = cusr2;
    }    
}

void StopSpyRAcc();

/* Nouvelle-Fonction */
void SpyPAProcessSignal(int s)
{
switch(s)
  {
  case SIGFPE :
    puts("SpyPAProcessSignal: ###Signal SIGFPE catched, DO nothing ###");
    break;
  case SIGSEGV :
    puts("SpyPAProcessSignal: ###Signal SIGSEGV catched, DO nothing ###");
    break;
  case SIGINT :
    puts("SpyPAProcessSignal: ###Signal SIGINT catched, calling StopSpyRAcc() ###");
    StopSpyRAcc();
    break;
  case SIGQUIT :
    puts("SpyPAProcessSignal: ###Signal SIGQUIT catched, calling StopSpyRAcc() ###");
    StopSpyRAcc();
    break;
  case SIGUSR1 :
    puts("SpyPAProcessSignal: ###Signal SIGUSR1 catched, calling StopSpyRAcc() ###");
    StopSpyRAcc();
    break;
  case SIGUSR2 :
    puts("SpyPAProcessSignal: ###Signal SIGUSR2 catched, calling StopSpyRAcc() ###");
    StopSpyRAcc();
    break;
  default :
    printf("SpyPAProcessSignal: ###Signal %d catched, DO nothing ### \n", s);
    break;
  }
 return;
}

/* Nouvelle-Fonction */
void SpyPAProcessSignalException(int s)
{
switch(s)
  {
  case SIGFPE :
    puts("SpyPAProcessSignalException: ###Signal SIGFPE catched, throw catchedSIGFPE ###");
    throw ( SigCatch("Caught SIGFPE", s) );
  case SIGSEGV :
    puts("SpyPAProcessSignalException: ###Signal SIGSEGV catched, throw catchedSIGSEGV ###");
    throw ( SigCatch("Caught SIGSEGV", s) );
  case SIGINT :
    puts("SpyPAProcessSignalException: ###Signal SIGINT catched, throw catchedSIGINT ###");
    throw ( SigCatch("Caught SIGINT", s) );
  case SIGQUIT :
    puts("SpyPAProcessSignalException: ###Signal SIGQUIT catched, throw catchedSIGQUIT ###");
    throw ( SigCatch("Caught SIGQUIT", s) );
  case SIGUSR1 :
    puts("SpyPAProcessSignalException: ###Signal SIGUSR1 catched, throw catchedSIGUSR1 ###");
    throw ( SigCatch("Caught SIGUSR1", s) );
  case SIGUSR2 :
    puts("SpyPAProcessSignalException: ###Signal SIGUSR2 catched, throw catchedSIGUSR2 ###");
    throw ( SigCatch("Caught SIGUSR2", s) );
  default :
    printf("SpyPAProcessSignalException: ###Signal %d catched, throw  Caught SIG??? ### \n", s);
    throw ( SigCatch("Caught SIG???", s) );
  }

// return;
}
