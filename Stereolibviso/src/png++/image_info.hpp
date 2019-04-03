#ifndef PNGPP_IMAGE_INFO_HPP_INCLUDED
      #define PNGPP_IMAGE_INFO_HPP_INCLUDED
      
      #include "types.hpp"
      #include "palette.hpp"
      #include "tRNS.hpp"
      #include "pixel_traits.hpp"
      
      namespace png
      {
      
          class image_info
          {
          public:
              image_info()
                  : m_width(0 ),
                    m_height(0 ),
                    m_bit_depth( 0),
                    m_color_type(color_type_none),
                    m_interlace_type(interlace_none),
                    m_compression_type(compression_type_default),
                    m_filter_type(filter_type_default)
              {
              }
      
              size_t get_width() const
              {
                  return m_width;
              }
      
              void set_width(size_t width)
              {
                  m_width = width;
              }
      
              size_t get_height() const
              {
                  return m_height;
              }
      
              void set_height(size_t height)
              {
                  m_height = height;
              }
      
              color_type get_color_type() const
              {
                  return m_color_type;
              }
      
              void set_color_type(color_type color_space)
              {
                  m_color_type = color_space;
              }
      
              size_t get_bit_depth() const
              {
                  return m_bit_depth;
              }
      
              void set_bit_depth(size_t bit_depth)
              {
                  m_bit_depth = bit_depth;
              }
      
              interlace_type get_interlace_type() const
              {
                  return m_interlace_type;
              }
      
              void set_interlace_type(interlace_type interlace)
              {
                  m_interlace_type = interlace;
              }
      
              compression_type get_compression_type() const
              {
                  return m_compression_type;
              }
      
              void set_compression_type(compression_type compression)
              {
                  m_compression_type = compression;
              }
      
              filter_type get_filter_type() const
              {
                  return m_filter_type;
              }
      
              void set_filter_type(filter_type filter)
              {
                  m_filter_type = filter;
              }
      
              palette const& get_palette() const
              {
                  return m_palette;
              }
      
              palette& get_palette()
              {
                  return m_palette;
              }
      
              void set_palette(palette const& plte)
              {
                  m_palette = plte;
              }
      
              void drop_palette()
              {
                  m_palette.clear();
              }
      
              tRNS const& get_tRNS() const
              {
                  return m_tRNS;
              }
      
              tRNS& get_tRNS()
              {
                  return m_tRNS;
              }
      
              void set_tRNS(tRNS const& trns)
              {
                  m_tRNS = trns;
              }
      
          protected:
              uint_32   m_width;
              uint_32   m_height;
              size_t m_bit_depth;
              color_type m_color_type;
              interlace_type m_interlace_type;
              compression_type m_compression_type;
              filter_type m_filter_type;
              palette m_palette;
              tRNS m_tRNS;
          };
      
          template< typename pixel >
          image_info
          make_image_info()
          {
              typedef pixel_traits< pixel > traits;
              image_info info;
              info.set_color_type(traits::get_color_type());
              info.set_bit_depth(traits::get_bit_depth());
              return info;
          }
      
      } // namespace png
      
      #endif // PNGPP_IMAGE_INFO_HPP_INCLUDED