  #include "stream_adapter.h"
  
  /**
   * @brief For output streams, this typically results in writing the contents
   * of the put area into the associated sequence, i.e. flushing of the output
   * buffer.
   *
   * @return int Returns ​0​ on success, -1 otherwise. The base class
   * version returns ​0​.
   */
  int OSAStreambuf::sync() {
    int ret = 0;
    if (pbase() < pptr()) {
      const int_type tmp = overflow();
      if (traits_type::eq_int_type(tmp, traits_type::eof())) {
        ret = -1;
      }
    }
    return ret;
  }

  /**
   * @brief Write overflowed chars to file, derived from std::streambuf
   *        It's user's responsibility to maintain correct sequence of
   *        output as we are using shared file pointer
   *
   * @param ch
   * @return int_type Returns unspecified value not equal to Traits::eof() on
   * success, Traits::eof() on failure.
   */
  OSAStreambuf::int_type OSAStreambuf::overflow(int_type ch) {
    // https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/include/bits/fstream.tcc
    int_type ret = traits_type::eof();
    const bool testeof = traits_type::eq_int_type(ch, ret);

    if (pptr() == nullptr) {
      reset_ptr();
      if (!testeof) {
        ret = sputc(ch);
      }
    } else {
      if (!testeof) {
        *pptr() = traits_type::to_char_type(ch);
        pbump(1);
      }
      if (receiver->receive(pbase(), pptr() - pbase())) {
        ret = traits_type::not_eof(ch);
      }
      reset_ptr();
    }
    return ret;
  }

  /**
   * @brief Writes \c count characters to the output sequence from the character
   * array whose first element is pointed to by \c s . Overwrite this function
   * to achieve no_buffered I/O
   * @note - this will attempt to write data in chunks equal to the buffer size.
   * Aim to call sync (i.e. flush on the stream) before we get to that state
   * so that the data arrives in one hit with sensible framing, not split
   * over multiple packets.
   * @param s
   * @param n
   * @return std::streamsize
   */
  std::streamsize OSAStreambuf::xsputn(const char_type *s,
                                std::streamsize n) {
    std::streamsize bufavail = epptr() - pptr();
    std::streamsize ret = n;

    // fill buffer up to "buf_size"
    std::streamsize nfill = std::min(n, bufavail + 1);
    std::copy(s, s + nfill, pptr());
    pbump(nfill); // if nfill == bufavail+1, pptr() == epptr()

    if (nfill == bufavail + 1) {
      // equiv: bufavail + 1<= n
      if (!receiver->receive(pbase(), pptr() - pbase())) {
        ret = -1;
      } else {
        reset_ptr();
        s += nfill;
        n -= nfill;

        /*
          repeatedly write every chunk until there is
          less data than buf_size - 1
        */
        while (n >= buf_size - 1) {
          receiver->receive(s, buf_size);
          s += buf_size;
          n -= buf_size;
        }
        std::copy(s, s + n, pptr());
        pbump(n);
      }
    }
    return ret;
  }

    /**
   * @brief Entirely fails to seek as underlying transport does not support it.
   *
   * @param off relative position to set the position indicator to.
   * @param dir defines base position to apply the relative offset to.
   *            It can be one of the following constants: beg, cur, end
   * @param which
   * @return -1 (fail)
   */
    OSAStreambuf::pos_type OSAStreambuf::seekoff(off_type off, std::ios_base::seekdir dir,
          __attribute__((__unused__)) std::ios_base::openmode which)  {
    return pos_type(off_type(-1));
  }

  OSAStreambuf::pos_type OSAStreambuf::seekpos(pos_type pos, 
            __attribute__((__unused__)) std::ios_base::openmode which)  {
    return seekoff(off_type(pos), std::ios_base::beg);
  }
