// $Id:$
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at 
// http://senf.fit.fraunhofer.de/license.html
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
    \brief WirelessInterfaceAPI public header */

#ifndef HH_SENF_Ext_NetEmu_WirelessInterfaceAPI_
#define HH_SENF_Ext_NetEmu_WirelessInterfaceAPI_ 1

// Custom includes
#include <boost/ptr_container/ptr_unordered_map.hpp>
#include "InterfaceAPI.hh"

//-/////////////////////////////////////////////////////////////////////////////////////////////////

namespace senf {
namespace emu {

    class WirelessReceiver;
    class WirelessTransmitter;
    class EmulatedWirelessInterface;
    class EmulatedWirelessReceiver;
    class EmulatedWirelessTransmitter;
    class ModulationParameterRegistry;


    /** \brief Wireless modulation parameter

        Every wireless modulation parameter describes a unique combination of internal wireless
        parameters like coding, rate or FEC. The exact interpretation depends on the interface
        type.
        \see \ref senf::emu::WirelessInterface\n
             \ref senf::emu::ModulationParameterRegistry
     */
    struct ModulationParameter
        : public boost::equality_comparable<ModulationParameter>,
          public boost::less_than_comparable<ModulationParameter>
    {
        typedef boost::uint32_t id_t;
        id_t id;  ///< unique identifier
        std::string coding;
        short minRssi;       ///< Minimum RSSI value
        unsigned rate;       ///< Bitrate in kbit/s
        unsigned bandwidth;  ///< Bandwidth in KHz
        boost::uint8_t linkTypeId;
        std::string info;    ///< Arbitrary additional information

        ModulationParameter(std::string const & _coding, short _minRssi, unsigned _rate, unsigned _bandwidth, boost::uint8_t _linkTypeId, std::string const & _info = "");
        virtual ~ModulationParameter() {}

        void dump(std::ostream & os) const;
        void dumpTableHeader(std::ostream & os) const;
        void dumpTableRow(std::ostream & os, bool is_current_modulation) const;

        struct Error : public senf::Exception {
            Error();
        };

        bool operator<(ModulationParameter const & other) const {
            return id < other.id;
        };

        bool operator==(ModulationParameter const & other) const {
            return id == other.id;
        };
        
    private:
        friend class ModulationParameterRegistry;
        virtual boost::uint16_t v_modulationId() const = 0;
        virtual ModulationParameter * v_clone() const = 0;
        virtual void v_dump(std::ostream & os) const = 0;
        virtual void v_dumpTableHeader(std::ostream & os) const = 0;
        virtual void v_dumpTableRow(std::ostream & os) const = 0;
    };

    class ModulationParameterRegistry
        : private boost::noncopyable
    {
    public:
        static ModulationParameterRegistry & instance() {
            static ModulationParameterRegistry _instance;
            return _instance;
        }

        unsigned registerModulation(ModulationParameter & parameter);
        ModulationParameter const & findModulationById(ModulationParameter::id_t id) const;

    private:
        typedef boost::ptr_unordered_map<ModulationParameter::id_t, ModulationParameter> Modulations;
        Modulations modulations_;
    };


    /** \brief Interface API base class specialization for wireless interfaces

        This interface base class is a specialization of senf::emu::Interface with extensions
        implementing a generic wireless interface API.

        A wireless interface configuration is characterized by two or three values:
        \li The <em>modulation parameter</em> identifies the coding and bandwidth setup.
        \li The \a frequency and \a bandwidth
        \li For transmit enabled interfaces, the \a txpower

        Every <em>parameter index</em> identifies a unique receiver/transmitter configuration. The
        exact configuration parameters (e.g. coding) are not public but implementation defined. All
        important external properties are accessible via the parameters() member.

        Every wireless interface manages a list of valid frequency/bandwidth ranges. Transmit
        capable interface additionally manage a list of valid txpower ranges.

        \see \ref senf_emu_interfaces
        \ingroup senf_emu_interfaces
     */
    class WirelessInterface
        : public Interface,
          public detail::CommfaceAccess<WirelessReceiver, WirelessTransmitter>
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        typedef TypedInterfaceDecorator<WirelessInterface, Interface::Decorator> Decorator;
                                        ///< WirelessInterface base class specific decorator type

        /** \brief Frequency table entry

            Every entry consists of frequency and bandwidth. Both values maybe either single values
            or ranges. For single values, the \a upper and \a lower values will be identical. If a
            frequency range is specified, this range \e includes the bandwidth, thus the valid
            frequencies range from <tt>frequencyLower + bandwidth/2</tt> to <tt>frequencyUpper -
            bandwidth/2</tt> where \a bandwidth is the desired bandwidth value.
         */
        struct Frequency {
            unsigned frequencyLower;     ///< Frequency interval lower bound
            unsigned frequencyUpper;     ///< Frequency interval upper bound
            unsigned bandwidthLower;     ///< Bandwidth interval lower bound
            unsigned bandwidthUpper;     ///< Bandwidth interval upper bound

            Frequency(unsigned frequencyLower_, unsigned frequencyUpper_, unsigned bandwidthLower_, unsigned bandwidthUpper_);
                                        ///< Table entry with ranges for frequency and bandwidth
            Frequency(unsigned frequency_, unsigned bandwidth_);
                                        ///< Table entry with single frequency/bandwidth
        };

    private:
        struct ByLower {};
        struct ByUpper {};

        // This greatly shortens the type name and thus error messages ...
        struct FrequenciesIndexes
            : public boost::multi_index::indexed_by<
                  boost::multi_index::ordered_non_unique<
                      boost::multi_index::tag<ByLower>,
                      boost::multi_index::composite_key<
                          Frequency,
                          boost::multi_index::member<Frequency, unsigned, &Frequency::bandwidthLower>,
                          boost::multi_index::member<Frequency, unsigned, &Frequency::frequencyLower> > >,
                  boost::multi_index::ordered_non_unique<
                      boost::multi_index::tag<ByUpper>,
                      boost::multi_index::composite_key<
                          Frequency,
                          boost::multi_index::member<Frequency, unsigned, &Frequency::bandwidthUpper>,
                          boost::multi_index::member<Frequency, unsigned, &Frequency::frequencyUpper> > > >
        {};

        typedef boost::multi_index_container<Frequency, FrequenciesIndexes> Frequencies;

        typedef std::pair<unsigned, unsigned> FrequencyInterval; // Frequency interval lower & upper bound
        typedef std::set<FrequencyInterval> FrequencyIntervals;

        struct frequency_is_whitelisted
        {
            FrequencyIntervals const & whitelistedFrequencies_;
            frequency_is_whitelisted(FrequencyIntervals const & whitelistedFrequencies);
            bool operator()(unsigned frequencyLower, unsigned frequencyUpper);
            bool operator()(Frequency const & frequency);
        };

        typedef std::set<ModulationParameter::id_t> ModulationParameterIds;

    public:
        typedef boost::filter_iterator<frequency_is_whitelisted, Frequencies::const_iterator> FrequencyIterator;
        typedef boost::iterator_range<FrequencyIterator> FrequenciesRange;
                                        ///< Range of frequency entries

        //-////////////////////////////////////////////////////////////////////////
        // Methods

        using detail::CommfaceAccess<WirelessReceiver,WirelessTransmitter>::receiver;
        using detail::CommfaceAccess<WirelessReceiver,WirelessTransmitter>::transmitter;

        ///\name Wireless Modulation
        //\{
        ModulationParameterIds const & modulationParameterIds() const; ///< Get list of valid parameters
        unsigned maxNSS() const; ///< Get max numer of spatial stream;
        //\}

        ///\name Frequency/bandwidth
        //\{
        unsigned frequency() const;     ///< Get current channel frequency
        unsigned bandwidth() const;     ///< Get current bandwidth
        void frequency(unsigned freq);  ///< Set channel frequency
        void frequency(unsigned freq, unsigned bw); ///< Set channel frequency and bandwidth

        FrequenciesRange frequencies() const; ///< Get valid frequency intervals

        void whitelistedFrequencyRange(unsigned frequencyLower, unsigned frequencyUpper);
        void whitelistedChannel(unsigned frequency, unsigned bandwidth);
        void clearWhitelistedFrequencies();

        void coverageRange(unsigned distance); ///< set the coverage range of the interface in meters
        //\}

    protected:
        WirelessInterface();

        void init();

        void clearRegisteredFrequencies(); ///< remove all registered frequencies
        void clearRegisteredModulations(); ///< remove all registered modulations

        void registerFrequency(unsigned frequency, unsigned bandwidth);
                                        ///< register single frequency/bandwidth combination
        void registerFrequency(unsigned frequencyLower, unsigned frequencyUpper,
                               unsigned bandwidthLower, unsigned bandwidthUpper);
                                        ///< register frequency/bandwidth range
        void registerModulation(ModulationParameter::id_t id);
                                        ///< register modulation configuration

#ifndef DOXYGEN
    private:
#else
    protected:
#endif
        virtual void v_coverageRange(unsigned distance) = 0;
        // channel frequency
        virtual unsigned v_frequency() const = 0; ///< Called to get current frequency
        virtual unsigned v_bandwidth() const = 0; ///< Called to get current bandwidth
        virtual void v_frequency(unsigned freq, unsigned bw) = 0;
                                        ///< Called to set frequency and bandwidth

    private:
        void listModulations(std::ostream & os) const;
        void listFrequencies(std::ostream & os) const;

        void lookupFrequency(unsigned freq, unsigned bw) const;

        ModulationParameterIds modulationIds_;
        Frequencies frequencies_;
        FrequencyIntervals whitelistedFrequencies_;

        friend class WirelessTransmitter;
        friend class EmulatedWirelessInterface;
    };


    /** \brief Receive capable wireless interface base class
        \see \ref WirelessInterface \n \ref senf_emu_interfaces
        \ingroup senf_emu_interfaces
     */
    class WirelessReceiver
        : public Receiver,
          public detail::InterfaceAccess<WirelessInterface>
    {
    protected:
        WirelessReceiver(ppi::connector::ActiveOutput<> & output);
        WirelessReceiver(ppi::connector::ActiveOutput<EthernetPacket> & output);
        WirelessReceiver(ppi::connector::ActiveOutputJack<> & output);
        WirelessReceiver(ppi::connector::ActiveOutputJack<EthernetPacket> & output);

        void init();

    private:
        using detail::InterfaceAccess<WirelessInterface>::interface;
    };


    /** \brief Transmit capable wireless interface base class

        This base class adds \e txPower() and \a parameter() members which allow changing those
        parameters to the API.

        \see \ref WirelessInterface \n \ref senf_emu_interfaces
        \ingroup senf_emu_interfaces
     */
    class WirelessTransmitter
        : public Transmitter,
          public detail::InterfaceAccess<WirelessInterface>
    {
    public:
        //-////////////////////////////////////////////////////////////////////////
        // Types

        /** \brief Transmit power table entry */
        struct TxPower {
            const int lower;  ///< Transmit power interval lower bound
            const int upper;  ///< Transmit power interval upper bound

            TxPower(int _lower, int _upper);
        };

    private:
        struct TxPowersIndices
            : public boost::multi_index::indexed_by<
                  boost::multi_index::ordered_unique<
                      boost::multi_index::member<TxPower, const int, &TxPower::upper> > >
        {};

        typedef boost::multi_index_container<TxPower, TxPowersIndices> TxPowers;

    public:
        typedef boost::iterator_range<TxPowers::const_iterator> TxPowersRange;

        //-////////////////////////////////////////////////////////////////////////
        // Methods

        ///\name Wireless Modulation
        //\{
        ModulationParameter::id_t modulation() const;
                                        ///< Get current modulation parameter configuration id
        void modulation(ModulationParameter::id_t id); ///< Set modulation parameter configuration by id
        //\}

        ///\name Transmit power
        //\{
        int txPower() const;            ///< Get current transmit power
        void txPower(int power);        ///< Set transmit power
        TxPowersRange txPowers() const; ///< Get valid transmit power ranges
        //\}

    protected:
        WirelessTransmitter(ppi::connector::PassiveInput<> & input);
        WirelessTransmitter(ppi::connector::PassiveInput<EthernetPacket> & input);
        WirelessTransmitter(ppi::connector::PassiveInputJack<> & input);
        WirelessTransmitter(ppi::connector::PassiveInputJack<EthernetPacket> & input);

        void init();

        void registerTxPower(int power);
        void registerTxPower(int powerLower, int powerUpper);

#ifndef DOXYGEN
    private:
#else
    protected:
#endif
        virtual ModulationParameter::id_t v_modulationId() const = 0;
                                        ///< Called to get current modulation parameter configuration
        virtual void v_modulationId(ModulationParameter::id_t id) = 0;
                                        ///< Called to set modulation parameter configuration
        virtual int v_txPower() const = 0;
                                        ///< Called to get current transmit power
        virtual void v_txPower(int power) = 0;
                                        ///< Called to set transmit power

    private:
        using detail::InterfaceAccess<WirelessInterface>::interface;

        void listTxpowers(std::ostream & os) const;

        TxPowers txPowers_;

        friend class EmulatedWirelessTransmitter;
    };

}}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#include "WirelessInterfaceAPI.cci"
//#include "WirelessInterfaceAPI.ct"
//#include "WirelessInterfaceAPI.cti"
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
