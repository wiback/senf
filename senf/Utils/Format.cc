//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: support@wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


/** \file
    \brief Format non-inline non-template implementation */

#include "Format.hh"
#include "Format.ih"

// Custom includes
#include <cmath>
#include <boost/io/ios_state.hpp>
#include <iomanip>
#include <sstream>

//#include "Format.mpp"
#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

    char const SIPrefix[] = { 'y', 'z', 'a', 'f', 'p', 'n', 'u', 'm',
                              ' ',
                              'k', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y' };
    unsigned const SIScales = 8;

}

prefix_ std::ostream & senf::format::operator<<(std::ostream & os, eng const & v_)
{
    boost::io::ios_base_all_saver ibas (os);
    boost::io::ios_fill_saver ifs (os);

    os.setf(v_.flags_, v_.mask_);
    if (v_.havePrecision_)
        os.precision(v_.precision_);
    if (v_.haveWidth_)
        os.width(v_.width_);
    if (v_.haveFill_)
        os.fill(v_.fill_);

    unsigned prec (os.precision());
    if (prec < 4)
        prec = 4;
    unsigned w (os.width());
    char fill (os.fill());
    unsigned minw (prec+2+((os.flags() & std::ios_base::showbase) ? 1 : 4));
    std::ios_base::fmtflags flags (os.flags());
    std::ios_base::fmtflags align (flags & std::ios_base::adjustfield);
    if (! std::isnan(v_.d_))
        minw += prec+3;

    float ref (std::fabs(v_.v_));
    float v (v_.v_);
    float d (0.0);
    if (! std::isnan(v_.d_)) d = std::fabs(v_.d_);
    int scale (0);

    if (d > ref) ref = d;
    while (ref >= 1000.0f) {
        ref /= 1000.0f;
        v /= 1000.0f;
        d /= 1000.0f;
        scale += 3;
    }
    while (ref > 0 && ref < 1) {
        ref *= 1000.0f;
        v *= 1000.0f;
        d *= 1000.0f;
        scale -= 3;
    }

    os << std::dec << std::setprecision(prec-3) << std::fixed;
    if (w > 0) {
        if ((align == 0 || align == std::ios_base::right || align == std::ios_base::internal))
            os << std::setw(prec+2+(w>minw ? w-minw : 0));
        else
            os << std::right << std::setfill(' ') << std::setw(prec+2);
    }
    else
        os << std::right;
    os << v;

    os << std::setfill('0') << std::noshowpos;
    if (! std::isnan(v_.d_)) {
        os << "+-";
        if (w > 0)
            os << std::setw(prec+1);
        os << d;
    }

    if ((flags & std::ios_base::showbase) && unsigned(std::abs(scale/3)) <= SIScales) {
        if (w > 0 || scale != 0)
            os << SIPrefix[scale/3+SIScales];
    }
    else if ((flags & std::ios_base::showpoint) || scale != 0)
        os << ((flags & std::ios_base::uppercase)?'E':'e')
           << std::showpos << std::internal << std::setw(3) << scale;
    else if (w > 0)
        os << "    ";
    if (w > minw && align == std::ios_base::left)
        os << std::setfill(fill) << std::setw(w-minw) << "";

    return os;
}

prefix_ std::string senf::format::detail::dumpintSigned(long long v, unsigned bits)
{
    if (v<0) return dumpintUnsigned(-v,bits,-1);
    else     return dumpintUnsigned(v,bits,+1);
}

prefix_ std::string senf::format::detail::dumpintUnsigned(unsigned long long v, unsigned bits,
                                                          int sign)
{
    int bytes ((bits+7)/8);
    int digs (int(2.4*bytes)+1);
    std::stringstream ss;
    ss << (sign ? (sign<0 ? "-" : " ") : "")
       << "0x" << std::setw(2*bytes) << std::setfill('0') << std::hex
       << 1u*v
       << " (" << std::setw(digs+(sign ? 1 : 0)) << std::setfill(' ') << std::dec;
    if (sign)
        ss << sign*static_cast<long long>(v);
    else
        ss << 1u*v;
    ss << ") (";
    for (int i (bytes-1); i>=0; --i) {
        char c ((v>>(8*i))&0xff);
        ss << ((c>=32 && c<=126) ? c : '.');
    }
    ss << ')';
    return ss.str();
}

unsigned int senf::format::IndentHelper::static_level = 0;


//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_
//#include "Format.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
