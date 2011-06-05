all:
	g++ -o xor_me -O3 xor_me.cxx
	g++ -o xor_doc -O3 xor_doc.cxx
	g++ -o brute_force -O3 brute_force.cxx

clean:
	rm -f xor_me xor_doc brute_force
