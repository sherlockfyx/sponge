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
                                    eof_index_(0),
                                    eof_end_(false),
                                    buffer_(capacity, '\0'),
                                    bitmap_((capacity + sizeof(bits_t) * 8 - 1) / (sizeof(bits_t) * 8), 0),
                                    output_(capacity), 
                                    capacity_(capacity) {}

void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    size_t unassembled_index = output_.bytes_written();        // 没有写入到output_的数据
    // size_t unacceptable_index = unassembled_index + capacity_;
    size_t unacceptable_index = unassembled_index + output_.remaining_capacity(); // 可以写入到buffer_中的数据
             
    size_t start_index = max(unassembled_index, index);
    size_t end_index = min(unacceptable_index, index + data.size());
    const uint8_t B = sizeof(bits_t) * 8;

    // 整合数据
    for(size_t i = start_index; i < end_index; i++) {
        if(!(bitmap_[(i % capacity_) / B] & (1 << ((i % capacity_) % B)))) { 
            buffer_[i % capacity_] = data[i - index];
            bitmap_[(i % capacity_) / B] |= static_cast<bits_t>(1 << ((i % capacity_) % B));
            unassembled_bytes_++;
        }
    }

    // 将要写入output_的数据的bitmap置0;
    size_t unwriteable_index = unassembled_index;
    while ((bitmap_[(unwriteable_index % capacity_) / B] & (1 << ((unwriteable_index % capacity_) % B)))) {
        // if(unwriteable_index - unassembled_index >= output_.remaining_capacity()) break;
        bitmap_[(unwriteable_index % capacity_) / B] &= ~static_cast<bits_t>(1 << ((unwriteable_index % capacity_) % B));
        unwriteable_index++;
    }

    string res = "";
    for(size_t j = unassembled_index; j < unwriteable_index; j++) res += buffer_[j % capacity_];
    // (unw - una) 一定小于等于 output_.remaining_capacity() 所以可以全部写入
    output_.write(res);
    unassembled_bytes_ -= (unwriteable_index - unassembled_index);

    if (eof) {
        eof_end_ = true;    
        eof_index_ = index + data.size();
    }
    
    if (eof_end_ && eof_index_ <= output_.bytes_written()) {    // 如果输入结束且所有数据都发送了，则结束字节流
       output_.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const {
    return unassembled_bytes_; 
}

bool StreamReassembler::empty() const { 
    return unassembled_bytes_ == 0;
}
