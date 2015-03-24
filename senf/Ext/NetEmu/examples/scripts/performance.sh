#!/bin/sh

rm -f /tmp/performance.dat

carmen=../carmen
master='--config=sample.conf --config=performance.conf --silent'
peer='--burn-cycles=4000 --num-packets=10 --probe-interval=200ms --config=sample.conf'

startpeers="$1"
maxpeers="$2"

if [ -z "$maxpeers" ]; then
    echo "Usage: $0 <startpeers> <maxpeers>"
    echo
    echo "Remember to start $0 as root"
    exit 1
fi

###########################################################################

echo "Launching master"
eval nice -n -5 $carmen $master &

sleep 4

for i in `seq $startpeers`; do 
    sleep 1
    echo "Launching peer $i"
    $carmen --id=$i $peer --silent &
done

sleep 20

for i in `seq $(expr $startpeers + 1) $maxpeers`; do
    sleep 10
    echo "Launching peer $i"
    $carmen --id=$i $peer --silent &
done

sleep 30

killall carmen

ploticus -png -o performance.png performance.plot
