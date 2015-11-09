// TripSource.h

#ifndef JUST_TRIP_TRIP_SOURCE_H_
#define JUST_TRIP_TRIP_SOURCE_H_

#include "just/trip/TripModule.h"

#include <just/cdn/pptv/P2pSource.h>

namespace just
{
    namespace trip_worker
    {
        class ClientStatus;
    }

    namespace trip
    {

        class TripSource
            : public just::cdn::P2pSource
        {
        public:
            TripSource(
                boost::asio::io_service & io_svc);

            virtual ~TripSource();

        protected:
            virtual void parse_param(
                std::string const & params);

            virtual bool prepare(
                framework::string::Url & url, 
                boost::uint64_t & beg, 
                boost::uint64_t & end, 
                boost::system::error_code & ec);

            bool use_trip();

        protected:
            TripModule & module_;

        private:
            bool trip_fail_;
        };

    } // namespace trip
} // namespace just

#endif // JUST_TRIP_TRIP_SOURCE_H_
