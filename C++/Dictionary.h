#pragma once

#include <inttypes.h>
#include <utility>
#include <iostream>


//default key compare
template<class KT>
struct _KeyCompare {
	bool operator()(const KT& lhs, const KT& rhs) const {
		return lhs < rhs;
	}
};

/* 
	Dictionary class. KT is the type of the key, VT is the
	type of the value, KeyComp is they type of the comparator used,
	and defaults to < if no type is given. The Itererator acts as a pointer
	to std::pair<KT,VT>.
*/

template<class KT, class VT,class KeyComp = _KeyCompare<KT>>
class Dictionary
{
private:
	using KeyT = KT;
	using ValT = VT;
	using DataT = std::pair<KeyT, ValT>;
	struct Node;
	using Nodeptr = Node * ;
	using init_t = std::initializer_list< DataT>;
	enum NodeColor {
		BLACK = 0,
		RED = 1,
		NULLNODE = 2,
	};

	//camparison callable
	KeyComp fCompare;

	//garbage node that prevents segfaults, might be bad 
	//because it requires default key and val constructors, but maybe fine
	Nodeptr NIL; 

	//the head of the tree, idk why its called root but it's too late now
	Nodeptr root; 

	//number of elements in the tree
	size_t numElements;
	
	struct Node {
		uint8_t m_color; // first bit is the color, second bit it set on NIL, so we can check if a node is nil.
		Nodeptr left;
		Nodeptr right;
		Nodeptr parent;

		DataT data;

		//create a NIL node, where all the pointers are nonsense and point to itself
		Node() : data() {
			left = right = parent = this; 
			m_color = (BLACK | NULLNODE);
		}

		//create a copy of a node
		Node(const Node& other, Nodeptr NIL) : data(other.data) {
			m_color = other.m_color;
			left = NIL;
			right = NIL;
			parent = NIL;
		}

		//create a new node with copies or moves of key and data elements
		template<class K, class V>
		Node(K&& k, V&& v, Nodeptr NIL) : data( std::move(k), std::move(v) ) {
			m_color = RED;
			left = NIL;
			right = NIL;
			parent = NIL;
		}

		//sets the color of a node
		void setColor(uint8_t c) {
			c &= RED;
			m_color ^= (m_color & RED);
			m_color |= c;
		}
		uint8_t color() const { return (m_color & RED); }
		bool isNil() const { return (m_color & NULLNODE); }
		void print() const { std::cout << data.first << ": " << data.second << std::endl; }
	};

	void init() {
		numElements = 0;
		NIL = new Node();
		root = NIL;
	}

	template<class I>
	void use_list(I&& list) {
		for (auto& pair : list) {
			insert(std::move(pair.first), std::move(pair.second));
		}
	}

	// rotate_left take a node with a nil right subtree and makes the left child of x the head of the tree
	void rotate_left(Nodeptr x) {
		Nodeptr y = x->right;
		x->right = y->left;
		if (!(y->left->isNil())) {
			y->left->parent = x;
		}
		y->parent = x->parent;
		if (x == root) {
			root = y;
		}
		else if (x == x->parent->left) {
			x->parent->left = y;
		}
		else {
			x->parent->right = y;
		}
		y->left = x;
		x->parent = y;
	}
	// rotate_right take a node with a nil left subtree and makes the right child of x the head of the tree
	void rotate_right(Nodeptr x) {
		Nodeptr y = x->left;
		x->left = y->right;
		if (!(y->right->isNil())) {
			y->right->parent = x;
		}
		y->parent = x->parent;
		if (x == root) {
			root = y;
		}
		else if (x == x->parent->right) {
			x->parent->right = y;
		}
		else {
			x->parent->left = y;
		}
		y->right = x;
		x->parent = y;
	}

	void add_fixup(Nodeptr z) {
		while (z->parent->color() == RED) {
			if (z->parent == z->parent->parent->left) { //z->parent is a left child
				Nodeptr y = z->parent->parent->right; 
				if (y->color() == RED) {
					z->parent->setColor(BLACK);
					y->setColor(BLACK);
					z->parent->parent->setColor(RED);
					z = z->parent->parent;
				}
				else {
					if (z == z->parent->right) {
						z = z->parent;
						rotate_left(z);
					}
					z->parent->setColor(BLACK);
					z->parent->parent->setColor(RED);
					rotate_right(z->parent->parent);
				}
			}
			else { // z->parent is a right child
				Nodeptr y = z->parent->parent->left;
				if (y->color() == RED) {
					z->parent->setColor(BLACK);
					y->setColor(BLACK);
					z->parent->parent->setColor(RED);
					z = z->parent->parent;
				}
				else {
					if (z == z->parent->left) {
						z = z->parent;
						rotate_right(z);
					}
					z->parent->setColor(BLACK);
					z->parent->parent->setColor(RED);
					rotate_left(z->parent->parent);
				}
			}
		}
		root->setColor(BLACK);
	}
	bool lessThan(const KeyT& lhs, const KeyT& rhs) const{
		return fCompare(lhs,rhs);
	}

	//yikes zone approaching! at least all 3 are similar
	Nodeptr find_node(const KeyT& k){
		Nodeptr cursor = root; 
		while (cursor->isNil() == false) {
			Nodeptr& next = lessThan(k, cursor->data.first) ? cursor->left : cursor->right; //the next node acording to BST pricipals
			if (next == (cursor)->right) {
				if (lessThan((cursor)->data.first, k) == false) return cursor; // only if < and > are both false are the keys ==
			}
			cursor = next;
		}
		return NIL;
	}
	template<class K>
	ValT& find_or_insert(K&& k) {
		Nodeptr* cursor = &root; //ptr to Nodeptr in order to change the value in place at the end.
		Nodeptr prev = NIL;
		while ((*cursor)->isNil() == false) {
			prev = *cursor;
			Nodeptr& next = lessThan(k, (*cursor)->data.first) ? (*cursor)->left : (*cursor)->right;
			if (next == (*cursor)->right) {
				if (lessThan((*cursor)->data.first, k) == false) return (*cursor)->data.second;
			}
			cursor = &next;
		}
		*cursor = new Node(std::move(k),ValT(),NIL);
		numElements += 1;
		(*cursor)->parent = prev;
		return (*cursor)->data.second;
	}
	void replace_nil_node(Nodeptr node) {
		Nodeptr* cursor = &root;
		Nodeptr prev = NIL;
		while ((*cursor)->isNil() == false) {
			prev = *cursor;
			Nodeptr& next = lessThan(node->data.first, (*cursor)->data.first) ? (*cursor)->left : (*cursor)->right;
			cursor = &next;
		}
		node->parent = prev;
		*cursor = node;
	}
	//yikes zone ends

	void free_nodes(Nodeptr head) {
		if (head->isNil()) return;
		free_nodes(head->left);
		free_nodes(head->right);
		delete head;
	}
	void copy_nodes(Nodeptr& old, Nodeptr parent, const Nodeptr head) {
		if (head->isNil()) return;
		if (old->isNil()) {
			old = new Node(*head, NIL);
			numElements += 1;
		}
		else {
			old->data = head->data;
		}
		old->parent = parent;
		copy_nodes(old->left, old, head->left);
		copy_nodes(old->right, old, head->right);

		
	}

	Nodeptr min(Nodeptr n) const{
		while (!n->left->isNil()) {
			n = n->left;
		}
		return n;
	}
	Nodeptr max(Nodeptr n) const{
		while (!n->right->isNil()) {
			n = n->right;
		}
		return n;
	}

	//puts node v where node u is, used in erase
	void transplant(Nodeptr u, Nodeptr v) {
		if (u == root) {
			root = v;
		}
		else if (u == u->parent->left) {
			u->parent->left = v;
		}
		else {
			u->parent->right = v;
		}
		v->parent = u->parent;
	}
	void delete_fixup(Nodeptr x) {
		while (x->color() == BLACK && x != root) {
			if (x == x->parent->left) {
				Nodeptr w = x->parent->right;
				if (w->color() == RED) {
					w->setColor(BLACK);
					x->parent->setColor(RED);
					rotate_left(x->parent);
					w = x->parent->right;
				}
				if (w->left->color() == BLACK && w->right->color() == BLACK) {
					w->setColor(RED);
					x = x->parent;
				}
				else {
					if (w->right->color() == BLACK) {
						w->left->setColor(BLACK);
						w->setColor(RED);
						rotate_right(w);
						w = x->parent->right;
					}
					w->setColor(x->parent->color());
					x->parent->setColor(BLACK);
					w->right->setColor(BLACK);
					rotate_left(x->parent);
					x = root;
				}
			}
			else { // x is a right child
				Nodeptr w = x->parent->left;
				if (w->color() == RED) {
					w->setColor(BLACK);
					x->parent->setColor(RED);
					rotate_right(x->parent);
					w = x->parent->left;
				}
				if (w->right->color() == BLACK && w->left->color() == BLACK) {
					w->setColor(RED);
					x = x->parent;
				}
				else {
					if (w->left->color() == BLACK) {
						w->right->setColor(BLACK);
						w->setColor(RED);
						rotate_left(w);
						w = x->parent->left;
					}
					w->setColor(x->parent->color());
					x->parent->setColor(BLACK);
					w->left->setColor(BLACK);
					rotate_right(x->parent);
					x = root;
				}
			}
		}
		x->setColor(BLACK);
	}
	void delete_node(Nodeptr z)  {
		Nodeptr y = z;
		auto yColor = y->color();
		Nodeptr x;
		if (z->left->isNil()) {
			x = z->right;
			transplant(z, z->right);
		}
		else if (z->right->isNil()) {
			x = z->left;
			transplant(z, z->left);
		}
		else {
			y = z->right;
			y = min(y);
			yColor = y->color();
			x = y->right;
			if (y->parent == z) {
				x->parent = y;
			}
			else {
				transplant(y, y->right);
				y->right = z->right;
				y->right->parent = y;
			}
			transplant(z, y);
			y->left = z->left;
			y->left->parent = y;
			y->setColor(z->color());
		}
		if (yColor == BLACK) {
			delete_fixup(x);
		}
		delete z;
	}

	template <class ptr, class ref>
	struct iter {
		using iterator_category = std::bidirectional_iterator_tag;
		using difference_type = std::nullptr_t;
		using pointer = ptr;
		using reference = ref;

		iter(Nodeptr _ptr) : m_ptr{ _ptr } {}
		reference operator*() { return m_ptr->data; }
		pointer operator->() { return &(m_ptr->data); }

		
		// Prefix increment
		iter& operator++() { next(); return *this; }

		// Postfix increment
		iter operator++(int) { iter tmp = *this; next(); return tmp; }

		// Prefix increment
		iter& operator--() { prev(); return *this; }

		// Postfix increment
		iter operator--(int) { iter tmp = *this; prev(); return tmp; }


		friend bool operator== (const iter& a, const iter& b) { return a.m_ptr == b.m_ptr; };
		friend bool operator!= (const iter& a, const iter& b) { return a.m_ptr != b.m_ptr; };

	private:
		Nodeptr m_ptr;

		void next() {

			Nodeptr forward = m_ptr->right;
			if (forward->isNil() == false) {

				while (forward->left->isNil() == false) {
					forward = forward->left;
				}
				m_ptr = forward;
			}
			else {
				Nodeptr backtracker = m_ptr->parent;
				while ((backtracker->isNil() == false) && (backtracker->right == m_ptr)) {
					m_ptr = backtracker;
					backtracker = backtracker->parent;
				}
				m_ptr = backtracker;
			}
		}
		void prev() {
			Nodeptr backward = m_ptr->left;
			if (backward->isNil() == false) {
				while (backward->right->isNil() == false) {
					backward = backward->right;
				}
			}
			else {
				Nodeptr backtracker = m_ptr->parent;
				while ((backtracker->isNil() == false) && (backtracker->left == m_ptr)) {
					m_ptr = backtracker;
					backtracker = backtracker->parent;
				}
				m_ptr = backtracker;
			}
		}

	};

public:
	using keytype = KeyT;
	using valtype = ValT;

	Dictionary() : fCompare(){
		init();
	}
	template<class KC,
		class = std::enable_if<std::is_constructible<KeyComp, KC>::value> >
	Dictionary(KeyComp&& comp) : fCompare(std::move(comp)) {
		init();
	}


	Dictionary(const init_t& list) : fCompare() {
		init();
		use_list(list);
		
	}


	//either moves or takes as const reference the key comparison
	template<class KC,
		class = std::enable_if<
			std::is_constructible<KeyComp,KC>::value> >
	Dictionary(const init_t& list, KC&& comp) : fCompare(std::move(comp)) {
		init();
		use_list(list);
	}

	Dictionary(const Dictionary& other) {
		init();
		copy_nodes(root, NIL, other.root);
	}
	Dictionary(Dictionary&& other) : 
		NIL(std::move(other.NIL)), 
		root(std::move(other.root)), 
		numElements{ std::move(other.numElements) } {}


	Dictionary& operator=(const Dictionary& rhs) {
		copy_nodes(root, NIL, rhs.root);
		return *this;
	}
	Dictionary& operator=(const init_t& list) {
		clear();
		use_list(list);
	}
	Dictionary& operator=(init_t&& list) {
		clear();
		use_list(std::move(list));
	}

	~Dictionary() {
		free_nodes(root);
		delete NIL;
	}



	bool contains(const keytype& k) {
		return !(find_node(k)->isNil());
	}

	template<class K, class V,
		class = std::enable_if_t<std::conjunction<
		std::is_constructible<keytype, K>,
		std::is_constructible<valtype, V> >::value> >
	void insert(K&& k, V&& v) {
		Nodeptr n = new Node(std::move(k), std::move(v), NIL);
		replace_nil_node(n);
		add_fixup(n);
		numElements += 1;
	}
	valtype& lookup(const keytype& k) {
		Nodeptr res = find_node(k);
		if (res->isNil()) {
			std::cerr << "Dictionary Error: key not in dict\n";
			exit(1);
		}
		return res->data.second;
	}
	void clear() {
		free_nodes(root);
		root = NIL;
		numElements = 0;
	}
	size_t size() {
		return numElements;
	}
	void erase(const keytype& k) {
		Nodeptr erased = find_node(k);
		if (erased->isNil()) {
			std::cerr << "Dictionary Error: key not in dict\n";
			exit(1);
		}
		delete_node(erased);
		numElements -= 1;
	}

	valtype& operator[](const keytype& k) {
		return find_or_insert(k); //find_or_insert handles numElements
	}
	valtype& operator[](keytype&& k) {
		return find_or_insert(std::move(k)); //find_or_insert handles numElements
	}

public:
	using iterator = iter<DataT *, DataT &>;
	using const_iterator = iter<const DataT *, const DataT &>;

	iterator begin() { return iterator(min(root)); }
	iterator end() { return iterator(NIL); }

	const_iterator begin() const { return const_iterator(min(root)); }
	const_iterator end() const { return const_iterator(NIL); }

};