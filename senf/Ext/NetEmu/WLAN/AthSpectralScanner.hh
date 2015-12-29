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
#include <map>
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

    class SampleAccumulator {
    private:
        std::map<std::int16_t,unsigned> samples_;  // powerLevel => count
    public:
        void accumulate(float val) {
            auto it (samples_.find(std::int16_t(val)));
            if ( it != samples_.end())
                it->second++;
            else
                samples_.insert(std::make_pair(std::int16_t(val),1));
        }
        
        void clear() {
            samples_.clear();
        }

        void data(senf::StatisticsData & data_, unsigned thresh1000=1) const {
            if (samples_.size() == 0) {
                data_.min = data_.avg = data_.max = 0.0f;
                data_.stddev = NAN;
                data_.cnt = 0;
            } else {
                data_.cnt = 0;
                // AVG + STDDEV
                float sum (0.0f);
                float sum2 (0.0f);
                for (auto it = samples_.begin(); it != samples_.end(); it++) {
                    sum  += float(it->first) * float(it->second);
                    for (unsigned n (0); n < it->second; n++)
                        sum2 += float(it->first) * float(it->first);
                    data_.cnt += it->second;
                }
                data_.avg = sum / float(data_.cnt);
                double _avg (data_.avg);
                data_.stddev = float(sqrt( std::max(0.0, (double(sum2) / double(data_.cnt) - (_avg * _avg) ))));
                // MIN
                unsigned cnt (0);
                sum = 0.0f;
                for (auto it = samples_.begin(); it != samples_.end(); it++) {
                    sum  += float(it->first) * float(it->second);
                    cnt += it->second;
                    if (cnt > ((data_.cnt * thresh1000 * 10) / 1000u))  // 10x 
                        break;
                }
                data_.min = sum / float(cnt);
                // MAX
                cnt = 0;
                sum = 0.0f;
                for (auto it = samples_.rbegin(); it != samples_.rend(); it++) {
                    sum  += float(it->first) * float(it->second);
                    cnt += it->second;
                    if (cnt > ((data_.cnt * thresh1000) / 1000u))  // 1x
                        break;
                }
                data_.max = sum / float(cnt);
            }
        }

        senf::StatisticsData data(unsigned thresh1000=1) const
        {
            StatisticsData tmp;
            data(tmp, thresh1000);
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
        SampleAccumulator signalLevel_;
        UnixFileHandle handle_;
        senf::scheduler::FdEvent event_;

    public:
        AthSpectralScanner(std::string phyName, std::string debugFS = "/sys/kernel/debug");
        ~AthSpectralScanner();

        bool start(unsigned frequency);
        bool stop(senf::StatisticsData * sd = NULL, unsigned thresh1000=1);
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
