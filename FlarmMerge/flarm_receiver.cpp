#include "flarm_receiver.h"
#include <stdlib.h>
#include <ctype.h>

bool FlarmReceiver::checkBuffer()
{
    if (offset >= sizeof(buffer) - 1)
    {
        // Message too big - can't process it (garbage data?)
        reset();
        return false;
    }
    return true;
}

void FlarmReceiver::receive(uint8_t *data, size_t nbytes)
{
    for (size_t idx = 0; idx < nbytes; ++idx)
    {
        char ch = (char)data[idx];

        switch (state)
        {
        case 0: // waiting for initial $
            if (ch == '$')
            {
                offset = 0; // start of a new message.
                buffer[offset++] = ch;
                state = 1;
            }
            // Otherwise ignore any chars.
            break;

        case 1: // Hoover up data until we reach the end of message * character
            if (checkBuffer())
            {
                buffer[offset++] = ch;
                if (ch == '*')
                {
                    state = 2;
                }
            }
            break;
        case 2: // First checksum digit?
            if (checkBuffer())
            {
                buffer[offset++] = ch;
                if (isxdigit(ch))
                {
                    state = 3;
                }
                else
                {
                    reset();
                }
            }
            break;
        case 3: // Second checksum digit?
            if (checkBuffer())
            {
                buffer[offset++] = ch;
                if (isxdigit(ch))
                {
                    state = 4;
                }
                else
                {
                    reset();
                }
            }
            break;
        case 4: // <CR>
            if (checkBuffer())
            {
                buffer[offset++] = ch;
                if (ch == '\r')
                {
                    state = 5;
                }
                else
                {
                    reset();
                }
            }
            break;
        case 5: // <LF>
            if (checkBuffer())
            {
                buffer[offset++] = ch;
                if (ch == '\n')
                {
                    // Buffer should contain a full Flarm (NMEA) message
                    // consisting of offset characters.
                    onReceive(buffer, offset);
                }
                reset();
            }
            break;

        default: // Invalid state
            reset(); // try to recover
            break;
        }
    }
}
