// TripModule.h

#ifndef _JUST_TRIP_TRIP_MODULE_H_
#define _JUST_TRIP_TRIP_MODULE_H_

#include <just/common/PortManager.h>
#include <just/trip_worker/ClientStatus.h>

#ifndef JUST_DISABLE_DAC
#include <just/dac/DacModule.h>
#endif

#ifndef JUST_CONTAIN_TRIP_WORKER
#include <framework/process/NamedMutex.h>

namespace framework
{
    namespace process { class Process; }
    namespace timer { class Timer; }
}
#endif

namespace just
{
    namespace trip
    {

        class TripModule
            : public just::common::CommonModuleBase<TripModule>
        {
        public:

            TripModule(
                util::daemon::Daemon & daemon);

            ~TripModule();

        public:
            virtual bool startup(
                boost::system::error_code & ec);

            virtual bool shutdown(
                boost::system::error_code & ec);

        public:
            boost::uint16_t port() const
            {
                return port_;
            }

            bool is_alive();

#ifndef JUST_CONTAIN_TRIP_WORKER
            framework::process::Process const & process() const
            {
                return *process_;
            }
#endif

        public:
            static std::string version();

            static std::string name();

        private:
            void check();

            bool is_lock();

        private:
#ifndef JUST_DISABLE_DAC
            just::dac::DacModule& dac_;
#endif
            just::common::PortManager &portMgr_;

            boost::uint16_t port_;

#ifndef JUST_CONTAIN_TRIP_WORKER
        private:
            framework::process::Process * process_;
            framework::timer::Timer * timer_;

            framework::process::NamedMutex mutex_;

            bool is_locked_;
#endif
        };

    } // namespace trip
} // namespace just

#endif // _JUST_TRIP_TRIP_MODULE_H_
