#ifndef SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
#define SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH

#include "byte_stream.hh"

#include <cstdint>
#include <string>


//! \brief A class that assembles a series of excerpts from a byte stream (possibly out of order,
//! possibly overlapping) into an in-order byte stream.
// 数据流重组
class StreamReassembler {
  private:
    // Your code here -- add private members as necessary.
    size_t unassembled_bytes_;    // 已接收但未发送至_output的数据大小
    size_t eof_index_; 
    bool eof_end_;                // 是否结束

    std::string buffer_;          // 缓存
    std::vector<uint8_t> bitmap_; // 标记是否写入

    ByteStream output_;  //!< The reassembled in-order byte stream
    size_t capacity_;    //!< The maximum number of bytes

  public:
    using bits_t = decltype(bitmap_)::value_type;

    StreamReassembler(const size_t capacity);

    void push_substring(const std::string &data, const uint64_t index, const bool eof);

    const ByteStream &stream_out() const { return output_; }

    ByteStream &stream_out() { return output_; }

    size_t unassembled_bytes() const;

    bool empty() const;
};

#endif  // SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
