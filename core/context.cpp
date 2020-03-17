#include "context.h"


Context::Context(std::string buffer, pos position)
{
    buffer_ = buffer;
    position_ = position;
}

Context::Context(const Context &other)
{
    buffer_ = other.buffer_;
    position_ = other.position_;
}

Context &Context::operator=(const Context &other){
    buffer_ = other.buffer_;
    position_ = other.position_;
    return *this;
}

bool Context::operator ==(Context other) {
    return ((buffer_ == other.buffer_) &&
            (position_ == other.position_));
}

bool Context::operator !=(Context other) {
    return ((buffer_ != other.buffer_) ||
            (position_ != other.position_));
}

pos Context::position() const
{
    return position_;
}

std::string Context::buffer() const
{
    return buffer_;
}
