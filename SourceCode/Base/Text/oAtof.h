/**************************************************************************
 * The MIT License                                                        *
 * Copyright (c) 2011 Antony Arciuolo & Kevin Myers                       *
 *                                                                        *
 * Permission is hereby granted, free of charge, to any person obtaining  *
 * a copy of this software and associated documentation files (the        *
 * "Software"), to deal in the Software without restriction, including    *
 * without limitation the rights to use, copy, modify, merge, publish,    *
 * distribute, sublicense, and/or sell copies of the Software, and to     *
 * permit persons to whom the Software is furnished to do so, subject to  *
 * the following conditions:                                              *
 *                                                                        *
 * The above copyright notice and this permission notice shall be         *
 * included in all copies or substantial portions of the Software.        *
 *                                                                        *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        *
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                  *
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE *
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION *
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION  *
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.        *
 **************************************************************************/

// A fast, but limited atof implementation that's reportedly
// 5x faster than the standard atof in simple cases.
#pragma once
#ifndef oAtof_h
#define oAtof_h

/** <citation
	usage="Implementation" 
	reason="In some file I/O cases atof isn't fast as it could be"
	author="Marcin Sokalski"
	description="http://stackoverflow.com/questions/98586/where-can-i-find-the-worlds-fastest-atof-implementation"
	license="*** Assumed Public Domain ***"
	licenseurl="http://stackoverflow.com/questions/98586/where-can-i-find-the-worlds-fastest-atof-implementation"
	modification="wchar_t -> char, renamed for lib consistency"
/>*/

// $(CitedCodeBegin)
oFORCEINLINE bool oAtof(const char* wcs, float* val)
// (C)2009 Marcin Sokalski gumix@ghnet.pl - All rights reserved.
{
int hdr=0;
while (wcs[hdr]==L' ')
        hdr++;

int cur=hdr;

bool negative=false;
bool has_sign=false;

if (wcs[cur]==L'+' || wcs[cur]==L'-')
{
        if (wcs[cur]==L'-')
                negative=true;
        has_sign=true;
        cur++;
}
else
        has_sign=false;

int quot_digs=0;
int frac_digs=0;

bool full=false;

char period=0;
int binexp=0;
int decexp=0;
unsigned long value=0;

while (wcs[cur]>=L'0' && wcs[cur]<=L'9')
{
        if (!full)
        {
                if (value>=0x19999999 && wcs[cur]-L'0'>5 ||     value>0x19999999)
                {
                        full=true;
                        decexp++;
                }
                else
                        value=value*10+wcs[cur]-L'0';
        }
        else
                decexp++;

        quot_digs++;
        cur++;
}

if (wcs[cur]==L'.' || wcs[cur]==L',')
{
        period=wcs[cur];
        cur++;

        while (wcs[cur]>=L'0' && wcs[cur]<=L'9')
        {
                if (!full)
                {
                        if (value>=0x19999999 && wcs[cur]-L'0'>5 ||     value>0x19999999)
                                full=true;
                        else
                        {
                                decexp--;
                                value=value*10+wcs[cur]-L'0';
                        }
                }

                frac_digs++;
                cur++;
        }
}

if (!quot_digs && !frac_digs)
        return false;

char exp_char=0;

int decexp2=0; // explicit exponent
bool exp_negative=false;
bool has_expsign=false;
int exp_digs=0;

// even if value is 0, we still need to eat exponent chars
if (wcs[cur]==L'e' || wcs[cur]==L'E')
{
        exp_char=wcs[cur];
        cur++;

        if (wcs[cur]==L'+' || wcs[cur]==L'-')
        {
                has_expsign=true;
                if (wcs[cur]=='-')
                        exp_negative=true;
                cur++;
        }

        while (wcs[cur]>=L'0' && wcs[cur]<=L'9')
        {
                if (decexp2>=0x19999999)
                        return false;
                decexp2=10*decexp2+wcs[cur]-L'0';
                exp_digs++;
                cur++;
        }

        if (exp_negative)
                decexp-=decexp2;
        else
                decexp+=decexp2;
}

// end of wcs scan, cur contains value's tail

if (value)
{
        while (value<=0x19999999)
        {
                decexp--;
                value=value*10;
        }

        if (decexp)
        {
                // ensure 1bit space for mul by something lower than 2.0
                if (value&0x80000000)
                {
                        value>>=1;
                        binexp++;
                }

                if (decexp>308 || decexp<-307)
                        return false;

                // convert exp from 10 to 2 (using FPU)
                int E;
                double v=pow(10.0,decexp);
                double m=frexp(v,&E);
                m=2.0*m;
                E--;
                value=(unsigned long)floor(value*m);

                binexp+=E;
        }

        binexp+=23; // rebase exponent to 23bits of mantisa


        // so the value is: +/- VALUE * pow(2,BINEXP);
        // (normalize manthisa to 24bits, update exponent)
        while (value&0xFE000000)
        {
                value>>=1;
                binexp++;
        }
        if (value&0x01000000)
        {
                if (value&1)
                        value++;
                value>>=1;
                binexp++;
                if (value&0x01000000)
                {
                        value>>=1;
                        binexp++;
                }
        }

        while (!(value&0x00800000))
        {
                value<<=1;
                binexp--;
        }

        if (binexp<-127)
        {
                // underflow
                value=0;
                binexp=-127;
        }
        else
        if (binexp>128)
                return false;

        //exclude "implicit 1"
        value&=0x007FFFFF;

        // encode exponent
        unsigned long exponent=(binexp+127)<<23;
        value |= exponent;
}

// encode sign
unsigned long sign=negative<<31;
value |= sign;

if (val)
{
        *(unsigned long*)val=value;
}

return true;
}

// $(CitedCodeEnd)
#endif
