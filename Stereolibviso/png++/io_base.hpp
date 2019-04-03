#ifndef PNGPP_IO_BASE_HPP_INCLUDED
      #define PNGPP_IO_BASE_HPP_INCLUDED
      
      #include <cassert>
      #include <cstdio>
      #include "error.hpp"
      #include "info.hpp"
      #include "end_info.hpp"
      
      namespace png
      {
      
          class io_base
          {
              io_base(io_base const&);
              io_base& operator=(io_base const&);
      
          public:
              explicit io_base(png_struct* png)
                  : m_png(png),
                    m_info(*this, m_png),
                    m_end_info(*this, m_png)
              {
              }
      
              ~io_base()
              {
                  assert(! m_png);
                  assert(! m_info.get_png_info());
                  assert(! m_end_info.get_png_info());
              }
      
              png_struct* get_png_struct() const
              {
                  return m_png;
              }
      
              info& get_info()
              {
                  return m_info;
              }
      
              info const& get_info() const
              {
                  return m_info;
              }
      
              image_info const& get_image_info() const
              {
                  return m_info;
              }
      
              void set_image_info(image_info const& info)
              {
                  static_cast< image_info& >(m_info) = info; // slice it
              }
      
              end_info& get_end_info()
              {
                  return m_end_info;
              }
      
              end_info const& get_end_info() const
              {
                  return m_end_info;
              }
      
              // info accessors
              //
              size_t get_width() const
              {
                  return m_info.get_width();
              }
      
              void set_width(size_t width)
              {
                  m_info.set_width(width);
              }
      
              size_t get_height() const
              {
                  return m_info.get_height();
              }
      
              void set_height(size_t height)
              {
                  m_info.set_height(height);
              }
      
              color_type get_color_type() const
              {
                  return m_info.get_color_type();
              }
      
              void set_color_type(color_type color_space)
              {
                  m_info.set_color_type(color_space);
              }
      
              size_t get_bit_depth() const
              {
                  return m_info.get_bit_depth();
              }
      
              void set_bit_depth(size_t bit_depth)
              {
                  m_info.set_bit_depth(bit_depth);
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
      
      
              bool has_chunk(chunk id)
              {
                  return png_get_valid(m_png,
                                       m_info.get_png_info(),
                                       uint_32(id)) == uint_32(id);
              }
      
      #if defined(PNG_READ_EXPAND_SUPPORTED)
              void set_gray_1_2_4_to_8() const
              {
                  png_set_gray_1_2_4_to_8(m_png);
              }
      
              void set_palette_to_rgb() const
              {
                  png_set_palette_to_rgb(m_png);
              }
      
              void set_tRNS_to_alpha() const
              {
                  png_set_tRNS_to_alpha(m_png);
              }
      #endif // defined(PNG_READ_EXPAND_SUPPORTED)
      
      #if defined(PNG_READ_BGR_SUPPORTED) || defined(PNG_WRITE_BGR_SUPPORTED)
              void set_bgr() const
              {
                  png_set_bgr(m_png);
              }
      #endif
      
      #if defined(PNG_READ_GRAY_TO_RGB_SUPPORTED)
              void set_gray_to_rgb() const
              {
                  png_set_gray_to_rgb(m_png);
              }
      #endif
      
      #ifdef PNG_FLOATING_POINT_SUPPORTED
              void set_rgb_to_gray(rgb_to_gray_error_action error_action
                                   = rgb_to_gray_silent,
                                   double red_weight   = - 1.0 ,
                                   double green_weight = - 1.0 ) const
              {
                  png_set_rgb_to_gray(m_png, error_action, red_weight, green_weight);
              }
      #else
              void set_rgb_to_gray(rgb_to_gray_error_action error_action
                                   = rgb_to_gray_silent,
                                   fixed_point red_weight   = -1 ,
                                   fixed_point green_weight = -1 ) const
              {
                  png_set_rgb_to_gray_fixed(m_png, error_action,
                                            red_weight, green_weight);
              }
      #endif // PNG_FLOATING_POINT_SUPPORTED
      
              // alpha channel transformations
              //
      #if defined(PNG_READ_STRIP_ALPHA_SUPPORTED)
              void set_strip_alpha() const
              {
                  png_set_strip_alpha(m_png);
              }
      #endif
      
      #if defined(PNG_READ_SWAP_ALPHA_SUPPORTED) \
          || defined(PNG_WRITE_SWAP_ALPHA_SUPPORTED)
              void set_swap_alpha() const
              {
                  png_set_swap_alpha(m_png);
              }
      #endif
      
      #if defined(PNG_READ_INVERT_ALPHA_SUPPORTED) \
          || defined(PNG_WRITE_INVERT_ALPHA_SUPPORTED)
              void set_invert_alpha() const
              {
                  png_set_invert_alpha(m_png);
              }
      #endif
      
      #if defined(PNG_READ_FILLER_SUPPORTED) || defined(PNG_WRITE_FILLER_SUPPORTED)
              void set_filler(uint_32  filler, filler_type type) const
              {
                  png_set_filler(m_png, filler, type);
              }
      
      #if !defined(PNG_1_0_X)
              void set_add_alpha(uint_32   filler, filler_type type) const
              {
                  png_set_add_alpha(m_png, filler, type);
              }
      #endif
      #endif // PNG_READ_FILLER_SUPPORTED || PNG_WRITE_FILLER_SUPPORTED
      
      #if defined(PNG_READ_SWAP_SUPPORTED) || defined(PNG_WRITE_SWAP_SUPPORTED)
              void set_swap() const
              {
                  png_set_swap(m_png);
              }
      #endif
      
      #if defined(PNG_READ_PACK_SUPPORTED) || defined(PNG_WRITE_PACK_SUPPORTED)
              void set_packing() const
              {
                  png_set_packing(m_png);
              }
      #endif
      
      #if defined(PNG_READ_PACKSWAP_SUPPORTED) \
          || defined(PNG_WRITE_PACKSWAP_SUPPORTED)
              void set_packswap() const
              {
                  png_set_packswap(m_png);
              }
      #endif
      
      #if defined(PNG_READ_SHIFT_SUPPORTED) || defined(PNG_WRITE_SHIFT_SUPPORTED)
              void set_shift(byte red_bits, byte green_bits, byte blue_bits,
                             byte alpha_bits = 0 ) const
              {
                  if (get_color_type() != color_type_rgb
                      || get_color_type() != color_type_rgb_alpha)
                  {
                      throw error("set_shift: expected RGB or RGBA color type");
                  }
                  color_info bits;
                  bits.red = red_bits;
                  bits.green = green_bits;
                  bits.blue = blue_bits;
                  bits.alpha = alpha_bits;
                  png_set_shift(m_png, & bits);
              }
      
              void set_shift(byte gray_bits, byte alpha_bits = 0 ) const
              {
                  if (get_color_type() != color_type_gray
                      || get_color_type() != color_type_gray_alpha)
                  {
                      throw error("set_shift: expected Gray or Gray+Alpha"
                                  " color type");
                  }
                  color_info bits;
                  bits.gray = gray_bits;
                  bits.alpha = alpha_bits;
                  png_set_shift(m_png, & bits);
              }
      #endif // PNG_READ_SHIFT_SUPPORTED || PNG_WRITE_SHIFT_SUPPORTED
      
      #if defined(PNG_READ_INTERLACING_SUPPORTED) \
          || defined(PNG_WRITE_INTERLACING_SUPPORTED)
              int set_interlace_handling() const
              {
                  return png_set_interlace_handling(m_png);
              }
      #endif
      
      #if defined(PNG_READ_INVERT_SUPPORTED) || defined(PNG_WRITE_INVERT_SUPPORTED)
              void set_invert_mono() const
              {
                  png_set_invert_mono(m_png);
              }
      #endif
      
      #if defined(PNG_READ_16_TO_8_SUPPORTED)
              void set_strip_16() const
              {
                  png_set_strip_16(m_png);
              }
      #endif
      
      #if defined(PNG_READ_USER_TRANSFORM_SUPPORTED)
              void set_read_user_transform(png_user_transform_ptr transform_fn)
              {
                  png_set_read_user_transform_fn(m_png, transform_fn);
              }
      #endif
      
      #if defined(PNG_READ_USER_TRANSFORM_SUPPORTED) \
          || defined(PNG_WRITE_USER_TRANSFORM_SUPPORTED)
              void set_user_transform_info(void* info, int bit_depth, int channels)
              {
                  png_set_user_transform_info(m_png, info, bit_depth, channels);
              }
      #endif
      
          protected:
              void* get_io_ptr() const
              {
                  return png_get_io_ptr(m_png);
              }
      
              void set_error(char const* message)
              {
                  assert(message);
                  m_error = message;
              }
      
              void reset_error()
              {
                  m_error.clear();
              }
      
      /*
              std::string const& get_error() const
              {
                  return m_error;
              }
      */
      
              bool is_error() const
              {
                  return !m_error.empty();
              }
      
              void raise_error()
              {
                  longjmp(m_png->jmpbuf, -1 );
              }
      
              static void raise_error(png_struct* png, char const* message)
              {
                  io_base* io = static_cast< io_base* >(png_get_error_ptr(png));
                  io->set_error(message);
                  longjmp(png->jmpbuf, - 1);
              }
      
              png_struct* m_png;
              info m_info;
              end_info m_end_info;
              std::string m_error;
          };
      
      } // namespace png
      
      #endif // PNGPP_IO_BASE_HPP_INCLUDED
