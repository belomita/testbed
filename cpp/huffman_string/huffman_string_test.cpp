#include "huffman_string_test.h"
#include "huffman_string.h"

#include <string>
#include <iostream>

using namespace std;

void TestHuffmanString()
{
	HuffmanEncoder encoder;
	HuffmanTree tree_out;
	string text = "beep loop beer!";
	string encoded = encoder.Encode(text, &tree_out);
	cout<<"Text:    "<<text<<endl;
	cout<<"Encoded: "<<encoded<<endl;

	HuffmanDecoder decoder;
	string decoded = decoder.Decode(encoded, &tree_out);
	cout<<"Encoded: "<<decoded<<endl;
}
