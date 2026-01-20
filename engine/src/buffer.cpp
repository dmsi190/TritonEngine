// buffer.cpp

#pragma once

#include "buffer.hpp"

namespace triton
{
    cDataBuffer::cDataBuffer(cContext* context) : iObject(context) {}

    cDataBuffer::~cDataBuffer()
    {
    }

    void cDataBuffer::Create(void* data, types::usize byteSize)
    {
    }
}