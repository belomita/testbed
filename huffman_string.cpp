#include "huffman_string.h"
#include <cassert>
#include <sstream>
#include <iostream>

//#define MY_DEBUG
#define CHAR_COUNT  256

#define SAFE_DELETE(p) { if(p) { delete (p); p = 0; } }

using namespace std;

namespace _HuffmanInternal
{
	void TraverseTree(HuffmanNode* node, HuffmanTable* table, vector<char>& codes)
	{
		assert(node);
		assert(table);

		if (node->left == 0 && node->right == 0)
		{
			(*table)[node->symbol] = string(codes.begin(), codes.end());
		}
		if (node->left != 0)
		{
			codes.push_back('0');
			TraverseTree(node->left, table, codes);
			if (codes.size() > 0)
			{
				codes.pop_back();
			}
		}
		if (node->right != 0)
		{
			codes.push_back('1');
			TraverseTree(node->right, table, codes);
			if (codes.size() > 0)
			{
				codes.pop_back();
			}
		}	
	}
}

HuffmanEncoder::HuffmanEncoder()
{

}

HuffmanEncoder::~HuffmanEncoder()
{

}

std::string HuffmanEncoder::Encode( const std::string& text, HuffmanTree* generated_tree /* = 0*/)
{
	HuffmanNodeQueue queue;
	HuffmanTree tree;
	HuffmanTable table;

	_ComputePriority(text, &queue);
	_BuildHuffmanTree(&queue, &tree);
	_BuildHuffmanTable(&tree, &table);

	if (generated_tree != 0)
	{
		generated_tree->root = tree.root;
		tree.root = 0;
	}
	
	ostringstream oss;
	for (unsigned int i = 0; i < text.length(); i++)
	{
		unsigned char ch   = text[i];
		const string& code = table[ch];
		oss<<code;

#ifdef MY_DEBUG
		oss<<" ";
#endif
	}

#ifndef MY_DEBUG
	string encoded = oss.str();
	ostringstream oss2;
	for(unsigned int i = 0; i < encoded.length(); i++)
	{
		oss2<<char(encoded[i]);
		if ((i + 1) % 4 == 0)
			oss2<<' ';
	}
	return oss2.str();
#else
	return oss.str();
#endif
}

void HuffmanEncoder::_ComputePriority( const std::string& text, HuffmanNodeQueue* queue )
{
	std::vector<unsigned int> priorities(CHAR_COUNT, 0);
	
	for (unsigned int i = 0; i < text.length(); i++)
	{
		unsigned int ch = text[i];
		priorities[ch]++;
	}

	for (unsigned int i = 0; i < CHAR_COUNT; i++)
	{
		if (priorities[i] > 0)
		{
			HuffmanNode *newNode = new HuffmanNode;
			newNode->priority = priorities[i];
			newNode->symbol = i;
			queue->push_back(newNode);
		}
	}
	stable_sort(queue->begin(), queue->end(), HuffmanNodeCompare());
}

void HuffmanEncoder::_BuildHuffmanTree( HuffmanNodeQueue* queue, HuffmanTree* tree )
{
	assert(queue);
	assert(tree);
	assert(queue->size() > 1);

	while(queue->size() > 1)
	{
		//HuffmanNode* first  = queue->top(); queue->pop();
		//HuffmanNode* second = queue->top(); queue->pop();
		HuffmanNode* first  = queue->back(); queue->pop_back(); 
		HuffmanNode* second = queue->back(); queue->pop_back(); 
		
		HuffmanNode* newNode = new HuffmanNode;
		newNode->left = first;
		newNode->right = second;
		newNode->priority = first->priority + second->priority;
		newNode->symbol = 0;
		//queue->push(newNode);
		queue->push_back(newNode);
		stable_sort(queue->begin(), queue->end(), HuffmanNodeCompare());
	}
	assert(queue->size() == 1);

	//HuffmanNode* root = queue->top(); queue->pop();
	HuffmanNode* root = queue->back(); queue->pop_back(); 
	tree->root = root;
}

void HuffmanEncoder::_BuildHuffmanTable( const HuffmanTree* tree, HuffmanTable* table )
{
	assert(tree);
	assert(tree->root);
	assert(table);

	std::vector<char> codes;
	_HuffmanInternal::TraverseTree(tree->root, table, codes);

#ifdef MY_DEBUG
	for (HuffmanTable::const_iterator it = table->begin(); it != table->end(); ++it)
	{
		cout<<"char: " << char(it->first) << " code: " << it->second<<endl;
	}
#endif
}

HuffmanNode::HuffmanNode() :
  symbol(0)
, priority(0)
, left(0)
, right(0)
{

}

HuffmanNode::~HuffmanNode()
{
	SAFE_DELETE(left);
	SAFE_DELETE(right);
	symbol = 0;
	priority = 0;
}

HuffmanTree::HuffmanTree() : root(0)
{

}

HuffmanTree::~HuffmanTree()
{
	SAFE_DELETE(root);
}


std::string HuffmanDecoder::Decode( const std::string& encoded, const HuffmanTree* tree )
{
	assert(tree);
	assert(tree->root);
	const HuffmanNode* node = tree->root;

	ostringstream oss;
	string::const_iterator it = encoded.begin(); 
	for (; it != encoded.end(); ++it)
	{
		if (node->left == 0 && node->right == 0)
		{
			oss<<node->symbol;
			node = tree->root;
		}
		if (*it == '0')
		{
			node = node->left;
		}else if (*it == '1')
		{
			node = node->right;
		}else if (*it == ' ')
		{
			;
		}else
		{
			cerr<<"The input string is not encoded correctly!\n";
			return "";
		}
	}
	if (node->left == 0 && node->right == 0)
	{
		oss<<node->symbol;
		node = tree->root;
	}
	return oss.str();
}
