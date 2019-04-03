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
00030  */
 #ifndef PNGPP_CONVERT_COLOR_SPACE_HPP_INCLUDED
 #define PNGPP_CONVERT_COLOR_SPACE_HPP_INCLUDED
 
 #include "error.hpp"
 #include "rgb_pixel.hpp"
 #include "rgba_pixel.hpp"
 #include "gray_pixel.hpp"
 #include "ga_pixel.hpp"
 #include "index_pixel.hpp"
 #include "reader.hpp"
 #include "writer.hpp"
 
 namespace png
{
 
    namespace detail
     {
 
        template< typename pixel >
         struct convert_color_space_impl
         {
             typedef pixel_traits< pixel > traits;
             typedef typename traits::component_type component_type;
             typedef basic_alpha_pixel_traits< component_type > alpha_traits;
 
             template< class reader >
             void operator()(reader& io) const
             {
                 handle_16(io);
                 handle_alpha(io, alpha_traits::get_alpha_filler());
                 handle_palette(io);
                 handle_rgb(io);
                 handle_gray(io);
 
                 io.set_color_type(traits::get_color_type());
                 io.set_bit_depth(traits::get_bit_depth());
             }
 
        protected:
             static void expand_8_to_16(png_struct*, png_row_info* row_info,
                                        byte* row)
             {
 //                dump_row(row, row_info->rowbytes);
 
                 for (size_t i = row_info->rowbytes; i-- > 0; )
                 {
                     row[i*2 + 1] = row[i];
                     row[i*2 + 0] = 0;
                 }
 
 //                dump_row(row, 2*row_info->rowbytes);
             }
 
             static void dump_row(byte const* row, size_t width)
             {
                 printf("{");
                 for (size_t i = 0; i < width; ++i)
                 {
                     printf(" %02x,", row[i]);
                 }
                 printf(" }\n");
             }
 
             template< class reader >
             static void handle_16(reader& io)
             {
                 if (io.get_bit_depth() == 16 && traits::get_bit_depth() == 8)
                 {
 #ifdef PNG_READ_16_TO_8_SUPPORTED
                     io.set_strip_16();
 #else
                     throw error("expected 8-bit data but found 16-bit;"
                                 " recompile with PNG_READ_16_TO_8_SUPPORTED");
 #endif
                }
                 if (io.get_bit_depth() != 16 && traits::get_bit_depth() == 16)
                {
 #ifdef PNG_READ_USER_TRANSFORM_SUPPORTED
                     io.set_read_user_transform(expand_8_to_16);
                     io.set_user_transform_info(NULL, 16,
                                               traits::get_channels());
 #else
                     throw error("expected 16-bit data but found 8-bit;"
                                 " recompile with"
                                 " PNG_READ_USER_TRANSFORM_SUPPORTED");
 #endif
                 }
             }
 
             template< class reader >
             static void handle_alpha(reader& io, uint_32 filler)
             {
                 bool src_alpha = io.get_color_type() & color_mask_alpha;
                 bool dst_alpha = traits::get_color_type() & color_mask_alpha;
                 if (src_alpha && !dst_alpha)
                 {
 #ifdef PNG_READ_STRIP_ALPHA_SUPPORTED
                     io.set_strip_alpha();
 #else
                     throw error("alpha channel unexpected;"
                                 " recompile with"
                                 " PNG_READ_STRIP_ALPHA_SUPPORTED");
 #endif
                 }
                 if (!src_alpha && dst_alpha)
                 {
 #if defined(PNG_tRNS_SUPPORTED) && defined(PNG_READ_EXPAND_SUPPORTED)
                     if ((io.get_color_type() & color_mask_palette)
                         && io.has_chunk(chunk_tRNS))
                     {
                         io.set_tRNS_to_alpha();
                         return;
                     }
 #endif
 #if defined(PNG_READ_FILLER_SUPPORTED) && !defined(PNG_1_0_X)
                     io.set_add_alpha(filler, filler_after);
 #else
                     throw error("expected alpha channel but none found;"
                                 " recompile with PNG_READ_FILLER_SUPPORTED"
                                 " and be sure to use libpng > 1.0.x");
 #endif
                 }
             }

             template< class reader >
             static void handle_palette(reader& io)
             {
                 if (io.get_color_type() == color_type_palette)
                 {
 #ifdef PNG_READ_EXPAND_SUPPORTED
                     io.set_palette_to_rgb();
 
                     if (traits::get_color_type() != color_type_palette)
                     {
                         io.get_info().drop_palette();
                     }
 #else
                     throw error("indexed colors unexpected;"
                                 " recompile with PNG_READ_EXPAND_SUPPORTED");
 #endif
                 }
             }
 
             template< class reader >
             static void handle_rgb(reader& io)
             {
                 bool src_rgb =
                     io.get_color_type() & (color_mask_rgb | color_mask_palette);
                 bool dst_rgb = traits::get_color_type() & color_mask_rgb;
                 if (src_rgb && !dst_rgb)
                 {
 #ifdef PNG_READ_RGB_TO_GRAY_SUPPORTED
                    io.set_rgb_to_gray(/*rgb_to_gray_error*/);
 #else
                     throw error("grayscale data expected;"
                                 " recompile with"
                                 " PNG_READ_RGB_TO_GRAY_SUPPORTED");
 #endif
                }
                 if (!src_rgb && dst_rgb)
                 {
 #ifdef PNG_READ_GRAY_TO_RGB_SUPPORTED
                     io.set_gray_to_rgb();
 #else
                     throw error("expected RGB data;"
                                 " recompile with"
                                 " PNG_READ_GRAY_TO_RGB_SUPPORTED");
 #endif
                 }
             }
 
             template< class reader >
            static void handle_gray(reader& io)
             {
                 if ((io.get_color_type() & ~color_mask_alpha)
                     == color_type_gray)
                 {
                     if (io.get_bit_depth() < 8 && traits::get_bit_depth() >= 8)
                     {
 #ifdef PNG_READ_EXPAND_SUPPORTED
                         io.set_gray_1_2_4_to_8();
 #else
                         throw error("convert_color_space: expected 8-bit data;"
                                     " recompile with"
                                     " PNG_READ_EXPAND_SUPPORTED");
 #endif
                     }
                 }
            }
         };
 
     } // namespace detal
 
     template< typename pixel >
     struct convert_color_space
     {
     };
 
     template<>
     struct convert_color_space< rgb_pixel >
         : detail::convert_color_space_impl< rgb_pixel >
     {
     };
 
     template<>
    struct convert_color_space< rgb_pixel_16 >
         : detail::convert_color_space_impl< rgb_pixel_16 >
     {
     };
 
     template<>
     struct convert_color_space< rgba_pixel >
         : detail::convert_color_space_impl< rgba_pixel >
     {
     };
 
     template<>
     struct convert_color_space< rgba_pixel_16 >
        : detail::convert_color_space_impl< rgba_pixel_16 >
     {
     };
 
     template<>
     struct convert_color_space< gray_pixel >
         : detail::convert_color_space_impl< gray_pixel >
     {
     };
 
     template<>
     struct convert_color_space< gray_pixel_16 >
         : detail::convert_color_space_impl< gray_pixel_16 >
     {
     };
 
     template<>
     struct convert_color_space< ga_pixel >
         : detail::convert_color_space_impl< ga_pixel >
     {
     };
 
     template<>
     struct convert_color_space< ga_pixel_16 >
         : detail::convert_color_space_impl< ga_pixel_16 >
     {
     };
 
 } // namespace png
 
 #endif // PNGPP_CONVERT_COLOR_SPACE_HPP_INCLUDED
