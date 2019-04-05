#ifndef READ_H_
#define READ_H_

#include <string>

#include "../scene/scene.h"

Scene *readScene( const std::string& filename );
Scene *readScene(std::istream& is );

#endif // READ_H_
