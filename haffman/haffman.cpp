#include<iostream>
#include<vector>
#include<string>
#include<map>
#include<fstream>
#include<queue>

using namespace std;
using HaffmanCharToRate = std::map<char, long>;
class HaffmanNode
{
public:
	HaffmanNode(char c = 0, long l = 0, bool b = false)
		:character(c)
		, count(l)
		, lchild(nullptr)
		, rchild(nullptr)
		, parent(nullptr)
		, bLeaf(b)
	{

	}
	char character;			// 字符
	long count;				// 频率
	bool bLeaf;				// 叶子节点
	HaffmanNode* lchild;	//左孩子
	HaffmanNode* rchild;	//右孩子
	HaffmanNode* parent;	//父节点
};

struct CompareHuffman {
	bool operator()(const HaffmanNode* l, const HaffmanNode* r) const noexcept
	{
		return l->count > r->count;//大顶堆
	}
};

HaffmanNode* BuildHuffmanTree(const HaffmanCharToRate& mapData)
{
	std::priority_queue<HaffmanNode*, std::vector<HaffmanNode*>, CompareHuffman> pq;
	for (const auto& pairData : mapData)
		pq.emplace(new HaffmanNode(pairData.first, pairData.second, true));

	while (pq.size() > 1)
	{
		auto pLeftSon = pq.top();
		pq.pop();
		auto pRightSon = pq.top();
		pq.pop();
		HaffmanNode* pParentNode = new HaffmanNode('$', pLeftSon->count + pRightSon->count, false);//由false来判别叶子节点，而非字符，字符不安全
		pParentNode->lchild = pLeftSon;
		pParentNode->rchild = pRightSon;
		pq.push(pParentNode);
	}
	return pq.top();
}

bool FileExists(const std::string& filename)
{
	std::ifstream file(filename, ios_base::in | ios_base::binary);
	return file.good(); // 检查文件是否成功打开
}

string GetFileName(bool bCheckFileValid = true, const string& strTips = "请输入文件名\n")
{
	cout << strTips;
	string strFileName;
	cin >> strFileName;
	if (bCheckFileValid)
	{
		while (!FileExists(strFileName))
		{
			cout << "文件路径有误\n";
			cin >> strFileName;
		}
	}
	return strFileName;
}

bool SaveDataToHeap(const string& strFileName, HaffmanNode*& pRootNode)
{
	ifstream file(strFileName, ios_base::in | ios_base::binary);
	if (!file)
	{
		cout << "文件打开失败\n";
		return false;
	}

	//存进code里
	HaffmanCharToRate vecCode;
	char ch;
	while (file.get(ch))
	{
		if (vecCode.find(ch) != vecCode.end())
			++vecCode[ch];
		else
			vecCode[ch] = 1;
	}
	pRootNode = BuildHuffmanTree(vecCode);
	return true;
}

//获取每个值所对应的哈夫曼编码
void GetHaffmanCode(HaffmanNode* root, const std::string& code, std::map<char, string>& mapHaffmanCode)
{
	if (root == nullptr)
		return;

	if (root->bLeaf)
	{
		mapHaffmanCode[root->character] = code;
	}
	GetHaffmanCode(root->lchild, code + "0", mapHaffmanCode);
	GetHaffmanCode(root->rchild, code + "1", mapHaffmanCode);
}

void releaseHaffmanTree(HaffmanNode* root)
{
	if (root == nullptr)
		return;
	releaseHaffmanTree(root->lchild);
	releaseHaffmanTree(root->rchild);
	delete root;
}

bool GetSaveData(const std::string& filename,string& strSaveData)
{
	strSaveData.clear();
	std::ifstream ifile(filename, ios_base::in | ios_base::binary);
	if (!ifile)
	{
		cout << "打开失败\n";
		return false;
	}
	char ch;
	while (ifile.get(ch))
		strSaveData += ch;
	ifile.close();
	return true;
}

//保存数据 myMap字典 outFilename保存到的文件 filename要保存数据的数据
bool SaveDataToFile(const std::map<char, string>& myMap, const std::string& outFilename, const string& strSaveData)
{
	std::ofstream file(outFilename, ios_base::out | ios_base::binary);
	if (!file)
	{
		cout << "打开失败\n";
		return false;
	}

	//存储哈夫曼编码字典
	size_t mapSize = myMap.size();
	file.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));
	for (const auto& pair : myMap)
	{
		file.write(reinterpret_cast<const char*>(&pair.first), sizeof(pair.first));
		// 写入哈夫曼编码长度
		int codeSize = pair.second.size();
		file.write(reinterpret_cast<const char*>(&codeSize), sizeof(codeSize));

		// 写入哈夫曼编码内容
		file.write(pair.second.c_str(), pair.second.size());
	}

	//存储数据
	string tempCode;//字符编码缓存
	for (const auto cData : strSaveData)
	{
		auto iter = myMap.find(cData);//红黑树获取，很快的
		if (iter == myMap.end())
		{
			cout << "哈夫曼编码有误!\n";
			return false;
		}

		const string& strCode = iter->second;//获取对应的哈夫曼编码
		for (const auto cTmp : strCode)
			tempCode += cTmp;//追加到字符缓冲中
		char ch = 0;//二进制00000000，临时的二进制容器
		while (tempCode.size() >= 8)//大于8，写入
		{
			for (int i = 0; i < 8; ++i)
			{
				if (tempCode[i] == '1')//左移，并加1
				{
					ch = ch << 1;
					ch = ch | 1;
				}
				else//左移
					ch = ch << 1;
			}
			file.write(&ch, sizeof(ch));
			tempCode.erase(0, 8);//丢弃8位
		}
	}
	if (tempCode.size() > 0)//补零
	{
		char ch = 0;//二进制00000000，临时的二进制容器
		for (int i = 0; i < 8; ++i)
			tempCode += "0";//加八个0
		for (int i = 0; i < 8; ++i)
		{
			if (tempCode[i] == '1')//左移，并加1
			{
				ch = ch << 1;
				ch = ch | 1;
			}
			else//左移
				ch = ch << 1;
		}
		file.write(&ch, sizeof(ch));
	}
	return true;
}

bool GetSaveDataAndMap(const string& strFileName, std::map<char, string>& myMap, string& strData)
{
	std::ifstream file(strFileName, ios_base::in | ios_base::binary);
	if (!file)
	{
		cout << "打开失败\n";
		return false;
	}

	// 读取哈夫曼编码字典大小
	size_t mapSize;
	file.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));

	// 逐个读取哈夫曼编码数据
	for (size_t i = 0; i < mapSize; ++i)
	{
		char character;
		file.read(reinterpret_cast<char*>(&character), sizeof(character));

		// 读取哈夫曼编码长度
		int codeSize;
		file.read(reinterpret_cast<char*>(&codeSize), sizeof(codeSize));

		// 读取哈夫曼编码内容
		std::string code(codeSize, '\0');
		file.read(&code[0], codeSize);

		myMap[character] = code;
	}

	char ch;
	while (file.get(ch))
		strData += ch;

	return true;
}

bool Compress()
{
	auto strFileName = GetFileName();//获取文件名

	HaffmanNode* pNode = nullptr;
	if (!SaveDataToHeap(strFileName, pNode))//生成哈夫曼树
		return false;

	//获取每个值所对应的哈夫曼编码
	std::map<char, string> mapHaffmanCode;
	GetHaffmanCode(pNode, "", mapHaffmanCode);

	auto strSaveFileName = GetFileName(false, "请输入存储文件名\n");//获取存储文件名
	string strSaveData;
	if (!GetSaveData(strFileName, strSaveData))
		return false;
	if (!SaveDataToFile(mapHaffmanCode, strSaveFileName, strSaveData))
		return false;
	
	releaseHaffmanTree(pNode);//释放
	return true;
}

HaffmanNode* GetHaffmanTreeByCode(const std::map<char, string>& mapHaffmanCode)
{
	HaffmanNode* pRoot = new HaffmanNode;
	for (const auto& pairHaffmanCode : mapHaffmanCode)
	{
		const auto& strCode = pairHaffmanCode.second;
		auto pNodeTmp = pRoot;
		for (const auto cCode : strCode)
		{
			if (cCode == '1')
			{
				if (pNodeTmp->rchild == nullptr)
					pNodeTmp->rchild = new HaffmanNode;
				pNodeTmp = pNodeTmp->rchild;
			}
			else
			{
				if (pNodeTmp->lchild == nullptr)
					pNodeTmp->lchild = new HaffmanNode;
				pNodeTmp = pNodeTmp->lchild;
			}
		}
		pNodeTmp->bLeaf = true;
		pNodeTmp->character = pairHaffmanCode.first;
	}
	return pRoot;
}

//解压文件到strFileName
bool DeFileByHaffmanTree(const string& strFileName, const string& strData, HaffmanNode* pNode)
{
	ofstream file(strFileName, ios_base::out | ios_base::binary);
	if (!file)
	{
		cout << "打开文件失败\n";
		return false;
	}
	string charBuff;//缓冲
	for (const auto cData : strData)
	{
		char tmp = cData;
		for (int i = 0; i < 8; ++i)
		{
			if (tmp & 128)//二进制10000000
				charBuff += '1';
			else
				charBuff += '0';
			tmp = tmp << 1;
		}
	}

	auto pNodeTmp = pNode;
	for (const auto cBuff : charBuff)
	{
		if (cBuff == '1')
			pNodeTmp = pNodeTmp->rchild;
		else
			pNodeTmp = pNodeTmp->lchild;
		if (pNodeTmp->bLeaf)
		{
			file.write(&pNodeTmp->character, sizeof(pNodeTmp->character));
			pNodeTmp = pNode;
		}
	}
	if (pNodeTmp->bLeaf)
	{
		file.write(&pNodeTmp->character, sizeof(pNodeTmp->character));
		pNodeTmp = pNode;
	}

	return true;
}

bool Decompression()
{
	auto strFileName = GetFileName();//获取文件名

	std::map<char, string> mapHaffmanCode;
	string strData;
	if (!GetSaveDataAndMap(strFileName, mapHaffmanCode, strData))
		return false;

	auto pNode = GetHaffmanTreeByCode(mapHaffmanCode);
	auto strDeName = GetFileName(false, "请输入解压文件名\n");//获取文件名
	
	DeFileByHaffmanTree(strDeName, strData, pNode);//解压
	releaseHaffmanTree(pNode);//释放
	return true;
}

int main()
{
	while (true)
	{
		cout << "1、压缩\n";
		cout << "2、解压\n";
		cout << "请输入你的选择：";
		int i = 0;
		cin >> i;
		switch (i)
		{
		case 1:
			{
				if(Compress())
					cout << "压缩成功！\n";
				else
					cout << "压缩失败！\n";
			}
			break;
		case 2:
			{
				if(Decompression())
					cout << "解压成功！\n";
				else
					cout << "解压失败！\n";
			}
		}
	}
	
	return 0;
}