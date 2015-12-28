// $Id:$
//
// Copyright (C) 2010
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
//       Mathias Kretschmer <mathias.kretschmer@fokus.fraunhofer.de>

#ifndef HH_SENF_Ext_NetEmu_WLAN_AthSpectralScanner_
#define HH_SENF_Ext_NetEmu_WLAN_AthSpectralScanner_ 1

// Custom includes
#include <senf/Socket/FileHandle.hh>
#include <senf/Scheduler/FdEvent.hh>
#include <senf/Utils/Statistics.hh>


///////////////////////////////hh.p////////////////////////////////////////

/* taken from ath9k.h */
#define SPECTRAL_HT20_NUM_BINS          56

enum ath_fft_sample_type {
    ATH_FFT_SAMPLE_HT20_OLD = 0,
    ATH_FFT_SAMPLE_HT20 = 1,
};

struct fft_sample_tlv {
    std::uint8_t  type;        /* see ath_fft_sample */
    std::uint16_t length;
        /* type dependent data follows */
} __attribute__((packed));

struct fft_sample_ht20 {
    struct fft_sample_tlv tlv;

    std::uint8_t  max_exp;

    std::uint16_t freq;
    std::int8_t   rssi;
    std::int8_t   noise;

    std::uint16_t max_magnitude;
    std::uint8_t  max_index;
    std::uint8_t  bitmap_weight;

    std::uint64_t tsf;

    std::uint8_t data[SPECTRAL_HT20_NUM_BINS];
} __attribute__((packed));

namespace senf {
namespace emu {

    class StatisticsAccu {
    public:
        float sum_;
        float sum_squared_;
        unsigned cnt_;
        float min_;
        float max_;
        float thresh_;
        bool maxSet_;
        bool minSet_;

        StatisticsAccu() :
            thresh_(0.75f)
        { clear(); }

        void accumulate(float val) {
            sum_  += val;
            sum_squared_ += val * val;
            cnt_++;

            if (val <= (thresh_ * min_)) {
                if (!minSet_) {
                    min_ = val / 2.0f;
                    minSet_ = true;
                }
                else
                    min_ = (min_ + val) / 2.0f;
            }

            if (val >= (thresh_ * max_)) {
                if (!maxSet_) {
                    max_ = val * 2.0f;
                    maxSet_ = true;
                }
                else
                    max_ = (max_ + val) / 2.0f;
            }
        }

        unsigned count()
            const
        {
            return cnt_;
        }

        float avg()
            const
        {
            return cnt_ == 0 ? NAN : float(double(sum_) / double(cnt_));
        }

        float stddev()
            const
        {
            if (cnt_ == 0)
                return NAN;
            if (cnt_ == 1)
                return 0.0f;
            double _avg (avg());
            // std::max avoid rounding errors, that might cause a sqrt(<0)
            return float(sqrt( std::max(0.0, (double(sum_squared_) / double( cnt_) - (_avg * _avg) ))));
        }

        void clear() {
            sum_ = sum_squared_ = 0.0f;
            min_ = std::numeric_limits<float>::max();
            max_ = std::numeric_limits<float>::min();  
            cnt_ = 0;
            maxSet_ = false;
            minSet_ = false;
        }

        void data(senf::StatisticsData & data_) const {
            if (cnt_ == 0) {
                data_.min = data_.avg = data_.max = 0.0f;
                data_.stddev = NAN;
                data_.cnt = 0;
            } else {
                data_.min = (float) min_;
                data_.avg = avg();
                data_.max = (float) max_;
                data_.stddev = stddev();
                data_.cnt = cnt_;
            }
        }

        senf::StatisticsData data() const
        {
            StatisticsData tmp;
            data(tmp);
            return tmp;
        }
    };

    class UnixFileHandle
        : public senf::FileHandle
    {
    public:
        UnixFileHandle() : senf::FileHandle(std::auto_ptr<senf::FileBody>(new senf::FileBody)) {
        }

        void SetFd(int fd_) {
            fd(fd_);
        };
    };

    class AthSpectralScanner
    {
    private:
        unsigned frequency_;
        unsigned mismatch_;
        unsigned truncated_;
        std::string ctlFile_;
        std::string dataFile_;
        StatisticsAccu signalLevel_;
        UnixFileHandle handle_;
        senf::scheduler::FdEvent event_;

    public:
        AthSpectralScanner(std::string phyName, std::string debugFS = "/sys/kernel/debug");
        ~AthSpectralScanner();

        bool start(unsigned frequency);
        bool stop(senf::StatisticsData * sd = NULL);
        bool valid() const;
        unsigned mismatch() const;
        unsigned truncated() const;
        std::string ctlFile() const;
      
    private:
        bool enable();
        void disable();
        bool process(int fd);
        float computeSignalLevel(fft_sample_ht20 const & sample);
    };
}
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "AthSpectralScan.cci"
//#include "AthSpectralScan.ct"
//#include "AthSpectralScan.cti"
#endif
