// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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
    \brief BufferingPolicy public header
 */

#ifndef HH_BufferingPolicy_
#define HH_BufferingPolicy_ 1

// Custom includes
#include "FileHandle.hh"
#include "SocketPolicy.hh"

//#include "BufferingPolicy.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /// \addtogroup policy_impl_group
    /// @{

    /** \brief BufferingPolicy implementing standard socket buffering

        This policy class implements standard BSD socket buffering.

        \todo Shouldn't this be dependent on Read / WritePolicy ?
     */
    struct SocketBufferingPolicy : public BufferingPolicyBase
    {
        static unsigned rcvbuf(FileHandle handle);
                                        ///< Check receive buffer size
                                        /**< \param[in] handle socket handle to check
                                             \returns size of receive buffer in bytes */
        static void rcvbuf(FileHandle handle, unsigned size);
                                        ///< Change receive buffer size
                                        /**< \param[in] handle socket handle
                                             \param[in] size new receive buffer size */

        static unsigned sndbuf(FileHandle handle);
                                        ///< Check send buffer size
                                        /**< \param[in] handle socket handle to check
                                             \returns size of send buffer in bytes */
        static void sndbuf(FileHandle handle, unsigned size);
                                        ///< Change size of send buffer
                                        /**< \param[in] handle socket handle
                                             \param[in] size new send buffer size */
    };

    /// @}

}

///////////////////////////////hh.e////////////////////////////////////////
//#include "BufferingPolicy.cci"
//#include "BufferingPolicy.ct"
//#include "BufferingPolicy.cti"
//#include "BufferingPolicy.mpp"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// End:
