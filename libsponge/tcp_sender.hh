#ifndef SPONGE_LIBSPONGE_TCP_SENDER_HH
#define SPONGE_LIBSPONGE_TCP_SENDER_HH

#include "byte_stream.hh"
#include "tcp_config.hh"
#include "tcp_segment.hh"
#include "wrapping_integers.hh"

#include <functional>
#include <queue>
#include <map>

class TCPSender {
  private:
    int _timeout{-1};
    int _timecount{0};  // 超时时间

    std::map<size_t, TCPSegment> _outgoing_map{};

    size_t _outgoing_bytes{0};  // 未确认的字节大小
    size_t _last_window_size{1};  // 滑动窗口大小

    bool _set_syn_flag{false};
    bool _set_fin_flag{false};
    
    size_t _consecutive_retransmissions_count{0}; // 连续重传计数

    WrappingInt32 _isn; // 初始序列号

    std::queue<TCPSegment> _segments_out{}; // 封装成TCPSegment, 发送的数据段

    unsigned int _initial_retransmission_timeout;   // 初始重传时间

    ByteStream _stream; // 内存中的数据

    uint64_t _next_seqno{0};  // 下一个发送数据的序号

  public:
    TCPSender(const size_t capacity = TCPConfig::DEFAULT_CAPACITY,
              const uint16_t retx_timeout = TCPConfig::TIMEOUT_DFLT,
              const std::optional<WrappingInt32> fixed_isn = {});
    // ByteStream
    ByteStream &stream_in() { return _stream; }
    const ByteStream &stream_in() const { return _stream; }

    void ack_received(const WrappingInt32 ackno, const uint16_t window_size);

    void send_empty_segment();

    void fill_window();

    void tick(const size_t ms_since_last_tick);

    size_t bytes_in_flight() const;

    unsigned int consecutive_retransmissions() const;

    std::queue<TCPSegment> &segments_out() { return _segments_out; }

    uint64_t next_seqno_absolute() const { return _next_seqno; }

    WrappingInt32 next_seqno() const { return wrap(_next_seqno, _isn); }
};

#endif  // SPONGE_LIBSPONGE_TCP_SENDER_HH
