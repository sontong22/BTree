template <typename T, int M>
class BTree;

template <typename T, int M>
class BTreeNode {
	friend class BTree<T, M>;
public:
	BTreeNode() {}
	BTreeNode(const T& el){
		keyTally = 1;
		keys[0] = el;
	}

	bool isFull() {
		return keyTally == M - 1;
	}
	int indexMiddle() {
		return (M - 1) / 2;
	}
	bool isUnderflow() {
		return keyTally < (M - 1) / 2;
	}
	bool shareable() {
		return keyTally > (M - 1) / 2;
	}
private:
	bool leaf;
	int keyTally;
	T keys[M - 1];
	BTreeNode* pointers[M];
};

template <typename T, int M>
class BTree {
public:
	BTree() { root = nullptr; }

	void insert(const T& key) {		
		if (root == nullptr) { // Tree is empty
			root = new BTreeNode<T, M>(key);
			root->leaf = true;
		}
		else {
			BTreeNode<T, M>* toInsert = root;
			BTreeNode<T, M>* parent = nullptr;
			int i = 0;

			// Find the leaf to insert key and split along the way
			while (!toInsert->leaf || toInsert->isFull()) {
				if (toInsert->isFull())
					toInsert = split(parent, i);
				
				int j = 0;
				if (key > toInsert->keys[toInsert->keyTally - 1])
					j = toInsert->keyTally;
				else {
					while (j < toInsert->keyTally && !(key < toInsert->keys[j])) {
						if (key == toInsert->keys[j]) // Do not insert duplicate value
							return; 
						else
							j++;
					}
				}

				i = j;
				parent = toInsert;
				toInsert = toInsert->pointers[i];				
			}

			// Add key to leaf
			addKey(toInsert, key);
		}
	}

	void remove(const T& key) {
		remove(nullptr, -1, key);
	}

	void traverse(std::ostream& out) {
		traverse(out, root);
		out << std::endl;
	}	

private:
	BTreeNode<T, M>* root;

	void addKey(BTreeNode<T, M>* toInsert, const T& key) {\
		int i = 0;
		if (key > toInsert->keys[toInsert->keyTally - 1])
			i = toInsert->keyTally;
		else {
			while (i < toInsert->keyTally && !(key < toInsert->keys[i])) {
				if (key == toInsert->keys[i]) // Do not insert duplicate
					return;
				else
					i++;
			}
		}

		toInsert->keyTally++;
		for (int j = toInsert->keyTally; j > i; j--)
			toInsert->keys[j] = toInsert->keys[j - 1];
		toInsert->keys[i] = key;
	}

	BTreeNode<T, M>* split(BTreeNode<T, M>* parent, int index) {
		BTreeNode<T, M>* toSplit;
		if (parent == nullptr)
			toSplit = root;
		else
			toSplit = parent->pointers[index];

		int m = toSplit->indexMiddle();		

		BTreeNode<T, M>* newNode = new BTreeNode<T, M>();
		for (int i = m + 1, j = 0; i < M - 1; i++, j++) {
			newNode->keys[j] = toSplit->keys[i];
			newNode->pointers[j] = toSplit->pointers[i];		
		}
		toSplit->keyTally = m;
		newNode->keyTally = M - m - 2;

		newNode->pointers[newNode->keyTally] = toSplit->pointers[M - 1];

		newNode->leaf = toSplit->leaf;

		if (toSplit == root) { // Splitting root
			BTreeNode<T, M>* newRoot = new BTreeNode<T, M>(toSplit->keys[m]);
			newRoot->pointers[0] = toSplit;
			newRoot->pointers[1] = newNode;
			newRoot->leaf = false;
			root = newRoot;

			return root;
		}
		else {
			for (int i = parent->keyTally; i > index; i--) {
				parent->keys[i] = parent->keys[i - 1];
				parent->pointers[i + 1] = parent->pointers[i];
			}
			parent->keys[index] = toSplit->keys[m];
			parent->pointers[index + 1] = newNode;
			parent->keyTally++;

			return parent;
		}		
	}

	bool removeFromLeaf(BTreeNode<T, M>* leaf, const T& key) {
		for(int i = 0; i < leaf->keyTally; i++)
			if (leaf->keys[i] == key) {
				leaf->keyTally--;
				while (i < leaf->keyTally) {
					leaf->keys[i] = leaf->keys[i + 1];
					i++;
				}
				return true;
			}
		return false;
	}

	void borrowRight(BTreeNode<T, M>* parent, int i) {
		BTreeNode<T, M>* receiver = parent->pointers[i];
		receiver->keys[receiver->keyTally] = parent->keys[i];
		receiver->keyTally++;

		BTreeNode<T, M>* donor = parent->pointers[i + 1];
		parent->keys[i] = donor->keys[0];
		for (int i = 0; i < donor->keyTally - 1; i++) 
			donor->keys[i] = donor->keys[i + 1];		
		donor->keyTally--;

		if (!receiver->leaf) {
			receiver->pointers[receiver->keyTally] = donor->pointers[0];
			for (int i = 0; i <= donor->keyTally; i++)
				donor->pointers[i] = donor->pointers[i + 1];
		}
	}

	void borrowLeft(BTreeNode<T, M>* parent, int i) {
		BTreeNode<T, M>* receiver = parent->pointers[i];
		for (int i = receiver->keyTally; i > 0; i--)
			receiver->keys[i] = receiver->keys[i - 1];
		receiver->keys[0] = parent->keys[i - 1];
		receiver->keyTally++;

		BTreeNode<T, M>* donor = parent->pointers[i - 1];
		parent->keys[i - 1] = donor->keys[donor->keyTally - 1];		
		donor->keyTally--;

		if (!receiver->leaf) {			
			for (int i = receiver->keyTally; i > 0; i--)
				receiver->pointers[i] = receiver->pointers[i - 1];
			receiver->pointers[0] = donor->pointers[donor->keyTally + 1];
		}
	}

	void mergeRight(BTreeNode<T, M>* parent, int i) {
		BTreeNode<T, M>* toMerge = parent->pointers[i];
		BTreeNode<T, M>* sibling = parent->pointers[i + 1];

		int index = toMerge->keyTally;

		toMerge->keys[toMerge->keyTally] = parent->keys[i];
		toMerge->keyTally++;
		parent->keyTally--;
		
		for (int i = 0, j = toMerge->keyTally; i < sibling->keyTally; i++, j++) {
			toMerge->keys[j] = sibling->keys[i];
			toMerge->keyTally++;
		}
		
		while (i < parent->keyTally) {
			parent->keys[i] = parent->keys[i + 1];
			parent->pointers[i + 1] = parent->pointers[i + 2];
			i++;
		}

		// When toMerge is an internal node
		if (!toMerge->leaf) {
			index++;
			for (int i = 0; i <= sibling->keyTally; i++, index++)
				toMerge->pointers[index] = sibling->pointers[i];
		}

		// When merge with root as parent
		if (parent == root && parent->keyTally == 0)
			root = toMerge;
	}

	void mergeLeft(BTreeNode<T, M>* parent, int i) {
		mergeRight(parent, i - 1);
	}

	bool fixUnderflow(BTreeNode<T, M>* parent, int index) {
		if (parent == nullptr)
			return false;
		BTreeNode<T, M>* node = parent->pointers[index];

		if (node->isUnderflow()) {
			// Check redistribution
			if (index < parent->keyTally) { // node has right sibling
				if (parent->pointers[index + 1]->shareable()) {
					borrowRight(parent, index);
					return false;
				}
			}
			if (index > 0) { // node has left sibling
				if (parent->pointers[index - 1]->shareable()) {
					borrowLeft(parent, index);
					return false;
				}
			}
			// Merge
			if (index < parent->keyTally) 
				mergeRight(parent, index);							
			else 
				mergeLeft(parent, index);		
			return true;
		}
		return false;
	}

	bool remove(BTreeNode<T, M>* parent, int index, const T& key) {
		if (root == nullptr) // Tree is empty
			return false;

		BTreeNode<T, M>* node;
		if (parent == nullptr) 
			node = root;		
		else
			node = parent->pointers[index];

		if (node->leaf) {
			if (removeFromLeaf(node, key)) 
				return fixUnderflow(parent, index);			
			return false;
		}
		else {
			int i = 0;
			if (key > node->keys[node->keyTally - 1])
				i = node->keyTally;
			else {				
				while (i < node->keyTally && !(key < node->keys[i])) {
					if (key == node->keys[i]){  // Delete a key in an internal node (by copy)
						// Find its predecessor
						BTreeNode<T, M>* child = node->pointers[i + 1];
						while (!child->leaf)
							child = child->pointers[0];

						node->keys[i] = child->keys[0];
						child->keys[0] = key;

						if (remove(node, i + 1, key))
							return fixUnderflow(node, i + 1);
						else
							return false;
					}						
					else
						i++;
				}
			}
			// Move down
			if (remove(node, i, key)) 
				return fixUnderflow(parent, index);				
			else
				return false;			
		}			
	}

	void traverse(std::ostream& out, BTreeNode<T, M>* node) {
		if (node == nullptr)
			return;
		if (node->leaf)
			for (int i = 0; i < node->keyTally; i++)
				out << node->keys[i] << ", ";			
		else {
			for (int i = 0; i < node->keyTally; i++) {
				traverse(out, node->pointers[i]);
				out << node->keys[i] << ", ";				
			}
			traverse(out, node->pointers[node->keyTally]);
		}
	}
};
