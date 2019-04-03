#ifndef PNGPP_INDEX_PIXEL_HPP_INCLUDED
      #define PNGPP_INDEX_PIXEL_HPP_INCLUDED
      
      #include "types.hpp"
      #include "packed_pixel.hpp"
      #include "pixel_traits.hpp"
      
      namespace png
      {
      
          class index_pixel
          {
          public:
              index_pixel(byte index = 0 )
                  : m_index(index)
              {
              }
      
              operator byte() const
              {
                  return m_index;
              }
      
          private:
              byte m_index;
          };
      
          template< size_t bits >
          class packed_index_pixel
              : public packed_pixel< bits >
          {
          public:
              packed_index_pixel(byte value = 0 )
                  : packed_pixel< bits >(value)
              {
              }
          };
      
          typedef packed_index_pixel< 1 > index_pixel_1 ;
      
          typedef packed_index_pixel< 2 > index_pixel_2 ;
      
          typedef packed_index_pixel< 4 > index_pixel_4;
      
          template<>
          struct pixel_traits< index_pixel >
              : basic_pixel_traits< index_pixel, byte, color_type_palette >
          {
          };
      
          template< size_t bits >
          struct pixel_traits< packed_index_pixel< bits > >
              : basic_pixel_traits< packed_index_pixel< bits >, byte,
                                    color_type_palette, /* channels = */ 1 , bits >
          {
          };
      
      } // namespace png
      
      #endif // PNGPP_INDEX_PIXEL_HPP_INCLUDED
