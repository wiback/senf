#
# Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
#                    Network Research Group (NET)
#                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
#                    Contact: http://wiback.org
#
# This file is part of the SENF code tree.
# It is licensed under the 3-clause BSD License (aka New BSD License).
# See LICENSE.txt in the top level directory for details or visit
# https://opensource.org/licenses/BSD-3-Clause
#


import sys, os, urllib, signal, time, signal, time

from string import *
from stat import *
from optparse import OptionParser



def make_group( group_id, num_groups):
    group = (group_id % num_groups) + 1;
    return "229.1." + str( group/256) + "." + str(group % 256) + ":12345"

def STOP():
    os.system( "killall carmen")


def START( num_nodes, num_groups):
    for n in range(0, num_nodes):
        command =  ("../carmen --name=" + str(n) + 
                   " " + make_group(n*4+0, num_groups) +
                   " " + make_group(n*4+1, num_groups) +
                   " " + make_group(n*4+2, num_groups) +
                   " " + make_group(n*4+3, num_groups) + " &")


        os.system( command)

                   


START( 10, 10)

time.sleep(60)

