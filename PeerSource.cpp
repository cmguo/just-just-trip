// TripSource.cpp

#include "just/trip/Common.h"
#include "just/trip/TripSource.h"

#include <just/cdn/pptv/PptvMedia.h>

#include <just/demux/base/DemuxEvent.h>
#include <just/demux/segment/SegmentDemuxer.h>

#include <just/merge/MergerBase.h>

#include <just/data/segment/SegmentSource.h>

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/string/Format.h>
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
            LOG_DEBUG("Use trip worker, BWType: " << pptv_media().jump().bw_type);

            std::string cdn_url = url.to_string();
            url = framework::string::Url();
            url.protocol("http");
            url.host("127.0.0.1");
            url.svc(format(module_.port()));
            url.param("url", cdn_url);
            url.param("BWType", format(pptv_media().jump().bw_type));
            url.param("autoclose", "false");

            url.encode();

            ec.clear();
            return true;
        }

        bool TripSource::use_trip()
        {
            if (!trip_fail_ && seg_source().num_try() > 3)
                trip_fail_ = true;
            return module_.port() > 0 && !trip_fail_ && pptv_media().jump().bw_type != 100;
        }

    } // namespace trip
} // namespace just
