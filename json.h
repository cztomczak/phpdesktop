// Modified by Czarek Tomczak <czarek.tomczak@gmail.com>
// for the PHP Desktop project (http://code.google.com/p/phpdesktop/).
// Changes: added string & wstring operators to the C++ oprator sugar.

/* vim: set et ts=3 sw=3 ft=c:
 *
 * Copyright (C) 2012 James McLaughlin et al.  All rights reserved.
 * https://github.com/udp/json-parser
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _JSON_H
#define _JSON_H

#ifndef json_char
   #define json_char char
#endif

#ifdef __cplusplus

   #include "defines.h"
   #include <string.h>
   #include <string>
   #include "string_utils.h"   

   extern "C"
   {

#endif

typedef struct
{
   unsigned long max_memory;
   int settings;

} json_settings;

#define json_relaxed_commas 1

typedef enum
{
   json_none,
   json_object,
   json_array,
   json_integer,
   json_double,
   json_string,
   json_boolean,
   json_null

} json_type;

extern const struct _json_value json_value_none;

typedef struct _json_value
{
   struct _json_value * parent;

   json_type type;

   union
   {
      int boolean;
      long integer;
      double dbl;

      struct
      {
         unsigned int length;
         json_char * ptr; /* null terminated */

      } string;

      struct
      {
         unsigned int length;

         struct
         {
            json_char * name;
            struct _json_value * value;

         } * values;

      } object;

      struct
      {
         unsigned int length;
         struct _json_value ** values;

      } array;

   } u;

   union
   {
      struct _json_value * next_alloc;
      void * object_mem;

   } _reserved;


   /* Some C++ operator sugar */

   #ifdef __cplusplus

      public:

         inline _json_value ()
         {  memset (this, 0, sizeof (_json_value));
         }

         inline const struct _json_value &operator [] (int index) const
         {
            if (type != json_array || index < 0
                     || ((unsigned int) index) >= u.array.length)
            {
               return json_value_none;
            }

            return *u.array.values [index];
         }

         inline const struct _json_value &operator [] (const char * index) const
         { 
            if (type != json_object)
               return json_value_none;

            for (unsigned int i = 0; i < u.object.length; ++ i)
               if (!strcmp (u.object.values [i].name, index))
                  return *u.object.values [i].value;

            return json_value_none;
         }

         inline operator const char * () const
         {  
            switch (type)
            {
               case json_string:
                  return u.string.ptr;

               default:
                  return "";
            };
         }

         inline operator const std::string ()
         {
            switch (type)
            {
               case json_string:
                   return u.string.ptr;

               default:
                   return "";
            }
         }

         inline operator std::wstring () const
         {
            std::string string;
            switch (type)
            {
               case json_string:
                  string = u.string.ptr;
                  break;
            }
            return Utf8ToWide(string);
         }

         inline operator long () const
         {  
            switch (type)
            {
               case json_integer:
                  return u.integer;

               case json_double:
                  return (long) u.dbl;

               default:
                  return 0;
            };
         }

         inline operator bool () const
         {  
            if (type != json_boolean)
               return false;

            return u.boolean != 0;
         }

         inline operator double () const
         {  
            switch (type)
            {
               case json_integer:
                  return u.integer;

               case json_double:
                  return u.dbl;

               default:
                  return 0;
            };
         }

   #endif

} json_value;

json_value * json_parse
   (const json_char * json);

json_value * json_parse_ex
   (json_settings * settings, const json_char * json, char * error);

void json_value_free (json_value *);


#ifdef __cplusplus
   } /* extern "C" */
#endif

#endif


