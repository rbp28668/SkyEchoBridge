#include <iostream>
#include "stream_receiver.h"

// https://stackoverflow.com/questions/772355/how-to-inherit-from-stdostream

class OSAStreambuf : public std::streambuf
{
    StreamReceiver *receiver;
    static const std::streamsize buf_size = 1024;
    char buffer_[buf_size];

public:
    using Base = std::streambuf;
    using char_type = typename Base::char_type;
    using int_type = typename Base::int_type;
    using pos_type = typename Base::pos_type;
    using off_type = typename Base::off_type;

private:
    /**
     * @brief Always save one extra space in buffer_ for overflow
     */
    inline void reset_ptr() { setp(buffer_, buffer_ + buf_size - 1); }

protected:
    int sync() override;
    int_type overflow(int_type ch = traits_type::eof()) override;
    std::streamsize xsputn(const char_type *s, std::streamsize n);
    pos_type seekoff(off_type off, std::ios_base::seekdir dir,
                     __attribute__((__unused__))
                     std::ios_base::openmode which = std::ios_base::out) override;
    pos_type seekpos(pos_type pos, __attribute__((__unused__))
                                   std::ios_base::openmode which =
                                       std::ios_base::out) override;

public:
    OSAStreambuf(StreamReceiver *receiver) : receiver(receiver) {}
    ~OSAStreambuf() { sync(); }
};

class OutputStreamAdapter : private OSAStreambuf, public std::basic_ostream<char>
{

public:
    // Types
    using Base = std::basic_ostream<char>;
    using int_type = typename Base::int_type;
    using char_type = typename Base::char_type;
    using pos_type = typename Base::pos_type;
    using off_type = typename Base::off_type;
    using traits_type = typename Base::traits_type;

    OutputStreamAdapter(StreamReceiver *receiver)
        : OSAStreambuf(receiver), std::basic_ostream<char>(this)

    {
    }
};