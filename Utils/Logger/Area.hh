// $Id$
//
// Copyright (C) 2007 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
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
    \brief Area public header */

#ifndef HH_Area_
#define HH_Area_ 1

// Custom includes
#include <map>
#include <functional>
#include <boost/iterator/transform_iterator.hpp>
#include "../singleton.hh"

//#include "Area.mpp"
///////////////////////////////hh.p////////////////////////////////////////

/** \brief Define log area

    Defines a new log area named \a area. The area is defined as a symbol in the current scope.

    \hideinitializer
 */
#define SENF_LOG_DEF_AREA(area) SENF_LOG_DEF_AREA_I(area, ; )

namespace senf {
namespace log { 

    namespace detail { struct AreaBase; }
    
    class AreaRegistry
        : public senf::singleton<AreaRegistry>
    {
        typedef std::map<std::string, detail::AreaBase const *> Registry;

        struct SelectName 
        {
            typedef std::string result_type;
            std::string const & operator()(Registry::value_type const & v) const;
        };

    public:
        typedef boost::transform_iterator<SelectName, Registry::const_iterator> iterator;

        using senf::singleton<AreaRegistry>::instance;

        iterator begin();
        iterator end();

    private:
        AreaRegistry();

        void registerArea(detail::AreaBase const & area);

        Registry registry_;

        friend class senf::singleton<AreaRegistry>;
        friend class detail::AreaBase;
        friend class Target;
    };        
    
}}

///////////////////////////////hh.e////////////////////////////////////////
#include "Area.cci"
//#include "Area.ct"
//#include "Area.cti"
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
