//
// Copyright (c) 2020 Fraunhofer Institute for Applied Information Technology (FIT)
//                    Network Research Group (NET)
//                    Schloss Birlinghoven, 53754 Sankt Augustin, GERMANY
//                    Contact: http://wiback.org
//
// This file is part of the SENF code tree.
// It is licensed under the 3-clause BSD License (aka New BSD License).
// See LICENSE.txt in the top level directory for details or visit
// https://opensource.org/licenses/BSD-3-Clause
//


#ifndef HH_SENF_Socket_Protocols_DVB_DVBProtocolWrapper_
#define HH_SENF_Socket_Protocols_DVB_DVBProtocolWrapper_ 1

// Custom includes
#include <senf/Utils/Console/ScopedDirectory.hh>
#include <senf/Utils/Console/Traits.hh>
#include "DVBDemuxHandles.hh"

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {

    /** \brief Helperclass for configuration and controlling DVB protocol handles.

     */
    class DVBSectionProtocolWrapper : public DVBProtocolWrapper
    {
    private:
        const DVBDemuxSectionSocketProtocol& protocol;

    public:
        enum Flags {CHECK_CRC = DMX_CHECK_CRC,
            ONESHOT = DMX_ONESHOT,
            IMMEDIATE_START = DMX_IMMEDIATE_START};
        senf::console::ScopedDirectory<> dir;

        DVBSectionProtocolWrapper(senf::DVBDemuxSectionHandle sh);
        ~DVBSectionProtocolWrapper() { }

        void setSectionFilter(unsigned short int pid, uint8_t filter,
                unsigned int flags, uint8_t mask, uint8_t mode, unsigned int timeout);

        void setBufferSize(unsigned long size);
                                ///< Set the size of the circular buffer used for filtered data.
                                /**< The default size is two maximum sized sections, i.e. if
                                     this function is not called a buffer size of 2 * 4096 bytes
                                     will be used.
                                     \param[in] size Size of circular buffer. */
        void startFiltering();  ///< Starts filtering
        void stopFiltering();   ///< Stops filtering
    };


    class DVBPESProtocolWrapper : public senf::DVBProtocolWrapper
    {
    private:
        const senf::DVBDemuxPESSocketProtocol& protocol;

    public:
        enum Flags {CHECK_CRC = DMX_CHECK_CRC,
            ONESHOT = DMX_ONESHOT,
            IMMEDIATE_START = DMX_IMMEDIATE_START};
        senf::console::ScopedDirectory<> dir;

        DVBPESProtocolWrapper(senf::DVBDemuxPESHandle sh);
        ~DVBPESProtocolWrapper() { };

        void setPESFilter(unsigned short int pid, dmx_input_t input,
                dmx_output_t output, dmx_pes_type_t pesType, unsigned int flags);
        void startFiltering(); ///< Starts filtering
        void stopFiltering();  ///< Stops filtering
    };

}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "DVBProtocolWrapper.cci"
//#include "DVBProtocolWrapper.ct"
//#include "DVBProtocolWrapper.cti"
#endif

