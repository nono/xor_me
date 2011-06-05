#include "binarycodec.cxx"

int main(int argc, char ** argv) {
	if (argc < 4) {
		std::cout << "Usage: " << argv[0] << " <passwd_to_test> <0xKey> <0xHash>" << std::endl;
		return EXIT_FAILURE;
	}

	unsigned char * pwdToTest = reinterpret_cast<unsigned char *>(argv[1]);
	unsigned short nKey;
	std::istringstream issk(argv[2]);
	issk >> std::hex >> nKey;
	unsigned short nHash;
	std::istringstream issh(argv[3]);
	issh >> std::hex >> nHash;

	std::cout << std::hex << "Key: " << nKey << std::endl;
	std::cout << std::hex << "Hash: " << nHash << std::endl;

	oox::core::BinaryCodec_XOR x(oox::core::BinaryCodec_XOR::CODEC_WORD);
	x.initKey(pwdToTest);

	if (x.verifyKey(nKey, nHash)) {
		std::cout << "Good guess: " << pwdToTest << std::endl;
	} else {
		std::cout << "FAIL! " << pwdToTest << std::endl;
		std::cout << "\tbkey: " << x.getBaseKey(pwdToTest, 16) << std::endl;
		std::cout << "\thash: " << x.getHash(pwdToTest, 16) << std::endl;
	}
	return EXIT_SUCCESS;
}
