#include "BTree.h"
#include <iostream>
#include <fstream>

template <typename T,int M>
void BTreeNode<T,M>::snapShot(std::ostream& out) {
	out << '\"' << keys[0];
	for (int n = 1; n < keyTally; n++) {
		out << ':' << keys[n];
	}
	out << '\"';
	if (!leaf) {
		out << '[';
		pointers[0]->snapShot(out);
		for (int n = 1; n <= keyTally; n++) {
			out << ',';
			pointers[n]->snapShot(out);
		}
		out << ']';
	}
}

template <typename T,int M>
void BTree<T, M>::snapShot(std::ostream& out) {
	out << "TreeForm[";
	root->snapShot(out);
	out << ']' << std::endl;
}

int main() {
	std::ifstream one("numbers.txt");
	BTree<int, 6> tree;
	int x;
	while (one >> x) {
		tree.insert(x);
	}
	tree.snapShot(std::cout);
	one.close();
	std::ifstream two("remove.txt");
	while (two >> x) {
		tree.remove(x);
	}
	tree.snapShot(std::cout);
	two.close();
	
	return 0;
}