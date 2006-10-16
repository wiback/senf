// $Id$
//
// Copyright (C) 2006 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

#ifndef HH_typeidvalue_
#define HH_typeidvalue_ 1

// Custom includes
#include <typeinfo>
#include <string>
#include <boost/scoped_ptr.hpp>
#include <boost/operators.hpp>

//#include "typeidvalue.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace lib {

    /** \brief Wrapper to use types as key's in a map
      */
    class TypeIdValue : public boost::totally_ordered<TypeIdValue>
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        // my default constructor
        // my copy constructor
        // my copy assignment
        // default destructor
        // no conversion constructors

        TypeIdValue();
        TypeIdValue(TypeIdValue const & other);
        TypeIdValue const & operator=(TypeIdValue const & other);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        bool operator==(TypeIdValue const & other) const;
        bool operator<(TypeIdValue const & other) const;

        std::string name() const;

    protected:

    private:
        template <class Type> TypeIdValue(Type *);

        struct Value {
            virtual ~Value() {}
            virtual std::type_info const & id() = 0;
            virtual Value * clone() = 0;
        };

        template <class Type>
        struct ValueImpl : public Value {
            virtual std::type_info const & id();
            virtual Value * clone();
        };

        boost::scoped_ptr<Value> value_;

        template <class Type> friend TypeIdValue const typeIdValue();
    };

    TypeIdValue const typeIdValue();
    
    template <class Type>
    TypeIdValue const typeIdValue();
    
}}

///////////////////////////////hh.e////////////////////////////////////////
#include "typeidvalue.cci"
//#include "typeidvalue.ct"
#include "typeidvalue.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
