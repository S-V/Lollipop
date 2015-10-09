/*
 * This source file is part of the CrunchyBytes Game Development Kit.
 *
 * For the latest information, see http://n00body.squarespace.com/
 *
 * Copyright (c) 2011 CrunchyBytes
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#pragma once


/** Generates integer IDs at compile time by hashing strings.

    Based on:
    http://www.humus.name/index.php?page=News&ID=296

    @remarks This class exploits two language details to get optimized out of
            the compiled code. Firstly, the use of array references lets the 
            compiler know the string's existence and size. Secondly, the lack
            of the @c explicit keyword allows a string literal to be converted 
            directly to a StringId.
            
    @author Joshua Ols <crunchy.bytes.blog@gmail.com>
    @date 2011-05-28
 */


class StringId
{
public:
    //StringId();      
    //StringId(const StringId& copy);
    //~StringId();
    //StringId& operator =(const StringId& rhs);

    //@{
    /** @name StringId conversion constructors
        
        @remarks These should be evaluated at compile time, since the input is
                constant and the constructors have no side-effects.
     */
    StringId(const char (&str)[2]);
    StringId(const char (&str)[3]);
    StringId(const char (&str)[4]);
    StringId(const char (&str)[5]);
    StringId(const char (&str)[6]);
    StringId(const char (&str)[7]);
    StringId(const char (&str)[8]);
    StringId(const char (&str)[9]);
    StringId(const char (&str)[10]);
    StringId(const char (&str)[11]);
    StringId(const char (&str)[12]);
    StringId(const char (&str)[13]);
    StringId(const char (&str)[14]);
    StringId(const char (&str)[15]);
    StringId(const char (&str)[16]);
    StringId(const char (&str)[17]);
    StringId(const char (&str)[18]);
    StringId(const char (&str)[19]);
    StringId(const char (&str)[20]);
    StringId(const char (&str)[21]);
	StringId(const char (&str)[22]);
	StringId(const char (&str)[23]);
	StringId(const char (&str)[24]);
	StringId(const char (&str)[25]);
	StringId(const char (&str)[26]);
	StringId(const char (&str)[27]);
	StringId(const char (&str)[28]);
	StringId(const char (&str)[29]);
	StringId(const char (&str)[30]);
    //@}
    
    unsigned int GetHash() const;

private:
    unsigned int m_hash;
};

FORCEINLINE
StringId::StringId(
    const char (&str)[2])
{
    m_hash = str[0];
} 

FORCEINLINE
StringId::StringId(
    const char (&str)[3])
{
    unsigned int hash = str[0];
    hash = (hash * 65599u) + str[1];
    m_hash = hash;
} 

FORCEINLINE
StringId::StringId(
    const char (&str)[4])
{
    unsigned int hash = str[0];
    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    m_hash = hash;
}

FORCEINLINE
StringId::StringId(
    const char (&str)[5])
{
    unsigned int hash = str[0];
    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    m_hash = hash;
}

FORCEINLINE
StringId::StringId(
    const char (&str)[6])
{
    unsigned int hash = str[0];
    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    m_hash = hash;
} 

FORCEINLINE
StringId::StringId(
    const char (&str)[7])
{
    unsigned int hash = str[0];
    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    m_hash = hash;
} 

FORCEINLINE
StringId::StringId(
    const char (&str)[8])
{
    unsigned int hash = str[0];
    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    m_hash = hash;
} 

FORCEINLINE
StringId::StringId(
    const char (&str)[9])
{
    unsigned int hash = str[0];
    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    hash = (hash * 65599u) + str[7];
    m_hash = hash;
} 

FORCEINLINE
StringId::StringId(
    const char (&str)[10])
{
    unsigned int hash = str[0];
    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    hash = (hash * 65599u) + str[7];
    hash = (hash * 65599u) + str[8];
    m_hash = hash;
} 

FORCEINLINE
StringId::StringId(
    const char (&str)[11])
{
    unsigned int hash = str[0];
    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    hash = (hash * 65599u) + str[7];
    hash = (hash * 65599u) + str[8];
    hash = (hash * 65599u) + str[9];
    m_hash = hash;
} 

FORCEINLINE
StringId::StringId(
    const char (&str)[12])
{
    unsigned int hash = str[0];
    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    hash = (hash * 65599u) + str[7];
    hash = (hash * 65599u) + str[8];
    hash = (hash * 65599u) + str[9];
    hash = (hash * 65599u) + str[10];
    m_hash = hash;
} 

FORCEINLINE
StringId::StringId(
    const char (&str)[13])
{
    unsigned int hash = str[0];
    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    hash = (hash * 65599u) + str[7];
    hash = (hash * 65599u) + str[8];
    hash = (hash * 65599u) + str[9];
    hash = (hash * 65599u) + str[10];
    hash = (hash * 65599u) + str[11];
    m_hash = hash;
} 

FORCEINLINE
StringId::StringId(
    const char (&str)[14])
{
    unsigned int hash = str[0];
    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    hash = (hash * 65599u) + str[7];
    hash = (hash * 65599u) + str[8];
    hash = (hash * 65599u) + str[9];
    hash = (hash * 65599u) + str[10];
    hash = (hash * 65599u) + str[11];
    hash = (hash * 65599u) + str[12];
    m_hash = hash;
} 

FORCEINLINE
StringId::StringId(
    const char (&str)[15])
{
    unsigned int hash = str[0];
    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    hash = (hash * 65599u) + str[7];
    hash = (hash * 65599u) + str[8];
    hash = (hash * 65599u) + str[9];
    hash = (hash * 65599u) + str[10];
    hash = (hash * 65599u) + str[11];
    hash = (hash * 65599u) + str[12];
    hash = (hash * 65599u) + str[13];
    m_hash = hash;
} 

FORCEINLINE
StringId::StringId(
    const char (&str)[16])
{
    unsigned int hash = str[0];
    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    hash = (hash * 65599u) + str[7];
    hash = (hash * 65599u) + str[8];
    hash = (hash * 65599u) + str[9];
    hash = (hash * 65599u) + str[10];
    hash = (hash * 65599u) + str[11];
    hash = (hash * 65599u) + str[12];
    hash = (hash * 65599u) + str[13];
    hash = (hash * 65599u) + str[14];
    m_hash = hash;
} 

FORCEINLINE
StringId::StringId(
    const char (&str)[17])
{
    unsigned int hash = str[0];
    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    hash = (hash * 65599u) + str[7];
    hash = (hash * 65599u) + str[8];
    hash = (hash * 65599u) + str[9];
    hash = (hash * 65599u) + str[10];
    hash = (hash * 65599u) + str[11];
    hash = (hash * 65599u) + str[12];
    hash = (hash * 65599u) + str[13];
    hash = (hash * 65599u) + str[14];
    hash = (hash * 65599u) + str[15];
    m_hash = hash;
} 

FORCEINLINE
StringId::StringId(
    const char (&str)[18])
{
    unsigned int hash = str[0];
    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    hash = (hash * 65599u) + str[7];
    hash = (hash * 65599u) + str[8];
    hash = (hash * 65599u) + str[9];
    hash = (hash * 65599u) + str[10];
    hash = (hash * 65599u) + str[11];
    hash = (hash * 65599u) + str[12];
    hash = (hash * 65599u) + str[13];
    hash = (hash * 65599u) + str[14];
    hash = (hash * 65599u) + str[15];
    hash = (hash * 65599u) + str[16];
    m_hash = hash;
} 

FORCEINLINE
StringId::StringId(
    const char (&str)[19])
{
    unsigned int hash = str[0];
    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    hash = (hash * 65599u) + str[7];
    hash = (hash * 65599u) + str[8];
    hash = (hash * 65599u) + str[9];
    hash = (hash * 65599u) + str[10];
    hash = (hash * 65599u) + str[11];
    hash = (hash * 65599u) + str[12];
    hash = (hash * 65599u) + str[13];
    hash = (hash * 65599u) + str[14];
    hash = (hash * 65599u) + str[15];
    hash = (hash * 65599u) + str[16];
    hash = (hash * 65599u) + str[17];
    m_hash = hash;
} 

FORCEINLINE
StringId::StringId(
    const char (&str)[20])
{
    unsigned int hash = str[0];
    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    hash = (hash * 65599u) + str[7];
    hash = (hash * 65599u) + str[8];
    hash = (hash * 65599u) + str[9];
    hash = (hash * 65599u) + str[10];
    hash = (hash * 65599u) + str[11];
    hash = (hash * 65599u) + str[12];
    hash = (hash * 65599u) + str[13];
    hash = (hash * 65599u) + str[14];
    hash = (hash * 65599u) + str[15];
    hash = (hash * 65599u) + str[16];
    hash = (hash * 65599u) + str[17];
    hash = (hash * 65599u) + str[18];
    m_hash = hash;
} 

FORCEINLINE
StringId::StringId(
    const char (&str)[21])
{
    unsigned int hash = str[0];
    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    hash = (hash * 65599u) + str[7];
    hash = (hash * 65599u) + str[8];
    hash = (hash * 65599u) + str[9];
    hash = (hash * 65599u) + str[10];
    hash = (hash * 65599u) + str[11];
    hash = (hash * 65599u) + str[12];
    hash = (hash * 65599u) + str[13];
    hash = (hash * 65599u) + str[14];
    hash = (hash * 65599u) + str[15];
    hash = (hash * 65599u) + str[16];
    hash = (hash * 65599u) + str[17];
    hash = (hash * 65599u) + str[18];
    hash = (hash * 65599u) + str[19];
    m_hash = hash;
} 


//#define HELPER_MACRO_HASH_COMBINE_1

FORCEINLINE
StringId::StringId(
    const char (&str)[22])
{
    unsigned int hash = str[0];

    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    hash = (hash * 65599u) + str[7];
    hash = (hash * 65599u) + str[8];
    hash = (hash * 65599u) + str[9];
    hash = (hash * 65599u) + str[10];
    hash = (hash * 65599u) + str[11];
    hash = (hash * 65599u) + str[12];
    hash = (hash * 65599u) + str[13];
    hash = (hash * 65599u) + str[14];
    hash = (hash * 65599u) + str[15];
    hash = (hash * 65599u) + str[16];
    hash = (hash * 65599u) + str[17];
    hash = (hash * 65599u) + str[18];
    hash = (hash * 65599u) + str[19];
	hash = (hash * 65599u) + str[20];

    m_hash = hash;
} 


FORCEINLINE
StringId::StringId(
    const char (&str)[23])
{
    unsigned int hash = str[0];

    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    hash = (hash * 65599u) + str[7];
    hash = (hash * 65599u) + str[8];
    hash = (hash * 65599u) + str[9];
    hash = (hash * 65599u) + str[10];
    hash = (hash * 65599u) + str[11];
    hash = (hash * 65599u) + str[12];
    hash = (hash * 65599u) + str[13];
    hash = (hash * 65599u) + str[14];
    hash = (hash * 65599u) + str[15];
    hash = (hash * 65599u) + str[16];
    hash = (hash * 65599u) + str[17];
    hash = (hash * 65599u) + str[18];
    hash = (hash * 65599u) + str[19];
	hash = (hash * 65599u) + str[20];
	hash = (hash * 65599u) + str[21];

    m_hash = hash;
} 



FORCEINLINE
StringId::StringId(
    const char (&str)[24])
{
    unsigned int hash = str[0];

    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    hash = (hash * 65599u) + str[7];
    hash = (hash * 65599u) + str[8];
    hash = (hash * 65599u) + str[9];
    hash = (hash * 65599u) + str[10];
    hash = (hash * 65599u) + str[11];
    hash = (hash * 65599u) + str[12];
    hash = (hash * 65599u) + str[13];
    hash = (hash * 65599u) + str[14];
    hash = (hash * 65599u) + str[15];
    hash = (hash * 65599u) + str[16];
    hash = (hash * 65599u) + str[17];
    hash = (hash * 65599u) + str[18];
    hash = (hash * 65599u) + str[19];
	hash = (hash * 65599u) + str[20];
	hash = (hash * 65599u) + str[21];
	hash = (hash * 65599u) + str[22];

    m_hash = hash;
} 


FORCEINLINE
StringId::StringId(
    const char (&str)[25])
{
    unsigned int hash = str[0];

    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    hash = (hash * 65599u) + str[7];
    hash = (hash * 65599u) + str[8];
    hash = (hash * 65599u) + str[9];
    hash = (hash * 65599u) + str[10];
    hash = (hash * 65599u) + str[11];
    hash = (hash * 65599u) + str[12];
    hash = (hash * 65599u) + str[13];
    hash = (hash * 65599u) + str[14];
    hash = (hash * 65599u) + str[15];
    hash = (hash * 65599u) + str[16];
    hash = (hash * 65599u) + str[17];
    hash = (hash * 65599u) + str[18];
    hash = (hash * 65599u) + str[19];
	hash = (hash * 65599u) + str[20];
	hash = (hash * 65599u) + str[21];
	hash = (hash * 65599u) + str[22];
	hash = (hash * 65599u) + str[23];

    m_hash = hash;
} 

FORCEINLINE
StringId::StringId(
    const char (&str)[26])
{
    unsigned int hash = str[0];

    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    hash = (hash * 65599u) + str[7];
    hash = (hash * 65599u) + str[8];
    hash = (hash * 65599u) + str[9];
    hash = (hash * 65599u) + str[10];
    hash = (hash * 65599u) + str[11];
    hash = (hash * 65599u) + str[12];
    hash = (hash * 65599u) + str[13];
    hash = (hash * 65599u) + str[14];
    hash = (hash * 65599u) + str[15];
    hash = (hash * 65599u) + str[16];
    hash = (hash * 65599u) + str[17];
    hash = (hash * 65599u) + str[18];
    hash = (hash * 65599u) + str[19];
	hash = (hash * 65599u) + str[20];
	hash = (hash * 65599u) + str[21];
	hash = (hash * 65599u) + str[22];
	hash = (hash * 65599u) + str[23];
	hash = (hash * 65599u) + str[24];

    m_hash = hash;
} 

FORCEINLINE
StringId::StringId(
    const char (&str)[27])
{
    unsigned int hash = str[0];

    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    hash = (hash * 65599u) + str[7];
    hash = (hash * 65599u) + str[8];
    hash = (hash * 65599u) + str[9];
    hash = (hash * 65599u) + str[10];
    hash = (hash * 65599u) + str[11];
    hash = (hash * 65599u) + str[12];
    hash = (hash * 65599u) + str[13];
    hash = (hash * 65599u) + str[14];
    hash = (hash * 65599u) + str[15];
    hash = (hash * 65599u) + str[16];
    hash = (hash * 65599u) + str[17];
    hash = (hash * 65599u) + str[18];
    hash = (hash * 65599u) + str[19];
	hash = (hash * 65599u) + str[20];
	hash = (hash * 65599u) + str[21];
	hash = (hash * 65599u) + str[22];
	hash = (hash * 65599u) + str[23];
	hash = (hash * 65599u) + str[24];
	hash = (hash * 65599u) + str[25];

    m_hash = hash;
} 


FORCEINLINE
StringId::StringId(
    const char (&str)[28])
{
    unsigned int hash = str[0];

    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    hash = (hash * 65599u) + str[7];
    hash = (hash * 65599u) + str[8];
    hash = (hash * 65599u) + str[9];
    hash = (hash * 65599u) + str[10];
    hash = (hash * 65599u) + str[11];
    hash = (hash * 65599u) + str[12];
    hash = (hash * 65599u) + str[13];
    hash = (hash * 65599u) + str[14];
    hash = (hash * 65599u) + str[15];
    hash = (hash * 65599u) + str[16];
    hash = (hash * 65599u) + str[17];
    hash = (hash * 65599u) + str[18];
    hash = (hash * 65599u) + str[19];
	hash = (hash * 65599u) + str[20];
	hash = (hash * 65599u) + str[21];
	hash = (hash * 65599u) + str[22];
	hash = (hash * 65599u) + str[23];
	hash = (hash * 65599u) + str[24];
	hash = (hash * 65599u) + str[25];
	hash = (hash * 65599u) + str[26];

    m_hash = hash;
} 


FORCEINLINE
StringId::StringId(
    const char (&str)[29])
{
    unsigned int hash = str[0];

    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    hash = (hash * 65599u) + str[7];
    hash = (hash * 65599u) + str[8];
    hash = (hash * 65599u) + str[9];
    hash = (hash * 65599u) + str[10];
    hash = (hash * 65599u) + str[11];
    hash = (hash * 65599u) + str[12];
    hash = (hash * 65599u) + str[13];
    hash = (hash * 65599u) + str[14];
    hash = (hash * 65599u) + str[15];
    hash = (hash * 65599u) + str[16];
    hash = (hash * 65599u) + str[17];
    hash = (hash * 65599u) + str[18];
    hash = (hash * 65599u) + str[19];
	hash = (hash * 65599u) + str[20];
	hash = (hash * 65599u) + str[21];
	hash = (hash * 65599u) + str[22];
	hash = (hash * 65599u) + str[23];
	hash = (hash * 65599u) + str[24];
	hash = (hash * 65599u) + str[25];
	hash = (hash * 65599u) + str[26];
	hash = (hash * 65599u) + str[27];

    m_hash = hash;
} 

FORCEINLINE
StringId::StringId(
    const char (&str)[30])
{
    unsigned int hash = str[0];

    hash = (hash * 65599u) + str[1];
    hash = (hash * 65599u) + str[2];
    hash = (hash * 65599u) + str[3];
    hash = (hash * 65599u) + str[4];
    hash = (hash * 65599u) + str[5];
    hash = (hash * 65599u) + str[6];
    hash = (hash * 65599u) + str[7];
    hash = (hash * 65599u) + str[8];
    hash = (hash * 65599u) + str[9];
    hash = (hash * 65599u) + str[10];
    hash = (hash * 65599u) + str[11];
    hash = (hash * 65599u) + str[12];
    hash = (hash * 65599u) + str[13];
    hash = (hash * 65599u) + str[14];
    hash = (hash * 65599u) + str[15];
    hash = (hash * 65599u) + str[16];
    hash = (hash * 65599u) + str[17];
    hash = (hash * 65599u) + str[18];
    hash = (hash * 65599u) + str[19];
	hash = (hash * 65599u) + str[20];
	hash = (hash * 65599u) + str[21];
	hash = (hash * 65599u) + str[22];
	hash = (hash * 65599u) + str[23];
	hash = (hash * 65599u) + str[24];
	hash = (hash * 65599u) + str[25];
	hash = (hash * 65599u) + str[26];
	hash = (hash * 65599u) + str[27];
	hash = (hash * 65599u) + str[28];

    m_hash = hash;
} 

FORCEINLINE
unsigned int
StringId::GetHash() const
{
    return m_hash;
}

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
