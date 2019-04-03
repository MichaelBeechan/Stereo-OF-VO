 #ifndef PNGPP_INFO_BASE_HPP_INCLUDED
      #define PNGPP_INFO_BASE_HPP_INCLUDED
      
      #include <cassert>
      #include "error.hpp"
      #include "types.hpp"
      
      namespace png
      {
      
          class io_base;
      
          class info_base
          {
              info_base(info_base const&);
              info_base& operator=(info_base const&);
      
          public:
              info_base(io_base& io, png_struct* png)
                  : m_io(io),
                    m_png(png),
                    m_info(png_create_info_struct(m_png))
              {
              }
      
              png_info* get_png_info() const
              {
                  return m_info;
              }
      
              png_info** get_png_info_ptr()
              {
                  return & m_info;
              }
      
          protected:
              io_base& m_io;
              png_struct* m_png;
              png_info* m_info;
          };
      
      } // namespace png
      
      #endif // PNGPP_INFO_BASE_HPP_INCLUDED