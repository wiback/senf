#proc page
title: carmen --burn-cycles=4000 --num-packets=10 --probe-interval=200ms

///////////////////////////////////////////////////////////////////////////
// numPeers

#proc getdata
file: /tmp/performance.dat
select: @@2 = numPeers

#proc areadef
rectangle: .7 .7 7.3 2.8
xautorange: datafield=1 lowfix=0
yautorange: datafield=4
#saveas bottomarea
#endproc
#set MaxTime = @XMAX

#proc xaxis
stubs: inc 10
ticlen: 0.05
grid: color=gray(0.8)
gridlineextent: min max+0.2

#proc yaxis
stubs: inc
ticlen: 0.05
label: numPeers
labeldetails: adjust=-.1,0
grid: color=gray(0.92)

// Redraw x axis since it has been hidden by the first yaxis grid line
#proc xaxis
stubs: none
ticlen: 0

#proc lineplot
xfield: 1
yfield: 4
stairstep: yes


///////////////////////////////////////////////////////////////////////////
// droppedPacketsPerSecond

#proc getdata
file: /tmp/performance.dat
select: @@2 = droppedPacketsPerSecond-1

#proc areadef
#clone bottomarea
xrange: 0 @MaxTime
yautorange: datafield=4 lowfix=0

#proc yaxis
stubs: inc
location: 7.3
stubdetails: adjust=.6,0 color=red
ticlen: -0.05
tics: color=red
label: droppedPacketsPerSecond
labeldetails: adjust=1,0, color=red
axisline: color=red

#proc lineplot
linedetails: color=red
xfield: 1
yfield: 5


///////////////////////////////////////////////////////////////////////////
// packetDelay

#proc getdata
file: /tmp/performance.dat
select: @@2 = packetDelay-40

#proc areadef
rectangle: .7 3 7.3 7
xrange: 0 @MaxTime
yautorange: datafield=4 lowfix=0 margin=0.005
#saveas toparea
#endproc
#set MaxDelay = @YMAX

// rangesweep does not support clip so we need to fake it .. hrmpf ..
// we need to run getdata twice: first to get MaxDelay and then to truncate the values
#proc getdata
file: /tmp/performance.dat
select: @@2 = packetDelay-40
filter:
  ##if @@5 > @MaxDelay
    ##print @@1 @@2 @@3 @@4 @@5 @MaxDelay
  ##else
    ##print @@1 @@2 @@3 @@4 @@5 @@5
  ##endif

#proc rangesweep
color: rgb(0.93,0.93,1.0)
xfield: 1
lofield: 3
hifield: 6

#proc xaxis
ticincrement: 10
stubs: none
tics: none
grid: color=gray(0.8)

#proc yaxis
stubs: inc
ticlen: 0.05
tics: color=blue
stubdetails: color=blue
axisline: color=blue
label: packetDelay
labeldetails: adjust=-.1,0 color=blue

#proc lineplot
linedetails: color=blue
xfield: 1
yfield: 4

#proc getdata
file: /tmp/performance.dat
select: @@2 = packetDelay-10

#proc areadef
#clone toparea
yrange: 0 @MaxDelay

#proc lineplot
linedetails: color=blue width=0.2
xfield: 1
yfield: 4
clip: yes


///////////////////////////////////////////////////////////////////////////
// packetsPerSecond

#proc getdata
file: /tmp/performance.dat
select: @@2 = packetsPerSecond-40

#proc areadef
#clone toparea
yautorange: datafield=4 lowfix=0
#endproc
#set MaxPackets = @YMAX

#proc yaxis
stubs: inc
location: 7.3
stubdetails: adjust=.6,0 color=green
ticlen: -0.05
tics: color=green
label: packetsPerSecond
labeldetails: adjust=1,0, color=green
axisline: color=green

#proc lineplot
linedetails: color=green
xfield: 1
yfield: 4

#proc getdata
file: /tmp/performance.dat
select: @@2 = packetsPerSecond-10

#proc areadef
#clone toparea
yrange: 0 @MaxPackets

#proc lineplot
linedetails: color=green width=0.2
xfield: 1
yfield: 4
clip: yes
