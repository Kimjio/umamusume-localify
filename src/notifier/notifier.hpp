#pragma once
#include <string>

namespace notifier
{
    void init();

    void notify_response(const std::string& data);

    void notify_request(const std::string& data);
}
