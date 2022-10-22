// Copyright Takatoshi Kondo 2022
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#if !defined(ASYNC_MQTT_PACKET_TOPIC_SHARENAME_HPP)
#define ASYNC_MQTT_PACKET_TOPIC_SHARENAME_HPP

#include <boost/assert.hpp>

#include <async_mqtt/buffer.hpp>

namespace async_mqtt {

class topic_sharename {
public:
    topic_sharename(
        buffer all_topic
    ): all_topic_{force_move(all_topic)} {
        BOOST_ASSERT(all_topic_.size() <= 0xffff);
        auto const shared_prefix = string_view("$share/");
        if (all_topic_.substr(0, shared_prefix.size()) == shared_prefix) {
            // must have sharename
            sharename_ = all_topic_.substr(shared_prefix.size());

            auto const idx = sharename_.find_first_of('/');
            if (idx == 0 || idx == string_view::npos) return;

            topic_ = sharename_.substr(idx + 1);
            sharename_.remove_suffix(sharename_.size() - idx);
        }
        else {
            topic_ = all_topic_;
        }
    }

    buffer const& topic() const {
        return topic_;
    }

    buffer const& sharename() const {
        return sharename_;
    }

    buffer const& all_topic() const {
        return all_topic_;
    }

    operator bool() const {
        return !topic_.empty();
    }

    friend
    bool operator<(topic_sharename const& lhs, topic_sharename const& rhs) {
        return
            std::tie(lhs.topic_, lhs.sharename_) <
            std::tie(rhs.topic_, rhs.sharename_);
    }

    friend
    bool operator==(topic_sharename const& lhs, topic_sharename const& rhs) {
        return
            std::tie(lhs.topic_, lhs.sharename_) ==
            std::tie(rhs.topic_, rhs.sharename_);
    }

private:
    buffer all_topic_;
    buffer topic_;
    buffer sharename_;
};

} // namespace async_mqtt

#endif // ASYNC_MQTT_PACKET_TOPIC_SHARENAME_HPP
