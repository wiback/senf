//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


#include "StatisticAccumulator.hh"

// Custom includes
#include <ostream>
#include "Format.hh"

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

prefix_ void senf::StatisticsData::clear()
{
    min = max = NAN;
    avg = stddev = NAN;
    cnt = 0;
}

std::ostream & senf::operator<<(std::ostream & os, senf::StatisticsData const & _data)
{
    if (_data.cnt > 0) {
        os << "("  << "#" << _data.cnt;
        os << ";" << format::eng(_data.min).setprecision(5);
        os << ";" << format::eng(_data.avg, _data.stddev).setprecision(5);
        os << ";" << format::eng(_data.max).setprecision(5);
        os << ")";
    } else { 
        os << "(no-data)";
    }
    return os;
};

prefix_ boost::property_tree::ptree senf::StatisticsData::asPTree()
    const
{
    boost::property_tree::ptree localelement;
    if (cnt > 0) {
        localelement.put("avg", avg);
        localelement.put("stddev", stddev);
        localelement.put("min", min);
        localelement.put("max", max);
        localelement.put("cnt", cnt);
    } else {
        localelement.put("avg", "NaN");
        localelement.put("stddev", "NaN");
        localelement.put("min", "NaN");
        localelement.put("max", "NaN");
        localelement.put("cnt", 0u);
    }
    return localelement;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
