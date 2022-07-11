/*
* $Id$
* Portable Audio I/O Library
* Ring Buffer utility.
*
* Author: Phil Burk, http://www.softsynth.com
* modified for SMP safety on OS X by Bjorn Roche.
* also allowed for const where possible.
* modified for multiple-byte-sized data elements by Sven Fischer
*
* Note that this is safe only for a single-thread reader
* and a single-thread writer.
*
* This program is distributed with the PortAudio Portable Audio Library.
* For more information see: http://www.portaudio.com
* Copyright (c) 1999-2000 Ross Bencina and Phil Burk
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files
* (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software,
* and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
* ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
* CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
* The text above constitutes the entire PortAudio license; however,
* the PortAudio community also makes the following non-binding requests:
*
* Any person wishing to distribute modifications to the Software is
* requested to send the modifications to the original developer so that
* they can be incorporated into the canonical version. It is also
* requested that these non-binding requests be included along with the
* license above.
*/

#pragma once

// The RingDelayBuffer code is based on the pa_ringbuffer.c
// from the Portable Audio I/O Library. The template code is used
// in the Mixxx.
//
// For the pa_ringbuffer.c see: mixxx/lib/portaudio/pa_ringbuffer.c
//
// Added functions:
// - isFull
// - isEmpty
// - length
// - getReadAvailable
// - getWriteAvailable
// - moveReadPositionBy
//
// Modified functions:
// - clear
// - read
// - write

#include "util/samplebuffer.h"
#include "util/types.h"

// TODO(davidchocholaty) Done documentation.

/// The ring delay buffer allows moving with the reading position subject
/// to certain rules. Another difference between the classic ring buffer is,
/// that the ring delay buffer offers to read zero values
/// which were not written by using the write method and write position.
/// Both of these two specific properties are based on the cross-fading
/// between changes of two delays.
class RingDelayBuffer final {
  public:
    RingDelayBuffer(SINT bufferSize);

    virtual ~RingDelayBuffer(){};

    bool isFull() {
        return getWriteAvailable() == 0;
    }

    bool isEmpty() const {
        return m_readPos == m_writePos;
    }

    void clear() {
        m_readPos = 0;
        m_writePos = 0;

        memset(m_buffer.data(), 0, sizeof(m_buffer));
    }

    SINT length() const {
        return m_bufferSize;
    }

    SINT getReadAvailable() {
        // The m_ringFullMask has to be XORed with m_jumpLeftAroundMask
        // to handle the situation, that the read position jump crossed the left side
        // of the delay buffer and the extra bit was set as empty.
        return (m_writePos - m_readPos) & (m_ringFullMask ^ m_jumpLeftAroundMask);
    }

    SINT getWriteAvailable() {
        return m_bufferSize - getReadAvailable();
    }

    SINT read(CSAMPLE* pBuffer, const SINT itemsToRead);
    SINT write(const CSAMPLE* pBuffer, const SINT numItems);
    SINT moveReadPositionBy(SINT jumpSize);

  private:
    // Size of the ring delay buffer.
    SINT m_bufferSize;
    // Position of next readable element.
    SINT m_readPos;
    // Position of next writable element.
    SINT m_writePos;
    // Used for wrapping indices with extra bit to distinguish full/empty.
    SINT m_ringFullMask;
    // Used for fitting indices to buffer.
    SINT m_ringMask;
    // In the case that the read position circled the delay buffer
    // and crossed the left side of the delay buffer, so, for next crossing
    // of the right size of the delay buffer this mask including the full mask
    // is used for masking. It handles specific situations, where the extra bit
    // for write position is empty (so, for the read position too)
    // and the read position jumps to the left over the left delay buffer side.
    SINT m_jumpLeftAroundMask;
    // Ring delay buffer.
    mixxx::SampleBuffer m_buffer;
};
