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
 #ifndef PNGPP_END_INFO_HPP_INCLUDED
 #define PNGPP_END_INFO_HPP_INCLUDED
 
 #include "info_base.hpp"

 namespace png
 {

    class end_info
         : public info_base
    {
     public:
        end_info(io_base& io, png_struct* png)
            : info_base(io, png)
         {
         }

        void destroy()
         {
             assert(m_info);
             png_destroy_info_struct(m_png, & m_info);
         }
 
         void read()
         {
            png_read_end(m_png, m_info);
         }
         
         void write() const
         {
             png_write_end(m_png, m_info);
         }

         // TODO: add methods to read/write text comments etc.
    };
 
 } // namespace png
 
#endif // PNGPP_END_INFO_HPP_INCLUDED