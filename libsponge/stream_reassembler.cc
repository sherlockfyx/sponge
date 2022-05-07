#include "stream_reassembler.hh"

#include <cassert>
// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : 
                                    unassembled_bytes_(0),
                                    next_index_(0),
                                    eof_end_(false),
                                    eof_index(0),
                                    buffer_(capacity, '\0'),
                                    bitmap_((capacity + sizeof(bits_t) * 8 - 1) / (sizeof(bits_t) * 8), 0),
                                    output_(capacity), 
                                    capacity_(capacity) {}

void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    
    next_index_ = output_.bytes_written();
    size_t unacceptable_index = next_index_ + output_.remaining_capacity(); // 可以写入到output_中的数据
    //size_t unreadable_index = output_.bytes_written();

    size_t end_index = index + data.size();             // 

    if(index >= unacceptable_index || end_index < next_index_ ) return;

    const uint8_t B = sizeof(bits_t) * 8;


    size_t i = max(next_index_, index);

    for(; i < min(end_index, unacceptable_index); i++) {
        if(!(bitmap_[(i % capacity_) / B] & (1 << ((i % capacity_) % B)))) {
            
            buffer_[i % capacity_] = data[i - index];
            bitmap_[(i % capacity_) / B] |= static_cast<bits_t>(1 << ((i % capacity_) % B));
            unassembled_bytes_++;
        } 
    }

    size_t start_index = next_index_;


    while ((bitmap_[(next_index_ % capacity_) / B] & (1 << ((next_index_ % capacity_) % B)))) {
        bitmap_[(next_index_ % capacity_) / B] &= ~static_cast<bits_t>(1 << ((next_index_ % capacity_) % B));
        next_index_++;
    }

    string res = "";
    for(size_t j = start_index; j < next_index_; j++) res += buffer_[j % capacity_];
    size_t len = output_.write(res);
    unassembled_bytes_ -= len;

    if (eof) {
        eof_end_ = true;     // 设置eof标志
        eof_index = index + data.size();
    }
    
    if (eof_end_ && output_.bytes_written() == eof_index) {    // 如果输入结束且所有数据都发送了，则结束字节流
       output_.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const {
    return unassembled_bytes_; 
}

bool StreamReassembler::empty() const { 
    return unassembled_bytes_ == 0;
}
