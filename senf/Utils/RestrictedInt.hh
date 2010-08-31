// $Id$
//
// Copyright (C) 2010
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

/** \file
    \brief RestrictedInt public header */

#ifndef HH_SENF_senf_Utils_RestrictedInt_
#define HH_SENF_senf_Utils_RestrictedInt_ 1

// Custom includes
#include <iostream>
#include <boost/operators.hpp>
#include <senf/Utils/safe_bool.hh>

//#include "RestrictedInt.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    // Really ... the correct spelling is 'euclidean' as in the current boost
    // Versions but older versions only have this spelling so we can' t use
    // the correct class name ...

    template <class Base, class Tag>
    class RestrictedInt
        : public boost::ordered_euclidian_ring_operators< RestrictedInt<Base,Tag>,
                 boost::unit_steppable< RestrictedInt<Base,Tag>,
                 boost::shiftable< RestrictedInt<Base,Tag>,
                 boost::bitwise1< RestrictedInt<Base,Tag>,
                 senf::comparable_safe_bool< RestrictedInt<Base,Tag> > > > > >
    {
    public:
        explicit RestrictedInt(Base value) : value_ (value) {}
        RestrictedInt() : value_ () {}

#       define BinaryOp(op)                                             \
            RestrictedInt & operator op ## = (RestrictedInt other)      \
                { value_ op ## = other.value_; return *this; }
#       define IncDecOp(op)                                             \
            RestrictedInt & operator op ()                              \
                { op value_; return *this; }
#       define PrefixOp(op)                                             \
            RestrictedInt const operator op () const                    \
                { return RestrictedInt(op value_); }
#       define CompareOp(op)                                            \
            bool operator op (RestrictedInt other) const                \
                { return value_ op other.value_; }

        BinaryOp(+)
        BinaryOp(-)
        BinaryOp(*)
        BinaryOp(/)
        BinaryOp(%)
        BinaryOp(>>)
        BinaryOp(<<)
        BinaryOp(&)
        BinaryOp(|)
        BinaryOp(^)

        IncDecOp(++)
        IncDecOp(--)

        PrefixOp(~)
        PrefixOp(-)

        CompareOp(<)
        CompareOp(==)

#       undef CompareOp
#       undef PrefixOp
#       undef PostfixOp
#       undef BinaryOp

        Base value() const
            { return value_; }

        bool boolean_test() const
            { return value_; }

    private:
        Base value_;
    };

    template <class Base, class Tag>
    std::ostream & operator<<(std::ostream & os, RestrictedInt<Base, Tag> value)
        { os << value.value(); return os; }

    template <class Base, class Tag>
    std::istream & operator>>(std::istream & is, RestrictedInt<Base, Tag> & value)
        { Base v; is >> v; value = RestrictedInt<Base,Tag>(v); return is; }

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "RestrictedInt.cci"
//#include "RestrictedInt.ct"
//#include "RestrictedInt.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
