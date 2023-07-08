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
	char character;			// �ַ�
	long count;				// Ƶ��
	bool bLeaf;				// Ҷ�ӽڵ�
	HaffmanNode* lchild;	//����
	HaffmanNode* rchild;	//�Һ���
	HaffmanNode* parent;	//���ڵ�
};

struct CompareHuffman {
	bool operator()(const HaffmanNode* l, const HaffmanNode* r) const noexcept
	{
		return l->count > r->count;//�󶥶�
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
		HaffmanNode* pParentNode = new HaffmanNode('$', pLeftSon->count + pRightSon->count, false);//��false���б�Ҷ�ӽڵ㣬�����ַ����ַ�����ȫ
		pParentNode->lchild = pLeftSon;
		pParentNode->rchild = pRightSon;
		pq.push(pParentNode);
	}
	return pq.top();
}

bool FileExists(const std::string& filename)
{
	std::ifstream file(filename, ios_base::in | ios_base::binary);
	return file.good(); // ����ļ��Ƿ�ɹ���
}

string GetFileName(bool bCheckFileValid = true, const string& strTips = "�������ļ���\n")
{
	cout << strTips;
	string strFileName;
	cin >> strFileName;
	if (bCheckFileValid)
	{
		while (!FileExists(strFileName))
		{
			cout << "�ļ�·������\n";
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
		cout << "�ļ���ʧ��\n";
		return false;
	}

	//���code��
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

//��ȡÿ��ֵ����Ӧ�Ĺ���������
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
		cout << "��ʧ��\n";
		return false;
	}
	char ch;
	while (ifile.get(ch))
		strSaveData += ch;
	ifile.close();
	return true;
}

//�������� myMap�ֵ� outFilename���浽���ļ� filenameҪ�������ݵ�����
bool SaveDataToFile(const std::map<char, string>& myMap, const std::string& outFilename, const string& strSaveData)
{
	std::ofstream file(outFilename, ios_base::out | ios_base::binary);
	if (!file)
	{
		cout << "��ʧ��\n";
		return false;
	}

	//�洢�����������ֵ�
	size_t mapSize = myMap.size();
	file.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));
	for (const auto& pair : myMap)
	{
		file.write(reinterpret_cast<const char*>(&pair.first), sizeof(pair.first));
		// д����������볤��
		int codeSize = pair.second.size();
		file.write(reinterpret_cast<const char*>(&codeSize), sizeof(codeSize));

		// д���������������
		file.write(pair.second.c_str(), pair.second.size());
	}

	//�洢����
	string tempCode;//�ַ����뻺��
	for (const auto cData : strSaveData)
	{
		auto iter = myMap.find(cData);//�������ȡ���ܿ��
		if (iter == myMap.end())
		{
			cout << "��������������!\n";
			return false;
		}

		const string& strCode = iter->second;//��ȡ��Ӧ�Ĺ���������
		for (const auto cTmp : strCode)
			tempCode += cTmp;//׷�ӵ��ַ�������
		char ch = 0;//������00000000����ʱ�Ķ���������
		while (tempCode.size() >= 8)//����8��д��
		{
			for (int i = 0; i < 8; ++i)
			{
				if (tempCode[i] == '1')//���ƣ�����1
				{
					ch = ch << 1;
					ch = ch | 1;
				}
				else//����
					ch = ch << 1;
			}
			file.write(&ch, sizeof(ch));
			tempCode.erase(0, 8);//����8λ
		}
	}
	if (tempCode.size() > 0)//����
	{
		char ch = 0;//������00000000����ʱ�Ķ���������
		for (int i = 0; i < 8; ++i)
			tempCode += "0";//�Ӱ˸�0
		for (int i = 0; i < 8; ++i)
		{
			if (tempCode[i] == '1')//���ƣ�����1
			{
				ch = ch << 1;
				ch = ch | 1;
			}
			else//����
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
		cout << "��ʧ��\n";
		return false;
	}

	// ��ȡ�����������ֵ��С
	size_t mapSize;
	file.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));

	// �����ȡ��������������
	for (size_t i = 0; i < mapSize; ++i)
	{
		char character;
		file.read(reinterpret_cast<char*>(&character), sizeof(character));

		// ��ȡ���������볤��
		int codeSize;
		file.read(reinterpret_cast<char*>(&codeSize), sizeof(codeSize));

		// ��ȡ��������������
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
	auto strFileName = GetFileName();//��ȡ�ļ���

	HaffmanNode* pNode = nullptr;
	if (!SaveDataToHeap(strFileName, pNode))//���ɹ�������
		return false;

	//��ȡÿ��ֵ����Ӧ�Ĺ���������
	std::map<char, string> mapHaffmanCode;
	GetHaffmanCode(pNode, "", mapHaffmanCode);

	auto strSaveFileName = GetFileName(false, "������洢�ļ���\n");//��ȡ�洢�ļ���
	string strSaveData;
	if (!GetSaveData(strFileName, strSaveData))
		return false;
	if (!SaveDataToFile(mapHaffmanCode, strSaveFileName, strSaveData))
		return false;
	
	releaseHaffmanTree(pNode);//�ͷ�
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

//��ѹ�ļ���strFileName
bool DeFileByHaffmanTree(const string& strFileName, const string& strData, HaffmanNode* pNode)
{
	ofstream file(strFileName, ios_base::out | ios_base::binary);
	if (!file)
	{
		cout << "���ļ�ʧ��\n";
		return false;
	}
	string charBuff;//����
	for (const auto cData : strData)
	{
		char tmp = cData;
		for (int i = 0; i < 8; ++i)
		{
			if (tmp & 128)//������10000000
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
	auto strFileName = GetFileName();//��ȡ�ļ���

	std::map<char, string> mapHaffmanCode;
	string strData;
	if (!GetSaveDataAndMap(strFileName, mapHaffmanCode, strData))
		return false;

	auto pNode = GetHaffmanTreeByCode(mapHaffmanCode);
	auto strDeName = GetFileName(false, "�������ѹ�ļ���\n");//��ȡ�ļ���
	
	DeFileByHaffmanTree(strDeName, strData, pNode);//��ѹ
	releaseHaffmanTree(pNode);//�ͷ�
	return true;
}

int main()
{
	while (true)
	{
		cout << "1��ѹ��\n";
		cout << "2����ѹ\n";
		cout << "���������ѡ��";
		int i = 0;
		cin >> i;
		switch (i)
		{
		case 1:
			{
				if(Compress())
					cout << "ѹ���ɹ���\n";
				else
					cout << "ѹ��ʧ�ܣ�\n";
			}
			break;
		case 2:
			{
				if(Decompression())
					cout << "��ѹ�ɹ���\n";
				else
					cout << "��ѹʧ�ܣ�\n";
			}
		}
	}
	
	return 0;
}