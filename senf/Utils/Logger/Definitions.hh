// $Id$
//
// Copyright (C) 2007
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
//   Stefan Bund <g0dil@berlios.de>

/** \file
    \brief Definitions public header */

#ifndef HH_SENF_Utils_Logger_Definitions_
#define HH_SENF_Utils_Logger_Definitions_ 1

// Custom includes

//#include "Definitions.mpp"
#include "Definitions.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace log {

    ///\addtogroup logging
    //\{
    ///\name Defining logger objects
    //\{

    /** \brief Define log stream

        Defines a new log stream named \a stream. The stream is defined as a symbol in the current
        scope.

        \a defaultLevel defines the default log level for messages posted to this stream. \a
        runtimeLimit defines the default log limit. Messages with a level below this will not be
        output.  \a compileLimit defines the default log level limit at compile time: Messages
        posted with a level below \a compileLimit will be discarded at compile time.

        Normally the \a compileLimit will be \c NOTICE. This will enable all messages
        <em>except</em> \c VERBOSE messages. These must then be enabled explicitly using the compile
        time \ref config macro.

        \hideinitializer
     */
#   define SENF_LOG_DEFINE_STREAM(stream, defaultLevel_, runtimeLimit_, compileLimit_)            \
        struct stream                                                                             \
            : public senf::log::detail::StreamBase, public senf::singleton<stream>                \
        {                                                                                         \
            typedef defaultLevel_ defaultLevel;                                                   \
            typedef runtimeLimit_ runtimeLimit;                                                   \
            typedef compileLimit_ compileLimit;                                                   \
            static std::string name() { return instance().v_name(); }                             \
            unsigned defaultRuntimeLimit() const { return runtimeLimit::value; }                  \
            using senf::singleton<stream>::instance;                                              \
        private:                                                                                  \
            stream() { init(); }                                                                  \
            friend class senf::singleton<stream>;                                                 \
        }

    /** \brief Define log area

        Defines a new log area named \a area. The area is defined as a symbol in the current scope.

        \hideinitializer
     */
#   define SENF_LOG_DEFINE_AREA(area) SENF_LOG_DEFINE_AREA_I(area, ; )

#   define SENF_LOG_DEFINE_NAMED_AREA(area, name)                                                 \
        SENF_LOG_DEFINE_AREA_I(area,                                                              \
            std::string v_name() const { return name; } );

    /** \brief Define new default log area for the class

        This command declares the containing class to be it's own default log area. It is such like
        a combination of \ref SENF_LOG_DEFINE_AREA and \ref SENF_LOG_DEFAULT_AREA with a twist.

        \hideinitializer
     */
    // See Definitions.ih for implementation details on SENF_LOG_CLASS_AREA
#   define SENF_LOG_CLASS_AREA()                                                                  \
        SENF_LOG_DEFINE_AREA_I(                                                                   \
            SENFLogArea,                                                                          \
            std::string v_name() const                                                            \
                { std::string s (fullName()); return std::string(s,0,s.size()-13); });            \
        SENF_LOG_DEFAULT_AREA(SENFLogArea)


    /** \brief Define log parameter alias

        Defines a new parameter alias named \a alias as an alias for the parameters in \a args. The
        alias is defined as a symbol in the current scope.

        \hideinitializer
     */
#   define SENF_LOG_DEFINE_ALIAS(alias,args)                                                      \
        struct alias : public senf::log::detail::AliasBase                                        \
        {                                                                                         \
            template <class Base>                                                                 \
            struct apply                                                                          \
            {                                                                                     \
                typedef typename SENF_LOG_MERGE_PARAMETERS_I(Base,args) type;                     \
            };                                                                                    \
        }

    /** \brief Default global log stream */
    SENF_LOG_DEFINE_STREAM(Debug, MESSAGE, MESSAGE, NOTICE);

    /** \brief Default global %log area */
    SENF_LOG_DEFINE_AREA(DefaultArea);

    //\}
    //\}

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
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
