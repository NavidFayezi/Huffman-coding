#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<bitset>
using namespace std;

enum Mode {
	ENCODING,
	DECODING
};

enum HEAP_TYPE {
	MINHEAP,
	MAXHEAP
};

class Node {
public:
	string huffman;
	char character;
	Node* left;
	Node* right;
	Node* parent;
	int count;
	Node(char str, int a) :count(a), character(str), left(NULL), right(NULL), parent(NULL), huffman ("") {}
	Node(const Node& a) :left(a.left), right(a.right), parent(a.parent), count(a.count), character(a.character), huffman (a.huffman) {}

};

class Heap {
public:
	HEAP_TYPE type;
	Node* root;
	vector<Node*>* heap;
	Heap(vector<Node*>* nodes, HEAP_TYPE t) :heap(nodes), type(t) {}
	bool compare(int parent, int child) {
		if (type == HEAP_TYPE::MINHEAP) {
			return parent > child;
		}
		else {
			return child > parent;
		}
	}
	void heapify(int index) {
		if (index * 2 >= heap->size()) {
			return;
		}
		else {//index has one child.
			if ((index * 2) + 1 == heap->size()) {
				if (this->compare((*heap)[index]->count, (*heap)[index * 2]->count)) {
					this->swap(index, index * 2);
					return;
				}
			}
			else {
				//index has both children,determin minimum, swap with root if necessary
				//max or min
				int minmax_index;
				if (type == HEAP_TYPE::MINHEAP) {
					if ((*heap)[index * 2]->count < (*heap)[index * 2 + 1]->count) {
						minmax_index = index * 2;
					}
					else {
						minmax_index = index * 2 + 1;
					}
				}
				else {
					if ((*heap)[index * 2]->count > (*heap)[index * 2 + 1]->count) {
						minmax_index = index * 2;
					}
					else {
						minmax_index = index * 2 + 1;
					}
				}
				//max or min
				if (this->compare((*heap)[index]->count, (*heap)[minmax_index]->count)) {
					this->swap(index, minmax_index);
					heapify(minmax_index);
				}
			}
		}
	}
	void makeHeap() {
		int temp = (heap->size() - 1) / 2;
		for (int i = temp; i > 0; i--) {
			heapify(i);
		}
	}
	void upHeapify(int index) {
		if (index == 1)
			return;
		if (this->compare((*heap)[index / 2]->count, (*heap)[index]->count)) {
			this->swap(index, index / 2);
			upHeapify(index / 2);
		}
	}
	void swap(int a, int b){
		Node* temp((*heap)[a]);
		(*heap)[a] = (*heap)[b];
		(*heap)[b] = temp;
		}
	void add(Node* a) {
		heap->push_back(a);
		this->upHeapify(heap->size() - 1);
	}
	void deleteNode(int index) {
		int last = heap->size() - 1;
		this->swap(index, last);
		heap->pop_back();
		this->heapify(index);
	}
};

class HuffmanInfo {
public:
	char character;
	string code;
	HuffmanInfo(char a, string b):character(a),code(b){}
};
	
void process(Node* b, vector<HuffmanInfo>& a) {
	if (b->parent != NULL) {
		if (b == b->parent->left) {
			b->huffman = b->parent->huffman + "0";
		}
		else {
			b->huffman = b->parent->huffman + "1";
		}
		if (b->parent->parent == NULL) {
			b->huffman.erase(b->huffman.begin());
		}
	}
	if (b->left == NULL && b->right == NULL) {
	a.push_back(HuffmanInfo(b->character, b->huffman));
	}
}
void preOrder(Node* a,vector<HuffmanInfo>& info) {
	if (a == NULL)
		return;
	process(a,info);
	preOrder(a->left,info);
	preOrder(a->right,info);
}

void huffmanFile(string filename,vector<HuffmanInfo> table) {
	string outPut = "Huffman-" + filename;
	fstream huffmanTable;
	huffmanTable.open(outPut, ios::out);
	huffmanTable.close();
	huffmanTable.open(outPut);
	for (int i = 0; i < table.size(); i++) {
		huffmanTable << table[i].character << "\t" << table[i].code.size() << "\t" << table[i].code << endl;
	}
	huffmanTable.close();
}

string find(vector<HuffmanInfo> table,char a) { //find the huffman code for given character
	for (int j = 0; j < table.size(); j++) {
		if (table[j].character == a) {
			return table[j].code;
		}
	}
}

void makeZip(string filename,vector<HuffmanInfo> table) {
	fstream file;
	file.open(filename);
	string buffer;
	char temp;
	while (file)
	{
		file.get(temp);
		buffer += find(table, temp);
		if (file.peek() == EOF)
			break;

	}
	buffer += find(table, '\0');
	if (buffer.size() % 8 != 0) {			//making the size multiple of 8
		for (int i = 8 - (buffer.size()%8); i > 0; i--) {
			buffer += "0";
		}
	}
	file.close();
	string zip = "encoded-" + filename;
	fstream zipFile;
	zipFile.open(zip, ios::out);
	zipFile.close();
	zipFile.open(zip);
	int j = 0;
	string temp1;
	for (int i = buffer.size() / 8; i > 0; i--) {
		temp1.assign(buffer, j, 8);
		j += 8;
		int data = stoi(temp1, nullptr, 2);
		zipFile << (char)data;
	}
	
	zipFile.close();

}

void decode(string fname, vector<HuffmanInfo> table, string chars) {
	fname = "Decoded-" + fname;
	fstream file;
	file.open(fname, ios::out);
	file.close();
	file.open(fname);
	int index=0;
	string str="";
	int eof=0;
	while (true)
	{
		str += chars[index];
		index++;
		for (int i = 0; i < table.size(); i++) {
			if (str == table[i].code) {
				if (table[i].character == '\0') {
					eof = 1;
					break;
				}
				else {
					file << table[i].character;
					str = "";
					break;
				}
			}
		}
		if (eof)
			break;
	}
	file.close();
}


int main() {
	cout << "Welcome\n\nWhat do you want to do?\n1.Encode\n2.Decode\n"<<endl;
	int choice;
	cin >> choice;
	Mode mode = (Mode)(choice - 1);
	if (mode == Mode::ENCODING) {
		string fileName;
		cout << "File name: ";
		cin >> fileName;
		vector<int> count;
		count.assign(256, 0);
		fstream file;
		vector<Node*> heap;
		file.open(fileName);
		char a;
		while (file) {
			file.get(a);
			count[a]++;
			if (file.peek() == EOF)
				break;
		}

		for (int i = 0; i < 256; i++) {
			if (count[i] > 0) {
				heap.push_back(new Node((char)i, count[i]));
			}
		}
		heap.push_back(new Node((char)0, 0)); //add EOF

		heap.push_back(heap[0]);		// index from 1

		Heap minHeap(&heap, HEAP_TYPE::MINHEAP);
		minHeap.makeHeap();


		vector<Node*> huffman_tree;
		int t = 0;
		while (minHeap.heap->size() > 2)
		{
			huffman_tree.push_back(new Node(' ', 0));
			(*(minHeap.heap))[1]->parent = huffman_tree[t];
			huffman_tree[t]->left = (*(minHeap.heap))[1];
			huffman_tree[t]->count += (*(minHeap.heap))[1]->count;
			minHeap.deleteNode(1);
			(*(minHeap.heap))[1]->parent = huffman_tree[t];
			huffman_tree[t]->right = (*(minHeap.heap))[1];
			huffman_tree[t]->count += (*(minHeap.heap))[1]->count;
			minHeap.deleteNode(1);
			minHeap.add(huffman_tree[t]);
			t++;
		}
		t--;
		huffman_tree[t]->huffman = "0";						//huffman code for root(process function)
		vector<HuffmanInfo> table;
		table.reserve(256);
		preOrder(huffman_tree[t], table);
		file.close();

		huffmanFile(fileName, table);
		makeZip(fileName, table);
		return 0;
	}
	else {
		vector<HuffmanInfo> table;
		string huffmanFile;
		string zippedFile;
		char tempchar;
		string tempcode;
		int tempnumber;
		string buffer;
		cout << "Enter the name of the file containing characters and their Huffman values: ";
		cin >> huffmanFile;
		cout << "Enter the name of the encoded file: ";
		cin >> zippedFile;
		ifstream hfile;
		hfile.open(huffmanFile);
		char tab;
		while (hfile)
		{
			hfile.get(tempchar);
			hfile.get(tab);
			hfile >> tempnumber;
			hfile.get(tab);
			hfile >> tempcode;
			hfile.get(tab);
			table.push_back(HuffmanInfo(tempchar, tempcode));
			if (hfile.peek() == EOF)
				break;

		}
		hfile.close();
		
		fstream zfile;
		zfile.open(zippedFile, ios_base::in | ios_base::binary);
		char t;
		while (zfile)
		{
			zfile.read(&t, sizeof(t));
			string s = bitset<8>((int)t).to_string();
			buffer += s;
			if (zfile.peek() == EOF)
				break;
		}
		zfile.close();
		decode(zippedFile, table, buffer);

	}
}