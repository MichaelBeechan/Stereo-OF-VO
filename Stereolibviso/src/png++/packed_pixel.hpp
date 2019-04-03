 #ifndef PNGPP_PACKED_PIXEL_HPP_INCLUDED
      #define PNGPP_PACKED_PIXEL_HPP_INCLUDED
      
      #include "types.hpp"
      
      namespace png
      {
      
          namespace detail
          {
              template< size_t bits > class allowed_bit_depth;
      
              template<> class allowed_bit_depth<1> {};
              template<> class allowed_bit_depth<2> {};
              template<> class allowed_bit_depth<4> {};
          } // namespace detail
      
          template< size_t bits >
          class packed_pixel
              : detail::allowed_bit_depth< bits >
          {
          public:
              packed_pixel(byte value = 0 )
                  : m_value(value & get_bit_mask())
              {
              }
      
              operator byte() const
              {
                  return m_value;
              }
      
              static size_t const get_bit_depth()
              {
                  return bits;
              }
      
              static byte const get_bit_mask()
              {
                  return ( 1 << bits) -1 ;
              }
      
          private:
              byte m_value;
          };
      
      } // namespace png
      
      #endif // PNGPP_PACKED_PIXEL_HPP_INCLUDED