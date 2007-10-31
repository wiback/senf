// $Id$
//
// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer NETwork research (NET)
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
    \brief IOStreamTarget public header */

#ifndef HH_IOStreamTarget_
#define HH_IOStreamTarget_ 1

// Custom includes
#include <boost/utility.hpp>
#include <boost/scoped_ptr.hpp>
#include "Target.hh"

//#include "IOStreamTarget.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace log {

    /** \brief Write log messages to arbitrary std::ostream

        This target will write log messages to an arbitrary std::ostream in the format
        <pre>
        <date> [<area>] <message>
        </pre>
        
        The date formatting is set using the Boost.DateTime date_facet, e.g.:
        \code
        stream.imbue( std::locale(stream.getloc(), 
                                  new boost::posix_time::time_facet("%Y%m%d %H:%M:%S")) );
        \endcode

        By default, the date-time will be written in extended ISO format.

        \warning The class keeps a reference to the passed stream.
        
        \note This class will permanently and globally change the date formating of the given
            stream if no \c boost::posix_time::time_facet has been set.

        \ingroup targets
      */
    class IOStreamTarget
        : public Target
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        explicit IOStreamTarget(std::ostream & os);

        ///@}
        ///////////////////////////////////////////////////////////////////////////

    protected:

    private:
        void v_write(boost::posix_time::ptime timestamp, std::string const & stream, 
                     std::string const & area, unsigned level, 
                     std::string const & message);

        std::ostream & stream_;
    };


}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "IOStreamTarget.cci"
//#include "IOStreamTarget.ct"
//#include "IOStreamTarget.cti"
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
