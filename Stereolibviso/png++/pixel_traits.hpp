 #ifndef PNGPP_PIXEL_TRAITS_HPP_INCLUDED
 #define PNGPP_PIXEL_TRAITS_HPP_INCLUDED
 
 #include <limits>
 #include "types.hpp"
 
 namespace png
 {
 
     template< typename pixel > struct pixel_traits;
 
     template< typename pixel,
              typename component,
              color_type pixel_color_type,
               size_t channels = sizeof(pixel) / sizeof(component),
               size_t bit_depth = std::numeric_limits< component >::digits >
     struct basic_pixel_traits
    {
         typedef pixel pixel_type;
         typedef component component_type;
 
         static color_type get_color_type()
         {
             return pixel_color_type;
        }
        static size_t get_channels()
         {
             return channels;
         }
        static size_t get_bit_depth()
         {
             return bit_depth;
         }
     };
 
    template< typename component >
     struct basic_alpha_pixel_traits
    {
         static component get_alpha_filler()
         {
             return std::numeric_limits< component >::max();
         }
     };
 
 } // namespace png
 
 #endif // PNGPP_PIXEL_TRAITS_HPP_INCLUDED