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
    \brief Log public header */

#ifndef HH_Log_
#define HH_Log_ 1

// Custom includes
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/dec.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/seq/pop_back.hpp>
#include "Parameters.hh"

//#include "Log.mpp"
///////////////////////////////hh.p////////////////////////////////////////

/** \brief Write log message

    This macro will write it's last argument to the log stream. The last argument must be an
    expression which will be placed after a streaming \c operator<< (like
    <i>some-log-sttream</i> \c << <i>last-macro-arg</i>).
    \code
    SENF_LOG((parameters...)("log message " << args << ...));
    \endcode

    \hideinitializer
 */
#define SENF_LOG(args)                                                                            \
    SENF_LOG_BLOCK_( SENF_LOG_MERGE_PARAMETERS(BOOST_PP_SEQ_POP_BACK(args)),                      \
                     { log << BOOST_PP_SEQ_ELEM(BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(args)),args); })

#define SENF_LOG_TPL(args)                                                                        \
    SENF_LOG_BLOCK_( SENF_LOG_MERGE_PARAMETERS_TPL(BOOST_PP_SEQ_POP_BACK(args)),                  \
                     { log << BOOST_PP_SEQ_ELEM(BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(args)),args); })

/** \brief Enable block based on logging parameters

    This macro is like SENF_LOG, however instead of writing a simple message, this macro allows
    to specify a complete block of code to be executed if the log message is enabled.
    \code
    SENF_LOG_BLOCK((parameters...)({
       // arbitrary code using 'log' for logging
       log << "log message";
    }));
    \endcode

    \hideinitializer
 */
#define SENF_LOG_BLOCK(args)                                                                      \
    SENF_LOG_BLOCK_( SENF_LOG_MERGE_PARAMETERS(BOOST_PP_SEQ_POP_BACK(args)),                      \
                     BOOST_PP_SEQ_ELEM(BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(args)),args))

#define SENF_LOG_BLOCK_TPL(args)                                                                  \
    SENF_LOG_BLOCK_( SENF_LOG_MERGE_PARAMETERS_TPL(BOOST_PP_SEQ_POP_BACK(args)),                  \
                     BOOST_PP_SEQ_ELEM(BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(args)),args))

#define SENF_LOG_BLOCK_(parameters, block)                                                        \
    do {                                                                                          \
        if (parameters::compile_enabled && parameters::enabled()) {                               \
            std::ostream & log (parameters::log_stream());                                         \
            do block while(0);                                                                    \
            log << std::endl;                                                                     \
        }                                                                                         \
    } while(0) 

///////////////////////////////hh.e////////////////////////////////////////
//#include "Log.cci"
//#include "Log.ct"
//#include "Log.cti"
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
