// XXZConsle.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include<vector>
using namespace std;

#pragma comment( lib,"D:/zhangjialiang/zjl/XXZConsle/Release/XXZ_PETool.lib")
extern "C" _declspec(dllimport) bool ReadPEFile(const char* file_name);

// #pragma comment( lib,"E:/XXZConsle/Release/XXZ_Algorithm.lib")
// XXZ_ALGORITHM vector<int> Sort_Bubble(vector<int> vec, bool small2big);
struct TreeNode
{
	int val;
	TreeNode *left;
	TreeNode *right;
	TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};
class Solution
{
	int index = 0;
public:
	TreeNode * ConstructBinaryTreeFromPreAndVin(vector<int> pre, vector<int> vin)
	{
		return splitTree(pre, vin);
	}

	TreeNode* splitTree(vector<int> pre, vector<int> vin)
	{
		vector<int> left, right;
		bool is_left = true;
		int root = pre[index++];
		TreeNode* node = new TreeNode(root);
		if (vin.size() == 1)
		{
			return node;
		}
		for (int i = 0; i < vin.size(); i++)
		{
			if (vin[i] == root)
			{
				is_left = false;
				continue;
			}
			if (is_left)
			{
				left.push_back(vin[i]);
			}
			else
			{
				right.push_back(vin[i]);
			}

		}
		node->left = splitTree(pre,left);
		node->right = splitTree(pre,right);
		return node;
	}
};
int main()
{
	
	ReadPEFile("C:/Program Files/Notepad++/notepad++.exe");
	//ReadPEFile("C:/Users/Administrator/Desktop/ODTest/Release/ODTest - 副本.exe");
    return 0;
}

