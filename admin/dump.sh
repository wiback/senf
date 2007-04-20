#!/bin/sh -e
#
# Dump the complete subversion repository from berlios into the dumps/ folder

target=`date +'%Y%m%d.dump.gz'`
dumpdir=/home/senf/dumps
log=/home/senf/dump.log
keep=4

###########################################################################

trap 'exec >/dev/null 2>&1; cat $log 1>&3; exit 1' ERR
exec 3>&1 >$log 2>&1

cd $dumpdir
ssh g0dil@svn.berlios.de "svnadmin dump /svnroot/repos/senf -r 1:HEAD | gzip" >$target
ls | sort -r | sed -e "1,${keep}d" | xargs -r rm
