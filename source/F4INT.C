/* *********************************************************************************************** */
/* Copyright (C) 1999-2015 by Sequiter, Inc., 9644-54 Ave, NW, Suite 209, Edmonton, Alberta Canada.*/
/* This program is free software: you can redistribute it and/or modify it under the terms of      */
/* the GNU Lesser General Public License as published by the Free Software Foundation, version     */
/* 3 of the License.                                                                               */
/*                                                                                                 */
/* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;       */
/* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.       */
/* See the GNU Lesser General Public License for more details.                                     */
/*                                                                                                 */
/* You should have received a copy of the GNU Lesser General Public License along with this        */
/* program. If not, see <https://www.gnu.org/licenses/>.                                           */
/* *********************************************************************************************** */

/* f4int.c (c)Copyright Sequiter Software Inc., 1988-2001.  All rights reserved. */

#include "d4all.h"
#ifndef S4UNIX
   #ifdef __TURBOC__
      #pragma hdrstop
   #endif
#endif

#ifndef S4OFF_WRITE
void S4FUNCTION f4assignInt( FIELD4 *field, const int iValue )
{
   CODE4 *c4 ;
   #ifdef S4DATA_ALIGN  /* LY 00/11/07 : changed from S4WINCE*/
      char *buffPtr ;
   #endif
   #ifdef S4BYTE_SWAP   /* LY 2001/06/19 */
      int tempInt ;
   #endif

   #ifdef E4VBASIC
      if ( c4parm_check( field, 3, E90514 ) )
         return ;
   #endif

   #ifdef E4PARM_HIGH
      if ( field == 0 )
      {
         error4( 0, e4parm_null, E90514 ) ;
         return ;
      }
      switch( field->type )
      {
         case r4date:
         case r4log:
         case r4memo:
         case r4gen:
         #ifdef S4CLIENT_OR_FOX
            case r4dateTime:
            case r4dateTimeMilli:  // AS Mar 10/03 - ms support in datetime
            case r4system:
            case r4memoBin:
         #endif
            error4( field->data->codeBase, e4parm, E81409 ) ;
            return ;
         default:
            break ;
      }
   #endif

   #ifdef E4ANALYZE
      if ( field->data == 0 )
      {
         error4( 0, e4struct, E90514 ) ;
         return ;
      }
      if ( field->data->codeBase == 0 )
      {
         error4( 0, e4struct, E90514 ) ;
         return ;
      }
   #endif

   c4 = field->data->codeBase ;

   if ( error4code( c4 ) < 0 )
      return ;

   #ifndef S4SERVER
      #ifndef S4OFF_ENFORCE_LOCK
         if ( c4->lockEnforce && field->data->recNum > 0L )
            if ( d4lockTest( field->data, field->data->recNum, lock4write ) != 1 )
            {
               error4( c4, e4lock, E90514 ) ;
               return ;
            }
      #endif
   #endif

   switch( field->type )
   {
      case r5ui2: // treat same as int
      case r5i2:  // treat same as int
         // LY Jun 29/04 : added S4DATA_ALIGN
         #ifdef S4DATA_ALIGN
         {
            short tempShort = iValue ;
            memcpy( f4assignPtr( field ), &tempShort, sizeof( short ) ) ;
         }
         #elif defined( S4BYTE_SWAP )  // LY Aug 23/04
            short tempShort = (short)iValue ;
            *((short *)f4assignPtr( field )) = x4reverseShort( &tempShort ) ;
         #else
            *((short *)f4assignPtr( field )) = iValue ;
         #endif
         break ;
      case r5ui4:  // treat same as int
      case r4int:
         #ifdef S4BYTE_SWAP
            #ifdef S4DATA_ALIGN  /* LY 2001/06/19 */
               tempInt = x4reverseLong( &iValue ) ;
               memcpy( f4assignPtr( field ), &tempInt, sizeof(tempInt) ) ;
            #else
               *((S4LONG *)f4assignPtr( field )) = x4reverseLong(&iValue) ;
            #endif
         #else
            #ifdef S4DATA_ALIGN  /* LY 00/11/07 : changed from S4WINCE */
               buffPtr = (char *) f4assignPtr( field ) ;
               memcpy( buffPtr, &iValue, sizeof(int) ) ; /* LY July 14/03 : changed from sizeof(long) */
            #else
               /* CS 2000/01/27 must cast to long, not int
                  because int is only 2 bytes in 16-bit */
               *((long *)f4assignPtr( field )) = iValue ;
            #endif
         #endif
         break ;
      #ifdef S4CLIENT_OR_FOX
         case r4currency:
         case r4double:
            f4assignDouble( field, (double)iValue ) ;
            break ;
         // AS Jul 21/05 - Support for new field type binary float
         case r4floatBin:
            f4assignFloat( field, (float)iValue ) ;
            break ;
      #endif
      default:
         if ( field->dec == 0 )
            c4ltoa45( (long)iValue, f4assignPtr( field ), field->len ) ;
         else
            f4assignDouble( field, (double)iValue ) ;
         break ;
   }
}
#endif

int S4FUNCTION f4int( const FIELD4 *field )
{
   #ifdef S4CLIENT_OR_FOX
      const char *ptr ;
   #endif
   #ifdef S4BYTE_SWAP
      S4LONG rcLong ;
   #endif
   #ifdef E4VBASIC
      if ( c4parm_check( field, 3, E90515 ) )
         return -1 ;
   #endif

   #ifdef E4PARM_HIGH
      if ( field == 0 )
      {
         error4( 0, e4parmNull, E90515 ) ;
         return -1 ;
      }
      switch( field->type )
      {
         case r4date:
         case r4log:
         case r4memo:
         case r4gen:
         #ifdef S4CLIENT_OR_FOX
            case r4dateTime:
            case r4dateTimeMilli:  // AS Mar 10/03 - ms support in datetime
            case r4system:
            case r4memoBin:
         #endif
            error4( field->data->codeBase, e4parm, E81409 ) ;
            return -1 ;
         default:
            break ;
      }
   #endif

   if ( error4code( field->data->codeBase ) < 0 )
      return -1 ;

   /* Convert the field data into an 'int' */
   switch( field->type )
   {
      case r5ui2: // treat same as int
      case r5i2:  // treat same as int
         // LY Jun 30/04 : added S4DATA_ALIGN
         #ifdef S4DATA_ALIGN
         {
            short tempShort ;
            memcpy( (char *)&tempShort, f4ptr( field ), sizeof( short ) ) ;
            return tempShort ;
         }
         #elif defined( S4BYTE_SWAP )  // LY Aug 23/04
            return x4reverseShort( f4ptr( field ) ) ;
         #else
            return *((short     *)f4ptr( field )) ;
         #endif
      case r5ui4:  // treat same as int
      case r4int:
         #ifdef S4BYTE_SWAP
            rcLong = x4reverseLong(f4ptr(field)) ;
            return (int)rcLong ;
         #else
            #ifdef S4DATA_ALIGN  /* LY 00/07/24 : for S4WINCE*/
               int retInt ;
               memcpy( &retInt, f4ptr( field ), sizeof(int) ) ;
               return retInt ;
            #else
               return *((int *)f4ptr( field )) ;
            #endif
         #endif
      #ifdef S4CLIENT_OR_FOX
         // AS Jul 21/05 - Support for new field type binary float
         case r4floatBin:
            #ifdef S4DATA_ALIGN  /* LY 00/07/24 : for S4WINCE*/
               float retFloat ;
               memcpy( &retFloat, f4ptr( field ), sizeof(float) ) ;
               #ifdef S4BYTE_SWAP  /* LY 2001/07/27 */
                  return (int) x4reverseFloat( &retFloat ) ;
               #else
                  return (int) retFloat ;
               #endif
            #else
               #ifdef S4BYTE_SWAP  /* LY 2002/08/11 */
                  return (int) x4reverseFloat( (float *)f4ptr( field ) ) ;
               #else
                  return (int)(*((float *)f4ptr( field ))) ;
               #endif
            #endif
         case r4double:
            #ifdef S4DATA_ALIGN  /* LY 00/07/24 : for S4WINCE*/
               double retDbl ;
               memcpy( &retDbl, f4ptr( field ), sizeof(double) ) ;
               #ifdef S4BYTE_SWAP  /* LY 2001/07/27 */
                  return (int) x4reverseDouble( &retDbl ) ;
               #else
                  return (int) retDbl ;
               #endif
            #else
               #ifdef S4BYTE_SWAP  /* LY 2002/08/11 */
                  return (int) x4reverseDouble( (double *)f4ptr( field ) ) ;
               #else
                  return (int)(*((double *)f4ptr( field ))) ;
               #endif
            #endif
         case r4currency:
            ptr = f4currency( field, 0 ) ;
            return c4atoi( ptr, c4strlen( ptr ) ) ;
      #endif
      default:
         return c4atoi( f4ptr( field ), field->len ) ;
   }
}
