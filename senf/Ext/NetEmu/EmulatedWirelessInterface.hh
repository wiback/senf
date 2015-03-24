// $Id:$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fokus.fraunhofer.de.de/license.html
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
//       Stefan Bund <g0dil@berlios.de>

/** \file
    \brief EmulatedWirelessInterface public header */

#ifndef HH_SENF_Ext_NetEmu_EmulatedWirelessInterface_
#define HH_SENF_Ext_NetEmu_EmulatedWirelessInterface_ 1

// Custom includes
#include <boost/signals2.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <senf/Scheduler/IdleEvent.hh>
#include <senf/Utils/Console/Traits.hh>
#include <senf/Utils/Console/UDPServer.hh>
#include <senf/Utils/Console/Utility.hh>
#include <senf/Utils/singleton.hh>
#include "WirelessInterfaceAPI.hh"
#include "EmulatedInterface.hh"
#include "Annotations.hh"

#include "EmulatedWirelessInterface.ih"
//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    /** \brief Wireless spectrum management

        EmulatedFrequencyRegistry manages all wireless channels. It associates each channel with a
        multicast group and port number on which all traffic for that channel is carried. The
        coordination between several nodes is managed using a special console server on another
        multicast port.

        The EmulatedFrequencyRegistry is a singleton. The registry manages all frequencies for any
        technology.

        <table class="senf">
        <tr><td></td><td>Default</td><td>Description</td></tr>

        <tr><th>consoleGroup</th><td>239.202.104.1:4701<td>multicast group and port for control
        messages</td></tr>

        <tr><th>addressRange</th><td>239.202.108.0/22, 11264</td><td>multicast address and port
        range to allocate to channels</td>
        </table>

        You will not normally use the registry directly. Instead, each wireless interface
        implementation calls on the registry for multicast group assignment.

        \warning You must start the registry before any traffic will flow. This start command must
            be sent to all nodes when all nodes are up and running. The command can be sent by
            multicasting \c start to the console group.

        To reinitialize after a node has died or a new node has come alive, restart the registry by
        sending first \c stop and (after a short delay) \c start to all nodes.

        \ingroup senf_emu_emulation
     */
    class EmulatedFrequencyRegistry
        : public senf::singleton<EmulatedFrequencyRegistry>
    {
        SENF_LOG_CLASS_AREA();

    public:
        typedef boost::function<void (UDPClientHandle::Address const & address)> UserCallback;
                                        ///< Callback type called to set channel address
        typedef boost::function<void (unsigned frequency, unsigned bandwidth)> CollisionCallback;
                                        ///< Callback type called for channel collisions

        /** \brief Channel allocation entry */
        struct Entry
        {
            unsigned frequency;         ///< Channel frequency
            unsigned bandwidth;         ///< Channel bandwidth
            UDPClientHandle::Address address; ///< Assigned multicast address
            unsigned nUsers;            ///< Number of active channel users
            unsigned ownerNodeId;       ///< Channel master

        private:
            Entry();
            Entry(unsigned frequency_, unsigned bandwidth_);
            Entry(unsigned frequency_, unsigned bandwidth_, UDPClientHandle::Address address_,
                  unsigned nUsers_, unsigned ownerNodeId_);

            friend class EmulatedFrequencyRegistry;
        };

    private:
        struct ByFrequency {};
        struct ByAddress {};

        // The ByAddress index really is unique except for the 'None' address, which may occur
        // more than once
        struct EntryIndices
            : public boost::multi_index::indexed_by<
                  boost::multi_index::ordered_unique<
                      boost::multi_index::tag<ByFrequency>,
                      boost::multi_index::composite_key<
                          Entry,
                          boost::multi_index::member<Entry, unsigned, &Entry::frequency>,
                          boost::multi_index::member<Entry, unsigned, &Entry::bandwidth> > >,
                  boost::multi_index::ordered_non_unique<
                      boost::multi_index::tag<ByAddress>,
                      boost::multi_index::member<Entry, UDPClientHandle::Address, &Entry::address> >
              >
        {};

        typedef boost::multi_index_container<Entry, EntryIndices> Entries;

        struct User
        {
            Entries::index_iterator<ByFrequency>::type entry;
            unsigned interfaceId;
            mutable UserCallback callback;

            User();
            User(Entries::index_iterator<ByFrequency>::type entry_, unsigned interfaceId_,
                 UserCallback callback_);
        };

        struct ByInterfaceId {};
        struct ByEntry {};

        // Since the multi_index_container iterators are not comparable, we sort the ByEntry index
        // using the address of the Entry referenced by the 'entry' iterator. Additional overloads
        // allow searching either by iterator or by Entry address.
        struct AddrCompare
        {
            bool operator()(Entries::index_iterator<ByFrequency>::type a,
                            Entries::index_iterator<ByFrequency>::type b) const;
            bool operator()(Entries::index_iterator<ByFrequency>::type a,
                            Entry const * b) const;
            bool operator()(Entry const * a,
                            Entries::index_iterator<ByFrequency>::type b) const;
        };

        struct UserIndices
            : public boost::multi_index::indexed_by<
                  boost::multi_index::ordered_unique<
                      boost::multi_index::tag<ByInterfaceId>,
                      boost::multi_index::member<User, unsigned, &User::interfaceId> >,
                  boost::multi_index::ordered_non_unique<
                      boost::multi_index::tag<ByEntry>,
                      boost::multi_index::member<
                          User, Entries::index_iterator<ByFrequency>::type, &User::entry>,
                      AddrCompare>
            >
        {};

        typedef boost::multi_index_container<User, UserIndices> Users;

        typedef boost::multi_index::member<User, unsigned, &User::interfaceId> GetInterfaceId;
        typedef boost::transform_iterator<
            GetInterfaceId, Users::index_iterator<ByEntry>::type> InterfaceIdIterator;

    public:
        typedef boost::iterator_range<Entries::index_iterator<ByFrequency>::type> EntriesRange;
                                        ///< Iterator range of channel entries
        typedef boost::iterator_range<InterfaceIdIterator> UsersRange;
                                        ///< Iterator range of interface id's using a channel

        using senf::singleton<EmulatedFrequencyRegistry>::instance;
        using senf::singleton<EmulatedFrequencyRegistry>::alive;

        UDPClientHandle::Address consoleGroup() const;
                                        ///< Get control channel multicast group/port
        void consoleGroup(UDPClientHandle::Address const & group);
                                        ///< Change control channel multicast group/port

        std::pair<INet4Network, unsigned> addressRange() const;
                                        ///< Get address/port range of channel multicast addresses
                                        /**< Returns a pair \a networkAndMask, \a
                                             startPort. Multicast ports are allocated from \a
                                             networkAndMask. Ports are allocated starting at \a
                                             startPort. The number of valid ports is given by the
                                             size of the multicast network.
                                             \warning It must be ensured, that all the multicast
                                                 addresses and ports in the range are free to
                                                 use. */
        void addressRange(INet4Network const & range, unsigned portbase);
                                        ///< Change address/port range of channel multicast addresses
                                        /**< \see \ref addressRange() */
        void nextAddress(unsigned index); ///< Set next address index to use (if free)
                                        /**< This sets the internal address counter to \a index. The
                                             next address allocated will be at \a index \e if that
                                             address is available. */

        void allocate(unsigned interfaceId, unsigned frequency, unsigned bandwidth, UserCallback cb);
                                        ///< Register for a wireless channel
                                        /**< Registers interest in the network channel at \a
                                             frequency/\a bandwidth. As soon as a multicast
                                             address/port assignment is available, \a cb is called
                                             with this address/port.

                                             \a cb will be called, whenever the multicast
                                             address/port changes.
                                          */
        void release(unsigned interfaceId);
                                        ///< Release channel registration
                                        /**< \see \ref allocate() */

        void collisionCallback(CollisionCallback cb);
                                        ///< Change collision callback
                                        /**< The callback \a cb is called whenever a channel
                                             collision is detected. A collision is detected if two
                                             different channels overlap. A collision is \e never
                                             detected for two identical channels since two requests
                                             for the same channel will automatically be merged into
                                             one channel. */

        void schedulerStart();
        void start();                   ///< Start multicast address/port allocation
        void stop();                    ///< Stop multicast address/port allocation
                                        /**< Calling stop will purge all channel entries except
                                             those registered by local interfaces. */

        EntriesRange entries() const;   ///< Get all channel entries
        UsersRange users(EntriesRange::iterator i) const;
                                        ///< Get interface id's of local users of a given channel

        enum ChangeType { JOIN, LEAVE, STOP };
        boost::signals2::signal<void (
            Entry const &, unsigned nodeId, ChangeType type)> channelPopulationChanged;
                                        ///< Channel population changed signal
                                        /**< This signal is emitted, whenever the population of a
                                             channel changes. */

    protected:

    private:
        EmulatedFrequencyRegistry();

        // User console commands
        void consoleList(std::ostream & os) const;

        // IPC console commands
        void add(unsigned nodeId, unsigned frequency, unsigned bandwidth,
                 UDPClientHandle::Address address);
        void del(unsigned nodeId, unsigned frequency, unsigned bandwidth);
        void join(unsigned nodeId, unsigned frequency, unsigned bandwidth);
        void leave(unsigned nodeId, unsigned frequency, unsigned bandwidth);
        int poll(unsigned nodeId, unsigned frequency, unsigned bandwidth);

        // Sending console commands to other nodes
        void sendAdd(unsigned nodeId, unsigned frequency, unsigned bandwidth,
                     UDPClientHandle::Address address);
        void sendDel(unsigned nodeId, unsigned frequency, unsigned bandwidth);
        void sendJoin(unsigned nodeId, unsigned frequency, unsigned bandwidth);
        void sendLeave(unsigned nodeId, unsigned frequency, unsigned bandwidth);
        void sendPoll(unsigned nodeId, unsigned frequency, unsigned bandwidth);

        UDPClientHandle::Address getAddress();
        void updateAddress(Entry const & entry);
        void checkCollisionOk(unsigned frequency, unsigned bandwidth);

        console::ScopedDirectory<> dir_;
        UDPClientHandle socket_;
        UDPClientHandle::Address group_;
        boost::scoped_ptr<console::UDPServer> server_;
        scheduler::IdleEvent schedulerStart_;
        INet4Network channelGroupRange_;
        unsigned portbase_;
        unsigned lastAddress_;
        Entries entries_;
        Users users_;
        bool running_;
        CollisionCallback cb_;

        friend class senf::singleton<EmulatedFrequencyRegistry>;
    };

    EmulatedFrequencyRegistry & emulatedFrequencyRegistry();

    //-////////////////////////////////////////////////////////////////////////

    class EmulatedWirelessReceiver;
    class EmulatedWirelessTransmitter;

    /** \brief Emulated wireless interface base-class

        This base class provides functionality to implement emulated wireless interfaces. It extends
        EmulatedInterface with public members to register frequencies and parameters.

        In addition to this class, you need to use the EmulatedWirelessReceiver and/or
        EmulatedWirelessTransmitter base-classes.

        \ingroup senf_emu_emulation
     */
    class EmulatedWirelessInterface
        : public EmulatedInterface,
          public detail::InterfaceAccess<WirelessInterface>,
          public detail::CommfaceAccess<EmulatedWirelessReceiver, EmulatedWirelessTransmitter>
    {
    public:
        using detail::InterfaceAccess<WirelessInterface>::interface;
        using detail::CommfaceAccess<EmulatedWirelessReceiver, EmulatedWirelessTransmitter>::receiver;
        using detail::CommfaceAccess<EmulatedWirelessReceiver, EmulatedWirelessTransmitter>::transmitter;

        typedef std::vector<console::ValueRange<unsigned> > Ranges;
        typedef boost::function<double (unsigned nodeId)> DistanceCallback;

        ~EmulatedWirelessInterface();

        void registerFrequency(unsigned frequency, unsigned bandwidth);
                                        ///< Register single frequency/bandwidth combination
        void registerFrequency(unsigned frequencyLower, unsigned frequencyUpper,
                               unsigned bandwidthLower,unsigned bandwidthUpper);
                                        ///< Register single frequency/bandwidth range
        void registerFrequency(Ranges const & frequencies, Ranges const & bandwidths);
                                        ///< register frequency/bandwidth ranges
                                        /**< Registers all possible combinations of each frequency
                                             or frequency range from \a frequencies with each
                                             bandwidth or bandwidth range from \a bandwidths. This
                                             command is normally called from the console. */

        void distanceCallback(DistanceCallback cb); //register the distance cb

    protected:
        EmulatedWirelessInterface();
        void init();

        unsigned emulatedFrequency() const; ///< Get current emulated frequency
        unsigned emulatedBandwidth() const; ///< Get current emulated bandwidth
        void emulatedFrequency(unsigned frequency, unsigned bandwidth);
                                        ///< Set channel frequency/bandwidth
                                        /**< Must be called by interface implementations
                                             \c v_frequency() member */
        void emulatedCoverageRange(unsigned distance);

    private:
        unsigned frequency_;
        unsigned bandwidth_;
        unsigned index_;

        ///< Callback type called to get the distance to a specific node
        DistanceCallback distanceCallback_;
        friend class EmulatedWirelessReceiver;
    };

    /** \brief Emulated wireless receiver base-class
        \see EmulatedWirelessInterface
        \ingroup senf_emu_emulation
     */
    class EmulatedWirelessReceiver
        : public EmulatedReceiver,
          public detail::InterfaceAccess<WirelessInterface>,
          public detail::InterfaceAccess<EmulatedWirelessInterface>
    {
    public:
        typedef detail::InterfaceAccess<EmulatedWirelessInterface> emui;
        typedef detail::InterfaceAccess<WirelessInterface> basei;

    protected:
        ppi::connector::ActiveOutputJack<> receiverJack;

        EmulatedWirelessReceiver();
        void init();

        virtual bool v_emulatedWirelessQuality(Packet packet, annotations::Quality & quality);
                                        ///< Overload to define wireless characteristics
                                        /**< This member is called for every received packet to
                                             calculate the wireless characteristics: The emulated
                                             reception quality and whether the packet should be
                                             dropped.

                                             \param[in] packet the packet received
                                             \param[out] quality output quality
                                             \returns \c true, if packet is ok, \c false if it
                                                 should be dropped. */

    private:
        detail::EmulatedWirelessInterfaceReceiveFilter filter_;

        unsigned rssiRandomnessRange_;
        unsigned noiseRandomnessRange_;

        friend class detail::EmulatedWirelessInterfaceReceiveFilter;
    };

    /** \brief Emulated wireless transmitter base-class
        \see EmulatedWirelessInterface
        \ingroup senf_emu_emulation
     */
    class EmulatedWirelessTransmitter
        : public EmulatedTransmitter,
          public detail::InterfaceAccess<WirelessInterface>,
          public detail::InterfaceAccess<EmulatedWirelessInterface>
    {
    public:
        typedef detail::InterfaceAccess<EmulatedWirelessInterface> emui;
        typedef detail::InterfaceAccess<WirelessInterface> basei;

        typedef std::vector< console::ValueRange<int> > Ranges;

        void registerTxPower(int power); ///< Register single txpower value
        void registerTxPower(int powerLow, int powerHigh); ///< Register range of txpower values
        void registerTxPower(Ranges const & powers); ///< txpower ranges
                                        /**< Registers a list of txpower values or ranges */

    protected:
        ppi::connector::PassiveInputJack<> transmitterJack;

        EmulatedWirelessTransmitter();
        void init();

    private:
        detail::EmulatedWirelessInterfaceTransmitFilter filter_;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "EmulatedWirelessInterface.cci"
//#include "EmulatedWirelessInterface.ct"
//#include "EmulatedWirelessInterface.cti"
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
