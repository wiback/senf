#!/usr/bin/python
# -*- coding: utf-8 -*-
# $Id:$
#
# Copyright (C) 2012
# Fraunhofer Institute for Open Communication Systems (FOKUS)
#
# The contents of this file are subject to the Fraunhofer FOKUS Public License
# Version 1.0 (the "License"); you may not use this file except in compliance
# with the License. You may obtain a copy of the License at 
# http://senf.fokus.fraunhofer.de.de/license.html
#
# The Fraunhofer FOKUS Public License Version 1.0 is based on, 
# but modifies the Mozilla Public License Version 1.1.
# See the full license text for the amendments.
#
# Software distributed under the License is distributed on an "AS IS" basis, 
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
# for the specific language governing rights and limitations under the License.
#
# The Original Code is Fraunhofer FOKUS code.
#
# The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V. 
# (registered association), Hansastraße 27 c, 80686 Munich, Germany.
# All Rights Reserved.

from optparse import OptionParser
import sys
import math

#node class
class Node(object):
  def __init__(self,id,latitude,longitude):
    self.id=id
    self.latitude=latitude
    self.longitude=longitude
    self.distances=dict()
    self.attenuations=dict()

#
# scream a bit ;-)
#
def fatal( msg):
        print
        print( msg)
        print
        sys.exit(1)

#calc distance between two nodes (coordindates in WGS84 format)
#algorithm from http://www.codeproject.com/KB/cs/distancebetweenlocations.aspx
#@return disctance of two nodes in meters
def calc_distance(node1,node2):

  lat1_rad=node1.latitude*(math.pi/180.0)
  lat2_rad=node2.latitude*(math.pi/180.0)
  long1_rad=node1.longitude*(math.pi/180.0)
  long2_rad=node2.longitude*(math.pi/180.0)

  latitude = lat2_rad - lat1_rad;
  longitude = long2_rad - long1_rad;

  #Intermediate result a.
  a = math.pow(math.sin(latitude / 2.0),2.0)+math.cos(lat1_rad)*math.cos(lat2_rad) \
    * math.pow(math.sin(longitude / 2.0),2.0)

  #Intermediate result c (great circle distance in Radians).
  c = 2.0 * math.atan2(math.sqrt(a), math.sqrt(1.0 - a))

  #Distance.

  #earth_radius_miles = 3956.0;

  earth_radius_km = 6376.5
  
  
  distance=earth_radius_km * c *1000
  #debug
  #print node1.id + " " +node2.id + " " + str(distance)
  return distance

#calc the constant factor of loss space attenuation 
#depending on the distance (part of loss space att. which does 
#not depened on the frequency)
def calc_attenuation_const_factor(distance):
  return 20*math.log10(4*math.pi*distance)-20*math.log10(3*pow(10,8))


#
# parse command line options
#
parser = OptionParser()
parser.add_option( "-f", "--file",dest="file", help="file which contains the node coordinates (default:coordinates.conf)", action="store", type="string", default="coordinates.conf")
(options, args) = parser.parse_args()

#open coordinates files
try:
    file = open(options.file,'r')
except IOError:
    fatal( "coordinates file not found")

node_list=list()

#parse coordinates files
for line in file.readlines():
  node=Node(line.split(' ')[1],
    float(line.split(' ')[2]),
    float(line.split(' ')[3].strip().strip(';')))
  node_list.append(node)

#close coordinates file
file.close()

for node in node_list:
  try:
    file = open("node-"+node.id+".conf",'a')
  except IOError:
    fatal ( "node config file is missing" )
  for other_node in node_list:
    if not node==other_node:
      node.distances[other_node]=calc_distance(node,other_node)
      node.attenuations[other_node]=\
        calc_attenuation_const_factor(node.distances[other_node])
      file.write("nodeAttenuationFactor "+other_node.id + " " + str(node.attenuations[other_node]) +";\n")
  file.close()



