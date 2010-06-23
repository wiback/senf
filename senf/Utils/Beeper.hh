// $Id$
//
// Copyright (C) 2010
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <tho@berlios.de>
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
    \brief Beeper public header */

#ifndef HH_SENF_Utils_Beeper_
#define HH_SENF_Utils_Beeper_ 1

// Custom includes
#include <string>
#include <senf/Scheduler/TimerEvent.hh>

///////////////////////////////hh.p////////////////////////////////////////

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
                                        /**< \param freq frequency in Hz, where 0 <freqN < 20000.
                                                 The regular terminal beep is around 750Hz.
                                             \param length duration in milliseconds.
                                             \param count number of repetitions (defaults to 1).
                                             \param delay delay between repetitions in milliseconds. */
        void beep_async(float freq, unsigned length, unsigned count = 1, unsigned delay = 100);
                                        ///< play beep asynchronous
                                        /**< \param freq frequency in Hz, where 0 <freqN < 20000.
                                                 The regular terminal beep is around 750Hz.
                                             \param length duration in milliseconds.
                                             \param count number of repetitions (defaults to 1).
                                             \param delay delay between repetitions in milliseconds. */

        void stop_beep();               ///< stop playing.

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

        bool start_beep(float freq);
        void timeout();
    };

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "Beeper.cci"
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
