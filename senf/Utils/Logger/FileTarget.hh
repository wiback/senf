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
    \brief FileTarget public header */

#ifndef HH_SENF_Utils_Logger_FileTarget_
#define HH_SENF_Utils_Logger_FileTarget_ 1

// Custom includes
#include <fstream>
#include <boost/shared_ptr.hpp>
#include "IOStreamTarget.hh"

//#include "FileTarget.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    namespace console { class DirectoryNode; }

namespace log {

    /** \brief Log target writing to a %log file.

        The FileTarget will save all %log messages in the given file. Messages will be appended at
        the end of the file.

        \code
        senf::log::FileTarget target ("file.name");

        // Route all messages to this file.
        target.route();
        \endcode

        After %log files have been rotated, the reopen() member should be called to create a new %log
        file.

        \ingroup targets
      */
    class FileTarget
        : private boost::base_from_member<std::ofstream>,
          public IOStreamTarget
    {
        typedef boost::base_from_member<std::ofstream> ofstream_t;

    public:
        //-////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        //\{

        explicit FileTarget(std::string const & filename, std::string const & nodename = "");
                                        ///< Construct FileTarget writing to \a file

        //\}
        //-////////////////////////////////////////////////////////////////////////

        void reopen();                  ///< Reopen %log after log-file rotation
        void reopen(std::string const & file);  ///< Reopen %log under a new name

        std::string const & filename() const; ///< Return current log file name

    private:
        std::string file_;

        struct RegisterConsole {
            RegisterConsole();
            static boost::shared_ptr<senf::console::DirectoryNode> create(
                std::string const & filename, std::string const & nodename);
            static RegisterConsole instance;
        };
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "FileTarget.cci"
//#include "FileTarget.ct"
//#include "FileTarget.cti"
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
