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
    \brief MMapSocketProtocol public header */

#ifndef HH_SENF_Socket_Protocols_Raw_MMapSocketProtocol_
#define HH_SENF_Socket_Protocols_Raw_MMapSocketProtocol_ 1

// Custom includes
#include <senf/Socket/SocketProtocol.hh>
#include <senf/Socket/QueueReadWritePolicy.hh>

//#include "MMapSocketProtocol.mpp"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    class MMapSocketProtocol
        : public virtual SocketProtocol,
          private FileHandleAccess
    {
    public:
        void close();
        void terminate() const;
        detail::QueueInfo::TxStats txStats() const;
        detail::QueueInfo::RxStats rxStats() const;
        bool interfaceDead() const;

        void timestamping(int sofFlags);

    protected:
        void init_mmap(unsigned frameSize, unsigned rxqlen, unsigned txqlen, unsigned reserve = 0, bool qDiscBypass = false) const;
        void close_mmap() const;
        void terminate_mmap() const;

        mutable detail::QueueInfo qi_;
    };

    class MMapReadableSocketProtocol
        : public virtual SocketProtocol,
          private FileHandleAccess
    {
    public:
        unsigned available() const;
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
//#include "MMapSocketProtocol.cci"
//#include "MMapSocketProtocol.ct"
//#include "MMapSocketProtocol.cti"
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
