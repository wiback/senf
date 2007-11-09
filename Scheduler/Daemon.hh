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
    \brief Daemon public header */

#ifndef HH_Daemon_
#define HH_Daemon_ 1

// Custom includes
#include <boost/utility.hpp>

//#include "Daemon.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \brief Daemon process

        This class provides the infrastructure to implement robust daemon processes. A daemon
        process is implemented by deriving from senf::Daemon and implementing the necessary
        (virtual) member functions.
        \code
        class MyDaemon : public senf::Daemon
        {
            void configure() {
                // Set configuration parameters like daemonize(), pidFile() etc.
            }

            void init() {
                // Initialize application. Setup all necessary objects. After init()
                // has completed, is startup should not fail anymore
            }

            void run() {
                // Main application code should be called here.
            }
        };
        \endcode

        The startup procedure is divided into three steps:
        \li First, configure() is called. configure() should be as simple as possible. It just needs
            to set the daemon parameters. No further setup should be done here.
        \li init() is called after fork() but while still connected to the terminal. init() should
            do all necessary application setup. Here, all configuration or user errors should be
            detected and properly diagnosed.
        \li After init() returns, the application will detach from the terminal. Now run() is called
            to enter the application main loop.

        Since there are times, where separating init() and run() into two separate functions is
        difficult, instead of defining init() and run(), the member main() may be defined. This
        member must call detach() as soon as initialization is completed to detach from the
        foreground terminal.
      */
    class Daemon : boost::noncopyable
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types
        
        /// Select standard stream to redirect
        enum StdStream { 
            StdOut  /** Standard output stream */
        ,   StdErr  /** Standard error stream */
        ,   Both    /** Both, standard output and error stream */
        }; 

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///\{

        virtual ~Daemon();

        ///\}
        ///\name Parameters
        ///\{

        void daemonize(bool);           ///< Configure whether to run in fore- or background
        bool daemon();                  ///< \c true, if running as daemon

        void consoleLog(std::string const &, StdStream which = Both); 
                                        ///< Configure console log file
                                        /**< May be called multiple times to set the log file
                                             for stdout and stderr seperately. Any standard stream
                                             not assigned to a log file will be redirected to
                                             <tt>/dev/null</tt>. 

                                             When running in the foreground, the log files will be
                                             ignored. */

        void pidFile(std::string const &); ///< Configure pid file

        ///\}
        ///\name Auxiliary helpers
        ///\{

        void detach();                  ///< Detach into background now
                                        /**< This is \e not the same as forking. The process will
                                             already have forked into the background but until
                                             detach() is called (either automatically after init()
                                             returns or manually), the front end (foreground)
                                             process will wait for the background process to ensure
                                             successful startup. */

        int argc();                     ///< Access command line parameter count
        char const ** argv();           ///< Access command line parameters

        void fail(int code=1);          ///< Terminate startup with failure

        ///\}
        
        int start(int argc, char const ** argv); ///< Called from main() to launch daemon.
                                        /**< Normally not called directly but from the
                                             \ref SENF_DAEMON_MAIN macro. */

    protected:
        Daemon();

#   ifdef DOXYGEN
    protected:
#   else
    private:
#   endif

        virtual void configure();       ///< Called before forking to configure the daemon class
        virtual void main();            ///< Called after forking to execute the main application
                                        /**< The default implementation will call init(), detach()
                                             and then run(). It is preferred to override init() and
                                             run() if possible. */
        virtual void init();            ///< Called to initialize the main application
                                        /**< While init() is running, the application still is
                                             connected to the controlling terminal. Error messages
                                             will be shown to the user.

                                             This member is only called, if the default main()
                                             implementation is not overridden. */
        virtual void run();             ///< Called to execute main application
                                        /**< Called after detaching from the controlling
                                             terminal.

                                             This member is only called, if the default main()
                                             implementation is not overridden. */
    private:

        void fork();
        bool pidfileCreate();

        int argc_;
        char const ** argv_;

        bool daemonize_;
        int stdout_;
        int stderr_;
        std::string pidfile_;

        bool detached_;
    };

    /** \brief Provide \c main() function

        This macro will provide a \c main() function to launch the daemon process defined in \a
        klass. \a klass must be a class derived from senf::Daemon.

        \ingroup process
     */
#   define SENF_DAEMON_MAIN(klass)                                                                \
        int main(int argc, char const ** argv)                                                    \
        {                                                                                         \
            klass instance;                                                                       \
            return instance.start(argc, argv);                                                    \
        }

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "Daemon.cci"
//#include "Daemon.ct"
//#include "Daemon.cti"
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
