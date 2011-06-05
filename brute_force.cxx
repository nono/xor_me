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

	// BRUTE FORCE exactly 4 chars
	for(unsigned char i=32; i < 127; ++i) {
		t[0] = i;
		std::cout << (short)i << std::endl;
		for(unsigned char j=32; j < 127; ++j) {
			t[1] = j;
			for(unsigned char k=32; k < 127; ++k) {
				t[2] = k;
				for(unsigned char l=32; l < 127; ++l) {
					t[3] = l;
					if (nHash == x.getHash(t, 16)) {
						x.initKey(t);
						bool res = x.verifyKey(nKey, nHash);
						if (res) {
							std::cout << t << " " << res << std::endl;
							return EXIT_SUCCESS;
						}
					}
				}
			}
		}
	}

	return EXIT_FAILURE;
}
