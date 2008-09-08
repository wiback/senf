#!/bin/bash -e

PATH=${HOME}/bin:${PATH}
LOGDIR=${HOME}/log

cd /home/senf/src

rm -f ${LOGDIR}/build.log ${LOGDIR}/upload.log
touch ${LOGDIR}/build.log ${LOGDIR}/upload.log

trap 'exec >/dev/null 2>&1; cat ${LOGDIR}/build.log ${LOGDIR}/upload.log 1>&3; exit 1' ERR

exec 3>&1 > ${LOGDIR}/build.log 2>&1

echo -n '# Starting build at '; date --utc

if [ "$1" == "-c" ]; then
    echo '$ find -mindepth 1 -maxdepth 1 ! -name .svn | xargs rm -rf'
    find -mindepth 1 -maxdepth 1 ! -name .svn | xargs rm -rf
fi

echo '$ svn update'
svn update | tee ${LOGDIR}/svn-update.log
if grep -qv '^At ' ${LOGDIR}/svn-update.log; then
    echo '$ rm -f doc/html/html.stamp'
    rm -f doc/html/html.stamp
fi
rm -f ${LOGDIR}/svn-update.log

echo "\$ nice ${SCONS:-scons} -kj2 all ${DOXYGEN:+DOXYGEN="$DOXYGEN"}"
nice ${SCONS:-scons} -kj2 all ${DOXYGEN:+DOXYGEN="$DOXYGEN"}
echo "\$ nice ${SCONS:-scons} all ${DOXYGEN:+DOXYGEN="$DOXYGEN"}"
nice ${SCONS:-scons} all ${DOXYGEN:+DOXYGEN="$DOXYGEN"}
echo "\$ nice ${SCONS:-scons} linklint ${DOXYGEN:+DOXYGEN="$DOXYGEN"}"
nice ${SCONS:-scons} linklint ${DOXYGEN:+DOXYGEN="$DOXYGEN"}
echo "\$ nice ${SCONS:-scons} fixlinks ${DOXYGEN:+DOXYGEN="$DOXYGEN"}"
nice ${SCONS:-scons} fixlinks ${DOXYGEN:+DOXYGEN="$DOXYGEN"}
echo -n '# Build completed at '; date --utc

exec > ${LOGDIR}/upload.log 2>&1

if [ "$1" == "-c" ]; then
    cp ${LOGDIR}/build.log ${LOGDIR}/build-full.log
fi

echo -n '# Upload started at '; date --utc
rsync -rzv --del --delete-excluded \
    --filter="- .svn" \
    --filter="- linklint" \
    --filter="- debian" \
    --filter="+ */" \
    --filter="+ *.html" \
    --filter="+ *.css" \
    --filter="+ *.png" \
    --filter="+ *.php" \
    --filter="+ *.idx" \
    --filter="+ *.log" \
    --filter="- *" \
    . g0dil@shell.berlios.de://home/groups/senf/htdocs/src
echo -n '# Upload completed at '; date --utc

exec >/dev/null 2>&1
scp ${LOGDIR}/upload.log g0dil@shell.berlios.de:/home/groups/senf/htdocs/upload.log
