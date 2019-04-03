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
  */
 #ifndef PNGPP_IMAGE_HPP_INCLUDED
 #define PNGPP_IMAGE_HPP_INCLUDED
 
#include <fstream>
#include "pixel_buffer.hpp"
 #include "generator.hpp"
 #include "consumer.hpp"
 #include "convert_color_space.hpp"
 
 namespace png
 {
 
     template< typename pixel >
     class image
     {
    public:
         typedef pixel_traits< pixel > traits;
 
        typedef pixel_buffer< pixel > pixbuf;
 
         typedef typename pixbuf::row_type row_type;
 
         typedef convert_color_space< pixel > transform_convert;
 
        struct transform_identity
         {
             void operator()(io_base&) const {}
         };
         image()
             : m_info(make_image_info< pixel >())
         {
         }
 
         image(size_t width, size_t height)
             : m_info(make_image_info< pixel >())
         {
             resize(width, height);
         }
 
         explicit image(std::string const& filename)
         {
             read(filename, transform_convert());
        }

         template< class transformation >
         image(std::string const& filename,
               transformation const& transform)
         {
             read(filename.c_str(), transform);
         }
 
         explicit image(char const* filename)
         {
             read(filename, transform_convert());
         }
 
         template< class transformation >
         image(char const* filename, transformation const& transform)
         {
             read(filename, transform);
         }
 
         explicit image(std::istream& stream)
        {
             read_stream(stream, transform_convert());
         }
 
        template< class transformation >
         image(std::istream& stream, transformation const& transform)
         {
             read_stream(stream, transform);
        }
 
         void read(std::string const& filename)
         {
             read(filename, transform_convert());
         }
 
         template< class transformation >
         void read(std::string const& filename, transformation const& transform)
         {
             read(filename.c_str(), transform);
         }
 
         void read(char const* filename)
         {
             read(filename, transform_convert());
         }
 
         template< class transformation >
         void read(char const* filename, transformation const& transform)
         {
             std::ifstream stream(filename, std::ios::binary);
             if (!stream.is_open())
             {
                 throw std_error(filename);
            }
             stream.exceptions(std::ios::badbit);
             read_stream(stream, transform);
         }
 
         void read(std::istream& stream)
         {
             read_stream(stream, transform_convert());
         }

         template< class transformation >
         void read(std::istream& stream, transformation const& transform)
         {
             read_stream(stream, transform);
         }
 
         template< class istream >
         void read_stream(istream& stream)
         {
             read_stream(stream, transform_convert());
         }
 
         template< class istream, class transformation >
        void read_stream(istream& stream, transformation const& transform)
         {
             pixel_consumer pixcon(m_info, m_pixbuf);
            pixcon.read(stream, transform);
         }
 
         void write(std::string const& filename)
         {
             write(filename.c_str());
        }
 
         void write(char const* filename)
         {
             std::ofstream stream(filename, std::ios::binary);
             if (!stream.is_open())
             {
                 throw std_error(filename);
             }
             stream.exceptions(std::ios::badbit);
             write_stream(stream);
         }
 
         void write_stream(std::ostream& stream)
         {
             write_stream(stream);
         }
 
         template< class ostream >
         void write_stream(ostream& stream)
         {
             pixel_generator pixgen(m_info, m_pixbuf);
             pixgen.write(stream);
         }
 
         pixbuf& get_pixbuf()
         {
             return m_pixbuf;
         }
         
         pixbuf const& get_pixbuf() const
         {
            return m_pixbuf;
         }
 
         void set_pixbuf(pixbuf const& buffer)
         {
             m_pixbuf = buffer;
         }
 
         size_t get_width() const
         {
             return m_pixbuf.get_width();
         }
         size_t get_height() const
         {
             return m_pixbuf.get_height();
         }

         void resize(size_t width, size_t height)
        {
            m_pixbuf.resize(width, height);
            m_info.set_width(width);
             m_info.set_height(height);
         }
 
        row_type& get_row(size_t index)
         {
             return m_pixbuf.get_row(index);
         }
         
         row_type const& get_row(size_t index) const
         {
             return m_pixbuf.get_row(index);
         }
 
         row_type& operator[](size_t index)
         {
             return m_pixbuf[index];
         }
         
         row_type const& operator[](size_t index) const
         {
            return m_pixbuf[index];
         }
 
         pixel get_pixel(size_t x, size_t y) const
         {
             return m_pixbuf.get_pixel(x, y);
         }
 
         void set_pixel(size_t x, size_t y, pixel p)
         {
             m_pixbuf.set_pixel(x, y, p);
        }
 
         interlace_type get_interlace_type() const
         {
             return m_info.get_interlace_type();
         }

         void set_interlace_type(interlace_type interlace)
         {
             m_info.set_interlace_type(interlace);
         }
 
         compression_type get_compression_type() const
         {
             return m_info.get_compression_type();
         }
 
         void set_compression_type(compression_type compression)
         {
             m_info.set_compression_type(compression);
         }
 
         filter_type get_filter_type() const
         {
            return m_info.get_filter_type();
         }
 
         void set_filter_type(filter_type filter)
         {
             m_info.set_filter_type(filter);
        }
 
         palette& get_palette()
         {
             return m_info.get_palette();
         }
 
         palette const& get_palette() const
         {
             return m_info.get_palette();
         }
 
         void set_palette(palette const& plte)
         {
             m_info.set_palette(plte);
         }
 
         tRNS const& get_tRNS() const
         {
             return m_info.get_tRNS();
         }
 
         tRNS& get_tRNS()
         {
             return m_info.get_tRNS();
         }
 
         void set_tRNS(tRNS const& trns)
         {
             m_info.set_tRNS(trns);
         }
 
     protected:
         template< typename base_impl >
         class streaming_impl
             : public base_impl
         {
         public:
             streaming_impl(image_info& info, pixbuf& pixels)
                 : base_impl(info),
                   m_pixbuf(pixels)
             {
             }
 
            byte* get_next_row(size_t pos)
            {
                 typedef typename pixbuf::row_traits row_traits;
                 return reinterpret_cast< byte* >
                     (row_traits::get_data(m_pixbuf.get_row(pos)));
             }
 
         protected:
             pixbuf& m_pixbuf;
         };
 
        class pixel_consumer
             : public streaming_impl< consumer< pixel,
                                                pixel_consumer,
                                                image_info_ref_holder,
                                                /* interlacing = */ true > >
         {
         public:
             pixel_consumer(image_info& info, pixbuf& pixels)
                : streaming_impl< consumer< pixel,
                                            pixel_consumer,
                                             image_info_ref_holder,
                                             true > >(info, pixels)
             {
             }
 
             void reset(size_t pass)
             {
                 if (pass == 0)
                 {
                     this->m_pixbuf.resize(this->get_info().get_width(),
                                           this->get_info().get_height());
                 }
             }
        };
 
         class pixel_generator
             : public streaming_impl< generator< pixel,
                                                 pixel_generator,
                                                 image_info_ref_holder,
                                                 /* interlacing = */ true > >
        {
         public:
            pixel_generator(image_info& info, pixbuf& pixels)
                 : streaming_impl< generator< pixel,
                                              pixel_generator,
                                              image_info_ref_holder,
                                              true > >(info, pixels)
            {
             }
         };
 
         image_info m_info;
         pixbuf m_pixbuf;
     };

 } // namespace png
 
 #endif // PNGPP_IMAGE_HPP_INCLUDED