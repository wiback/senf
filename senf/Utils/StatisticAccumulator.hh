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
    \brief StatisticAccumulator public header */

#ifndef HH_SENF_Utils_StatisticAccumulator_
#define HH_SENF_Utils_StatisticAccumulator_ 1

// Custom includes
#include <math.h>
#include <iosfwd>
#include <boost/operators.hpp>
#include <boost/property_tree/json_parser.hpp>

///////////////////////////////hh.p////////////////////////////////////////
namespace senf {

     struct StatisticsData
        : public boost::less_than_comparable<StatisticsData>
     {
         StatisticsData();
         StatisticsData(float min_, float avg_, float max_, float stddev_, unsigned cnt_);
         StatisticsData(float singleValue);

         void clear();

         explicit operator bool() const;
         bool operator<(StatisticsData const & other) const;

         boost::property_tree::ptree asPTree() const;

         float min;
         float avg;
         float max;
         float stddev;
         unsigned cnt;
     };

     std::ostream & operator<<(std::ostream & os, StatisticsData const & _data);


     /** \brief Accumulate measurement values

         The accumulator mainly do the preliminary work for the senf::Statistic class.
         It accumulates certain values within an interval to be used by senf::Statistics

         \li the senf::Statistics class
         \li statistics sources

         This class provides the average, standard deviation, min, max values over one
         interval, means until clear() is called. It rather directly calculates the results
         then collection all single values provided by calling accumulate().
         \see senf::Statistics to process accumulated values
         \ingroup senf_statistics
      */
    template <class T>
    class StatisticAccumulator
    {
    public:
        StatisticAccumulator(); 
        StatisticAccumulator(T const & value); 
        StatisticAccumulator(T const & sum, T const & sumSquared, T const & min, T const & max, unsigned count);
        StatisticAccumulator(senf::StatisticsData const & data);

        void clear();                   ///< Reset accumulated values.
                                        /**< This member reset all values. */
        void accumulate(T const & value); ///< Gather value to be accumulated.
                                        /**< This method accumulates a value.
                                             \param[in] value to be accumulated value */

        T     min() const;              ///< Returns current minimal value.
                                        /**< This method returns the minimal value of the
                                             current accumulation. */
        T     max() const;              ///< Returns current maximal value.
                                        /**< This method returns the maximal value of the
                                             current accumulation.*/
        T     sum() const;              ///< Returns current sum value.
                                        /**< This method returns the sum value of the
                                             current accumulation. */
        T     sum2() const;             ///< Returns current squared sum value.
                                        /**< This method returns the squared sum value of the
                                             current accumulation. */
        float avg() const;              ///< Returns arithmetic average value.
                                        /**< This method returns the arithmetic average value of the
                                             current accumulation.*/
        float rms() const;              ///< Returns root mean square value.
                                        /**< This method returns the root mean square value of the
                                             current accumulation.*/
        float stddev() const;           ///< Returns standard deviation value.
                                        /**< This method returns the standard deviation
                                             value of the current accumulation.*/
        unsigned count() const;         ///< Returns count of accumulated values.
                                        /**< This method returns count of accumulated
                                             values of the current accumulation.*/
         void data(StatisticsData & data_) const;
                                        ///< Returns the accumulated data as a tuple
                                        /**< This method returns the accumulated information
                                             as a tuple.*/
        StatisticsData data() const;    ///< Returns the accumulated data as a tuple
                                        /**< This method returns the accumulated information
                                             as a tuple.*/

        explicit operator bool() const; ///< Returns true if valid data is present
                                        /**< This method indicates if valid data is present */

        StatisticAccumulator<T> operator +=(StatisticAccumulator<T> const & other);
        StatisticAccumulator<T> operator*=(const float & scale);
        
    private:
        T sum_;
        T sum_squared_;
        T min_;
        T max_;
        unsigned cnt_;
    };

    template <class T>
    class StatisticsEWMA
        : public boost::less_than_comparable<StatisticsEWMA<T>>
    {
    public:
        StatisticsEWMA(float alpha = 0.25f, T const & value = T());
        
        void clear(T const & value = T()); ///< Reset accumulated values.
                                            /**< This member reset all values. */
        void accumulate(T const & value);   ///< Gather value to be accumulated.
        void accumulateWithLoss(T const & value, unsigned numLost); ///< Gather value to be accumulated.

        bool operator<(StatisticsEWMA<T> const & other) const;
        
        explicit operator bool() const;     ///< Returns true if valid data is present
                                            /**< This method indicates if valid data is present */

        unsigned count() const;             ///< Returns count of accumulated values.
                                            /**< This method returns count of accumulated
                                                 values of the current accumulation.*/
        T const & ewma() const;

        float const & alpha() const;
        
    private:
        T ewma_;
        float alpha_;
        unsigned cnt_;
    };
  
}

///////////////////////////////hh.e////////////////////////////////////////
#include "StatisticAccumulator.cci"
#include "StatisticAccumulator.ct"
#include "StatisticAccumulator.cti"
#endif

