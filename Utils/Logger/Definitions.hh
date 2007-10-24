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
    \brief Definitions public header */

#ifndef HH_Definitions_
#define HH_Definitions_ 1

// Custom includes

//#include "Definitions.mpp"
#include "Definitions.ih"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace log {

    ///\addtogroup logging
    ///\{
    ///\name Defining logger objects
    ///\{

    /** \brief Define log stream

        Defines a new log stream named \a stream. The stream is defined as a symbol in the current
        scope.

        \a defaultLevel defines the default log level for messages posted to this stream. \a
        runtimeLimit defines the default log limit. Messages with a level below this will not be
        output.  \a compileLimit defines the default log level limit at compile time: Messages
        posted with a level below \a compileLimit will be discarded at compile time.

        \hideinitializer
     */
#   define SENF_LOG_DEF_STREAM(stream, defaultLevel_, runtimeLimit_, compileLimit_)               \
        struct stream                                                                             \
            : public senf::log::detail::StreamBase, public senf::singleton<stream>                \
        {                                                                                         \
            typedef defaultLevel_ defaultLevel;                                                   \
            typedef runtimeLimit_ runtimeLimit;                                                   \
            typedef compileLimit_ compileLimit;                                                   \
            static std::string name() { return instance().v_name(); }                             \
        private:                                                                                  \
            stream() { init(); }                                                                  \
            friend class senf::singleton<stream>;                                                 \
        }

    /** \brief Define log area

        Defines a new log area named \a area. The area is defined as a symbol in the current scope.

        \hideinitializer
        \ingroup logging
     */
#   define SENF_LOG_DEF_AREA(area) SENF_LOG_DEF_AREA_I(area, ; )

    /** \brief Define new default log area for the class

        This command declares the containing class to be it's own default log area. It is such like a
        combination of \ref SENF_LOG_DEF_AREA and \ref SENF_LOG_DEFAULT_AREA with a twist.
     */
#   define SENF_LOG_CLASS_AREA()                                                                  \
        SENF_LOG_DEF_AREA_I(                                                                      \
            SENFLogArea,                                                                          \
            std::string v_name() const                                                            \
                { std::string s (fullName()); return std::string(s,0,s.size()-13); });            \
        SENF_LOG_DEFAULT_AREA(SENFLogArea)


    /** \brief Define log parameter alias

        Defines a new parameter alias named \a alias as an alias for the parameters in \a args. The
        alias is defined as a symbol in the current scope.

        \hideinitializer
     */
#   define SENF_LOG_DEF_ALIAS(alias,args)                                                         \
        struct alias : public senf::log::detail::AliasBase                                        \
        {                                                                                         \
            template <class Base>                                                                 \
            struct apply                                                                          \
            {                                                                                     \
                typedef typename SENF_LOG_MERGE_PARAMETERS_I(Base,args) type;                     \
            };                                                                                    \
        }

        /** \brief Default global log stream */
        SENF_LOG_DEF_STREAM(Debug, MESSAGE, DISABLED, DISABLED);

        /** \brief Default global log area */
        SENF_LOG_DEF_AREA_I(DefaultArea,
                            std::string v_name() const { return ""; });

    ///\}
    ///\}

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "Definitions.cci"
//#include "Definitions.ct"
//#include "Definitions.cti"
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
