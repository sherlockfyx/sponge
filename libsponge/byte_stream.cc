#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) : 
                        capacity_(capacity) ,
                        read_ptr_(0),
                        write_ptr_(0),
                        end_input_(false), 
                        buffer_(capacity) { 

}

size_t ByteStream::write(const string &data) {
    int min_len = min(remaining_capacity(), data.size());
    for(int i = 0; i < min_len; i++) {
        buffer_[(write_ptr_ + i) % capacity_] = data[i];
    }
    write_ptr_ += min_len;
    return min_len;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    string res = "";
    int min_len = min(len, buffer_size());
    for(int i = 0; i < min_len; i++) {
        res += buffer_[(read_ptr_ + i) % capacity_];
    }
    return res;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) { 
    read_ptr_ += min(len, buffer_size());
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string res = peek_output(len);
    pop_output(len);
    return res;
}

void ByteStream::end_input() {
    end_input_ = true;
}

bool ByteStream::input_ended() const { 
    return end_input_;
}

size_t ByteStream::buffer_size() const { 
    return write_ptr_ - read_ptr_;
}

bool ByteStream::buffer_empty() const { 
    return write_ptr_ == read_ptr_; 
}

bool ByteStream::eof() const { 
    return end_input_ && buffer_empty(); 
}

size_t ByteStream::bytes_written() const { 
    return write_ptr_;
}

size_t ByteStream::bytes_read() const { 
    return read_ptr_;
}

size_t ByteStream::remaining_capacity() const { 
    return capacity_ - (write_ptr_ - read_ptr_);
}
