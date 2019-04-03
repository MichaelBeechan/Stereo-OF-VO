#ifndef PNGPP_PIXEL_BUFFER_HPP_INCLUDED
      #define PNGPP_PIXEL_BUFFER_HPP_INCLUDED
      
      #include <cassert>
      #include <cstddef>
      #include <stdexcept>
      #include <vector>
      
      #include "packed_pixel.hpp"
      #include "gray_pixel.hpp"
      #include "index_pixel.hpp"
      
      namespace png
      {
      
          template< typename row > class row_traits;
      
          template< typename pixel,
                    typename row,
                    class traits = row_traits< row > >
          class basic_pixel_buffer
          {
          public:
              typedef row row_type;
              typedef traits row_traits;
      
              basic_pixel_buffer()
                  : m_width(0 ),
                    m_height(0 )
              {
              }
      
              basic_pixel_buffer(size_t width, size_t height)
                  : m_width(0),
                    m_height(0)
              {
                  resize(width, height);
              }
      
              size_t get_width() const
              {
                  return m_width;
              }
      
              size_t get_height() const
                  {
                  return m_height;
                  }
      
              void resize(size_t width, size_t height)
              {
                  m_width = width;
                  m_height = height;
                  m_rows.resize(height);
                  for (typename row_vec::iterator r = m_rows.begin();
                       r != m_rows.end();
                       ++r)
                  {
                      r->resize(width);
                  }
              }
      
              row_type& get_row(size_t index)
              {
                  return m_rows.at(index);
              }
      
              row_type const& get_row(size_t index) const
              {
                  return m_rows.at(index);
              }
      
              row_type& operator[](size_t index)
              {
                  return m_rows[index];
              }
      
              row_type const& operator[](size_t index) const
              {
                  return m_rows[index];
              }
      
              void put_row(size_t index, row_type const& r)
              {
                  assert(r.size() == m_width);
                  m_rows.at(index) = r;
              }
      
              pixel get_pixel(size_t x, size_t y) const
              {
                  return get_row(y).at(x);
              }
      
              void set_pixel(size_t x, size_t y, pixel p)
              {
                  get_row(y).at(x) = p;
              }
      
          protected:
              size_t m_width;
              size_t m_height;
              typedef std::vector< row_type > row_vec;
              row_vec m_rows;
          };
      
          template< typename pixel >
          class row_traits< std::vector< pixel > >
          {
          public:
              static pixel* get_data(std::vector< pixel >& vec)
              {
                  assert(vec.size());
                  return & vec[0 ];
              }
          };
      
          template< typename pixel >
          class pixel_buffer
              : public basic_pixel_buffer< pixel, std::vector< pixel > >
          {
          public:
              pixel_buffer()
              {
              }
      
              pixel_buffer(size_t width, size_t height)
                  : basic_pixel_buffer< pixel, std::vector< pixel > >(width, height)
              {
              }
          };
      
          namespace detail
          {
      
              template< class pixel, typename reference >
              class basic_packed_pixel_proxy
              {
              public:
                  explicit basic_packed_pixel_proxy(reference ref)
                      : m_ref(ref),
                        m_shift( )
                  {
                  }
      
                  basic_packed_pixel_proxy(reference ref, size_t index)
                      : m_ref(ref),
                        m_shift(get_shift(index))
                  {
                  }
      
                  operator pixel() const
                  {
                      return pixel((m_ref >> m_shift) & pixel::get_bit_mask());
                  }
      
              protected:
                  /*
                   * bits: . .   .
                   * bits: . .   .            
*    1: 7 6 5 4 3 2 1 0             
*    2:   6   4   2              
*    4:       4       0
             
                   */
                  static size_t get_shift(size_t index)
                  {
                      size_t const bits = pixel::get_bit_depth();
                      return ( 8 - bits) - (index % get_pixels_per_byte()) * bits;
                  }
      
                  static size_t get_pixels_per_byte()
                  {
                      return  8 / pixel::get_bit_depth();
                  }
      
                  reference m_ref;
                  size_t m_shift;
              };
      
              template< class pixel >
              class const_packed_pixel_proxy
                  : public basic_packed_pixel_proxy< pixel, byte const& >
              {
              public:
                  const_packed_pixel_proxy(byte const& ref, size_t index)
                      : basic_packed_pixel_proxy< pixel, byte const& >(ref, index)
                  {
                  }
              };
      
              template< class pixel >
              class packed_pixel_proxy
                  : public basic_packed_pixel_proxy< pixel, byte& >
              {
              public:
                  typedef basic_packed_pixel_proxy< pixel, byte& > basic_proxy;
      
                  packed_pixel_proxy(byte& ref, size_t index)
                      : basic_proxy(ref, index)
                  {
                  }
      
                  packed_pixel_proxy(packed_pixel_proxy const& other)
                      : basic_proxy(other.m_ref)
                  {
                      this->m_shift = other.m_shift;
                  }
      
                  packed_pixel_proxy& operator=(packed_pixel_proxy const& other)
                  {
                      return *this = static_cast< pixel >(other);
                  }
      
                  template< typename reference >
                  packed_pixel_proxy&
                  operator=(basic_packed_pixel_proxy< pixel, reference > const& other)
                  {
                      return *this = static_cast< pixel >(other);
                  }
      
                  packed_pixel_proxy& operator=(pixel p)
                  {
                      this->m_ref = this->m_ref
                          & ~(pixel::get_bit_mask() << this->m_shift)
                          | (p << this->m_shift);
      
                      return *this;
                  }
              };
      
          } // namespace detail
      
          template< class pixel >
          class packed_pixel_row
          {
          public:
              explicit packed_pixel_row(size_t size = 0 )
              {
                  resize(size);
              }
      
              size_t size() const
              {
                  return m_size;
              }
      
              void resize(size_t size)
              {
                  m_vec.resize(size / get_pixels_per_byte()
                               + (size % get_pixels_per_byte() ? 1 :  0));
                  m_size = size;
              }
      
              typedef detail::const_packed_pixel_proxy< pixel > const_pixel_proxy;
      
              typedef detail::packed_pixel_proxy< pixel > pixel_proxy;
      
              const_pixel_proxy at(size_t index) const
              {
                  return const_pixel_proxy(m_vec.at(index / get_pixels_per_byte()),
                                           index);
              }
      
              pixel_proxy at(size_t index)
              {
                  return pixel_proxy(m_vec.at(index / get_pixels_per_byte()),
                                     index);
              }
      
              const_pixel_proxy operator[](size_t index) const
              {
                  return const_pixel_proxy(m_vec[index / get_pixels_per_byte()],
                                           index);
              }
      
              pixel_proxy operator[](size_t index)
              {
                  return pixel_proxy(m_vec[index / get_pixels_per_byte()],
                                     index);
              }
      
              byte* get_data()
              {
                  assert(m_vec.size());
                  return & m_vec[ 0];
              }
      
          private:
              static size_t get_pixels_per_byte()
              {
                  return  8 / pixel::get_bit_depth();
              }
      
              std::vector< byte > m_vec;
              size_t m_size;
          };
      
          template< typename pixel >
          class row_traits< packed_pixel_row< pixel > >
          {
          public:
              static byte* get_data(packed_pixel_row< pixel >& row)
              {
                  return row.get_data();
              }
          };
      
          template< size_t bits >
          class pixel_buffer< packed_gray_pixel< bits > >
              : public basic_pixel_buffer< packed_gray_pixel< bits >,
                                           packed_pixel_row< packed_gray_pixel
                                                             < bits > > >
          {
          public:
              typedef packed_gray_pixel< bits > pixel_type;
              typedef packed_pixel_row< pixel_type > pixel_row_type;
      
              pixel_buffer()
              {
              }
      
              pixel_buffer(size_t width, size_t height)
                  : basic_pixel_buffer< pixel_type,
                                        pixel_row_type >(width, height)
              {
              }
          };
      
          template< size_t bits >
          class pixel_buffer< packed_index_pixel< bits > >
              : public basic_pixel_buffer< packed_index_pixel< bits >,
                                           packed_pixel_row< packed_index_pixel
                                                             < bits > > >
          {
          public:
              typedef packed_index_pixel< bits > pixel_type;
              typedef packed_pixel_row< pixel_type > pixel_row_type;
      
              pixel_buffer()
              {
              }
      
              pixel_buffer(size_t width, size_t height)
                  : basic_pixel_buffer< pixel_type,
                                        pixel_row_type >(width, height)
              {
              }
          };
      
      } // namespace png
      
      #endif // PNGPP_PIXEL_BUFFER_HPP_INCLUDED
