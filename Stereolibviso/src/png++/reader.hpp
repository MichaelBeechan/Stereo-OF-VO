#ifndef PNGPP_READER_HPP_INCLUDED
      #define PNGPP_READER_HPP_INCLUDED
      
      #include <cassert>
      #include "io_base.hpp"
      
      namespace png
      {
      
          template< class istream >
          class reader
              : public io_base
          {
          public:
              explicit reader(istream& stream)
                  : io_base(png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                   static_cast< io_base* >(this),
                                                   raise_error,
                                                   0 ))
              {
                  png_set_read_fn(m_png, & stream, read_data);
              }
      
              ~reader()
              {
                  png_destroy_read_struct(& m_png,
                                          m_info.get_png_info_ptr(),
                                          m_end_info.get_png_info_ptr());
              }
      
              void read_png()
              {
                  if (setjmp(m_png->jmpbuf))
                  {
                      throw error(m_error);
                  }
                  png_read_png(m_png,
                               m_info.get_png_info(),
                               /* transforms = */ 0 ,
                               /* params = */ 0 );
              }
      
              void read_info()
              {
                  if (setjmp(m_png->jmpbuf))
                  {
                      throw error(m_error);
                  }
                  m_info.read();
              }
      
              void read_row(byte* bytes)
              {
                  if (setjmp(m_png->jmpbuf))
                  {
                      throw error(m_error);
                  }
                  png_read_row(m_png, bytes, 0 );
              }
      
              void read_end_info()
              {
                  if (setjmp(m_png->jmpbuf))
                  {
                      throw error(m_error);
                  }
                  m_end_info.read();
              }
      
              void update_info()
              {
                  m_info.update();
              }
      
          private:
              static void read_data(png_struct* png, byte* data, size_t length)
              {
                  io_base* io = static_cast< io_base* >(png_get_error_ptr(png));
                  reader* rd = static_cast< reader* >(io);
                  rd->reset_error();
                  istream* stream = reinterpret_cast< istream* >(png_get_io_ptr(png));
                  try
                  {
                      stream->read(reinterpret_cast< char* >(data), length);
                      if (!stream->good())
                      {
                          rd->set_error("istream::read() failed");
                      }
                  }
                  catch (std::exception const& error)
                  {
                      rd->set_error(error.what());
                  }
                  catch (...)
                  {
                      assert(!"read_data: caught something wrong");
                      rd->set_error("read_data: caught something wrong");
                  }
                  if (rd->is_error())
                  {
                      rd->raise_error();
                  }
              }
          };
      
      } // namespace png
      
      #endif // PNGPP_READER_HPP_INCLUDED