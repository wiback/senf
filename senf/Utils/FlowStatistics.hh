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


#ifndef HH_SENF_Utils_FlowStatistics_
#define HH_SENF_Utils_FlowStatistics_ 1

// Custom includes
#include <senf/Scheduler/ClockService.hh>
#include <senf/Utils/Statistics.hh>

///////////////////////////////hh.p////////////////////////////////////////
namespace senf {

    namespace Detail {
        class DifferenceSigned
        {
        public:
            DifferenceSigned(std::uint8_t bits) :
                mask_(std::uint64_t(1 << bits) - 1),
                limit_(1 << (bits-1))
            {
            }

            std::int32_t difference(std::uint32_t current, std::uint32_t last)
            {
                return std::int32_t(((current - last) & mask_) ^ limit_) - limit_;
            }
  
        private:
            std::uint32_t const mask_;
            std::uint32_t const limit_;
        };
    }

    class SequenceNumberStatistics
        : public Detail::DifferenceSigned
    {
    public:
        SequenceNumberStatistics(std::uint8_t bits, std::int32_t maxLate);
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
        std::uint32_t lateMax;
        std::uint32_t lost;
        std::uint32_t lostLate;
        std::uint32_t resyncs;
        std::uint64_t goodTotal;
        std::uint64_t goodBytesTotal;

    private:
        std::int32_t const maxLate_;
        std::uint32_t last_;
        bool firstUpdateAfterReset_;
    };

    struct TimestampStatistics
        : public Detail::DifferenceSigned
    {
    public:
        TimestampStatistics(std::uint8_t bits);
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
