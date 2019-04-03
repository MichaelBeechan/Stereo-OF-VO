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
 #ifndef PNGPP_GENERATOR_HPP_INCLUDED
 #define PNGPP_GENERATOR_HPP_INCLUDED
 
 #include <cassert>
 #include <stdexcept>
 #include <iostream>
 #include <ostream>
 
 #include <endian.h>
 
 #include "error.hpp"
 #include "streaming_base.hpp"
 #include "writer.hpp"
 
 namespace png
 {
 
    template< typename pixel,
               class pixgen,
               class info_holder = def_image_info_holder,
               bool interlacing_supported = false >
     class generator
         : public streaming_base< pixel, info_holder >
     {
     public:
         template< typename ostream >
         void write(ostream& stream)
         {
            writer< ostream > wr(stream);
             wr.set_image_info(this->get_info());
             wr.write_info();
 
 #if __BYTE_ORDER == __LITTLE_ENDIAN
             if (pixel_traits< pixel >::get_bit_depth() == 16)
             {
 #ifdef PNG_WRITE_SWAP_SUPPORTED
                 wr.set_swap();
 #else
                 throw error("Cannot write 16-bit image --"
                             " recompile with PNG_WRITE_SWAP_SUPPORTED.");
 #endif
             }
 #endif

            size_t pass_count;
             if (this->get_info().get_interlace_type() != interlace_none)
             {
 #ifdef PNG_WRITE_INTERLACING_SUPPORTED
                 if (interlacing_supported)
                 {
                     pass_count = wr.set_interlace_handling();
                 }
                 else
                 {
                     throw std::logic_error("Cannot write interlaced image --"
                                            " generator does not support it.");
                 }
#else
                 throw error("Cannot write interlaced image --"
                             " interlace handling disabled.");
 #endif
             }
            else
             {
                 pass_count = 1;
             }
            pixgen* pixel_gen = static_cast< pixgen* >(this);
             for (size_t pass = 0; pass < pass_count; ++pass)
             {
                 pixel_gen->reset(pass);

                for (size_t pos = 0; pos < this->get_info().get_height(); ++pos)
                {
                     wr.write_row(pixel_gen->get_next_row(pos));
                 }
             }
 
             wr.write_end_info();
         }
 
     protected:
         typedef streaming_base< pixel, info_holder > base;
 
        explicit generator(image_info& info)
             : base(info)
         {
         }
 
         generator(size_t width, size_t height)
             : base(width, height)
        {
         }
    };
 
 } // namespace png
 
 #endif // PNGPP_GENERATOR_HPP_INCLUDED