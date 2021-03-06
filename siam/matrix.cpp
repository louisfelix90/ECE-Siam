#include "inc/matrix.hpp"

using namespace std; //inclusion of the namespaces
using namespace Siam;
using namespace Siam::Matrixs;
using namespace Siam::Objects;

Matrix::Matrix() : m_board( 5, vector<Siam::Object*>( 5 ) ) { //default constructor
	for( unsigned int i = 0; i < 5; i++ ) {
		for( unsigned int j = 0; j < 5; j++ ) {
			if( ( i == 2 && j == 1 ) || ( i == 2 && j == 2 ) || ( i == 2 && j == 3 ) ) {
				this->set( i, j, new Mountain( "M", 0.9 ) );
				m_mountainCount++;
			} else {
				this->set( i, j, nullptr );
			}
		}
	}

}

Matrix::~Matrix() { //destructor
	for( auto& invec : this->m_board ) { //also delete every Object in all the cases
		for( auto& elem : invec ) {
			if( elem != nullptr )
				delete elem;
		}
	}
}

void Matrix::add( Siam::Object* A, unsigned int x, unsigned int y ) { //add function
	if( ( x == 0 ) || ( x == 4 ) || ( y == 0 ) || ( y == 4 ) ) { //check if we are effectively on the edge of the board
		try {
			if( this->at( x, y ) == nullptr ) { // then if the chosen spot is empty we'll just fill it
				this->set( x, y, A );
			} else {
				throw Siam::exceptions::invalid_move( "Add: Already occupied" ); //else an error
			}
		} catch( out_of_range e ) {
			throw Siam::exceptions::invalid_move( "Add: Out of bound" ); //shielding
		}
	} else {
		throw Siam::exceptions::invalid_move( "Add: Not authorized" );
	}
}

Siam::Object* Matrix::remove( unsigned int x, unsigned int y ) { //remove a piece from the board
	Siam::Object* obj;

	if( ( x == 0 ) || ( x == 4 ) || ( y == 0 ) || ( y == 4 ) ) { //check if we are within the boundaries
		try {
			if( this->at( x, y ) != nullptr ) { //delete the pointer on object
				obj = this->at( x, y );
				this->set( x, y, nullptr );
				return obj;
			} else {
				throw Siam::exceptions::invalid_move( "Remove: Empty space" );
			}
		} catch( out_of_range e ) {
			throw Siam::exceptions::invalid_move( "Remove: Out of bound" ); //shielding
		}
	} else {
		throw Siam::exceptions::invalid_move( "Remove: Not authorized" );
	}
}

DirectionVector Matrix::getDirectionVector( Direction dir ) { //used to get the direction of an Object
	switch( dir ) {
		case Left:
			return DirectionVector( -1, 0 ); //get the four different directions
		case Right:
			return DirectionVector( 1, 0 );
		case Up:
			return DirectionVector( 0, -1 );
		case Down:
			return DirectionVector( 0, 1 );
	}

	return DirectionVector();
}

inline Siam::Object* Matrix::at( unsigned int x, unsigned int y ) { //at is a function that replaces [] with more efficiency
	return this->m_board.at( x ).at( y );
}

inline void Matrix::set( unsigned int x, unsigned int y, Siam::Object* obj ) { //places an object in a spot x,y
	if( x < m_board.size() && y < m_board.size() )
		this->m_board[ x ][ y ] = obj;
	else
		throw out_of_range( "Accessing outside the defined matrix" );
}

inline Siam::Object* Matrix::at( unsigned int x, unsigned int y, DirectionVector dvec ) { //surcharged version of at
	return this->m_board.at( x + dvec.x ).at( y + dvec.y );
}

inline void Matrix::set( unsigned int x, unsigned int y, DirectionVector dvec, Siam::Object* obj ) { //surcharged version of set
	if( x < m_board.size() && y < m_board.size() )
		this->m_board[ x + dvec.x ][ y + dvec.y ] = obj;
	else
		throw out_of_range( "Accessing outside the defined matrix" );
}

float Matrix::getForce( unsigned int x, unsigned int y, DirectionVector dvec ) {
	float forceSum = 0;

	try {
		Object* initobj = this->at( x, y );
		Object* obj;
		DirectionVector invdvec = -1 * dvec;

		if( initobj != nullptr ) {
			try {
				obj = this->at( x, y );
				for( int nb = 0; obj != nullptr; nb++, obj = this->at( x, y, nb * dvec ) ) {
					if( obj->getType() == Types::Type::Mountain )
                        forceSum -= obj->getForce();
					else if( initobj->getType() == obj->getType() && this->getDirectionVector( obj->getDirection() ) == dvec )
						forceSum += obj->getForce();
					else if( initobj->getType() != obj->getType() && this->getDirectionVector( obj->getDirection() ) == invdvec )
						forceSum -= obj->getForce();
				}
			} catch( out_of_range e ) {}

			return forceSum;
		} else {
			throw Siam::exceptions::invalid_move( "Front force: empty space" );
		}
	} catch( out_of_range e ) {
		throw Siam::exceptions::invalid_move( "Front force: out of range" );
	}
}

Object* Matrix::getWiningObject( unsigned int x, unsigned int y, Direction dir ) {
	DirectionVector dvec = this->getDirectionVector( dir );
	Object* obj = nullptr;
	int nb = 0;

	//browse the matrix until empty spot or limit
	try {
		obj = this->at( x, y, dvec );
		for( nb = 1; obj != nullptr; nb++, obj = this->at( x, y, nb * dvec ) ) {}
	} catch( out_of_range e ) {}

	// nb contains the numbers of moves "until the end"
	// then browse the matrix but the other way
	for( ; nb > 0; nb-- ) {
		try {
			obj = this->at( x, y, nb * dvec );
			if( obj == nullptr )
				break;
			if( obj->getDirection() == dir )
				return obj;
		} catch( out_of_range e ) {}
	}

	return obj;
}

Object* Matrix::move( unsigned int x, unsigned int y, Direction direction, map<const Object*, pair<unsigned int, unsigned int>>& movements ) { //move a piece
	DirectionVector dvec = this->getDirectionVector( direction ); //get the direction of the piece
	DirectionVector* nbdvec;
	DirectionVector* nbsubdvec;
	Object *obj, *ejectedobj = nullptr;
	int nb = 0;

	if( this->at( x, y ) != nullptr ) { //if the chosen spot is not empty
		try {
			if( this->at( x, y, dvec ) == nullptr ) { //if the arrival spot is empty just make the change
				this->set( x, y, dvec, this->at( x, y ) );
				this->set( x, y, nullptr );

				movements[ this->at( x, y, dvec ) ] =  make_pair( x + dvec.x, y + dvec.y );
			} else {
				if( this->getForce( x, y, dvec ) > 0 ) {
					// browse the board until empty spot or limit
					try {
						obj = this->at( x, y );
						for( nb = 0; obj != nullptr; nb++, obj = this->at( x, y, nb * dvec ) ) {}
					} catch( out_of_range e ) {}

						// nb contains the numbers of moves "until the end"
                        // then browse the matrix but the other way
					for( ; nb > 0; nb-- ) {
						try {
							//Interverts actual used object with the next one opposed to moving direction
							nbdvec = new DirectionVector( nb * dvec );
							nbsubdvec = new DirectionVector( ( nb - 1 ) * dvec );

							obj = this->at( x, y, *nbdvec );
							this->set( x, y, *nbdvec, this->at( x, y, *nbsubdvec ) );
							this->set( x, y, *nbsubdvec, obj );

							movements[ this->at( x, y, *nbdvec ) ] = make_pair( x + nbdvec->x, y + nbdvec->y );

							delete nbdvec;
							delete nbsubdvec;
						} catch( out_of_range e ) {
							// Out of bound, just return the object

							// shields, you can only eject one object at the same time
							if( ejectedobj != nullptr )
								throw exceptions::invalid_move( "Ejecting more than one piece at a time." ); //not supposed to happen

							//else stock the eject piece
							ejectedobj = this->at( x, y, ( nb - 1 ) * dvec );
							//empty his former spot
							this->set( x, y, ( nb - 1 ) * dvec, nullptr );
						}
					}
				}
			}
		} catch( out_of_range e ) {
			throw Siam::exceptions::invalid_move( "Move: out of range" ); //shielding
		}
	}

	if( ejectedobj != nullptr && ejectedobj->getType() == Types::Type::Mountain )
		// If the ejected object is a mountain
		this->m_mountainCount--; //decrement the mountain count
	return ejectedobj;
}

void Matrix::orient( unsigned int x, unsigned int y, Direction direction ) { //reorient a piece
	try {
		this->at( x, y )->setDirection( direction ); //reorienting
	} catch( out_of_range e ) {
		throw Siam::exceptions::invalid_move( "Set direction: out of range" ); //shielding
	}
}

const std::vector<std::vector<Siam::Object*>>& Matrix::getBoard() { //read access to the board
	return this->m_board;
}

Types::Type Matrix::getType( unsigned int x, unsigned int y ) { //read access to the type of a spot
	Object* obj;
	try {
		obj = this->at( x, y );
		if( obj != nullptr )
			return obj->getType();
		else
			throw Siam::exceptions::invalid_object_type();
	} catch( out_of_range e ) { //shielding
		throw Siam::exceptions::invalid_move( "Get type: out of range" );
	}
}

unsigned int Matrix::getMountainsCount() { //read access to the mountain count
	return this->m_mountainCount;
}

Siam::Matrixs::Direction Matrix::getDirection( unsigned int x, unsigned int y ) { //read access to the direction
	Object* obj;
	try {
		obj = this->at( x, y );
		if( obj != nullptr )
			return obj->getDirection();
		else
			throw Siam::exceptions::invalid_object_type();
	} catch( out_of_range e ) {
		throw Siam::exceptions::invalid_move( "Get direction: out of range" );
	}
}

const Siam::Object* Matrix::getObject( unsigned int x, unsigned int y ) { //read access to the object
	return this->at( x, y );
}
