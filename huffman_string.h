#ifndef huffman_string_h__
#define huffman_string_h__

#include <string>
#include <map>
#include <vector>
//#include <queue>

struct HuffmanNode
{
	unsigned char symbol;
	unsigned int priority;
	HuffmanNode *left;
	HuffmanNode *right;
	HuffmanNode();
	~HuffmanNode();	
};

struct HuffmanNodeCompare
{
public:
	bool operator()(HuffmanNode* left, HuffmanNode* right)
	{
		return (left->priority > right->priority);		
	}
};

struct HuffmanTree
{
	HuffmanNode *root;
	HuffmanTree();
	~HuffmanTree();
};


// NOTE: The std::priority_queue is *NOT* used here because its implementation uses container's
//       push_back() method to add elements, which means new elements are always added at the
//       back of the container, then adjusted up using heap. But here, I expect the new element 
//       can be added from front and then adjusted down, by which elements can keep stable order.
//       Thus, instead of re-implement a new priority queue, I used the expensive 
//       std::stable_sort on std::vector to keep elements in order, stupidly and lazily.
//       Refer: http://en.nerdaholyc.com/huffman-coding-on-a-string/
// typedef std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, HuffmanNodeCompare> HuffmanNodeQueue;

typedef std::vector<HuffmanNode*> HuffmanNodeQueue;
typedef std::map<unsigned char, std::string> HuffmanTable;

class HuffmanEncoder
{
public:
	HuffmanEncoder();
	~HuffmanEncoder();

	std::string Encode(const std::string& text, HuffmanTree* generated_tree = 0);
private:		
	void _ComputePriority(const std::string& text, HuffmanNodeQueue* queue);
	void _BuildHuffmanTree(HuffmanNodeQueue* queue, HuffmanTree* tree);
	void _BuildHuffmanTable(const HuffmanTree* tree, HuffmanTable* table);
};

class HuffmanDecoder
{
public:
	std::string Decode(const std::string& encoded, const HuffmanTree* tree );
};

#endif // huffman_string_h__
