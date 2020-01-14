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
    \brief FileTarget public header */

#ifndef HH_SENF_Utils_Logger_FileTarget_
#define HH_SENF_Utils_Logger_FileTarget_ 1

// Custom includes
#include <fstream>
#include <boost/utility/base_from_member.hpp>
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
