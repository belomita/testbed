#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <list>

using namespace std;

void lzw_encode_test()
{
	string text = "01001111000011010111";

	int id_gen = 0;
	map<string, int> dict;
	dict["0"] = ++id_gen; 	dict["1"] = ++id_gen;

	ostringstream oss;
	string prefix = "";
	for (int i = 0; i < text.length(); i++)
	{
		string s = prefix + text.substr(i, 1);
		printf("prefix: %10s, prefix+char: %10s\n", prefix.c_str(), s.c_str());
		if (dict.find(s) != dict.end())
		{
			prefix = s;
		} else {
			oss<<(dict[prefix]);			
			dict[s] = ++id_gen;
			prefix = text.substr(i, 1);
		}
	}	
	oss<<(dict[prefix]);

	for (map<string, int>::const_iterator it = dict.begin(); it != dict.end(); ++it)
	{
		printf("prefix: %10s, code: %10d\n", it->first.c_str(), it->second);
	}
	cout<<"encoded: "<<oss.str()<<endl;	
}

void lzw_decode_test()
{
	int input[] = {1, 2, 3, 4, 5, 7, 8, 6, 8};
	list<int> codestream(input, input + sizeof(input) / sizeof(int));
	
	map<int, string> dict;	
	int id_gen = 0;
	dict[++id_gen] = "A"; 	dict[++id_gen] = "B";
	dict[++id_gen] = "C"; 	dict[++id_gen] = "D";

	int cur_code, pre_code;
	int index = 0;
	// step.1
	cur_code = codestream.front(); 	codestream.pop_front();
	cout<<"output: "<<dict[cur_code]<<endl;
	// step.2
	pre_code = cur_code;	
	// step 3
	while(!codestream.empty())
	{
		cout<<"-------------------\n";
		cur_code = codestream.front(); codestream.pop_front();
		id_gen++;
		if (dict.find(cur_code) != dict.end())
		{
			cout<<"output: "<<dict[cur_code]<<endl;
			string prefix = dict[pre_code];
			char first_char = dict[cur_code][0];
			dict[id_gen] = prefix + first_char;
			cout<<"code: "<<id_gen<<" prefix: "<<dict[id_gen]<<endl;
			pre_code = cur_code;			
		}
		else
		{
			string prefix = dict[pre_code];
			char first_char = prefix[0];
			dict[id_gen] = prefix + first_char;				
			cout<<"output"<<dict[cur_code]<<endl;
			cout<<"code: "<<id_gen<<" prefix: "<<dict[id_gen]<<endl;			
			pre_code = cur_code;
		}		
	}
}