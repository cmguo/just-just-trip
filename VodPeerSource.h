// VodTripSource.h

#ifndef JUST_TRIP_VOD_TRIP_SOURCE_H_
#define JUST_TRIP_VOD_TRIP_SOURCE_H_

#include "just/trip/TripSource.h"

namespace just
{
    namespace trip
    {
        class VodTripSource
            : public TripSource
        {
        public:
            VodTripSource(
                boost::asio::io_service & io_svc);

            ~VodTripSource();

        private:
            // not use encodings
            virtual std::size_t private_read_some(
                buffers_t const & buffers,
                boost::system::error_code & ec);

            virtual void private_async_read_some(
                buffers_t const & buffers, 
                handler_t const & handler);

        private:
            virtual bool prepare(
                framework::string::Url & url, 
                boost::uint64_t & beg, 
                boost::uint64_t & end, 
                boost::system::error_code & ec);
        };

        UTIL_REGISTER_URL_SOURCE("ppvod", VodTripSource);
        UTIL_REGISTER_URL_SOURCE("ppvod2", VodTripSource);

    } // namespace trip
} // namespace just

#endif // JUST_TRIP_VOD_TRIP_SOURCE_H_
