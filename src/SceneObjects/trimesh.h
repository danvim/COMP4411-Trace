#ifndef TRIMESH_H_
#define TRIMESH_H_

#include <vector>

#include "../scene/material.h"
#include "../scene/scene.h"
class TrimeshFace;

class Trimesh : public MaterialSceneObject
{
    friend class TrimeshFace;
    typedef std::vector<vec3f> Normals;
    typedef std::vector<vec3f> Vertices;
    typedef std::vector<TrimeshFace*> Faces;
    typedef std::vector<Material*> Materials;
    Vertices vertices;
    Faces faces;
    Normals normals;
    Materials materials;
public:
    Trimesh( Scene *scene, Material *mat, TransformNode *transform )
        : MaterialSceneObject(scene, mat)
    {
        this->transform = transform;
    }

    ~Trimesh();
    
    // must add vertices, normals, and materials IN ORDER
    void addVertex( const vec3f & );
    void addMaterial( Material *m );
    void addNormal( const vec3f & );

    bool addFace( int a, int b, int c );

    std::string doubleCheck() const;
    
    void generateNormals();
};

class TrimeshFace : public MaterialSceneObject
{
    Trimesh *parent;
    int ids[3]{};
public:
    TrimeshFace( Scene *scene, Material *mat, Trimesh *parent, int a, int b, int c)
        : MaterialSceneObject( scene, mat )
    {
        this->parent = parent;
        ids[0] = a;
        ids[1] = b;
        ids[2] = c;
    }

    int operator[]( int i ) const
    {
        return ids[i];
    }

    bool intersectLocal( const Ray& r, ISect& i ) const override;

    bool hasBoundingBoxCapability() const override { return true; }

    BoundingBox ComputeLocalBoundingBox() const override
    {
        BoundingBox localBounds;
        localBounds.max = maximum( parent->vertices[ids[0]], parent->vertices[ids[1]]);
		localBounds.min = minimum( parent->vertices[ids[0]], parent->vertices[ids[1]]);
        
        localBounds.max = maximum( parent->vertices[ids[2]], localBounds.max);
		localBounds.min = minimum( parent->vertices[ids[2]], localBounds.min);
        return localBounds;
    }
    
};


#endif // TRIMESH_H_
