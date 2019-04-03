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
 #ifndef PNGPP_GRAY_PIXEL_HPP_INCLUDED
 #define PNGPP_GRAY_PIXEL_HPP_INCLUDED
 
 #include "types.hpp"
 #include "packed_pixel.hpp"
 #include "pixel_traits.hpp"
 
 namespace png
 {
 
     typedef byte gray_pixel;

     typedef uint_16 gray_pixel_16;
 
     template< size_t bits >
    class packed_gray_pixel
         : public packed_pixel< bits >
    {
     public:
        packed_gray_pixel(byte value = 0)
            : packed_pixel< bits >(value)
         {
         }
     };
 
     typedef packed_gray_pixel< 1 > gray_pixel_1;
 
     typedef packed_gray_pixel< 2 > gray_pixel_2;
 
     typedef packed_gray_pixel< 4 > gray_pixel_4;
 
     template<>
     struct pixel_traits< gray_pixel >
         : basic_pixel_traits< gray_pixel, byte, color_type_gray >
     {
     };
 
     template<>
     struct pixel_traits< gray_pixel_16 >
         : basic_pixel_traits< gray_pixel_16, uint_16, color_type_gray >
     {
     };
 
     template< size_t bits >
     struct pixel_traits< packed_gray_pixel< bits > >
         : basic_pixel_traits< packed_gray_pixel< bits >, byte,
                               color_type_gray, /* channels = */ 1, bits >
    {
     };
 
 } // namespace png

 #endif // PNGPP_GRAY_PIXEL_HPP_INCLUDED