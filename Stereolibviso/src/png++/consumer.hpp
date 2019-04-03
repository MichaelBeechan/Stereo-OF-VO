/*
00002  * Copyright (C) 2007   Alex Shulgin
00003  *
00004  * This file is part of png++ the C++ wrapper for libpng.  Png++ is free
00005  * software; the exact copying conditions are as follows:
00006  *
00007  * Redistribution and use in source and binary forms, with or without
00008  * modification, are permitted provided that the following conditions are met:
00009  *
00010  * 1. Redistributions of source code must retain the above copyright notice,
00011  * this list of conditions and the following disclaimer.
00012  *
00013  * 2. Redistributions in binary form must reproduce the above copyright
00014  * notice, this list of conditions and the following disclaimer in the
00015  * documentation and/or other materials provided with the distribution.
00016  *
00017  * 3. The name of the author may not be used to endorse or promote products
00018  * derived from this software without specific prior written permission.
00019  *
00020  * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
00021  * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
00022  * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
00023  * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
00024  * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
00025  * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
00026  * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
00027  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
00028  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
00029  * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  */
 #ifndef PNGPP_CONSUMER_HPP_INCLUDED
 #define PNGPP_CONSUMER_HPP_INCLUDED
 #include <cassert>
 #include <stdexcept>
 #include <iostream>
 #include <istream>
 
 #include <endian.h>
 #include "error.hpp"
 #include "streaming_base.hpp"
 #include "reader.hpp"
 #include "pixel_buffer.hpp"
 
 namespace png
 {
 
     template< typename pixel,
               class pixcon,
               class info_holder = def_image_info_holder,
               bool interlacing_supported = false >
     class consumer
         : public streaming_base< pixel, info_holder >
     {
     public:
         typedef pixel_traits< pixel > traits;
 
         struct transform_identity
         {
             void operator()(io_base&) const {}
         };
 
         template< typename istream >
         void read(istream& stream)
         {
             read(stream, transform_identity());
         }
 
         template< typename istream, class transformation >
         void read(istream& stream, transformation const& transform)
         {
             reader< istream > rd(stream);
             rd.read_info();
             transform(rd);
 
 #if __BYTE_ORDER == __LITTLE_ENDIAN
             if (pixel_traits< pixel >::get_bit_depth() == 16)
             {
 #ifdef PNG_READ_SWAP_SUPPORTED
                 rd.set_swap();
 #else
                 throw error("Cannot read 16-bit image --"
                             " recompile with PNG_READ_SWAP_SUPPORTED.");
 #endif
             }
 #endif
 
             // interlace handling _must_ be set up prior to info update
             size_t pass_count;
             if (rd.get_interlace_type() != interlace_none)
             {
 #ifdef PNG_READ_INTERLACING_SUPPORTED
                 pass_count = rd.set_interlace_handling();
 #else
                 throw error("Cannot read interlaced image --"
                            " interlace handling disabled.");
 #endif
             }
             else
             {
                 pass_count = 1;
             }
 
             rd.update_info();
             if (rd.get_color_type() != traits::get_color_type()
                 || rd.get_bit_depth() != traits::get_bit_depth())
             {
                 throw std::logic_error("color type and/or bit depth mismatch"
                                        " in png::consumer::read()");
             }
 
             this->get_info() = rd.get_image_info();
 
             pixcon* pixel_con = static_cast< pixcon* >(this);
             if (pass_count > 1 && !interlacing_supported)
             {
                 skip_interlaced_rows(rd, pass_count);
                 pass_count = 1;
             }
             read_rows(rd, pass_count, pixel_con);
 
             rd.read_end_info();
         }
 
     protected:
         typedef streaming_base< pixel, info_holder > base;
 
         explicit consumer(image_info& info)
             : base(info)
         {
         }
 
     private:
         template< typename istream >
         void skip_interlaced_rows(reader< istream >& rd, size_t pass_count)
         {
            typedef std::vector< pixel > row;
             typedef row_traits< row > row_traits_type;
             row dummy_row(this->get_info().get_width());
            for (size_t pass = 1; pass < pass_count; ++pass)
            {
                rd.read_row(reinterpret_cast< byte* >
                             (row_traits_type::get_data(dummy_row)));
             }
        }
 
         template< typename istream >
         void read_rows(reader< istream >& rd, size_t pass_count,
                        pixcon* pixel_con)
         {
             for (size_t pass = 0; pass < pass_count; ++pass)
             {
                 pixel_con->reset(pass);
 
                 for (size_t pos = 0; pos < this->get_info().get_height(); ++pos)
                 {
                     rd.read_row(pixel_con->get_next_row(pos));
                 }
             }
         }
     };
 
 } // namespace png
 
 #endif // PNGPP_CONSUMER_HPP_INCLUDED