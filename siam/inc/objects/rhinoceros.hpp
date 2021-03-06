#ifndef _ECE_SIAM_RHINOCEROS_HPP_
	#define _ECE_SIAM_RHINOCEROS_HPP_

	#include "entity.hpp"

	namespace Siam { //in the namespace Siam
		namespace Objects { //in the namespace Objects

			class Rhinoceros : public Entity { //create a Rhino
				public:
					Rhinoceros();//default constructor
					Rhinoceros( std::string nom, float force, Siam::Matrixs::Direction direction );//overload constructor

					Siam::Objects::Types::Type getType() const;//gets type of object --> useful because classes will inherit of object
			};

		}
	}
#endif
