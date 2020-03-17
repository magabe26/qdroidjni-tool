/**
 * Copyright 2020 - MagabeLab (Tanzania). All Rights Reserved.
 * Author Edwin Magabe    edyma50@yahoo.com
 */

#ifndef CONTEXT_H
#define CONTEXT_H

#include "def.h"

class Context
{
public:
    Context(std::string buffer,pos position);
    Context(const Context &other);

    Context &operator=(const Context &other);

    bool operator ==(Context other);
    bool operator !=(Context other);

    pos position() const;
    std::string buffer() const;

private:
    std::string buffer_;
    pos position_;
};

#endif // CONTEXT_H
