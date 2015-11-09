// LiveTripSource.cpp

#include "just/trip/Common.h"
#include "just/trip/LiveTripSource.h"

#include <just/cdn/pptv/PptvLive.h>

#include <framework/logger/StreamRecord.h>
#include <framework/string/Format.h>
#include <framework/string/Slice.h>
using namespace framework::string;

#include <boost/bind.hpp>

namespace just
{
    namespace trip
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("just.trip.LiveTripSource", Debug);

        LiveTripSource::LiveTripSource(
            boost::asio::io_service & io_svc)
            : TripSource(io_svc)
            , seq_(0)
        {
        }

        LiveTripSource::~LiveTripSource()
        {
        }

        boost::uint64_t LiveTripSource::total(
            boost::system::error_code & ec)
        {
            if (!use_trip()) {
                return TripSource::total(ec) - 1400;
            } else {
                ec.clear();
                return boost::uint64_t(0x8000000000000000ULL); // 一个非常大的数值，假设永远下载不完
            }
        }

        bool LiveTripSource::prepare(
            framework::string::Url & url, 
            boost::uint64_t & beg, 
            boost::uint64_t & end, 
            boost::system::error_code & ec)
        {
            just::cdn::PptvLive const & live = (just::cdn::PptvLive const &)pptv_media();

            if (!use_trip()) {
                ec.clear();
                return true;
            }

            // "/live/<stream_id>/<file_time>"
            std::vector<std::string> vec;
            std::vector<std::string> vec1;
            slice<std::string>(url.path(), std::back_inserter(vec), "/");
            slice<std::string>(vec[3], std::back_inserter(vec1), ".");

            url.path("/live/");
            TripSource::prepare(url, beg, end, ec);
                url.param("channelid", live.video().rid);

            if (!ec) {
                url.path("/playlive.flv");
                url.param("rid", live.video().rid);
            }

            return !ec;
        }

    } // namespace trip
} // namespace just
