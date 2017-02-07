// $Id$
//
// Copyright (C) 2012
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// http://senf.fokus.fraunhofer.de.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on,
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis,
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V.
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>

#include "MCSInfo.hh"

// Custom includes
#include <senf/Utils/senfassert.hh>
#include <senf/Utils/senflikely.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

// http://mcsindex.com/
static senf::WLAN_MCSInfo::Info mcsInfos[] = {
//       bandwidth:           20 MHz          40 MHz            80 MHz          160 MHz           20   40   80  160     I  S
//              GI:     800 ns   400 ns  800 ns  400 ns   800 ns   400 ns    800 ns  400 ns
    {    "BPSK_1/2", {   6500,    7200,  13500,  15000,   29300,   32500,   58500,   65000 }, { -82, -79, -76, -73 },  0, 1 },
    {    "QPSK_1/2", {  13000,   14400,  27000,  30000,   58500,   65000,  117000,  130000 }, { -79, -76, -73, -70 },  1, 1 },
    {    "QPSK_3/4", {  19500,   21700,  40500,  45000,   87800,   97500,  175500,  195000 }, { -77, -74, -71, -68 },  2, 1 },
    {   "16QAM_1/2", {  26000,   28900,  54000,  60000,  117000,  130000,  234000,  260000 }, { -74, -71, -71, -68 },  3, 1 },
    {   "16QAM_3/4", {  39000,   43300,  81000,  90000,  175500,  195300,  351000,  390000 }, { -70, -67, -64, -61 },  4, 1 },
    {   "64QAM_2/3", {  52000,   57800, 108000, 120000,  234000,  260000,  468000,  520000 }, { -66, -63, -60, -57 },  5, 1 },
    {   "64QAM_3/4", {  58500,   65000, 121500, 135000,  263300,  292500,  526500,  585000 }, { -65, -62, -59, -56 },  6, 1 },
    {   "64QAM_5/6", {  65000,   72200, 135000, 150000,  292500,  325000,  585000,  650000 }, { -64, -61, -58, -55 },  7, 1 },
    {  "256QAM_3/4", {  78500,   86700, 162000, 180000,  351000,  390000,  702000,  780000 }, { -62, -59, -56, -53 },  8, 1 },
    {  "256QAM_5/6", {      0,       0, 180000, 200000,  390000,  433300,  780000,  866700 }, { -60, -57, -54, -51 },  9, 1 },
    { "1024QAM_3/4", {      0,       0,      0,      0,       0,       0,       0,       0 }, { -57, -54, -51, -48 }, 10, 1 },
    { "1024QAM_5/6", {      0,       0,      0,      0,       0,       0,       0,       0 }, { -55, -52, -49, -46 }, 11, 1 },

    {    "BPSK_1/2", {  13000,   14400,  27000,  30000,   58500,   65000,  117000,  130000 }, { -82, -79, -76, -73 },  0, 2 },
    {    "QPSK_1/2", {  26000,   28900,  54000,  60000,  117000,  130000,  234000,  260000 }, { -79, -76, -73, -70 },  1, 2 },
    {    "QPSK_3/4", {  39000,   43300,  81000,  90000,  175500,  195000,  351000,  390000 }, { -77, -74, -71, -68 },  2, 2 },
    {   "16QAM_1/2", {  52000,   57800, 108000, 120000,  234000,  260000,  468000,  520000 }, { -74, -71, -71, -68 },  3, 2 },
    {   "16QAM_3/4", {  78000,   86700, 162000, 180000,  351000,  390000,  702000,  780000 }, { -70, -67, -64, -61 },  4, 2 },
    {   "64QAM_2/3", { 104000,  115600, 216000, 240000,  468000,  520000,  936000, 1040000 }, { -66, -63, -60, -57 },  5, 2 },
    {   "64QAM_3/4", { 117000,  130000, 243000, 270000,  526500,  585000, 1053000, 1170000 }, { -65, -62, -59, -56 },  6, 2 },
    {   "64QAM_5/6", { 130000,  144400, 270000, 300000,  585000,  650000, 1170000, 1300000 }, { -64, -61, -58, -55 },  7, 2 },
    {  "256QAM_3/4", { 156000,  173300, 324000, 360000,  702000,  780000, 1404000, 1560000 }, { -62, -59, -56, -53 },  8, 2 },
    {  "256QAM_5/6", {      0,       0, 360000, 400000,  780000,  866700, 1560000, 1733400 }, { -60, -57, -54, -51 },  9, 2 },
    { "1024QAM_3/4", {      0,       0,      0,      0,       0,       0,       0,       0 }, { -57, -54, -51, -48 }, 10, 2 },
    { "1024QAM_5/6", {      0,       0,      0,      0,       0,       0,       0,       0 }, { -55, -52, -49, -46 }, 11, 2 },
    
    {    "BPSK_1/2", {  19500,   21700,  40500,  45000,   87800,   97500,  175500,  195000 }, { -82, -79, -76, -73 },  0, 3 },
    {    "QPSK_1/2", {  39000,   43300,  81000,  90000,  175500,  195000,  351000,  390000 }, { -79, -76, -73, -70 },  1, 3 },
    {    "QPSK_3/4", {  58500,   65000, 121500, 135000,  263300,  292500,  526500,  585000 }, { -77, -74, -71, -68 },  2, 3 },
    {   "16QAM_1/2", {  78000,   86700, 162000, 180000,  351000,  390000,  702000,  780000 }, { -74, -71, -71, -68 },  3, 3 },
    {   "16QAM_3/4", { 117000,  130700, 243000, 270000,  526500,  585000, 1053000, 1170000 }, { -70, -67, -64, -61 },  4, 3 },
    {   "64QAM_2/3", { 156000,  173300, 324000, 360000,  702000,  780000, 1404000, 1560000 }, { -66, -63, -60, -57 },  5, 3 },
    {   "64QAM_3/4", { 175500,  195000, 364500, 405000,       0,       0, 1579500, 1755000 }, { -65, -62, -59, -56 },  6, 3 },
    {   "64QAM_5/6", { 195000,  216700, 405000, 450000,  877500,  975000, 1755000, 1950000 }, { -64, -61, -58, -55 },  7, 3 },
    {  "256QAM_3/4", { 234000,  260000, 486000, 540000, 1053000,  117000, 2106000, 2340000 }, { -62, -59, -56, -53 },  8, 3 },
    {  "256QAM_5/6", { 260000,  288900, 540000, 600000, 1170000,  130000,       0,       0 }, { -60, -57, -54, -51 },  9, 3 },
    { "1024QAM_3/4", {      0,       0,      0,      0,       0,       0,       0,       0 }, { -57, -54, -51, -48 }, 10, 3 },
    { "1024QAM_5/6", {      0,       0,      0,      0,       0,       0,       0,       0 }, { -55, -52, -49, -46 }, 11, 3 },
    
    {    "BPSK_1/2", {  26000,   28800,  54000,  60000,  117000,  130000,  234000,  260000 }, { -82, -79, -76, -73 },  0, 4 },
    {    "QPSK_1/2", {  52000,   57600, 108000, 120000,  234000,  260000,  468000,  520000 }, { -79, -76, -73, -70 },  1, 4 },
    {    "QPSK_3/4", {  78000,   86800, 162000, 180000,  351000,  390000,  702000,  780000 }, { -77, -74, -71, -68 },  2, 4 },
    {   "16QAM_1/2", { 104000,  115600, 216000, 240000,  468000,  520000,  936000, 1040000 }, { -74, -71, -71, -68 },  3, 4 },
    {   "16QAM_3/4", { 156000,  173200, 324000, 360000,  702000,  780000, 1404000, 1560000 }, { -70, -67, -64, -61 },  4, 4 },
    {   "64QAM_2/3", { 208000,  231200, 432000, 480000,  936000, 1040000, 1872000, 2080000 }, { -66, -63, -60, -57 },  5, 4 },
    {   "64QAM_3/4", { 234000,  260000, 486000, 540000, 1053000, 1170000, 2106000, 2340000 }, { -65, -62, -59, -56 },  6, 4 },
    {   "64QAM_5/6", { 260000,  288800, 540000, 600000, 1170000, 1300000, 2340000, 2600000 }, { -64, -61, -58, -55 },  7, 4 },
    {  "256QAM_3/4", { 312000,  346700, 648000, 720000, 1404000, 1560000, 2808000, 3120000 }, { -62, -59, -56, -53 },  8, 4 },
    {  "256QAM_5/6", {      0,       0, 720000, 800000, 1560000, 1733300, 3120000, 3466700 }, { -60, -57, -54, -51 },  9, 4 },
    { "1024QAM_3/4", {      0,       0,      0,      0,       0,       0,       0,       0 }, { -57, -54, -51, -48 }, 10, 4 },
    { "1024QAM_5/6", {      0,       0,      0,      0,       0,       0,       0,       0 }, { -55, -52, -49, -46 }, 11, 4 }
};

}

prefix_ std::vector<senf::WLAN_MCSInfo::Info> senf::WLAN_MCSInfo::getInfos()
{
    return std::vector<Info>(mcsInfos, mcsInfos + sizeof(mcsInfos) / sizeof(Info));
}

prefix_ unsigned senf::WLAN_MCSInfo::getRate(std::uint8_t mcsIndex, unsigned bandwidth, bool shortGI)
{
    if (SENF_UNLIKELY(mcsIndex >= (NUM_STREAMS * NUM_HT_INDEX) or bandwidth > 40))
        return 0;
    auto tmp (fromHTIndex(mcsIndex));
    return getRate(tmp.first, tmp.second, bandwidth, shortGI);
}

prefix_ unsigned senf::WLAN_MCSInfo::getRate(std::uint8_t index, std::uint8_t streams, unsigned bandwidth, bool shortGI)
{
    if (SENF_UNLIKELY(index >= MAX_INDEX or streams == 0 or streams > NUM_STREAMS))
        return 0;
    return mcsInfos[(streams-1) * MAX_INDEX + index].rate[toBandwidthIndex(bandwidth, shortGI)];
}

prefix_ std::uint8_t senf::WLAN_MCSInfo::toHTIndex(std::uint8_t index, std::uint8_t streams)
{
    if (SENF_UNLIKELY(index >= NUM_HT_INDEX or streams == 0 or streams > NUM_STREAMS))
        return 0;
    return (streams-1) * NUM_HT_INDEX + index;
}

prefix_ std::pair<std::uint8_t,std::uint8_t> senf::WLAN_MCSInfo::fromHTIndex(std::uint8_t mcsIndex)
{
    return std::make_pair(mcsIndex % NUM_HT_INDEX, (mcsIndex / NUM_HT_INDEX) + 1);
}

prefix_ std::uint8_t senf::WLAN_MCSInfo::toBandwidthIndex(unsigned bandwidth, bool shortGI)
{
    switch (bandwidth) {
    case  20000:
        return 0 + shortGI;
    case  40000:
        return 2 + shortGI;
    case  80000:
        return 4 + shortGI;
    case 160000:
        return 6 + shortGI;
    default:
        return 0;
    }
}

prefix_ unsigned senf::WLAN_MCSInfo::fromBandwidthIndex(std::uint8_t bandwidthIndex)
{
    switch (bandwidthIndex) {
    case 0:
    case 1:
        return  20000;
    case 2:
    case 3:
        return  40000;
    case 4:
    case 5:
        return  80000;
    case 6:
    case 7:
        return 160000;
    default:
        return 0;
    }
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
