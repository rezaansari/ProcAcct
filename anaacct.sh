if [[ $# -lt 1 ]] ; then
  echo ' Usage anaacct spypracc_accouting_logfile_name -> exit'
  exit 1 ;
fi
ACCTFILE=$1
echo '--- Processing spypracc log file: ' $ACCTFILE 
BOUTFILE=`basename $ACCTFILE`
OUTFILE=/tmp/$BOUTFILE
sed 's/(%)//' $ACCTFILE > $OUTFILE
spiapp -term -exec ana_acct.pic $OUTFILE
stty sane

