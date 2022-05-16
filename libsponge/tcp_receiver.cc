#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;


void TCPReceiver::segment_received(const TCPSegment &seg) {
    TCPHeader head = seg.header();
    if(!_is_syn) {
        if(!head.syn) return;
        _isn = head.seqno;
        _is_syn = true;
    }
    uint64_t abs_ackno = _reassembler.stream_out().bytes_written() + 1;
    uint64_t curr_abs_seqno = unwrap(head.seqno, _isn, abs_ackno);
    uint64_t stream_index = curr_abs_seqno - 1 + (head.syn);
    _reassembler.push_substring(seg.payload().copy(), stream_index, head.fin);
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if(!_is_syn) return std::nullopt;

    uint64_t abs_ackno = _reassembler.stream_out().bytes_written() + 1;

    if(stream_out().input_ended())  abs_ackno++;
    return wrap(abs_ackno, _isn);
}

size_t TCPReceiver::window_size() const {
    return _reassembler.stream_out().remaining_capacity();
}
