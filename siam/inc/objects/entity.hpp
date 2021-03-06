#ifndef _ECE_SIAM_ENTITY_HPP_
	#define _ECE_SIAM_ENTITY_HPP_

	#include <vector>
	#include <string>
	#include "../object.hpp"
	#include "../matrixs/direction.hpp"
	#include "../objects/types/type.hpp"

	namespace Siam { //in the namespace Siam
		namespace Objects { //in the namespace Objects

			class Entity : public Siam::Object { //inherits from object
				public:
					Entity(); //default constructor
					Entity( std::string nom, float force, Siam::Matrixs::Direction direction ); //overload constructor

					virtual Siam::Objects::Types::Type getType() const;//gets type of object --> useful because classes will inherit of object
			};

		}
    }
#endif
