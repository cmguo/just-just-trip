// TripModule.cpp

#include "just/trip/Common.h"
#include "just/trip/TripModule.h"
#include "just/trip/Error.h"
#include "just/trip/ClassRegister.h"

#include <p2p/trip/worker/Name.h>

#ifndef JUST_DISABLE_DAC
#include <just/dac/DacModule.h>
#include <just/dac/DacInfoWorker.h>
using namespace just::dac;
#endif

#ifdef JUST_CONTAIN_TRIP_WORKER
#include <framework/process/Environments.h>
using namespace framework::process;
namespace trip { namespace worker {
    void register_module(util::daemon::Daemon & daemon);
}}
#else
#include <framework/process/Process.h>
#include <framework/process/ProcessEnviron.h>
#include <framework/timer/Timer.h>
#include <framework/string/Url.h>
using namespace framework::timer;
using namespace framework::process;
#endif
#include <framework/memory/MemoryReference.h>
#include <framework/system/ErrorCode.h>
#include <framework/system/LogicError.h>
#include <framework/string/Format.h>
#include <framework/logger/StreamRecord.h>
using namespace framework::system;
using namespace framework::string;

#include <boost/bind.hpp>
using namespace boost::system;

namespace just
{
    namespace trip
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("just.trip.TripModule", framework::logger::Debug)

        TripModule::TripModule(
            util::daemon::Daemon & daemon)
            : just::common::CommonModuleBase<TripModule>(daemon, "TripModule")
#ifndef JUST_DISABLE_DAC
            , dac_(util::daemon::use_module<just::dac::DacModule>(daemon))
#endif
            , port_("2015")
#ifndef JUST_CONTAIN_TRIP_WORKER
            , mutex_(9000)
            , is_locked_(false)
#endif
        {
#ifdef JUST_CONTAIN_TRIP_WORKER
            ::trip::worker::register_module(daemon);
#else
            process_ = new Process;
            timer_ = new Timer(timer_queue(), 
                    10, // 5 seconds
                boost::bind(&TripModule::check, this));
#endif
        }

        TripModule::~TripModule()
        {
#ifndef JUST_CONTAIN_TRIP_WORKER
            if (is_lock()) {
                mutex_.unlock();
                is_locked_ = false;
            }
            if (process_) {
                delete process_;
                process_ = NULL;
            }
            if (timer_) {
                delete timer_;
                timer_ = NULL;
            }
#endif
        }


        bool TripModule::startup(
            error_code & ec)
        {
            LOG_INFO("[startup]");
#ifndef JUST_DISABLE_DAC
            dac_.set_vod_version(version());
            dac_.set_vod_name(name());
#endif
#ifndef JUST_CONTAIN_TRIP_WORKER
            timer_->start();
            if (is_lock()) {
                LOG_INFO("[startup] try_lock");
#ifdef __APPLE__
                boost::filesystem::path cmd_file(MAC_TRIP_WORKER);
#else
                boost::filesystem::path cmd_file(name());
#endif
                Process::CreateParamter param;
                param.wait = true;
                process_->open(cmd_file, param, ec);
                if (!ec) {
                    update_port();
                } else {
                    LOG_WARN("[startup] ec = " << ec.message());
                    if (ec == boost::system::errc::no_such_file_or_directory) {
                        ec.clear();
                    }
                    timer_->stop();
                }
            }
#else
            update_port();
#endif
            return !ec;
        }

        bool TripModule::shutdown(
            error_code & ec)
        {
            port_.clear();
#ifndef JUST_CONTAIN_TRIP_WORKER
            if (process_) {
                process_->signal(Signal::sig_int, ec);
                process_->timed_join(1000, ec);
                if (!ec) {
                    LOG_INFO("[shutdown] ok");
                } else {
                    LOG_WARN("[shutdown] ec = " << ec.message());
                }
                process_->kill(ec);
                process_->close(ec);
            }
            if (timer_) {
                timer_->stop();
            }
            if (is_locked_) {
                mutex_.unlock();
                is_locked_ = false;
            }    
#endif            
            return !ec;
        }

        void TripModule::check()
        {
#ifndef JUST_CONTAIN_TRIP_WORKER
            error_code ec;
            if (is_lock()) {
                if (process_ && !process_->is_alive(ec)) {
                    LOG_ERROR("[check] worker is dead: " << ec.message());
                    port_.clear();
#ifndef JUST_DISABLE_DAC
                    util::daemon::use_module<just::dac::DacModule>(get_daemon())
                        .submit(DacRestartInfo(DacRestartInfo::vod));
#endif
                    process_->close(ec);
#ifdef __APPLE__
                    boost::filesystem::path cmd_file(MAC_TRIP_WORKER);
#else
                    boost::filesystem::path cmd_file(name());
#endif
                    Process::CreateParamter param;
                    param.wait = true;
                    process_->open(cmd_file, param, ec);
                    if (!ec) {
                        update_port();
                    } else {
                        LOG_WARN("[check] ec = " << ec.message());
                        timer_->stop();
                    }
                }
            }
#endif
        }

        bool TripModule::is_alive()
        {
            error_code ec;
#ifdef JUST_CONTAIN_TRIP_WORKER
            return true;
#else

            if (is_locked_) {
                return process_ && process_->is_alive(ec);
            } else {
                framework::process::Process process;
#ifdef __APPLE__
                boost::filesystem::path cmd_file(MAC_TRIP_WORKER);
#else
                boost::filesystem::path cmd_file(name());
#endif
                process.open(cmd_file, ec);
                return !ec;
            }
#endif
        }

        std::string TripModule::version()
        {
            //return just::trip_worker::version_string();
            return "1.0.0.1";
        }

        std::string TripModule::name()
        {
            return ::trip::worker::name_string();
        }

        framework::string::Url & TripModule::get_p2p_url(
            framework::string::Url const & cdn_url, 
            framework::string::Url & url)
        {
            return TripSource::get_p2p_url(
                cdn_url, port_, url);
        }

#ifndef JUST_CONTAIN_TRIP_WORKER
        bool TripModule::is_lock()
        {
            if (!is_locked_) {
                is_locked_ = mutex_.try_lock();
            }

            return is_locked_;
        }
#endif

        void TripModule::update_port()
        {
            /*
#ifndef JUST_CONTAIN_TRIP_WORKER
            get_process_environ(process_->id(), ENVIRON_HTTP_PORT, port_);
#else
            port_ = get_environment(ENVIRON_HTTP_PORT);
#endif
*/
            LOG_INFO("[update_port] port = " << port_); 
        }

    } // namespace trip
} // namespace just
