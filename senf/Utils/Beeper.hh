// $Id$
//
// Copyright (C) 2010
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
//   Thorsten Horstmann <tho@berlios.de>

/** \file
    \brief Beeper public header */

#ifndef HH_SENF_Utils_Beeper_
#define HH_SENF_Utils_Beeper_ 1

// Custom includes
#include <string>
#include <senf/Scheduler/TimerEvent.hh>

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Helper class to beep the pc speaker

        This helper class allows you to beep the pc speaker with precision like frequency and
        duration. The beep can be played synchronous and asynchronous.

        Typically the <tt>/dev/console</tt> device is used to control the local pc
        speaker. Note, that opening <tt>/dev/console</tt> is a privileged operation on
        most systems.

        \see manual page console_ioctl(4) for the according ioctl <tt>KIOCSOUND</tt>
      */
    class Beeper
    {
    public:
        Beeper(std::string const & device = "/dev/console");
                                        ///< Construct a new Beeper for the given device.
        ~Beeper();

        void beep_sync(float freq, unsigned length, unsigned count = 1, unsigned delay = 100);
                                        ///< play beep synchronous
                                        /**< \param freq frequency in Hz, where 0 < \e freq < 20000.
                                                 The regular terminal beep is around 750Hz.
                                             \param length duration in milliseconds.
                                             \param count number of repetitions (defaults to 1).
                                             \param delay delay between repetitions in milliseconds. */
        void beep_async(float freq, unsigned length, unsigned count = 1, unsigned delay = 100);
                                        ///< play beep asynchronous
                                        /**< \param freq frequency in Hz, where 0 < \e freq < 20000.
                                                 The regular terminal beep is around 750Hz.
                                             \param length duration in milliseconds.
                                             \param count number of repetitions (defaults to 1).
                                             \param delay delay between repetitions in milliseconds. */

        void stop_beep();               ///< stop playing.
        bool start_beep(float freq);    ///< start playing.
                                        /**< start playing until any other member is called.
                                             \param freq frequency in Hz, where 0 < \e freq < 20000.
                                             \returns \a true on success.*/

    private:
        static const unsigned CLOCK_TICK_RATE = 1193180;

        int fd_;
        scheduler::TimerEvent timer_;

#ifndef DOXYGEN
        struct {
            float freq;
            unsigned length;
            unsigned count;
            unsigned delay;
            bool action;
        } params_;
#endif

        void timeout();
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "Beeper.cci"
//#include "Beeper.ct"
//#include "Beeper.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
