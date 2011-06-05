#include <iostream>
#include <sstream>


inline void lclRotateRight(unsigned short& rnValue, size_t nBits)
{
	rnValue = (rnValue >> nBits) | (rnValue << (sizeof(unsigned short) * 8 - nBits));
}

inline void lclRotateLeft(unsigned short& rnValue, size_t nBits)
{
	rnValue = (rnValue << nBits) | (rnValue >> (sizeof(unsigned short) * 8 - nBits));
}

inline signed long lclGetLen(const unsigned char* pnPassData, signed long nBufferSize)
{
	signed long nLen = 0;
	while ( (nLen < nBufferSize) && pnPassData[ nLen ] ) { ++nLen; }
	return nLen;
}

unsigned short lclGetKey(const unsigned char* pnPassData, signed long nBufferSize)
{
	signed long nLen = lclGetLen(pnPassData, nBufferSize);
	if ( nLen <= 0 ) return 0;

	unsigned short nKey = 0;
	unsigned short nKeyBase = 0x8000;
	unsigned short nKeyEnd = 0xFFFF;
	const unsigned char* pnChar = pnPassData + nLen - 1;
	for (signed long nIndex = 0; nIndex < nLen; ++nIndex, --pnChar) {
		unsigned char cChar = *pnChar & 0x7F;
		for (size_t nBit = 0; nBit < 8; ++nBit) {
			lclRotateLeft( nKeyBase, 1 );
			if (nKeyBase & 1) nKeyBase ^= 0x1020;
			if (cChar & 1) nKey ^= nKeyBase;
			cChar >>= 1;
			lclRotateLeft(nKeyEnd, 1);
			if (nKeyEnd & 1) nKeyEnd ^= 0x1020;
		}
	}
	return nKey ^ nKeyEnd;
}

unsigned short lclGetHash(const unsigned char* pnPassData, const unsigned short* pnRotatedData,signed long nBufferSize)
{
	signed long nLen = lclGetLen(pnPassData, nBufferSize);
	unsigned short nHash = static_cast<unsigned short>(nLen) ^ 0xCE4B;

	const unsigned short* pnChar = pnRotatedData;
	for(signed long nIndex = 0; nIndex < nLen; ++nIndex, ++pnChar) {
		nHash ^= *pnChar;
	}
	return nHash;
}



int main(int argc, char ** argv) {
	if (argc < 3) {
		std::cout << "Usage: " << argv[0] << " <0xKey> <0xHash>" << std::endl;
		return -1;
	}

	unsigned short nKey;
	std::istringstream issk(argv[1]);
	issk >> std::hex >> nKey;
	unsigned short nHash;
	std::istringstream issh(argv[2]);
	issh >> std::hex >> nHash;

	std::cout << std::hex << "Key: " << nKey << std::endl;
	std::cout << std::hex << "Hash: " << nHash << std::endl;

	unsigned char  t[9] = {    1, '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' };
	unsigned short r[9] = { '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' };
	unsigned short hash = lclGetHash(t, r, 16);

	// BRUTE FORCE up to 8 chars
	for (unsigned char i=32; i < 127; ++i) {
		std::cout << "  " << (short)i << std::endl;
		for (unsigned char j=32; j < 128; ++j) {
			std::cout << "    " << (short)j << std::endl;
			for (unsigned char k=32; k < 128; ++k) {
				std::cout << "      " << (short)k << std::endl;
				for (unsigned char l=32; l < 128; ++l) {
					for (unsigned char m=32; m < 128; ++m) {
						for (unsigned char n=32; n < 128; ++n) {
							for (unsigned char o=32; o < 128; ++o) {
								unsigned short x = nHash ^ hash;
								lclRotateRight(x, 1);
								if (32 <= x && x < 127) {
									t[0] = static_cast<unsigned char>(x);
									if (nKey == lclGetKey(t, 16)) {
										std::cout << "Password: '" << t << "'" << std::endl;
										return 0;
									}
								}
								hash ^= r[1];
								r[1] = t[1] = o;
								lclRotateLeft(r[1], 2);
								hash ^= r[1];
								if (o == 32) {
									r[0] = '\0';
									hash = lclGetHash(t, r, 16);
								}
							}
							hash ^= r[2];
							r[2] = t[2] = n;
							lclRotateLeft(r[2], 3);
							hash ^= r[2];
							if (n == 32) {
								r[0] = '\0';
								hash = lclGetHash(t, r, 16);
							}
						}
						hash ^= r[3];
						r[3] = t[3] = m;
						lclRotateLeft(r[3], 4);
						hash ^= r[3];
						if (m == 32) {
							r[0] = '\0';
							hash = lclGetHash(t, r, 16);
						}
					}
					hash ^= r[4];
					r[4] = t[4] = l;
					lclRotateLeft(r[4], 5);
					hash ^= r[4];
					if (l == 32) {
						r[0] = '\0';
						hash = lclGetHash(t, r, 16);
					}
				}
				hash ^= r[5];
				r[5] = t[5] = k;
				lclRotateLeft(r[5], 6);
				hash ^= r[5];
				if (k == 32) {
					r[0] = '\0';
					hash = lclGetHash(t, r, 16);
				}
			}
			hash ^= r[6];
			r[6] = t[6] = j;
			lclRotateLeft(r[6], 7);
			hash ^= r[6];
			if (j == 32) {
				r[0] = '\0';
				hash = lclGetHash(t, r, 16);
			}
		}
		hash ^= r[7];
		r[7] = t[7] = i;
		lclRotateLeft(r[7], 8);
		hash ^= r[7];
		if (i == 32) {
			r[0] = '\0';
			hash = lclGetHash(t, r, 16);
		}
	}
}
