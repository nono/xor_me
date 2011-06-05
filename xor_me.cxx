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

	oox::core::BinaryCodec_XOR x(algo);
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
