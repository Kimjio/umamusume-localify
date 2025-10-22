#include <string>
#include <httplib.h>
#include "stdinclude.hpp"

#include "config/config.hpp"
#include "string_utils.hpp"

namespace notifier
{
    httplib::Client* client = nullptr;

    void init()
    {
        client = new httplib::Client(wide_u8(config::msgpack_notifier_host));
        client->set_connection_timeout(0, config::msgpack_notifier_connection_timeout_ms);
    }

    void notify_response(const string& data)
    {
        if (client == nullptr)
        {
            init();
        }

        auto res = client->Post("/notify/response", data, "application/x-msgpack");
        const auto error = res.error();
        if (error != httplib::Error::Success && config::msgpack_notifier_print_error)
        {
            cout << "Unexpected error from notifier: " << httplib::to_string(error).data() << endl;
        }
    }

    void notify_request(const string& data) {
        if (client == nullptr)
        {
            init();
        }

        auto res = client->Post("/notify/request", data, "application/x-msgpack");
        const auto error = res.error();
        if (error != httplib::Error::Success && config::msgpack_notifier_print_error)
        {
            cout << "Unexpected error from notifier: " << httplib::to_string(error).data() << endl;
        }
    }
}
