/*
=============================================================================
	File:	LZ4.cpp
	Desc:	LZ4 compression library.
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

/*
   LZ4 - Fast LZ compression algorithm
   Copyright (C) 2011, Yann Collet.
   BSD License


   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
  
       * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following disclaimer
   in the documentation and/or other materials provided with the
   distribution.
  
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "LZ4.h"

mxNAMESPACE_BEGIN

//**************************************
// Compilation Directives
//**************************************
#if __STDC_VERSION__ >= 199901L
  /* "restrict" is a known keyword */
#elif defined(_MSC_VER)
#define restrict  __restrict
#else
#define restrict  // Disable restrict
#endif




//**************************************
// Includes
//**************************************
#include <stdlib.h>   // for malloc
#include <string.h>   // for memset
#include "lz4.h"




//**************************************
// Performance parameter               
//**************************************
// Increasing this value improves compression ratio
// Lowering this value reduces memory usage
// Lowering may also improve speed, typically on reaching cache size limits (L1 32KB for Intel, 64KB for AMD)
// Memory usage formula for 32 bits systems : N->2^(N+2) Bytes (examples : 17 -> 512KB ; 12 -> 16KB)
#define HASH_LOG 12




//**************************************
// Basic Types
//**************************************
#if defined(_MSC_VER)    // Visual Studio does not support 'stdint' natively
#define BYTE    unsigned __int8
#define U16             unsigned __int16
#define U32             unsigned __int32
#define S32             __int32
#else
#include <stdint.h>
#define BYTE    uint8_t
#define U16             uint16_t
#define U32             uint32_t
#define S32             int32_t
#endif




//**************************************
// Constants
//**************************************
#define MINMATCH 4
#define SKIPSTRENGTH 6
#define STACKLIMIT 13
#define HEAPMODE (HASH_LOG>STACKLIMIT)  // Defines if memory is allocated into the stack (local variable), or into the heap (malloc()).
#define COPYTOKEN 4
#define COPYLENGTH 8
#define LASTLITERALS 5
#define MFLIMIT (COPYLENGTH+MINMATCH)
#define MINLENGTH (MFLIMIT+1)


#define MAXD_LOG 16
#define MAX_DISTANCE ((1 << MAXD_LOG) - 1)


#define HASHTABLESIZE (1 << HASH_LOG)
#define HASH_MASK (HASHTABLESIZE - 1)


#define ML_BITS 4
#define ML_MASK ((1U<<ML_BITS)-1)
#define RUN_BITS (8-ML_BITS)
#define RUN_MASK ((1U<<RUN_BITS)-1)




//**************************************
// Local structures
//**************************************
struct refTables
{
        const BYTE* hashTable[HASHTABLESIZE];
};


#ifdef __GNUC__
#  define _PACKED __attribute__ ((packed))
#else
#  define _PACKED
#endif


typedef struct _U32_S
{
        U32 v;
} _PACKED U32_S;


typedef struct _U16_S
{
        U16 v;
} _PACKED U16_S;


#define A32(x) (((U32_S *)(x))->v)
#define A16(x) (((U16_S *)(x))->v)




//**************************************
// Macros
//**************************************
#define LZ4_HASH_FUNCTION(i)    (((i) * 2654435761U) >> ((MINMATCH*8)-HASH_LOG))
#define LZ4_HASH_VALUE(p)               LZ4_HASH_FUNCTION(A32(p))
#define LZ4_COPYPACKET(s,d)             A32(d) = A32(s); d+=4; s+=4; A32(d) = A32(s); d+=4; s+=4;
#define LZ4_WILDCOPY(s,d,e)             do { LZ4_COPYPACKET(s,d) } while (d<e);
#define LZ4_BLINDCOPY(s,d,l)    { BYTE* e=d+l; LZ4_WILDCOPY(s,d,e); d=e; }






//****************************
// Compression CODE
//****************************


int LZ4_compressCtx(void** ctx,
                                 char* source, 
                                 char* dest,
                                 int isize)
{       
#if HEAPMODE
        struct refTables *srt = (struct refTables *) (*ctx);
        const BYTE** HashTable;
#else
        const BYTE* HashTable[HASHTABLESIZE] = {0};
#endif


        const BYTE* ip = (BYTE*) source;       
        const BYTE* anchor = ip;
        const BYTE* const iend = ip + isize;
        const BYTE* const mflimit = iend - MFLIMIT;
#define matchlimit (iend - LASTLITERALS)


        BYTE* op = (BYTE*) dest;
        
        const size_t DeBruijnBytePos[32] = { 0, 0, 3, 0, 3, 1, 3, 0, 3, 2, 2, 1, 3, 2, 0, 1, 3, 3, 1, 2, 2, 2, 2, 0, 3, 1, 2, 0, 1, 0, 1, 1 };
        int len, length;
        const int skipStrength = SKIPSTRENGTH;
        U32 forwardH;




        // Init 
        if (isize<MINLENGTH) goto _last_literals;
#if HEAPMODE
        if (*ctx == NULL) 
        {
                srt = (struct refTables *) malloc ( sizeof(struct refTables) );
                *ctx = (void*) srt;
        }
        HashTable = srt->hashTable;
        memset((void*)HashTable, 0, sizeof(srt->hashTable));
#else
        (void) ctx;
#endif




        // First Byte
        HashTable[LZ4_HASH_VALUE(ip)] = ip;
        ip++; forwardH = LZ4_HASH_VALUE(ip);
        
        // Main Loop
    for ( ; ; ) 
        {
                int findMatchAttempts = (1U << skipStrength) + 3;
                const BYTE* forwardIp = ip;
                const BYTE* ref;
                BYTE* token;


                // Find a match
                do {
                        U32 h = forwardH;
                        int step = findMatchAttempts++ >> skipStrength;
                        ip = forwardIp;
                        forwardIp = ip + step;


                        if (forwardIp > mflimit) { goto _last_literals; }


                        forwardH = LZ4_HASH_VALUE(forwardIp);
                        ref = HashTable[h];
                        HashTable[h] = ip;


                } while ((ref < ip - MAX_DISTANCE) || (A32(ref) != A32(ip)));


                // Catch up
                while ((ip>anchor) && (ref>(BYTE*)source) && (ip[-1]==ref[-1])) { ip--; ref--; }  


                // Encode Literal length
                length = ip - anchor;
                token = op++;
                if (length>=(int)RUN_MASK) { *token=(RUN_MASK<<ML_BITS); len = length-RUN_MASK; for(; len > 254 ; len-=255) *op++ = 255; *op++ = (BYTE)len; } 
                else *token = (length<<ML_BITS);


                // Copy Literals
                LZ4_BLINDCOPY(anchor, op, length);




_next_match:
                // Encode Offset
                A16(op) = (ip-ref); op+=2;


                // Start Counting
                ip+=MINMATCH; ref+=MINMATCH;   // MinMatch verified
                anchor = ip;
                while (ip<matchlimit-3)
                {
                        int diff = A32(ref) ^ A32(ip);
                        if (!diff) { ip+=4; ref+=4; continue; }
                        ip += DeBruijnBytePos[((U32)((diff & -diff) * 0x077CB531U)) >> 27];
                        goto _endCount;
                }
                if ((ip<(matchlimit-1)) && (A16(ref) == A16(ip))) { ip+=2; ref+=2; }
                if ((ip<matchlimit) && (*ref == *ip)) ip++;
_endCount:
                len = (ip - anchor);
                
                // Encode MatchLength
                if (len>=(int)ML_MASK) { *token+=ML_MASK; len-=ML_MASK; for(; len > 509 ; len-=510) { *op++ = 255; *op++ = 255; } if (len > 254) { len-=255; *op++ = 255; } *op++ = (BYTE)len; } 
                else *token += len;     


                // Test end of chunk
                if (ip > mflimit) { anchor = ip;  break; }


                // Fill table
                HashTable[LZ4_HASH_VALUE(ip-2)] = ip-2;


                // Test next position
                ref = HashTable[LZ4_HASH_VALUE(ip)];
                HashTable[LZ4_HASH_VALUE(ip)] = ip;
                if ((ref > ip - (MAX_DISTANCE + 1)) && (A32(ref) == A32(ip))) { token = op++; *token=0; goto _next_match; }


                // Prepare next loop
                anchor = ip++; 
                forwardH = LZ4_HASH_VALUE(ip);
        }


_last_literals:
        // Encode Last Literals
        {
                int lastRun = iend - anchor;
                if (lastRun>=(int)RUN_MASK) { *op++=(RUN_MASK<<ML_BITS); lastRun-=RUN_MASK; for(; lastRun > 254 ; lastRun-=255) *op++ = 255; *op++ = (BYTE) lastRun; } 
                else *op++ = (lastRun<<ML_BITS);
                memcpy(op, anchor, iend - anchor);
                op += iend-anchor;
        } 


        // End
        return (int) (((char*)op)-dest);
}






// Note : this function is valid only if isize < LZ4_64KLIMIT
#define LZ4_64KLIMIT ((1U<<16) + (MFLIMIT-1))
#define HASHLOG64K (HASH_LOG+1)
#define LZ4_HASH64K_FUNCTION(i) (((i) * 2654435761U) >> ((MINMATCH*8)-HASHLOG64K))
#define LZ4_HASH64K_VALUE(p)    LZ4_HASH64K_FUNCTION(A32(p))
int LZ4_compress64kCtx(void** ctx,
                                 char* source, 
                                 char* dest,
                                 int isize)
{       
#if HEAPMODE
        struct refTables *srt = (struct refTables *) (*ctx);
        U16* HashTable;
#else
        U16 HashTable[HASHTABLESIZE<<1] = {0};
#endif


        const BYTE* ip = (BYTE*) source;       
        const BYTE* anchor = ip;
        const BYTE* const base = ip;
        const BYTE* const iend = ip + isize;
        const BYTE* const mflimit = iend - MFLIMIT;
#define matchlimit (iend - LASTLITERALS)


        BYTE* op = (BYTE*) dest;
        
        const size_t DeBruijnBytePos[32] = { 0, 0, 3, 0, 3, 1, 3, 0, 3, 2, 2, 1, 3, 2, 0, 1, 3, 3, 1, 2, 2, 2, 2, 0, 3, 1, 2, 0, 1, 0, 1, 1 };
        int len, length;
        const int skipStrength = SKIPSTRENGTH;
        U32 forwardH;




        // Init 
        if (isize<MINLENGTH) goto _last_literals;
#if HEAPMODE
        if (*ctx == NULL) 
        {
                srt = (struct refTables *) malloc ( sizeof(struct refTables) );
                *ctx = (void*) srt;
        }
        HashTable = (U16*)(srt->hashTable);
        memset((void*)HashTable, 0, sizeof(srt->hashTable));
#else
        (void) ctx;
#endif




        // First Byte
        ip++; forwardH = LZ4_HASH64K_VALUE(ip);
        
        // Main Loop
    for ( ; ; ) 
        {
                int findMatchAttempts = (1U << skipStrength) + 3;
                const BYTE* forwardIp = ip;
                const BYTE* ref;
                BYTE* token;


                // Find a match
                do {
                        U32 h = forwardH;
                        int step = findMatchAttempts++ >> skipStrength;
                        ip = forwardIp;
                        forwardIp = ip + step;


                        if (forwardIp > mflimit) { goto _last_literals; }


                        forwardH = LZ4_HASH64K_VALUE(forwardIp);
                        ref = base + HashTable[h];
                        HashTable[h] = ip - base;


                } while (A32(ref) != A32(ip));


                // Catch up
                while ((ip>anchor) && (ref>(BYTE*)source) && (ip[-1]==ref[-1])) { ip--; ref--; }  


                // Encode Literal length
                length = ip - anchor;
                token = op++;
                if (length>=(int)RUN_MASK) { *token=(RUN_MASK<<ML_BITS); len = length-RUN_MASK; for(; len > 254 ; len-=255) *op++ = 255; *op++ = (BYTE)len; } 
                else *token = (length<<ML_BITS);


                // Copy Literals
                LZ4_BLINDCOPY(anchor, op, length);




_next_match:
                // Encode Offset
                A16(op) = (ip-ref); op+=2;


                // Start Counting
                ip+=MINMATCH; ref+=MINMATCH;   // MinMatch verified
                anchor = ip;
                while (ip<matchlimit-3)
                {
                        int diff = A32(ref) ^ A32(ip);
                        if (!diff) { ip+=4; ref+=4; continue; }
                        ip += DeBruijnBytePos[((U32)((diff & -diff) * 0x077CB531U)) >> 27];
                        goto _endCount;
                }
                if ((ip<(matchlimit-1)) && (A16(ref) == A16(ip))) { ip+=2; ref+=2; }
                if ((ip<matchlimit) && (*ref == *ip)) ip++;
_endCount:
                len = (ip - anchor);
                
                // Encode MatchLength
                if (len>=(int)ML_MASK) { *token+=ML_MASK; len-=ML_MASK; for(; len > 509 ; len-=510) { *op++ = 255; *op++ = 255; } if (len > 254) { len-=255; *op++ = 255; } *op++ = (BYTE)len; } 
                else *token += len;     


                // Test end of chunk
                if (ip > mflimit) { anchor = ip;  break; }


                // Test next position
                ref = base + HashTable[LZ4_HASH64K_VALUE(ip)];
                HashTable[LZ4_HASH64K_VALUE(ip)] = ip - base;
                if (A32(ref) == A32(ip)) { token = op++; *token=0; goto _next_match; }


                // Prepare next loop
                anchor = ip++; 
                forwardH = LZ4_HASH64K_VALUE(ip);
        }


_last_literals:
        // Encode Last Literals
        {
                int lastRun = iend - anchor;
                if (lastRun>=(int)RUN_MASK) { *op++=(RUN_MASK<<ML_BITS); lastRun-=RUN_MASK; for(; lastRun > 254 ; lastRun-=255) *op++ = 255; *op++ = (BYTE) lastRun; } 
                else *op++ = (lastRun<<ML_BITS);
                memcpy(op, anchor, iend - anchor);
                op += iend-anchor;
        } 


        // End
        return (int) (((char*)op)-dest);
}






int LZ4_compress(char* source, 
                                 char* dest,
                                 int isize)
{
#if HEAPMODE
        void* ctx = malloc(sizeof(struct refTables));
        int result;
        if (isize < LZ4_64KLIMIT)
                result = LZ4_compress64kCtx(&ctx, source, dest, isize);
        else result = LZ4_compressCtx(&ctx, source, dest, isize);
        free(ctx);
        return result;
#else
        if (isize < (int)LZ4_64KLIMIT) return LZ4_compress64kCtx(NULL, source, dest, isize);
        return LZ4_compressCtx(NULL, source, dest, isize);
#endif
}








//****************************
// Decompression CODE
//****************************


// Note : The decoding functions LZ4_uncompress() and LZ4_uncompress_unknownOutputSize() 
//              are safe against "buffer overflow" attack type
//              since they will *never* write outside of the provided output buffer :
//              they both check this condition *before* writing anything.
//              A corrupted packet however can make them *read* within the first 64K before the output buffer.


int LZ4_uncompress(char* source, 
                                 char* dest,
                                 int osize)
{       
        // Local Variables
        const BYTE* restrict ip = (const BYTE*) source;
        const BYTE* restrict ref;


        BYTE* restrict op = (BYTE*) dest;
        BYTE* const oend = op + osize;
        BYTE* cpy;


        BYTE token;
        
        U32     dec[4]={0, 3, 2, 3};
        int     len, length;




        // Main Loop
        while (1)
        {
                // get runlength
                token = *ip++;
                if ((length=(token>>ML_BITS)) == RUN_MASK)  { for (;(len=*ip++)==255;length+=255){} length += len; } 


                // copy literals
                cpy = op+length;
                if (cpy>oend-COPYLENGTH) 
                { 
                        if (cpy > oend) goto _output_error;
                        memcpy(op, ip, length);
                        ip += length;
                        break;    // Necessarily EOF
                }
                LZ4_WILDCOPY(ip, op, cpy); ip -= (op-cpy); op = cpy;




                // get offset
                ref = cpy - A16(ip); ip+=2;


                // get matchlength
                if ((length=(token&ML_MASK)) == ML_MASK) { for (;*ip==255;length+=255) {ip++;} length += *ip++; } 


                // copy repeated sequence
                if (op-ref<COPYTOKEN)
                {
                        *op++ = *ref++;
                        *op++ = *ref++;
                        *op++ = *ref++;
                        *op++ = *ref++;
                        ref -= dec[op-ref];
                        A32(op)=A32(ref); 
                } else { A32(op)=A32(ref); op+=4; ref+=4; }
                cpy = op + length;
                if (cpy > oend-COPYLENGTH)
                {
                        if (cpy > oend) goto _output_error;     
                        LZ4_WILDCOPY(ref, op, (oend-COPYLENGTH));
                        while(op<cpy) *op++=*ref++;
                        op=cpy;
                        if (op == oend) break;    // Check EOF (should never happen, since last 5 bytes are supposed to be literals)
                        continue;
                }
                LZ4_WILDCOPY(ref, op, cpy);
                op=cpy;         // correction
        }


        // end of decoding
        return (int) (((char*)ip)-source);


        // write overflow error detected
_output_error:
        return (int) (-(((char*)ip)-source));
}




int LZ4_uncompress_unknownOutputSize(
                                char* source, 
                                char* dest,
                                int isize,
                                int maxOutputSize)
{       
        // Local Variables
        const BYTE* restrict ip = (const BYTE*) source;
        const BYTE* const iend = ip + isize;
        const BYTE* restrict ref;


        BYTE* restrict op = (BYTE*) dest;
        BYTE* const oend = op + maxOutputSize;
        BYTE* cpy;


        BYTE token;
        
        U32     dec[4]={0, 3, 2, 3};
        int     len, length;




        // Main Loop
        while (ip<iend)
        {
                // get runlength
                token = *ip++;
                if ((length=(token>>ML_BITS)) == RUN_MASK)  { for (;(len=*ip++)==255;length+=255){} length += len; } 


                // copy literals
                cpy = op+length;
                if (cpy>oend-COPYLENGTH) 
                { 
                        if (cpy > oend) goto _output_error;
                        memcpy(op, ip, length);
                        op += length;
                        break;    // Necessarily EOF
                }
                LZ4_WILDCOPY(ip, op, cpy); ip -= (op-cpy); op = cpy;
                if (ip>=iend) break;    // check EOF




                // get offset
                ref = cpy - A16(ip); ip+=2;


                // get matchlength
                if ((length=(token&ML_MASK)) == ML_MASK) { for (;(len=*ip++)==255;length+=255){} length += len; }


                // copy repeated sequence
                if (op-ref<COPYTOKEN)
                {
                        *op++ = *ref++;
                        *op++ = *ref++;
                        *op++ = *ref++;
                        *op++ = *ref++;
                        ref -= dec[op-ref];
                        A32(op)=A32(ref); 
                } else { A32(op)=A32(ref); op+=4; ref+=4; }
                cpy = op + length;
                if (cpy>oend-COPYLENGTH)
                {
                        if (cpy > oend) goto _output_error;     
                        LZ4_WILDCOPY(ref, op, (oend-COPYLENGTH));
                        while(op<cpy) *op++=*ref++;
                        op=cpy;
                        if (op == oend) break;    // Check EOF (should never happen, since last 5 bytes are supposed to be literals)
                        continue;
                }
                LZ4_WILDCOPY(ref, op, cpy);
                op=cpy;         // correction
        }


        // end of decoding
        return (int) (((char*)op)-dest);


        // write overflow error detected
_output_error:
        return (int) (-(((char*)ip)-source));
}

mxNAMESPACE_END




















#if 0
/*
        Note : this is *only* a demo program, an example to show how LZ4 can be used.
        It is not considered part of LZ4 compression library.
        The license of the demo program is GPL.
        The license of LZ4 is BSD.
*/


//****************************
// Includes
//****************************
#include <stdio.h>              // fprintf, fopen, fread, _fileno(?)
#include <stdlib.h>             // malloc
#include <string.h>             // strcmp
#include <time.h>               // clock
#ifdef _WIN32 
#include <io.h>                 // _setmode
#include <fcntl.h>              // _O_BINARY
#endif
#include "lz4.h"




//**************************************
// Basic Types
//**************************************






//****************************
// Constants
//****************************
#define COMPRESSOR_NAME "Compression CLI using LZ4 algorithm"
#define COMPRESSOR_VERSION ""
#define COMPILED __DATE__
#define AUTHOR "Yann Collet"
#define BINARY_NAME "lz4demo.exe"
#define EXTENSION ".lz4"
#define WELCOME_MESSAGE "*** %s %s, by %s (%s) ***\n", COMPRESSOR_NAME, COMPRESSOR_VERSION, AUTHOR, COMPILED


#define CHUNKSIZE (8<<20)    // 8 MB
#define CACHELINE 64
#define OUT_CHUNKSIZE (CHUNKSIZE + (CHUNKSIZE/255) + CACHELINE)
#define ARCHIVE_MAGICNUMBER 0x184C2102
#define ARCHIVE_MAGICNUMBER_SIZE 4




//**************************************
// MACRO
//**************************************
#define DISPLAY(...) fprintf(stderr, __VA_ARGS__)






//****************************
// Functions
//****************************
int usage()
{
        DISPLAY( "Usage :\n");
        DISPLAY( "      %s [arg] input output\n",BINARY_NAME);
        DISPLAY( "Arguments :\n");
        DISPLAY( " -c : compression (default)\n");
        DISPLAY( " -d : decompression \n");
        DISPLAY( " -t : test compressed file \n");
        DISPLAY( " -h : help (this text)\n");   
        DISPLAY( "input  : can be 'stdin' (pipe) or a filename\n");
        DISPLAY( "output : can be 'stdout'(pipe) or a filename or 'null'\n");
        return 0;
}




int badusage()
{
        DISPLAY("Wrong parameters\n");
        usage();
        return 0;
}






int get_fileHandle(char* input_filename, char* output_filename, FILE** pfinput, FILE** pfoutput)
{
        char stdinmark[] = "stdin";
        char stdoutmark[] = "stdout";


        if (!strcmp (input_filename, stdinmark)) {
                DISPLAY( "Using stdin for input\n");
                *pfinput = stdin;
#ifdef _WIN32 // Need to set stdin/stdout to binary mode specifically for windows
                _setmode( _fileno( stdin ), _O_BINARY );
#endif
        } else {
                *pfinput = fopen( input_filename, "rb" );
        }


        if (!strcmp (output_filename, stdoutmark)) {
                DISPLAY( "Using stdout for output\n");
                *pfoutput = stdout;
#ifdef _WIN32 // Need to set stdin/stdout to binary mode specifically for windows
                _setmode( _fileno( stdout ), _O_BINARY );
#endif
        } else {
                *pfoutput = fopen( output_filename, "wb" );
        }
        
        if ( *pfinput==0 ) { DISPLAY( "Pb opening %s\n", input_filename);  return 2; }
        if ( *pfoutput==0) { DISPLAY( "Pb opening %s\n", output_filename); return 3; }


        return 0;
}






int compress_file(char* input_filename, char* output_filename)
{
        unsigned long long filesize = 0;
        unsigned long long compressedfilesize = ARCHIVE_MAGICNUMBER_SIZE;
        char* in_buff;
        char* out_buff;
        FILE* finput;
        FILE* foutput;
        int r;
        clock_t start, end;




        // Init
        start = clock();
        r = get_fileHandle(input_filename, output_filename, &finput, &foutput);
        if (r) return r;
        
        // Allocate Memory
        in_buff = (char*)malloc(CHUNKSIZE);
        out_buff = (char*)malloc(OUT_CHUNKSIZE);
        if (!in_buff || !out_buff) { DISPLAY("Allocation error : not enough memory\n"); return 8; }
        
        // Write Archive Header
        *(unsigned long*)out_buff = ARCHIVE_MAGICNUMBER;
        fwrite(out_buff, 1, ARCHIVE_MAGICNUMBER_SIZE, foutput);


        // Main Loop
        while (1) 
        {       
                int outSize;
                // Read Block
            int inSize = fread(in_buff, 1, CHUNKSIZE, finput);
                if( inSize<=0 ) break;
                filesize += inSize;


                // Compress Block
                outSize = LZ4_compress(in_buff, out_buff+4, inSize);
                * (unsigned int*) out_buff = outSize;
                compressedfilesize += outSize+4;


                // Write Block
                fwrite(out_buff, 1, outSize+4, foutput);
        }


        // Status
        end = clock();
        DISPLAY( "Compressed %llu bytes into %llu bytes ==> %.2f%%\n", 
                (unsigned long long) filesize, (unsigned long long) compressedfilesize, (double)compressedfilesize/filesize*100);
        {
                double seconds = (double)(end - start)/CLOCKS_PER_SEC;
                DISPLAY( "Done in %.2f s ==> %.2f MB/s\n", seconds, (double)filesize / seconds / 1024 / 1024);
        }


        // Close & Free
        free(in_buff);
        free(out_buff);
        fclose(finput);
        fclose(foutput);


        return 0;
}




int decode_file(char* input_filename, char* output_filename)
{
        unsigned long long filesize = 0;
        char* in_buff;
        char* out_buff;
        size_t uselessRet;
        int sinkint;
        unsigned int nextSize;
        FILE* finput;
        FILE* foutput;
        clock_t start, end;
        int r;




        // Init
        start = clock();
        r = get_fileHandle(input_filename, output_filename, &finput, &foutput);
        if (r) return r;


        // Allocate Memory
        in_buff = (char*)malloc(OUT_CHUNKSIZE);
        out_buff = (char*)malloc(CHUNKSIZE);
        if (!in_buff || !out_buff) { DISPLAY("Allocation error : not enough memory\n"); return 7; }
        
        // Check Archive Header
        uselessRet = fread(out_buff, 1, ARCHIVE_MAGICNUMBER_SIZE, finput);
        if (*(unsigned long*)out_buff != ARCHIVE_MAGICNUMBER) { DISPLAY("Unrecognized header : file cannot be decoded\n"); return 6; }
        uselessRet = fread(in_buff, 1, 4, finput);
        nextSize = *(unsigned long*)in_buff;


        // Main Loop
        while (1) 
        {       
                // Read Block
            uselessRet = fread(in_buff, 1, nextSize, finput);


                // Check Next Block
                uselessRet = (unsigned long) fread(&nextSize, 1, 4, finput);
                if( uselessRet==0 ) break;


                // Decode Block
                sinkint = LZ4_uncompress(in_buff, out_buff, CHUNKSIZE);
                filesize += CHUNKSIZE;


                // Write Block
                fwrite(out_buff, 1, CHUNKSIZE, foutput);
        }


        // Last Block
    uselessRet = fread(in_buff, 1, nextSize, finput);
        sinkint = LZ4_uncompress_unknownOutputSize(in_buff, out_buff, nextSize, CHUNKSIZE);
        filesize += sinkint;
        fwrite(out_buff, 1, sinkint, foutput);


        // Status
        end = clock();
        DISPLAY( "Successfully decoded %llu bytes \n", (unsigned long long)filesize);
        {
                double seconds = (double)(end - start)/CLOCKS_PER_SEC;
                DISPLAY( "Done in %.2f s ==> %.2f MB/s\n", seconds, (double)filesize / seconds / 1024 / 1024);
        }


        // Close & Free
        free(in_buff);
        free(out_buff);
        fclose(finput);
        fclose(foutput);


        return 0;
}




int main(int argc, char** argv) 
{
  int i,
          compression=1,   // default action if no argument
          decode=0;
  char* input_filename=0;
  char* output_filename=0;
#ifdef _WIN32 
  char nulmark[] = "nul";
#else
  char nulmark[] = "/dev/null";
#endif
  char nullinput[] = "null";


  // Welcome message
  DISPLAY( WELCOME_MESSAGE);


  if (argc<2) { badusage(); return 1; }


  for(i=1; i<argc; i++)
  {
    char* argument = argv[i];
        char command = 0;


    if(!argument) continue;   // Protection if argument empty


        if (argument[0]=='-') command++;  // valid command trigger


        // Select command
        if (command)
        {
                argument += command;
                
                // Display help on usage
                if ( argument[0] =='h' ) { usage(); return 0; }


                // Compression (default)
                if ( argument[0] =='c' ) { compression=1; continue; }


                // Decoding
                if ( argument[0] =='d' ) { decode=1; continue; }


                // Test
                if ( argument[0] =='t' ) { decode=1; output_filename=nulmark; continue; }
        }


        // first provided filename is input
    if (!input_filename) { input_filename=argument; continue; }


        // second provided filename is output
    if (!output_filename) 
        { 
                output_filename=argument; 
                if (!strcmp (output_filename, nullinput)) output_filename = nulmark;
                continue; 
        }
  }


  // No input filename ==> Error
  if(!input_filename) { badusage(); return 1; }


  // No output filename 
  if (!output_filename) { badusage(); return 1; }


  if (decode) return decode_file(input_filename, output_filename);


  if (compression) return compress_file(input_filename, output_filename);


  badusage();


  return 0;
}
#endif

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
