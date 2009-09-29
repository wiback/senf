// $Id$
//
// Copyright (C) 2008 
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
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
    \brief Statistics public header */

#ifndef HH_SENF_Utils_Statistics_
#define HH_SENF_Utils_Statistics_ 1

// Custom includes
#include <map>
#include <vector>
#include <deque>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/utility.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/signals.hpp>
#include "Exception.hh"
#include <senf/Utils/Logger/Logger.hh>
#include <senf/Utils/Console/Console.hh>

//#include "Statistics.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {

    /** \defgroup senf_statistics Statistics

        The statistics functionality has two parts:
        
        \li the senf::Statistics class
        \li statistics sources

        Each senf::Statistics instance collects information about a single parameter. Which
        parameter is set up by connecting the Statistics instance with an arbitrary statistics
        source.

        %Statistics sources are <a href="http://www.boost.org/doc/libs/1_37_0/doc/html/signals.html">
        Boost Signals</a> which are emitted periodically to provide new data.
     */

    class Collector;
    class Statistics;
    
    /** \brief Internal: Generic Statistics collection */
    class StatisticsBase
    { 
        typedef std::map<unsigned, Collector> Children;

        struct Transform {
            typedef Children::value_type & first_argument_type;
            typedef Collector & result_type;
            result_type operator()(first_argument_type i) const;
        };

        typedef boost::transform_iterator<Transform,Children::iterator> ValueIterator;

        struct OutputEntry;

    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::iterator_range<ValueIterator> CollectorRange;

        /** \brief Output connection interface

            This class is returned from senf::StatisticsBase::output() and the derived
            <tt>output()</tt> implementations to allow connecting an output with an arbitrary
            target.

            There are two types of targets:
            \li <em>Externally managed targets</em>. These targets live outside of the statistics
                module, just a reference to those targets is saved. The target should derive from
                <tt>boost::signals::trackable</tt> to ensure they are automatically disconnected
                when destroyed.
            \li <em>Internally managed targets</em>. Those targets are owned by the statistics
                module and will be destroyed when the statistics class is destroyed.

            Externally managed targets are passed by non-const reference to connect(), internally
            managed targets are passed using <tt>std::auto_ptr</tt>.

            A target is any callable object which takes three float values as argument: The current
            minimum, average and maximum value.

            \code
            // Simple function as statistics target
            void collect(float min, float avg, float max)
                { ... }

            // Function object
            struct Collector
            {
                void operator()(float min, float avg, float max, float dev) 
                    { ... }
            };
            \endcode

           \ingroup senf_statistics
         */
        template <class Owner>
        class OutputProxy
        {
        public:
            template <class Target> Owner & connect(Target & target,
                                                    std::string label="") const;
                                        ///< Connect externally managed target
            template <class PTarget> Owner & connect(std::auto_ptr<PTarget> target, 
                                                     std::string label="") const;
                                        ///< Connect internally managed target
            Owner & noconnect() const;  ///< Don't connect the output
            senf::console::ScopedDirectory<> & dir() const;
                                        ///< Get target's console directory

#ifdef DOXYGEN
        private:
#endif
            OutputProxy(Owner * owner, OutputEntry * entry);
            template <class OtherOwner> 
            OutputProxy(Owner * owner, OutputProxy<OtherOwner> const & other);

        private:
            Owner * owner_;
            OutputEntry * entry_;

            template <class OtherOwner> friend class OutputProxy;
        };
            
        ///////////////////////////////////////////////////////////////////////////
        ///\name Accessing the current value
        ///\{

        float min() const;              ///< Last min value entered
        float avg() const;              ///< Last avg value entered
        float max() const;              ///< Last max value entered
        float dev() const;              ///< Last dev value entered

        virtual unsigned rank() const;  ///< Return collectors rank value
                                        /**< \returns number of basic values collected into each new
                                             value by this collector. */

        ///\}
        ///////////////////////////////////////////////////////////////////////////
        ///\name Child collectors
        ///\{

        Collector & operator[](unsigned rank);
                                        ///< Get child collector
                                        /**< This member will return a reference to the collector
                                             collecting \a rank values.
                                             \param[in] rank Number of values the requested
                                                 collector collects into each combined value. 
                                             \throws InvalidRankException if \a rank is not a valid
                                                 registered rank value. */
        CollectorRange collectors();    ///< List all child collectors
                                        /**< \returns iterator range of child collector
                                             references */

        Collector & collect(unsigned rank); ///< Register a new collector
                                        /**< Adds a collector collecting \a rank values into each
                                             combined value.
                                             \param[in] rank number of values to collect
                                             \returns Reference to new collector
                                             \throws DuplicateRankException if a collector
                                                 collecting \a rank values already exists. */

        Statistics & base();            ///< Get base statistics object
                                        /**< Returns the base statistics object. If this is
                                             a child collector, this will return the outermost
                                             statistics object, otherwise it will return 
                                             \c *this. */

        std::string path() const;       ///< Get the path to this collector
                                        /**< Returns the '-'-separated list of collectors up to
                                             here. If this is the basic statistics object, the value
                                             is empty, otherwise it is built by joining the
                                             collector ranks. */

        ///\}
        ///////////////////////////////////////////////////////////////////////////
        ///\name Result generation

        OutputProxy<StatisticsBase> output(unsigned n = 1u);
                                        ///< Register output
                                        /**< This call will request the collector to output
                                             statistics build by averaging the last \a n
                                             values. This output is generated for every new value in
                                             the collector. The output signal can be connected to an
                                             arbitrary target using the returned proxy. Example:
                                             \code
                                             stats.output(4u).connect(
                                                 senf::StatisticsLogger());
                                             \endcode
                                             \param[in] n size of sliding average window */

        ///\}
        ///////////////////////////////////////////////////////////////////////////
        // Exceptions

        struct InvalidRankException : public senf::Exception
        { InvalidRankException() : senf::Exception("Invalid rank value") {} };

        struct DuplicateRankException : public senf::Exception
        { DuplicateRankException() : senf::Exception("Duplicate rank value") {} };

        void consoleList(unsigned level, std::ostream & os) const;

    protected:
        StatisticsBase();
        virtual ~StatisticsBase();
        void enter(float min, float avg, float max, float dev);

    private:
        virtual Statistics & v_base() = 0;
        virtual std::string v_path() const = 0;

        void generateOutput();
        void signalChildren();

        float min_;
        float avg_;
        float max_;
        float dev_;
        Children children_;
        
        struct QueueEntry {
            float min;
            float avg;
            float max;
            float dev;
            QueueEntry() : min(), avg(), max(), dev() {}
            QueueEntry(float min_, float avg_, float max_, float dev_)
                : min(min_), avg(avg_), max(max_), dev(dev_) {}
        };
        typedef std::deque<QueueEntry> Queue;
        Queue queue_;

        struct OutputEntry {
            struct TargetBase
            {
                explicit TargetBase(std::string const & label_) : label (label_) {}
                virtual ~TargetBase() {};
                std::string label;
            };

            template <class PTarget>
            struct Target : public TargetBase
            {
                boost::scoped_ptr<PTarget> target_;
                Target(std::auto_ptr<PTarget> target, std::string const & label) 
                    : TargetBase (label), target_ (target.release()) {}
                explicit Target(std::string const & label)
                    : TargetBase (label), target_ (0) {}
            };

            OutputEntry();
            explicit OutputEntry(unsigned n_);
            OutputEntry(const OutputEntry& other);
            OutputEntry& operator=(const OutputEntry& other);

            void initDir();
            void consoleList(std::ostream & os);

            unsigned n;
            float min;
            float avg;
            float max;
            float dev;

            boost::signal<void(float,float,float,float)> signal;
            boost::ptr_vector<TargetBase> targets_;

            senf::console::ScopedDirectory<> dir;
        };
        typedef std::map<unsigned, OutputEntry> OutputMap;
        OutputMap outputs_;
        unsigned maxQueueLen_;
    };

    /** \brief  Collect statistics and generate %log messages

        The Statistics class collects information about a single value. The assumption is, that
        Statistics::operator() is called \e periodically to supply new data.

        Each data entry is comprised of a minimum, average and maximum value. These values should
        describe the value of whatever parameter is analyzed by a Statistics instance over the last
        period. The length of the period is defined by the interval, at which data is entered.

        The Statistics class combines successive data values into ever larger groups. These groups
        form a tree where each node combines a fixed number of data values of it's parent node. An
        example:

        Let us assume, that Statistics::operator() is called every 100ms. We can than build a chain
        of collectors:
        \li The basic statistics module provides information with a resolution of 1/10th of a second
        \li A collector collecting 10 values provides information with 1 second resolution
        \li The next collector collects 60 values and provides a resolution of 1 minute
        \li Again the next collector collects 60 values and provides a resolution of 1 hour
        \li ... and so on

        This way, we create a hierarchy of values. Each collector manages a minimum, average and
        maximum value always created over it's the last complete interval.

        It is possible to have more than one collector based on the same basic interval. In above
        scenario, we might want to add another collector which for example collects information
        with a 100 second scale. This is possible and changes the list of collectors into a tree.

        Now to turn all this into code:

        \code
        senf::ppi::module::RateAnalyzer rateAnalyzer;
        senf::Statistics packetStats;

        rateAnalyzer.signals.packetsPerSecond.connect(boost::ref(packetStats));

        packetStats                      // called 10 times / second
            .collect(10u)                // seconds
            .collect(60u)                // minutes
            .collect(60u);               // hours
        
        packetStats[10u].collect(100u);  // 100 seconds

        rateAnalyzer.startStatistics(senf::ClockService::milliseconds(100u));
        \endcode

        This code will collect the statistics as described in the Example above. However, no output
        will be generated.

        For every collector, any number of outputs may be defined. Each output consists of the
        number of values to calculate a sliding average over and an identifying label.

        Lets say, we want to produce the following outputs:
        \li A sliding average of 5 values based on the raw 1/10th second data.
        \li Three different outputs from the seconds staistics: current value without average,
            sliding average over 10 seconds and sliding average over 60 seconds.
        \li Output the minutes and hourly value without averaging.

        To achieve this, we can augment above code in the following way:

        \code
        packetStats
                .output( 5u).connect(senf::StatisicsLogger("pps 100ms 5"))
            .collect(10u)
                .output(   ).noconnect()
                .output(10u).connect(senf::StatisicsLogger("pps 1s 10"))
                .output(60u).connect(senf::StatisicsLogger("pps 1s 60"))
            .collect(60u)
                .output(   ).connect(senf::StatisicsLogger("pps 1min 1"))
            .collect(60u)
                .output(   ).connect(senf::StatisicsLogger("pps 1h 1"));

        packetStats.output(5u).connect(
            senf::StatisticsLogger<senf::log::Debug, senf::log::VERBOSE>("pps"));

        senf::log::FileTarget statslog ("stats.log");

        statslog.showArea(false);
        statslog.showLevel(false);
        statslog.tag("");
        statslog.timeFormat("");

        statslog.route<senf::StatisticsStream>();
        \endcode

        We use a StatisticsLogger to send the %log messages to the senf::StatisticsStream %log
        stream. The stream, area an level to send the statistics %log messages to may be configured
        using template arguments to StatisticsLogger.

        It is also possible to skip sending the output to any target or send one output to several
        targets.

        Here we have opted to use a label which explicitly describes the name of the variable, the
        basic interval and the size of the sliding average window. However, the label is completely
        arbitrary.

        All output is generated using the Senf Logger on the senf::StatisticsStream %log stream. In
        the example above, we have configured a special logfile \c stats.log which contains the
        statistics values each prefixed with a timestamp in nanoseconds (but no further information
        like %log level or tag):
        <pre>
        0000000000.000000000 pps 100ms 5 43.3 43.3 43.3
        0000000000.010311928 pps 100ms 5 42.5 42.5 42.5
        ...
        0000000001.002413391 pps 100ms 5 62.0 62.0 62.0
        0000000001.003920018 pps 1s 1 42.1 53.4 62.0
        ...
        </pre>
        (the nanosecond values will of course be somewhat different ...)

        \see senf::StatisticsBase::OutputProxy for the output proxy (connect) interface
        \ingroup senf_statistics
     */
    class Statistics 
        : public StatisticsBase, boost::noncopyable
    {
    public:
#ifndef SENF_DISABLE_CONSOLE
        console::ScopedDirectory<Statistics> dir;
#endif

        Statistics();

        void operator()(float min, float avg, float max, float dev=0.0f);
                                        ///< Enter new data
                                        /**< This member must be called whenever a new data value is
                                             available. It is important to call this member \e
                                             periodically. The frequency at which this member is
                                             called defines the basic statistics time scale.

                                             If \a min and \a max values are not available, this
                                             member should be called with \a min, \a avg and \a max
                                             set to the same value.

                                             \param[in] min minimal data value since last call
                                             \param[in] avg average data value since last call
                                             \param[in] max maximal data values since last call
                                             \param[in] dev standard deviation of avg value */

        void operator()(float value, float dev=0.0f); 
                                        ///< Same as enter() with \a min == \a avg == \a max
                                        /**< Provided so a Statistics instance can be directly used
                                             as a signal target. */

        StatisticsBase::OutputProxy<Statistics> output(unsigned n = 1u);

        void consoleList(std::ostream & os);
        void consoleCollect(std::vector<unsigned> & ranks);
        boost::shared_ptr<senf::console::DirectoryNode> consoleOutput(
            std::vector<unsigned> & ranks, unsigned window);

    private:
        Statistics & v_base();
        std::string v_path() const;
    };

    /** \brief Accumulated statistics collector

        This class collects accumulated statistics. It is automatically created by
        senf::Statistics::collect()

        \see senf::Statistics
     */
    class Collector : public StatisticsBase
    {
    public:
        virtual unsigned rank() const;  

        StatisticsBase::OutputProxy<Collector> output(unsigned n = 1u);
        
    private:
        Collector(StatisticsBase * owner, unsigned rank);
        void enter(float min, float avg, float max, float dev);
        Statistics & v_base();
        std::string v_path() const;

        unsigned rank_;
        unsigned i_;
        float accMin_;
        float accSum_;
        float accSumSq_;
        float accMax_;
        StatisticsBase * owner_;

        friend class StatisticsBase;
    };

}

///////////////////////////////hh.e////////////////////////////////////////
#include "Statistics.cci"
//#include "Statistics.ct"
#include "Statistics.cti"
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
