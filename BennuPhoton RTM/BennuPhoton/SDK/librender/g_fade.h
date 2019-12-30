/*
*  Copyright � 2006-2009 SplinterGU (Fenix/Bennugd)
*  Copyright � 2002-2006 Fenix Team (Fenix)
*  Copyright � 1999-2002 Jos� Luis Cebri�n Pag�e (Fenix)
*
*  This file is part of Bennu - Game Development
*
*  Bennu is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  Bennu is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*
*/

#ifndef __FADE_H
#define __FADE_H

#ifdef TARGET_MAC
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

extern int fade_on ;               /* �Hay un fade activo?                  */
extern int fade_set ;              /* �Hay un fade seteado pero inactivo?   */
extern int fade_step ;             /* Si lo hay, posici�n (0=off)           */

extern SDL_Color vpalette[ 256 ] ;

extern void gr_fade_init( int pr, int pg, int pb, int speed ) ;
extern void gr_fade_step() ;

#endif
