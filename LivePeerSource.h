// LiveTripSource.h

#ifndef JUST_TRIP_LIVE_TRIP_SOURCE_H_
#define JUST_TRIP_LIVE_TRIP_SOURCE_H_

#include "just/trip/TripSource.h"

namespace just
{
    namespace trip
    {
        class LiveTripSource
            : public TripSource
        {
        public:
            LiveTripSource(
                boost::asio::io_service & io_svc);

            ~LiveTripSource();

        public:
            virtual boost::uint64_t total(
                boost::system::error_code & ec);

        private:
            virtual bool prepare(
                framework::string::Url & url, 
                boost::uint64_t & beg, 
                boost::uint64_t & end, 
                boost::system::error_code & ec);

        private:
            size_t seq_;
        };

        UTIL_REGISTER_URL_SOURCE("pplive2", LiveTripSource);
        UTIL_REGISTER_URL_SOURCE("pplive3", LiveTripSource);

    } // namespace trip
} // namespace just

#endif // JUST_TRIP_LIVE_TRIP_SOURCE_H_
