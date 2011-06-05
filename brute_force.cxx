#include "binarycodec.cxx"

int main(int argc, char ** argv) {
	if (argc < 3) {
		std::cout << "Usage: " << argv[0] << " <0xKey> <0xHash>" << std::endl;
		return EXIT_FAILURE;
	}

	unsigned short nKey;
	std::istringstream issk(argv[1]);
	issk >> std::hex >> nKey;
	unsigned short nHash;
	std::istringstream issh(argv[2]);
	issh >> std::hex >> nHash;

	std::cout << std::hex << "Key: " << nKey << std::endl;
	std::cout << std::hex << "Hash: " << nHash << std::endl;

	oox::core::BinaryCodec_XOR x(oox::core::BinaryCodec_XOR::CODEC_EXCEL);
	unsigned char t[9] = { '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' };

	// BRUTE FORCE up to 5 chars
	for(unsigned char i=32; i < 127; ++i) {
		std::cout << (short)i << std::endl;
		for(unsigned char j=32; j < 128; ++j) {
			std::cout << "  " << (short)j << std::endl;
			for(unsigned char k=32; k < 128; ++k) {
				for(unsigned char l=32; l < 128; ++l) {
					for(unsigned char m=32; m < 128; ++m) {
						if (nHash == x.getHash(t, 16)) {
							x.initKey(t);
							bool res = x.verifyKey(nKey, nHash);
							if (res) {
								std::cout << t << " " << res << std::endl;
								return EXIT_SUCCESS;
							}
						}
						t[0] = m;
					}
					t[1] = l;
				}
				t[2] = k;
			}
			t[3] = j;
		}
		t[4] = i;
	}

	return EXIT_FAILURE;
}
