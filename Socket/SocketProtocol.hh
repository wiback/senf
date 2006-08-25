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

#ifndef HH_SocketProtocol_
#define HH_SocketProtocol_ 1

// Custom includes
#include <boost/utility.hpp>

//#include "SocketProtocol.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace lib {

    class SocketBody;

    /** \brief
     */
    class SocketProtocol
        : boost::noncopyable
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        SocketProtocol();
        virtual ~SocketProtocol() = 0;

        // no default constructor
        // no copy
        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        SocketBody & body() const;

        ///////////////////////////////////////////////////////////////////////////

        virtual unsigned available() const = 0;

    protected:

    private:
        // That SocketBody instance owns us and controls our lifetime
        // Do we need this ??
        SocketBody * body_;

        friend class SocketBody;
    };

    /** \brief
     */
    template <class SocketPolicy>
    class ConcreteSocketProtocol
        : public SocketProtocol
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef SocketPolicy Policy;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        ~ConcreteSocketProtocol() = 0;

        // no default constructor
        // no copy
        // no conversion constructors

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        Policy const & policy() const;

    protected:

    private:
        Policy policy_;

    };

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "SocketProtocol.cci"
//#include "SocketProtocol.ct"
#include "SocketProtocol.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
