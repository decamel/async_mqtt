// Copyright Takatoshi Kondo 2022
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#if !defined(ASYNC_MQTT_PACKET_V3_1_1_PUBREL_HPP)
#define ASYNC_MQTT_PACKET_V3_1_1_PUBREL_HPP

#include <utility>
#include <numeric>

#include <boost/numeric/conversion/cast.hpp>

#include <async_mqtt/exception.hpp>
#include <async_mqtt/buffer.hpp>

#include <async_mqtt/util/move.hpp>
#include <async_mqtt/util/static_vector.hpp>
#include <async_mqtt/util/endian_convert.hpp>

#include <async_mqtt/packet/packet_id_type.hpp>
#include <async_mqtt/packet/fixed_header.hpp>

namespace async_mqtt::v3_1_1 {

namespace as = boost::asio;

template <std::size_t PacketIdBytes>
class basic_pubrel_packet {
public:
    using packet_id_t = typename packet_id_type<PacketIdBytes>::type;
    basic_pubrel_packet(
        packet_id_t packet_id
    )
        : all_(all_.capacity())
    {
        all_[0] = static_cast<char>(make_fixed_header(control_packet_type::pubrel, 0b0010));
        all_[1] = boost::numeric_cast<char>(PacketIdBytes);
        endian_store(packet_id, &all_[2]);
    }

    basic_pubrel_packet(buffer buf) {
        // fixed_header
        if (buf.empty()) {
            throw make_error(
                errc::bad_message,
                "v3_1_1::pubrel_packet fixed_header doesn't exist"
            );
        }
        all_.push_back(buf.front());
        buf.remove_prefix(1);
        auto cpt_opt = get_control_packet_type_with_check(static_cast<std::uint8_t>(all_.back()));
        if (!cpt_opt || *cpt_opt != control_packet_type::pubrel) {
            throw make_error(
                errc::bad_message,
                "v3_1_1::pubrel_packet fixed_header is invalid"
            );
        }

        // remaining_length
        if (buf.empty()) {
            throw make_error(
                errc::bad_message,
                "v3_1_1::pubrel_packet remaining_length doesn't exist"
            );
        }
        all_.push_back(buf.front());
        buf.remove_prefix(1);
        if (static_cast<std::uint8_t>(all_.back()) != PacketIdBytes) {
            throw make_error(
                errc::bad_message,
                "v3_1_1::pubrel_packet remaining_length is invalid"
            );
        }

        // variable header
        if (buf.size() != PacketIdBytes) {
            throw make_error(
                errc::bad_message,
                "v3_1_1::pubrel_packet variable header doesn't match its length"
            );
        }
        std::copy(buf.begin(), buf.end(), std::back_inserter(all_));
    }

    constexpr control_packet_type type() const {
        return control_packet_type::pubrel;
    }

    /**
     * @brief Create const buffer sequence
     *        it is for boost asio APIs
     * @return const buffer sequence
     */
    std::vector<as::const_buffer> const_buffer_sequence() const {
        std::vector<as::const_buffer> ret;

        ret.emplace_back(as::buffer(all_.data(), all_.size()));
        return ret;
    }

    /**
     * @brief Get whole size of sequence
     * @return whole size
     */
    std::size_t size() const {
        return all_.size();
    }

    /**
     * @brief Get number of element of const_buffer_sequence
     * @return number of element of const_buffer_sequence
     */
    static constexpr std::size_t num_of_const_buffer_sequence() {
        return 1; // all
    }

    packet_id_t packet_id() const {
        return endian_load<packet_id_t>(&all_[2]);
    }

private:
    static_vector<char, 2 + PacketIdBytes> all_;
};

template <std::size_t PacketIdBytes>
inline std::ostream& operator<<(std::ostream& o, basic_pubrel_packet<PacketIdBytes> const& v) {
    o <<
        "v3_1_1::pubrel{" <<
        "pid:" << v.packet_id() <<
        "}";
    return o;
}

using pubrel_packet = basic_pubrel_packet<2>;

} // namespace async_mqtt::v3_1_1

#endif // ASYNC_MQTT_PACKET_V3_1_1_PUBREL_HPP
