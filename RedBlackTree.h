#ifndef UTIL_REDBLACKTREE_H
#define UTIL_REDBLACKTREE_H

#include <functional>
#include <cassert>
#include <limits>

namespace util
{
	template< class _KeyT,
	          class _LessT = std::less< _KeyT > >
	class RedBlackTree
	{
		public:

		using KeyT = _KeyT;
		using LessT = _LessT;
		using SizeT = std::size_t;
		using ByteT = std::uint8_t;

		class Node
		{
			public:

			friend RedBlackTree< KeyT, LessT >;

			Node * getChild( bool left )
			{
				return m_children[ ByteT( !left ) ];
			}

			Node const * getChild( bool left ) const
			{
				return m_children[ ByteT( !left ) ];
			}

			Node * getLeftChild()
			{
				return m_children[ 0 ];
			}

			Node const * getLeftChild() const
			{
				return m_children[ 0 ];
			}

			Node * getRightChild()
			{
				return m_children[ 1 ];
			}

			Node const * getRightChild() const
			{
				return m_children[ 1 ];
			}

			Node * getParent()
			{
				return m_parent;
			}

			Node const * getParent() const
			{
				return m_parent;
			}

			Node * getPrevious() const
			{
				Node * node = this;
				
				// Check for right child.
				if( m_children[ 0 ] != nullptr )
				{
					node = m_children[ 0 ];

					while( node->m_children[ 1 ] != nullptr )
					{
						node = node->m_children[ 1 ];
					}
					return node;
				}

				// No left child so traverse upwards.
				while( node->m_parent != nullptr )
				{
					if( node->isGreaterThanParent() )
					{
						return node->m_parent;
					}
					node = node->m_parent;
				}
				return this;
			}
			
			Node const * getPrevious() const
			{
				return const_cast< Node * >( this )->getPrevious();
			}
			
			Node * getNext()
			{
				Node * node = this;
				
				// Check for right child.
				if( m_children[ 1 ] != nullptr )
				{
					node = m_children[ 1 ];

					while( node->m_children[ 0 ] != nullptr )
					{
						node = node->m_children[ 0 ];
					}
					return node;
				}

				// No right child so traverse upwards.
				while( node->m_parent != nullptr )
				{
					if( node->isLessThanParent() )
					{
						return node->m_parent;
					}
					node = node->m_parent;
				}
				return this;
			}

			Node const * getNext() const
			{
				return const_cast< Node * >( this )->getNext();
			}

			KeyT const & getKey() const
			{
				return m_key;
			}

			private:

			static ByteT constexpr RED_INDEX	= 0;
			static ByteT constexpr LESS_INDEX	= 1;

			static ByteT constexpr RED	= 1 << RED_INDEX;
			static ByteT constexpr LESS	= 1 << LESS_INDEX;

			Node( KeyT const & key = KeyT{} ): 
				m_children{ nullptr, nullptr },
			    m_parent{ nullptr },
			    m_type{ 0 },
			    m_key{ key }
			{}

			Node( Node const & other ): 
				m_children{ other.m_children[ 0 ], other.m_children[ 1 ] },
			    m_parent{ other.m_parent },
			    m_type{ other.m_type },
			    m_key{ other.m_key }
			{}

			Node( Node && other ): 
				m_children{ other.m_children[ 0 ], other.m_children[ 1 ] },
			    m_parent{ other.m_parent },
			    m_type{ other.m_type },
			    m_key{ other.m_key }
			{}

			~Node()
			{}

			void setChild( bool left, Node * child )
			{
				m_children[ ByteT( !left ) ] = child;
			}

			void setLeftChild( Node * child )
			{
				m_children[ 0 ] = child;
			}

			void setRightChild( Node * child )
			{
				m_children[ 1 ] = child;
			}

			void setParent( Node * parent )
			{
				m_parent = parent;
			}

			bool isRed() const
			{
				return ( ( m_type & RED ) > 0 );
			}

			void setRed( bool isRed = true )
			{
				m_type &= ~RED;
				m_type |= ( ByteT( isRed ) << RED_INDEX );
			}

			bool isBlack() const
			{
				return !isRed();
			}

			void setBlack( bool isBlack = true )
			{
				setRed( !isBlack );
			}

			bool isLessThanParent() const
			{
				return ( ( m_type & LESS ) > 0 );
			}

			void setLessThanParent( bool isLess = true )
			{
				m_type &= ~LESS;
				m_type |= ( ByteT( isLess ) << LESS_INDEX );
			}

			bool isGreaterThanParent() const
			{
				return !isLessThanParent();
			}

			void setGreaterThanParent( bool isGreater = true )
			{
				setLessThanParent( !isGreater );
			}

			Node & operator=( Node const & other )
			{
				m_children[ 0 ] = other.m_children[ 0 ];
				m_children[ 1 ] = other.m_children[ 1 ];
				m_parent		= other.m_parent;
				m_type			= other.m_type;
				m_key			= other.m_key;

				return *this;
			}

			Node & operator=( Node && other )
			{
				m_children[ 0 ] = other.m_children[ 0 ];
				m_children[ 1 ] = other.m_children[ 1 ];
				m_parent		= other.m_parent;
				m_type			= other.m_type;
				m_key			= other.m_key;

				return *this;
			}

			Node *	m_children[ 2 ];
			Node *	m_parent;
			ByteT	m_type;
			KeyT	m_key;
		};

		RedBlackTree():
			m_root{ nullptr },
			m_size{ 0 }
		{}

		RedBlackTree( std::initializer_list< KeyT > keys ):
			RedBlackTree()
		{
			for( auto & key: keys )
			{
				add( key );
			}
		}

		template< class _FirstIter, class _LastIter >
		RedBlackTree( _FirstIter first, _LastIter last ):
			RedBlackTree()
		{
			while( first != last )
			{
				add( *first );
				++first;
			}
		}

		bool add( KeyT const & key, Node * & node )
		{
			// Check for empty container.
			if( m_root == nullptr )
			{
				m_root = new Node( key );
				m_size = 1;
				node = m_root;
				return true;
			}

			// Search for matching node.
			Node * nearest = nullptr;
			bool lessThan = false;

			if( find( key, nearest, lessThan ) )
			{
				node = nearest;
				return false;
			}

			// Matching node not found, create new node.
			node = new Node( key );
			node->setParent( nearest );
			node->setLessThanParent( lessThan );
			m_size += 1;

			// Add node to tree.
			Node * freeNode = node;

			while( freeNode != m_root )
			{
				freeNode = add( freeNode );
			}
			return true;
		}

		Node * add( KeyT const & key )
		{
			Node * node = nullptr;
			add( key, node );
			return node;
		}

		bool remove( KeyT const & key, Node * & next )
		{
			Node * node = find( key );

			if( node == nullptr )
			{
				return false;
			}
			return remove( node, next );
		}

		bool remove( Node * node, Node * & next )
		{
			assert( node != nullptr );

			m_size -= 1;

			// Check for leaf node.
			next = getNextLargestChild( node );

			if( next != nullptr )
			{
				// Node is not a leaf participant.
				swap( node, next );

				if( node == m_root )
				{
					m_root = next;
				}
			}

			// Node is a leaf participant.
			Node * parent = node->getParent();
			Node * left = node->getLeftChild();

			if( parent != nullptr )
			{
				// Set next pointer to next largest parent of leaf.
				if( node->isLessThanParent() )
				{
					next = parent;
				}
				else
				{
					next = getNextLargestParent( parent );
				}

				// Check colour of node.
				if( node->isBlack() )
				{
					if( left != nullptr && left->isRed() )
					{
						// Node is black participant in a leaf 3-node.
						assert( left->getLeftChild() == nullptr );

						setChild( parent, left, node->isLessThanParent() );
						left->setBlack();
						delete node;
					}
					else
					{
						// Node is leaf 2-node.
						assert( node != m_root );

						while( node != m_root )
						{
							node = rem( node );
						}
					}
				}
				else
				{
					// Node is red participant in a leaf 3-node.
					assert( left == nullptr );
					parent->setLeftChild( nullptr );
					delete node;
				}
			}
			else
			{
				// Node is both leaf and root.
				assert( node->isBlack() );

				if( left != nullptr && left->isRed() )
				{
					// Root node is a 3-node.
					assert( m_size == 1 );
					left->setParent( nullptr );
					left->setBlack();
				}
				m_root = left;
				delete node;
			}
			return true;
		}

		bool remove( KeyT const & key )
		{
			Node * next = nullptr;
			return remove( key, next );
		}

		bool remove( Node * node )
		{
			Node * next = nullptr;
			return remove( node, next );
		}

		Node * find( KeyT const & key )
		{
			Node * node = m_root;

			while( node != nullptr )
			{
				if( less( key, node->getKey() ) )
				{
					node = node->getLeftChild();
				}
				else if( less( node->getKey(), key ) )
				{
					node = node->getRightChild();
				}
				else
				{
					break;
				}
			}
			return node;
		}

		Node const * find( KeyT const & key ) const
		{
			return const_cast< RedBlackTree * >( this )->find( key );
		}

		SizeT getSize() const
		{
			return m_size;
		}

		bool validate() const
		{
			Node * node = m_root;
			Node * next = nullptr;

			if( node != nullptr )
			{
				// Ensure root is black.
				if( !( node->isBlack() ) )
				{
					return false;
				}
				next = node->getPrevious();
			}
			else
			{
				return true;
			}

			// Go to smallest value node.
			while( node != next )
			{
				node = next;
				next = node->getPrevious();
			}
			// Iterate through all nodes.
			next = node->getNext();

			while( node != next )
			{
				Node * parent = node->getParent();
				Node * left = node->getLeftChild();
				Node * right = node->getRightChild();
				bool lessThan = node->isLessThanParent();
				bool black = node->isBlack();

				if( parent != nullptr )
				{
					if( lessThan )
					{
						if( !( less( node->getKey(), parent->getKey() ) ) )
						{
							return false;
						}
					}
					else
					{
						if( less( node->getKey(), parent->getKey() ) )
						{
							return false;
						}
					}
				}

				if( !black )
				{
					if( parent == nullptr )
					{
						return false;
					}

					if( !( parent->isBlack() ) )
					{
						return false;
					}

					if( !lessThan )
					{
						return false;
					}
					
					if( !( less( node->getKey(), parent->getKey() ) ) )
					{
						return false;
					}
				}

				if( left != nullptr )
				{
					if( left->isRed() )
					{
						if( right != nullptr )
						{
							if( left->getLeftChild() == nullptr )
							{
								return false;
							}
							else if( left->getRightChild() == nullptr )
							{
								return false;
							}
						}
					}
					else if( right == nullptr )
					{
						return false;
					}
				}

				if( right != nullptr && left == nullptr )
				{
					return false;
				}
				node = next;
				next = node->getNext();
			}
			return true;
		}

		static bool less( KeyT const & k1, KeyT const & k2 )
		{
			static LessT lessFunc;
			return lessFunc( k1, k2 );
		}

		private:

		bool find( KeyT const & key, Node * & nearest, bool & lessThan )
		{
			Node * nextNode = m_root;
			nearest = nullptr;

			while( nextNode != nullptr )
			{
				nearest = nextNode;
				
				if( less( key, nextNode->getKey() ) )
				{
					nextNode = nextNode->getLeftChild();
					lessThan = true;
				}
				else if( less( nextNode->getKey(), key ) )
				{
					nextNode = nextNode->getRightChild();
					lessThan = false;
				}
				else
				{
					lessThan = true;
					return true;
				}
			}
			return false;
		}
		
		Node * getNextLargestParent( Node * node )
		{
			assert( node != nullptr );

			Node * next = node->getParent();

			while( next != nullptr )
			{
				if( node->isLessThanParent() )
				{
					return next;
				}
				node = next;
				next = node->getParent();
			}
			return nullptr;
		}

		Node * getNextLargestChild( Node * node )
		{
			assert( node != nullptr );

			Node * next = node->getRightChild();

			if( next == nullptr )
			{
				return nullptr;
			}

			do
			{
				node = next;
				next = node->getLeftChild();
			}
			while( next != nullptr );

			return node;
		}

		void swap( Node * first, Node * second )
		{
			Node * node1 = first->getLeftChild();
			Node * node2 = second->getLeftChild();

			if( node1 != nullptr )
			{
				node1->setParent( second );
			}

			if( node2 != nullptr )
			{
				node2->setParent( first );
			}
			first->setLeftChild( node2 );
			second->setLeftChild( node1 );
			node1 = first->getRightChild();
			node2 = second->getRightChild();

			if( node1 != nullptr )
			{
				node1->setParent( second );
			}

			if( node2 != nullptr )
			{
				node2->setParent( first );
			}
			first->setRightChild( node2 );
			second->setRightChild( node1 );
			node1 = first->getParent();
			node2 = second->getParent();

			if( node1 != nullptr )
			{
				node1->setChild( first->isLessThanParent(), second );
			}

			if( node2 != nullptr )
			{
				node2->setChild( second->isLessThanParent(), first );
			}
			first->setParent( node2 );
			second->setParent( node1 );
			bool flag = first->isRed();
			first->setRed( second->isRed() );
			second->setRed( flag );
			flag = first->isLessThanParent();
			first->setLessThanParent( second->isLessThanParent() );
			second->setLessThanParent( flag );
		}

		void setChild( Node * parent, Node * child, bool lessThan )
		{
			assert( parent != nullptr );
			
			if( child != nullptr )
			{
				child->setParent( parent );
				child->setLessThanParent( lessThan );
			}
			parent->setChild( lessThan, child );
		}

		void setLeftChild( Node * parent, Node * child )
		{
			assert( parent != nullptr );
			
			if( child != nullptr )
			{
				child->setParent( parent );
				child->setLessThanParent();
			}
			parent->setLeftChild( child );
		}

		void setRightChild( Node * parent, Node * child )
		{
			assert( parent != nullptr );
			
			if( child != nullptr )
			{
				child->setParent( parent );
				child->setGreaterThanParent();
			}
			parent->setRightChild( child );
		}

		Node * add( Node * node )
		{
			Node * target = node->getParent();
			assert( target != nullptr );

			if( target->isBlack() )
			{
				if( node->isLessThanParent() )
				{
					return addTwoLeft( node );
				}
				else
				{
					Node * left = target->getLeftChild();

					if( left == nullptr || left->isBlack() )
					{
						return addTwoRight( node );
					}
					else
					{
						return addThreeRight( node );
					}
				}
			}
			else
			{
				if( node->isLessThanParent() )
				{
					return addThreeLeft( node );
				}
				else
				{
					return addThreeMiddle( node );
				}
			}
			// Thread must never reach this point.
			assert( false );
			return nullptr;
		}

		Node * addTwoLeft( Node * node )
		{
			assert( node != nullptr );
			assert( node->isBlack() );
			assert( node->isLessThanParent() );
			
			Node * right = node->getParent();
			assert( right != nullptr );
			assert( right->getLeftChild() == nullptr );
			assert( right->isBlack() );

			right->setLeftChild( node );
			node->setLessThanParent();
			node->setRed();
			return m_root;
		}

		Node * addTwoRight( Node * node )
		{
			assert( node != nullptr );
			assert( node->isBlack() );
			assert( node->isGreaterThanParent() );
			
			Node * left = node->getParent();
			assert( left != nullptr );
			assert( left->getRightChild() == nullptr );
			assert( left->isBlack() );

			Node * child = node->getLeftChild();
			left->setRightChild( child );

			if( child != nullptr )
			{
				child->setParent( left );
				child->setGreaterThanParent();
			}
			Node * parent = left->getParent();
			bool lessThan = left->isLessThanParent();

			if( parent != nullptr )
			{
				parent->setChild( lessThan, node );
			}
			else
			{
				m_root = node;
			}
			node->setParent( parent );
			node->setLessThanParent( lessThan );
			node->setLeftChild( left );
			left->setParent( node );
			left->setLessThanParent();
			left->setRed();
			return m_root;
		}

		Node * addThreeLeft( Node * node )
		{
			assert( node != nullptr );
			assert( node->isBlack() );
			assert( node->isLessThanParent() );
			
			Node * middle = node->getParent();
			assert( middle != nullptr );
			assert( middle->getLeftChild() == nullptr );
			assert( middle->isLessThanParent() );
			assert( middle->isRed() );

			Node * right = middle->getParent();
			assert( right != nullptr );
			assert( right->isBlack() );

			Node * parent = right->getParent();
			bool lessThan = right->isLessThanParent();

			if( parent != nullptr )
			{
				parent->setChild( lessThan, nullptr );
			}
			else
			{
				m_root = middle;
			}
			middle->setParent( parent );
			middle->setLessThanParent( lessThan );
			Node * child = middle->getRightChild();

			if( child != nullptr )
			{
				child->setParent( right );
				child->setLessThanParent();
			}
			right->setLeftChild( child );
			right->setParent( middle );
			right->setGreaterThanParent();
			middle->setRightChild( right );
			middle->setLeftChild( node );
			middle->setBlack();
			return middle;
		}

		Node * addThreeMiddle( Node * node )
		{
			assert( node != nullptr );
			assert( node->isBlack() );
			assert( node->isGreaterThanParent() );

			Node * left = node->getParent();
			assert( left != nullptr );
			assert( left->getRightChild() == nullptr );
			assert( left->isLessThanParent() );
			assert( left->isRed() );

			Node * right = left->getParent();
			assert( right != nullptr );
			assert( right->isBlack() );

			Node * parent = right->getParent();
			bool lessThan = right->isLessThanParent();

			if( parent != nullptr )
			{
				parent->setChild( lessThan, nullptr );
			}
			else
			{
				m_root = node;
			}
			node->setParent( parent );
			node->setLessThanParent( lessThan );
			Node * child = node->getLeftChild();

			if( child != nullptr )
			{
				child->setParent( left );
				child->setGreaterThanParent();
			}
			left->setRightChild( child );
			child = node->getRightChild();

			if( child != nullptr )
			{
				child->setParent( right );
				child->setLessThanParent();
			}
			right->setLeftChild( child );
			node->setLeftChild( left );
			node->setRightChild( right );
			left->setParent( node );
			right->setParent( node );
			left->setLessThanParent();
			right->setGreaterThanParent();
			left->setBlack();
			return node;
		}

		Node * addThreeRight( Node * node )
		{
			assert( node != nullptr );
			assert( node->isBlack() );
			assert( node->isGreaterThanParent() );

			Node * middle = node->getParent();
			assert( middle != nullptr );
			assert( middle->isBlack() );
			assert( middle->getRightChild() == nullptr );

			Node * left = middle->getLeftChild();
			assert( left != nullptr );
			assert( left->isRed() );
			assert( left->isLessThanParent() );

			left->setBlack();
			middle->setRightChild( node );
			Node * parent = middle->getParent();
			bool lessThan = middle->isLessThanParent();

			if( parent != nullptr )
			{
				parent->setChild( lessThan, nullptr );
			}
			else
			{
				m_root = middle;
			}
			return middle;
		}

		Node * rem( Node * node )
		{
			Node * parent = node->getParent();
			assert( parent != nullptr );

			if( parent->isBlack() )
			{
				if( node->isLessThanParent() )
				{
					return remTwoLeft( node );
				}
				else
				{
					Node * leftChild = parent->getLeftChild();

					if( leftChild != nullptr && leftChild->isRed() )
					{
						return remThreeRight( node );
					}
					else
					{
						return remTwoRight( node );
					}
				}
			}
			else if( node->isLessThanParent() )
			{
				return remThreeLeft( node );
			}
			return remThreeMiddle( node );
		}

		Node * remTwoLeft( Node * node )
		{
			Node * sibling = node->getParent()->getRightChild();
			assert( sibling != nullptr );

			Node * nephew = sibling->getLeftChild();

			if( nephew != nullptr && nephew->isRed() )
			{
				// Sibling is a 3-node.
				return remTwoLeft_Three( node );
			}
			// Sibling is a 2-node.
			return remTwoLeft_Two( node );
		}

		Node * remTwoLeft_Two( Node * node )
		{
			Node * a = node->getParent();
			assert( a != nullptr && a->isBlack() );

			Node * b = a->getRightChild();
			assert( b != nullptr && b->isBlack() );

			Node * x = a->getParent();
			bool xLess = a->isLessThanParent();
			setLeftChild( a, node->getLeftChild() );
			setRightChild( a, b->getLeftChild() );
			setLeftChild( b, a );
			a->setRed();

			if( x != nullptr )
			{
				node->setLeftChild( b );
				node->setParent( x );
				node->setLessThanParent( xLess );
				return node;
			}
			b->setParent( nullptr );
			m_root = b;
			delete node;
			return m_root;
		}

		Node * remTwoLeft_Three( Node * node )
		{
			Node * a = node->getParent();
			assert( a != nullptr && a->isBlack() );

			Node * c = a->getRightChild();
			assert( c != nullptr && c->isBlack() );

			Node * b = c->getLeftChild();
			assert( b != nullptr && b->isRed() );

			Node * x = a->getParent();
			bool xLess = a->isLessThanParent();
			setLeftChild( a, node->getLeftChild() );
			setRightChild( a, b->getLeftChild() );
			setLeftChild( c, b->getRightChild() );
			setLeftChild( b, a );
			setRightChild( b, c );
			b->setBlack();

			if( x != nullptr )
			{
				setChild( x, b, xLess );
			}
			else
			{
				b->setParent( nullptr );
				m_root = b;
			}
			delete node;
			return m_root;
		}

		Node * remTwoRight( Node * node )
		{
			Node * sibling = node->getParent()->getLeftChild();
			assert( sibling != nullptr );

			Node * nephew = sibling->getLeftChild();

			if( nephew != nullptr && nephew->isRed() )
			{
				// Sibling is a 3-node.
				return remTwoRight_Three( node );
			}
			// Sibling is a 2-node.
			return remTwoRight_Two( node );
		}

		Node * remTwoRight_Two( Node * node )
		{
			Node * a = node->getParent();
			assert( a != nullptr && a->isBlack() );

			Node * b = a->getLeftChild();
			assert( b != nullptr && b->isBlack() );

			Node * x = a->getParent();
			bool xLess = a->isLessThanParent();
			setRightChild( a, node->getLeftChild() );
			b->setRed();

			if( x != nullptr )
			{
				node->setLeftChild( a );
				node->setParent( x );
				node->setLessThanParent( xLess );
				return node;
			}
			a->setParent( nullptr );
			m_root = a;
			delete node;
			return m_root;
		}

		Node * remTwoRight_Three( Node * node )
		{
			Node * a = node->getParent();
			assert( a != nullptr && a->isBlack() );

			Node * c = a->getLeftChild();
			assert( c != nullptr && c->isBlack() );

			Node * b = c->getLeftChild();
			assert( b != nullptr && b->isRed() );

			Node * x = a->getParent();
			bool xLess = a->isLessThanParent();
			setRightChild( a, node->getLeftChild() );
			setLeftChild( a, c->getRightChild() );
			setRightChild( c, a );
			b->setBlack();

			if( x != nullptr )
			{
				setChild( x, c, xLess );
			}
			else
			{
				c->setParent( nullptr );
				m_root = c;
			}
			delete node;
			return m_root;
		}

		Node * remThreeLeft( Node * node )
		{
			Node * a = node->getParent();
			assert( a != nullptr && a->isRed() );

			Node * d = a->getRightChild();
			assert( d != nullptr && d->isBlack() );

			Node * c = d->getLeftChild();

			if( c != nullptr && c->isRed() )
			{
				// Middle is 3-node.
				return remThreeLeft_Three_X( node );
			}
			// Middle is 2-node.
			return remThreeLeft_Two_X( node );
		}
		
		Node * remThreeLeft_Two_X( Node * node )
		{
			Node * a = node->getParent();
			assert( a != nullptr && a->isRed() );

			Node * b = a->getParent();
			assert( b != nullptr && b->isBlack() );

			Node * c = a->getRightChild();
			assert( c != nullptr && c->isBlack() );

			setLeftChild( a, node->getLeftChild() );
			setRightChild( a, c->getLeftChild() );
			setLeftChild( c, a );
			setLeftChild( b, c );

			delete node;
			return m_root;
		}

		Node * remThreeLeft_Three_X( Node * node )
		{
			Node * a = node->getParent();
			assert( a != nullptr && a->isRed() );

			Node * b = a->getParent();
			assert( b != nullptr && b->isBlack() );

			Node * d = a->getRightChild();
			assert( d != nullptr && d->isBlack() );

			Node * c = d->getLeftChild();
			assert( c != nullptr && c->isRed() );

			setLeftChild( a, node->getLeftChild() );
			setRightChild( a, c->getLeftChild() );
			setLeftChild( d, c->getRightChild() );
			setLeftChild( c, a );
			setRightChild( c, d );
			setLeftChild( b, c );
			a->setBlack();

			delete node;
			return m_root;
		}
		
		Node * remThreeMiddle( Node * node )
		{
			Node * a = node->getParent();
			assert( a != nullptr && a->isRed() );

			Node * d = a->getLeftChild();
			assert( d != nullptr && d->isBlack() );

			Node * c = d->getLeftChild();

			if( c != nullptr && c->isRed() )
			{
				// Left sibling is 3-node.
				return remThreeMiddle_Three_X( node );
			}
			// Left sibling is 2-node.
			return remThreeMiddle_Two_X( node );
		}

		Node * remThreeMiddle_Two_X( Node * node )
		{
			Node * a = node->getParent();
			assert( a != nullptr && a->isRed() );

			Node * b = a->getParent();
			assert( b != nullptr && b->isBlack() );

			Node * c = a->getLeftChild();
			assert( c != nullptr && c->isBlack() );

			setRightChild( a, node->getLeftChild() );
			a->setBlack();
			c->setRed();

			delete node;
			return m_root;
		}

		Node * remThreeMiddle_Three_X( Node * node )
		{
			Node * a = node->getParent();
			assert( a != nullptr && a->isRed() );

			Node * b = a->getParent();
			assert( b != nullptr && b->isBlack() );

			Node * d = a->getLeftChild();
			assert( d != nullptr && d->isBlack() );

			Node * c = d->getLeftChild();
			assert( c != nullptr && c->isRed() );

			setLeftChild( a, d->getRightChild() );
			setRightChild( a, node->getLeftChild() );
			setRightChild( d, a );
			setLeftChild( b, d );
			a->setBlack();
			c->setBlack();
			d->setRed();

			delete node;
			return m_root;
		}

		Node * remThreeRight( Node * node )
		{
			Node * b = node->getParent();
			assert( b != nullptr && b->isBlack() );

			Node * a = b->getLeftChild();
			assert( a != nullptr && a->isRed() );

			Node * f = a->getRightChild();
			assert( f != nullptr && f->isBlack() );

			Node * e = f->getLeftChild();

			if( e != nullptr && e->isRed() )
			{
				// Middle sibling is 3-node.
				return remThreeRight_X_Three( node );
			}
			// Middle sibling is 2-node.
			return remThreeRight_X_Two( node );
		}

		Node * remThreeRight_X_Two( Node * node )
		{
			Node * b = node->getParent();
			assert( b != nullptr && b->isBlack() );

			Node * a = b->getLeftChild();
			assert( a != nullptr && a->isRed() );

			Node * c = a->getRightChild();
			assert( c != nullptr && c->isBlack() );
			assert( c->getLeftChild() == nullptr ||
					c->getLeftChild()->isBlack() );

			Node * x = b->getParent();
			bool xLess = b->isLessThanParent();
			setLeftChild( b, c );
			setRightChild( b, node->getLeftChild() );
			setRightChild( a, b );
			a->setBlack();
			c->setRed();

			if( x != nullptr )
			{
				setChild( x, a, xLess );
			}
			else
			{
				a->setParent( nullptr );
				m_root = a;
			}
			delete node;
			return m_root;
		}

		Node * remThreeRight_X_Three( Node * node )
		{
			Node * b = node->getParent();
			assert( b != nullptr && b->isBlack() );

			Node * a = b->getLeftChild();
			assert( a != nullptr && a->isRed() );

			Node * d = a->getRightChild();
			assert( d != nullptr && d->isBlack() );

			Node * c = d->getLeftChild();
			assert( c != nullptr && c->isRed() );

			Node * x = b->getParent();
			bool xLess = b->isLessThanParent();
			setLeftChild( b, d->getRightChild() );
			setRightChild( b, node->getLeftChild() );
			setRightChild( a, c );
			setLeftChild( d, a );
			setRightChild( d, b );
			c->setBlack();

			if( x != nullptr )
			{
				setChild( x, d, xLess );
			}
			else
			{
				d->setParent( nullptr );
				m_root = d;
			}
			delete node;
			return m_root;
		}
		Node *	m_root;
		SizeT	m_size;
	};
}

#endif