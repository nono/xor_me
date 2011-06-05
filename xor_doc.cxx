#include <iostream>
#include <fstream>
#include <cstdlib>

int main(int argc, char **argv) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <file>" << std::endl;
		return EXIT_FAILURE;
	} else {
		std::ifstream ifs(argv[1], std::ios_base::in & std::ios_base::binary );

		char data[0x220];
	
		if (ifs.good()) {
			ifs.get(data, 0x21F);
			ifs.close();
			if (!ifs.good()) {
				std::cerr << "File error" << std::endl;
				return EXIT_FAILURE;
			}
		} else {
			std::cerr << "File error" << std::endl;
			return EXIT_FAILURE;
		}

		unsigned char bitfield = data[0x20B];
		bool fObfuscated = bitfield & 0x80;
		bool fEncrypted = bitfield & 0x01;

		if (fObfuscated && fEncrypted) {
			std::cout << "Could be a XOR-ciphered doc file." << std::endl;
		} else {
			std::cerr << "Not a XOR-ciphered doc file. fObfuscated=" << fObfuscated << " fEncrypted=" << fEncrypted << std::endl;  
		}

		unsigned short nKey = data[0x211] * 256 + data[0x210];
		unsigned short nHash = data[0x20F] * 256 + data[0x20E];

		std::cout << "nKey  " << std::hex << nKey << std::endl;
		std::cout << "nHash " << std::hex << nHash << std::endl;

		return EXIT_SUCCESS;
	}
}
