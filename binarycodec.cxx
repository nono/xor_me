#include <algorithm>
#include <iostream>
#include <sstream>
#include <string.h>

//#include "oox/helper/attributelist.hxx"


//#include <rtl/cipher.h>
//#include <rtl/digest.h>

namespace oox { class AttributeList; }

namespace oox {
namespace core {

// ============================================================================
/*
class CodecHelper
{
public:
    static unsigned short   getPasswordHash( const AttributeList& rAttribs, signed long nElement );

private:
                        CodecHelper();
                        ~CodecHelper();
};
*/
// ============================================================================

/** Encodes and decodes data from/to protected MS Office documents.

    Implements a simple XOR encoding/decoding algorithm used in MS Office
    versions up to MSO 95.
 */
class BinaryCodec_XOR
{
public:
    /** Enumerates codec types supported by this XOR codec implementation. */
    enum CodecType
    {
        CODEC_WORD,     /// MS Word XOR codec.
        CODEC_EXCEL     /// MS Excel XOR codec.
    };

public:
    /** Default constructor.

        Two-step construction in conjunction with the initKey() and verifyKey()
        functions allows to try to initialize with different passwords (e.g.
        built-in default password used for Excel workbook protection).
     */
    explicit            BinaryCodec_XOR( CodecType eCodecType );

                        ~BinaryCodec_XOR();

    /** Initializes the algorithm with the specified password.

        @param pnPassData
            Character array containing the password. Must be zero terminated,
            which results in a maximum length of 15 characters.
     */
    void                initKey( const unsigned char pnPassData[ 16 ] );

    /** Verifies the validity of the password using the passed key and hash.

        @precond
            The codec must be initialized with the initKey() function before
            this function can be used.

        @param nKey
            Password key value read from the file.
        @param nHash
            Password hash value read from the file.

        @return
            True = test was successful.
     */
    bool                verifyKey( unsigned short nKey, unsigned short nHash ) const;

    /** Reinitializes the codec to start a new memory block.

        Resets the internal key offset to 0.

        @precond
            The codec must be initialized with the initKey() function before
            this function can be used.
     */
    void                startBlock();

    /** Decodes a block of memory.

        @precond
            The codec must be initialized with the initKey() function before
            this function can be used.

        @param pnDestData
            Destination buffer. Will contain the decrypted data afterwards.
        @param pnSrcData
            Encrypted data block.
        @param nBytes
            Size of the passed data blocks. pnDestData and pnSrcData must be of
            this size.

        @return
            True = decoding was successful (no error occured).
    */
    bool                decode(
                            unsigned char* pnDestData,
                            const unsigned char* pnSrcData,
                            signed long nBytes );

    /** Lets the cipher skip a specific amount of bytes.

        This function sets the cipher to the same state as if the specified
        amount of data has been decoded with one or more calls of decode().

        @precond
            The codec must be initialized with the initKey() function before
            this function can be used.

        @param nBytes
            Number of bytes to be skipped (cipher "seeks" forward).

        @return
            True = skip was successful (no error occured).
     */
    bool                skip( signed long nBytes );

    // static -----------------------------------------------------------------

    /** Calculates the 16-bit hash value for the given password.

        The password data may be longer than 16 bytes. The array does not need
        to be terminated with a null byte (but it can without invalidating the
        result).
     */
    static unsigned short   getHash( const unsigned char* pnPassData, signed long nSize );

    static unsigned short   getBaseKey( const unsigned char* pnPassData, signed long nSize );

private:
    CodecType           meCodecType;        /// Codec type.
    unsigned char           mpnKey[ 16 ];       /// Encryption key.
    long           mnOffset;           /// Key offset.
    unsigned short          mnBaseKey;          /// Base key from password.
    unsigned short          mnHash;             /// Hash value from password.
};

// ============================================================================

namespace {

/** Rotates rnValue left by nBits bits. */
template< typename Type >
inline void lclRotateLeft( Type& rnValue, size_t nBits )
{
//    OSL_ENSURE( nBits < sizeof( Type ) * 8, "lclRotateLeft - rotation count overflow" );
    rnValue = static_cast< Type >( (rnValue << nBits) | (rnValue >> (sizeof( Type ) * 8 - nBits)) );
}

/** Rotates the lower nWidth bits of rnValue left by nBits bits. */
template< typename Type >
inline void lclRotateLeft( Type& rnValue, size_t nBits, size_t nWidth )
{
//    OSL_ENSURE( (nBits < nWidth) && (nWidth < sizeof( Type ) * 8), "lclRotateLeft - rotation count overflow" );
    Type nMask = static_cast< Type >( (1UL << nWidth) - 1 );
    rnValue = static_cast< Type >(
        ((rnValue << nBits) | ((rnValue & nMask) >> (nWidth - nBits))) & nMask );
}

signed long lclGetLen( const unsigned char* pnPassData, signed long nBufferSize )
{
    signed long nLen = 0;
    while( (nLen < nBufferSize) && pnPassData[ nLen ] ) ++nLen;
    return nLen;
}

unsigned short lclGetKey( const unsigned char* pnPassData, signed long nBufferSize )
{
    signed long nLen = lclGetLen( pnPassData, nBufferSize );
    if( nLen <= 0 ) return 0;

    unsigned short nKey = 0;
    unsigned short nKeyBase = 0x8000;
    unsigned short nKeyEnd = 0xFFFF;
    const unsigned char* pnChar = pnPassData + nLen - 1;
    for( signed long nIndex = 0; nIndex < nLen; ++nIndex, --pnChar )
    {
        unsigned char cChar = *pnChar & 0x7F;
        for( size_t nBit = 0; nBit < 8; ++nBit )
        {
            lclRotateLeft( nKeyBase, 1 );
            if( nKeyBase & 1 ) nKeyBase ^= 0x1020;
            if( cChar & 1 ) nKey ^= nKeyBase;
            cChar >>= 1;
            lclRotateLeft( nKeyEnd, 1 );
            if( nKeyEnd & 1 ) nKeyEnd ^= 0x1020;
        }
    }
    return nKey ^ nKeyEnd;
}

unsigned short lclGetHash( const unsigned char* pnPassData, signed long nBufferSize )
{
    signed long nLen = lclGetLen( pnPassData, nBufferSize );

    unsigned short nHash = static_cast< unsigned short >( nLen );
    if( nLen > 0 )
        nHash ^= 0xCE4B;

    const unsigned char* pnChar = pnPassData;
    for( signed long nIndex = 0; nIndex < nLen; ++nIndex, ++pnChar )
    {
        unsigned short cChar = *pnChar;
        size_t nRot = static_cast< size_t >( (nIndex + 1) % 15 );
        lclRotateLeft( cChar, nRot, 15 );
        nHash ^= cChar;
    }
    return nHash;
}

} // namespace

// ============================================================================
/*
unsigned short CodecHelper::getPasswordHash( const AttributeList& rAttribs, signed long nElement )
{
    signed long nPasswordHash = rAttribs.getIntegerHex( nElement, 0 );
    OSL_ENSURE( (0 <= nPasswordHash) && (nPasswordHash <= SAL_MAX_UINT16), "CodecHelper::getPasswordHash - invalid password hash" );
    return static_cast< unsigned short >( ((0 <= nPasswordHash) && (nPasswordHash <= SAL_MAX_UINT16)) ? nPasswordHash : 0 );
}
*/
// ============================================================================

BinaryCodec_XOR::BinaryCodec_XOR( CodecType eCodecType ) :
    meCodecType( eCodecType ),
    mnOffset( 0 ),
    mnBaseKey( 0 ),
    mnHash( 0 )
{
    (void)memset( mpnKey, 0, sizeof( mpnKey ) );
}

BinaryCodec_XOR::~BinaryCodec_XOR()
{
    (void)memset( mpnKey, 0, sizeof( mpnKey ) );
    mnBaseKey = mnHash = 0;
}

void BinaryCodec_XOR::initKey( const unsigned char pnPassData[ 16 ] )
{
    // calculate base key and hash from passed password
    mnBaseKey = lclGetKey( pnPassData, 16 );
    mnHash = lclGetHash( pnPassData, 16 );

     static const unsigned char spnFillChars[] =
    {
        0xBB, 0xFF, 0xFF, 0xBA,
        0xFF, 0xFF, 0xB9, 0x80,
        0x00, 0xBE, 0x0F, 0x00,
        0xBF, 0x0F, 0x00
    };

    (void)memcpy( mpnKey, pnPassData, 16 );
    signed long nIndex;
    signed long nLen = lclGetLen( pnPassData, 16 );
    const unsigned char* pnFillChar = spnFillChars;
    for( nIndex = nLen; nIndex < static_cast< signed long >( sizeof( mpnKey ) ); ++nIndex, ++pnFillChar )
        mpnKey[ nIndex ] = *pnFillChar;

    // rotation of key values is application dependent
    size_t nRotateSize = 0;
    switch( meCodecType )
    {
        case CODEC_WORD:    nRotateSize = 7;    break;
        case CODEC_EXCEL:   nRotateSize = 2;    break;
        // compiler will warn, if new codec type is introduced and not handled here
    }

    // use little-endian base key to create key array
    unsigned char pnBaseKeyLE[ 2 ];
    pnBaseKeyLE[ 0 ] = static_cast< unsigned char >( mnBaseKey );
    pnBaseKeyLE[ 1 ] = static_cast< unsigned char >( mnBaseKey >> 8 );
    unsigned char* pnKeyChar = mpnKey;
    for( nIndex = 0; nIndex < static_cast< signed long >( sizeof( mpnKey ) ); ++nIndex, ++pnKeyChar )
    {
        *pnKeyChar ^= pnBaseKeyLE[ nIndex & 1 ];
        lclRotateLeft( *pnKeyChar, nRotateSize );
    }
}

bool BinaryCodec_XOR::verifyKey( unsigned short nKey, unsigned short nHash ) const
{
    return (nKey == mnBaseKey) && (nHash == mnHash);
}

void BinaryCodec_XOR::startBlock()
{
    mnOffset = 0;
}

bool BinaryCodec_XOR::decode( unsigned char* pnDestData, const unsigned char* pnSrcData, signed long nBytes )
{
    const unsigned char* pnCurrKey = mpnKey + mnOffset;
    const unsigned char* pnKeyLast = mpnKey + 0x0F;

    // switch/case outside of the for loop (performance)
    const unsigned char* pnSrcDataEnd = pnSrcData + nBytes;
    switch( meCodecType )
    {
        case CODEC_WORD:
        {
            for( ; pnSrcData < pnSrcDataEnd; ++pnSrcData, ++pnDestData )
            {
                unsigned char nData = *pnSrcData ^ *pnCurrKey;
                if( (*pnSrcData != 0) && (nData != 0) )
                    *pnDestData = nData;
                if( pnCurrKey < pnKeyLast ) ++pnCurrKey; else pnCurrKey = mpnKey;
            }
        }
        break;
        case CODEC_EXCEL:
        {
            for( ; pnSrcData < pnSrcDataEnd; ++pnSrcData, ++pnDestData )
            {
                *pnDestData = *pnSrcData;
                lclRotateLeft( *pnDestData, 3 );
                *pnDestData ^= *pnCurrKey;
                if( pnCurrKey < pnKeyLast ) ++pnCurrKey; else pnCurrKey = mpnKey;
            }
        }
        break;
        // compiler will warn, if new codec type is introduced and not handled here
    }

    // update offset and leave
    return skip( nBytes );
}

bool BinaryCodec_XOR::skip( signed long nBytes )
{
    mnOffset = static_cast< signed long >( (mnOffset + nBytes) & 0x0F );
    return true;
}

unsigned short BinaryCodec_XOR::getHash( const unsigned char* pnPassData, signed long nSize )
{
    return lclGetHash( pnPassData, nSize );
}

unsigned short BinaryCodec_XOR::getBaseKey( const unsigned char* pnPassData, signed long nSize )
{
    return lclGetKey( pnPassData, nSize );
}

// ============================================================================

} // namespace core
} // namespace oox
