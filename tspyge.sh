#!/usr/local/bin/bash
echo '------ tspyge.sh (WITH-QUIT) JOB_ID=' $JOB_ID ' JOB_NAME=' $JOB_NAME ' ---- '
### Define the Spy-Process-Accounting executable path 
SPYPRACPATH=/sps/lsst/Library/SL6/bin/
### Define the output-path for spy-process accounting log file  
SPYLOGPATH=/sps/lsst/dev/ansari/Test/
##############################################################################
##  Starting spy accounting process - refresh and log inormation every second 
${SPYPRACPATH}spypracc 1 ${SPYLOGPATH}acct_${JOB_ID}.log 1 &
##############################################################################

### Now, let's do some computation and I/O 
### We use SOPHYA test programs for this purpose 
## Initializing SOPHYA (to run the test programs)
source /sps/lsst/Library/Sophya/env.sh new 
echo '------ executing jobs and ps commands '
jobs
ps u
echo '------ lpk and tfitsdt '
time lpk inverse 1500,1500 0
sleep 2 
time tfitsdt -veccol 3 4500000
ls -ltr *.ppf
sleep 5
time lpk inverse 1800,1800 0 
sleep 2
time tfitsdt -veccol 3 9000000
ls -ltr *.ppf
sleep 5
echo '------ tmtfft and tfitsdt '
time tmtfft 1D 976547 30 100 4 
time lpk inverse 1800,1800 0 
time tmtfft 1D 976547 30 100 4 
time lpk inverse 1500,1500 0
time tfitsdt -veccol 3 9000000
sleep 2
ls -ltr *.ppf
sleep 2
###########################################
## Stopping the spy accounting process 
/sps/lsst/Library/SL6/bin/stopspypracc
###########################################
echo '------ END of tspyge.sh script ' 





