// $Id$
//
// Copyright (C) 2012
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// http://senf.berlios.de/license.html
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
//   Thorsten Horstmann <tho@berlios.de>

#include "MCSInfo.hh"

// Custom includes
#include <senf/Utils/senfassert.hh>

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

// http://mcsindex.com/
static senf::WLAN_MCSInfo::Info mcsInfos[] = {
//                        bandwidth:     20 MHz           40 MHz        20 MHz 40 MHz
//                               GI: 800 ns   400 ns  800 ns  400 ns
        { 0,   1,  "BPSK_1/2",    {   6500,    7200,  13500,  15000 }, { -82, -79 } },
        { 1,   1,  "QPSK_1/2",    {  13000,   14400,  27000,  30000 }, { -79, -76 } },
        { 2,   1,  "QPSK_3/4",    {  19500,   21700,  40500,  45000 }, { -77, -74 } },
        { 3,   1,  "16-QAM_1/2",  {  26000,   28900,  54000,  60000 }, { -74, -71 } },
        { 4,   1,  "16-QAM_3/4",  {  39000,   43300,  81000,  90000 }, { -70, -67 } },
        { 5,   1,  "64-QAM_2/3",  {  52000,   57800, 108000, 120000 }, { -66, -63 } },
        { 6,   1,  "64-QAM_3/4",  {  58500,   65000, 121500, 135000 }, { -65, -62 } },
        { 7,   1,  "64-QAM_5/6",  {  65000,   72200, 135000, 150000 }, { -64, -61 } },

        { 8,   2,  "BPSK_1/2",    {  13000,   14400,  27000,  30000 }, { -82, -79 } },
        { 9,   2,  "QPSK_1/2",    {  26000,   28900,  54000,  60000 }, { -79, -76 } },
        { 10,  2,  "QPSK_3/4",    {  39000,   43300,  81000,  90000 }, { -77, -74 } },
        { 11,  2,  "16-QAM_1/2",  {  52000,   57800, 108000, 120000 }, { -74, -71 } },
        { 12,  2,  "16-QAM_3/4",  {  78000,   86700, 162000, 180000 }, { -70, -67 } },
        { 13,  2,  "64-QAM_2/3",  { 104000,  115600, 216000, 240000 }, { -66, -63 } },
        { 14,  2,  "64-QAM_3/4",  { 117000,  130000, 243000, 270000 }, { -65, -62 } },
        { 15,  2,  "64-QAM_5/6",  { 130000,  144400, 270000, 300000 }, { -64, -61 } },

        { 16,  3,  "BPSK_1/2",    {  19500,   21700,  40500,  45000 }, { -82, -79 } },
        { 17,  3,  "QPSK_1/2",    {  39000,   43300,  81000,  90000 }, { -79, -76 } },
        { 18,  3,  "QPSK_3/4",    {  58500,   65000, 121500, 135000 }, { -77, -74 } },
        { 19,  3,  "16-QAM_1/2",  { 780000,   86700, 162000, 180000 }, { -74, -71 } },
        { 20,  3,  "16-QAM_3/4",  { 117000,  130700, 243000, 270000 }, { -70, -67 } },
        { 21,  3,  "64-QAM_2/3",  { 156000,  173300, 324000, 360000 }, { -66, -63 } },
        { 22,  3,  "64-QAM_3/4",  { 175500,  195000, 364500, 405000 }, { -65, -62 } },
        { 23,  3,  "64-QAM_5/6",  { 195000,  216700, 405000, 450000 }, { -64, -61 } },

        { 24,  4,  "BPSK_1/2",    {  26000,   28800,  54000,  60000 }, { -82, -79 } },
        { 25,  4,  "QPSK_1/2",    {  52000,   57600, 108000, 120000 }, { -79, -76 } },
        { 26,  4,  "QPSK_3/4",    {  78000,   86800, 162000, 180000 }, { -77, -74 } },
        { 27,  4,  "16-QAM_1/2",  { 104000,  115600, 216000, 240000 }, { -74, -71 } },
        { 28,  4,  "16-QAM_3/4",  { 156000,  173200, 324000, 360000 }, { -70, -67 } },
        { 29,  4,  "64-QAM_2/3",  { 208000,  231200, 432000, 480000 }, { -66, -63 } },
        { 30,  4,  "64-QAM_3/4",  { 234000,  260000, 486000, 540000 }, { -65, -62 } },
        { 31,  4,  "64-QAM_5/6",  { 260000,  288800, 540000, 600000 }, { -64, -61 } }
};

}

prefix_ std::vector<senf::WLAN_MCSInfo::Info> senf::WLAN_MCSInfo::getInfos()
{
    return std::vector<Info>(mcsInfos, mcsInfos + sizeof(mcsInfos) / sizeof(Info));
}

prefix_ senf::WLAN_MCSInfo::Info const & senf::WLAN_MCSInfo::getInfo(boost::uint8_t mcsIndex)
{
    SENF_ASSERT(mcsIndex > 31, "invalid mcs index");
    return mcsInfos[mcsIndex];
}

prefix_ unsigned senf::WLAN_MCSInfo::getRate(boost::uint8_t mcsIndex, unsigned bandwidth, bool shortGI)
{
    if (mcsIndex > 31)
        return 0;
    return mcsInfos[mcsIndex].rate[ (bandwidth==20 ? 0 : 2) + (shortGI ? 1 : 0) ];
}


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
