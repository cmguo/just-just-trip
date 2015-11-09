// VodTripSource.cpp

#include "just/trip/Common.h"
#include "just/trip/VodTripSource.h"

#include <just/cdn/pptv/PptvVod.h>

#include <util/protocol/http/HttpSocket.hpp>

#include <framework/string/Format.h>
using namespace framework::string;

#include <framework/logger/StreamRecord.h>

namespace just
{
    namespace trip
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("just.trip.VodTripSource", framework::logger::Debug);

        VodTripSource::VodTripSource(
            boost::asio::io_service & io_svc)
            : TripSource(io_svc)
        {
        }

        VodTripSource::~VodTripSource()
        {
        }

        std::size_t VodTripSource::private_read_some(
            buffers_t const & buffers,
            boost::system::error_code & ec)
        {
            assert(http_.is_open(ec));
            return http_.read_some(buffers, ec);
        }

        void VodTripSource::private_async_read_some(
            buffers_t const & buffers,
            util::stream::StreamHandler const & handler)
        {
            boost::system::error_code ec;
            (void)ec;
            assert(http_.is_open(ec));
            http_.async_read_some(buffers, handler);
        }

        bool VodTripSource::prepare(
            framework::string::Url & url, 
            boost::uint64_t & beg, 
            boost::uint64_t & end, 
            boost::system::error_code & ec)
        {
            just::cdn::PptvVod const & vod = (just::cdn::PptvVod const &)pptv_media();
            
            char const * str_no = url.path().c_str() + 1;
            size_t no = 0;
            for (; *str_no >= '0' && *str_no <= '9'; ++str_no) {
                no = no * 10 + (*str_no - '0');
            }

            // ��ʽ���ԵĶ�ֱ��ͨ��CDN����������
            if (*str_no != '/' || !use_trip()) {
                ec.clear();
                return true;
            }

            just::data::SegmentInfo info;
            vod.segment_info(no, info, ec);

            TripSource::prepare(url, beg, end, ec);

            if (!ec) {
                url.path("/ppvaplaybyopen");
                url.param("filelength", format(info.size));
                url.param("headlength", format(info.head_size));
                url.param("drag", status_->buffer_time() < 15000 ? "1" : "0");
                url.param("headonly", end <= info.head_size ? "1" : "0");
            }

            return !ec;
        }

    } // namespace trip
} // namespace just
