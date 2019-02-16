//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Christian Niephaus <cni@berlios.de>
//

#ifndef HH_SENF_Utils_FlowStatistics_
#define HH_SENF_Utils_FlowStatistics_ 1

// Custom includes
#include <senf/Scheduler/ClockService.hh>
#include <senf/Utils/Statistics.hh>

///////////////////////////////hh.p////////////////////////////////////////
namespace senf {

    namespace detail {
        class Distance
        {
        public:
            Distance(std::int32_t maxValue, std::int32_t threshold) :
                maxValue_(maxValue), threshold_(threshold)
            {
            }

            std::int32_t distance(std::uint32_t current, std::uint32_t last)
            {
                std::int32_t dist (current - last);

                if (SENF_UNLIKELY(dist + threshold_ < 0))
                    return dist + maxValue_;
                if (SENF_UNLIKELY(dist - (maxValue_ - threshold_) > 0))
                    return dist - maxValue_;

                return dist;
            }

        private:
            std::int32_t const maxValue_;
            std::int32_t const threshold_;
        };
    }

    class SequenceNumberStatistics
        : public detail::Distance
    {
    public:
        SequenceNumberStatistics(std::int32_t maxValue, std::int32_t threshold, std::int32_t maxLate);
        virtual ~SequenceNumberStatistics() {};

        virtual void clear();
        virtual void reset();

        bool update(std::uint32_t seqNo, std::uint32_t payloadSize);

        void dump(std::ostream & os, ClockService::clock_type const & period = ClockService::clock_type(0)) const;

        std::int32_t maxLate() const;

    protected:
        void countGood(std::uint32_t payloadSize);

    public:
        std::uint32_t good;
        std::uint32_t goodBytes;
        std::uint32_t duplicate;
        std::uint32_t late;
        std::uint32_t lost;
        std::uint32_t resyncs;
        std::uint64_t goodTotal;
        std::uint64_t goodBytesTotal;

    private:
        std::uint32_t last_;
        std::int32_t const maxLate_;
        bool firstUpdateAfterReset_;
    };

    struct TimestampStatistics
        : public detail::Distance
    {
    public:
        TimestampStatistics(std::int32_t maxValue, std::int32_t threshold);
        virtual ~TimestampStatistics() {};

        virtual void clear();
        virtual void reset();

        void update(std::uint32_t txTStamp, std::uint32_t rxTStamp, bool updateDelay);

    public:
        StatisticAccumulator<std::int32_t> delay;
        StatisticAccumulator<std::uint32_t> pdv;
        // no unsigned here!
        std::int32_t lastPD_;

    private:
        bool firstUpdateAfterReset_;
    };
}

///////////////////////////////hh.e////////////////////////////////////////
#include "FlowStatistics.cci"
//#include "FlowStatistics.ct"
//#include "FlowStatistics.cti"
#endif
