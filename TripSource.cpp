// TripSource.cpp

#include "just/trip/Common.h"
#include "just/trip/TripSource.h"

#include <just/cdn/trip/TripMedia.h>

#include <just/demux/base/DemuxEvent.h>
#include <just/demux/segment/SegmentDemuxer.h>

#include <just/merge/MergerBase.h>

#include <just/data/segment/SegmentSource.h>

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/string/Format.h>
#include <framework/string/Md5.h>
#include <framework/string/Digest.hpp>
using namespace framework::string;

namespace just
{
    namespace trip
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("just.trip.TripSource", framework::logger::Debug);

        TripSource::TripSource(
            boost::asio::io_service & io_svc)
            : just::cdn::P2pSource(io_svc)
            , module_(util::daemon::use_module<just::trip::TripModule>(io_svc))
            , trip_fail_(false)
        {
        }

        TripSource::~TripSource()
        {
        }

        void TripSource::parse_param(
            std::string const & params)
        {
            if (use_trip()) {
                const_cast<just::data::SegmentSource &>(seg_source()).set_time_out(0);
            }
        }

        bool TripSource::prepare(
            framework::string::Url & url, 
            boost::uint64_t & beg, 
            boost::uint64_t & end, 
            boost::system::error_code & ec)
        {
            LOG_DEBUG("Use trip worker");

            url.svc(format(module_.port()));

            ec.clear();
            return true;
        }

        bool TripSource::use_trip()
        {
            if (!trip_fail_ && seg_source().num_try() > 3)
                trip_fail_ = true;
            return !module_.port().empty() && !trip_fail_;
        }

        framework::string::Url & TripSource::get_p2p_url(
            framework::string::Url const & cdn_url, 
            std::string const & port, 
            framework::string::Url & url)
        {
            url.from_string("http://127.0.0.1/meta");
            url.svc(port);
            url.param("url", cdn_url.to_string());
            url.param("session", md5(cdn_url.to_string()).to_string());
            return url;
        }


    } // namespace trip
} // namespace just
