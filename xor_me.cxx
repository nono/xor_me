#include "binarycodec.cxx"

int main(int argc, char ** argv) {

	oox::core::BinaryCodec_XOR::CodecType algo;
	unsigned char * pwdToTest = reinterpret_cast<unsigned char *>(argv[1]);
	if (argc < 4) {
		std::cout << "Usage: " << argv[0] << " <passwd_to_test> <0xKey> <0xHash> [1 for XLS]" << std::endl;
		return EXIT_FAILURE;
	} else if (argc >= 5) {
		algo = oox::core::BinaryCodec_XOR::CODEC_EXCEL;
		std::cout << "Algorithm XOR MS Excel" << std::endl;
	} else {
		algo = oox::core::BinaryCodec_XOR::CODEC_WORD;
		std::cout << "Algorithm XOR MS Word" << std::endl;
	}
	unsigned short nKey;
	std::istringstream issk(argv[2]);
	issk >> std::hex >> nKey;
	unsigned short nHash;
	std::istringstream issh(argv[3]);
	issh >> std::hex >> nHash;

	std::cout << std::hex << "Key: " << nKey << std::endl;
	std::cout << std::hex << "Hash: " << nHash << std::endl;

/* BRUTE FORCE exactly 4 chars
	oox::core::BinaryCodec_XOR x(oox::core::BinaryCodec_XOR::CODEC_EXCEL);

	unsigned short nKey = 0x499a;
	unsigned short nHash = 0xcc61;
	unsigned char t[9] = { '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' };

	for(unsigned char i=32; i < 127; ++i) {
		t[0] = i;
		std::cout << (short)i << std::endl;
		for(unsigned char j=32; j < 127; ++j) {
			t[1] = j;
			for(unsigned char k=32; k < 127; ++k) {
				t[2] = k;
				for(unsigned char l=32; l < 127; ++l) {
					t[3] = l;
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
*/

	oox::core::BinaryCodec_XOR x(algo);
	x.initKey(pwdToTest);

	if (x.verifyKey(nKey, nHash)) {
		std::cout << "Good guess: " << pwdToTest << std::endl;
  } else {
		std::cout << "FAIL! " << pwdToTest << std::endl;
	}
	return EXIT_SUCCESS;
}
