#include <iostream>
#include <fstream>
#include "BTree.h"


int main() {
	BTree<int, 5> x;		
	
	std::ofstream out("tree.txt");	
	std::ifstream in;

	in.open("numbers.txt");
	int t;
	while (in >> t) {
		x.insert(t);
	}
	in.close();
	x.traverse(out);

	in.open("remove.txt");
	while (in >> t) {
		x.remove(t);
	}
	in.close();
	x.traverse(out);

	out.close();

	return 0;
}