#!/bin/sh -e

cd /home/senf/src

rm -f build.log ../upload.log
touch build.log ../upload.log

trap 'exec >/dev/null 2>&1; cat build.log ../upload.log 1>&3; exit 1' ERR

exec 3>&1 >build.log 2>&1

echo -n '# Starting build at '; date --utc

if [ "$1" == "-c" ]; then
    echo '$ find -mindepth 1 -maxdepth 1 ! -name .svn | xargs rm -rf'
    find -mindepth 1 -maxdepth 1 ! -name .svn ! -name build.log | xargs rm -rf
fi

echo '$ svn update'
svn update | tee ../svn-update.log
if grep -qv '^At ' ../svn-update.log; then
    echo '$ rm -f doc/html/html.stamp'
    rm -f doc/html/html.stamp
fi
rm -f ../svn-update.log

echo '$ scons -k all'
scons -k all
echo '$ scons linklint'
scons linklint
echo '$ scons fixlinks'
scons fixlinks
echo -n '# Build completed at '; date --utc

exec >../upload.log 2>&1

if [ "$1" == "-c" ]; then
    cp build.log build-full.log
fi

echo -n '# Upload started at '; date --utc
rsync -rzv --del --delete-excluded \
	--filter="- .svn" \
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
scp ../upload.log g0dil@shell.berlios.de:/home/groups/senf/htdocs/upload.log
