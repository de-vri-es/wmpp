#include <xcb/xcb.h>

#include <vector>

namespace wmpp {

struct Node {
	/// The type of node.
	enum class Type {
		horizontal,
		vertical,
		window,
		empty,
	} type;

	/// The size of the node in pixels.
	int size;

	/// The child nodes.
	std::vector<Node> children;

	bool isParent() const { return type == Type::horizontal || type == Type::vertical; }
	bool isLeaf()   const { return !isParent(); }
};

/// An iterator type that iterates a graph in pre-order.
class PreOrderIterator {
	struct State {
		Node * node;
		std::size_t child;
	};
	std::vector<State> state;

public:
	using value_type        = Node;
	using reference         = value_type &;
	using pointer           = value_type *;
	using iterator_category = std::forward_iterator_tag;

	/// Make a PreOrderIterator representing the end of a sequence.
	PreOrderIterator() {};

	/// Make a PreOrderIterator starting at a given root node.
	PreOrderIterator(Node * root) : state({{root, 0}}) {};

	Node & operator*()  const { return *state.back().node; }
	Node * operator->() const { return  state.back().node; }

	PreOrderIterator & operator++() {
		// Find the first node up the stack which has a next child.
		while (state.back().node->isLeaf() || state.back().child >= state.back().node->children.size()) {
			state.pop_back();
			if (state.empty()) return *this;
		}

		// If we get here the is a next child, so add it to the state stack.
		state.push_back(State{&state.back().node->children[state.back().child++], 0});
		return *this;
	}

	PreOrderIterator operator++(int) {
		PreOrderIterator old = *this;
		operator++();
		return old;
	}

	friend bool operator<  (PreOrderIterator const & lhs, PreOrderIterator const & rhs);
	friend bool operator<= (PreOrderIterator const & lhs, PreOrderIterator const & rhs);
	friend bool operator>  (PreOrderIterator const & lhs, PreOrderIterator const & rhs);
	friend bool operator>= (PreOrderIterator const & lhs, PreOrderIterator const & rhs);
};

bool operator< (PreOrderIterator const & lhs, PreOrderIterator const & rhs) {
	for (std::size_t i = 0; i < std::min(lhs.state.size(), rhs.state.size()); ++i) {
		if (lhs.state[i].child < rhs.state[i].child) return true;
		if (lhs.state[i].child > rhs.state[i].child) return false;
	}
	// Since all child indices so were the same the iterators must point to the same node
	// or nodes from a different sequence in which case the iterators shouldn't have been compared.
	return false;
}

bool operator<= (PreOrderIterator const & lhs, PreOrderIterator const & rhs) {
	for (std::size_t i = 0; i < std::min(lhs.state.size(), rhs.state.size()); ++i) {
		if (lhs.state[i].child < rhs.state[i].child) return true;
		if (lhs.state[i].child > rhs.state[i].child) return false;
	}
	return true;
}

bool operator> (PreOrderIterator const & lhs, PreOrderIterator const & rhs) {
	for (std::size_t i = 0; i < std::min(lhs.state.size(), rhs.state.size()); ++i) {
		if (lhs.state[i].child < rhs.state[i].child) return false;
		if (lhs.state[i].child > rhs.state[i].child) return true;
	}
	return false;
}

bool operator>= (PreOrderIterator const & lhs, PreOrderIterator const & rhs) {
	for (std::size_t i = 0; i < std::min(lhs.state.size(), rhs.state.size()); ++i) {
		if (lhs.state[i].child < rhs.state[i].child) return false;
		if (lhs.state[i].child > rhs.state[i].child) return true;
	}
	return true;
}

template<typename PreFunctor, typename PostFunctor>
void visitNodes(Node & root, PreFunctor pre, PostFunctor post) {
	pre(root);
	if (root.isParent()) {
		for (Node & child : root.children) {
			visit(child, pre, post);
		}
	}
	post(root);
}

template<typename PreFunctor>
void visitNodes(Node & root, PreFunctor pre, std::nullptr_t) {
	pre(root);
	if (root.isParent()) {
		for (Node & child : root.children) {
			visit(child, pre, nullptr);
		}
	}
}

template<typename PostFunctor>
void visitNodes(Node & root, std::nullptr_t, PostFunctor post) {
	if (root.isParent()) {
		for (Node & child : root.children) {
			visit(child, nullptr, post);
		}
	}
	post(root);
}

template<typename Functor>
void visitNodesPreOrder(Node & root, Functor f) {
	visitNodes(root, f, nullptr);
}

template<typename Functor>
void visitNodesPostOrder(Node & root, Functor f) {
	visitNodes(root, nullptr, f);
}

}
