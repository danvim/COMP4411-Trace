#ifndef TRIMESH_H_
#define TRIMESH_H_

#include <vector>

#include "../scene/material.h"
#include "../scene/scene.h"
class TrimeshFace;

class Trimesh : public MaterialSceneObject
{
    friend class TrimeshFace;
    typedef std::vector<Eigen::Vector3d> Normals;
    typedef std::vector<Eigen::Vector3d> Vertices;
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
    void addVertex( const Eigen::Vector3d & );
    void addMaterial( Material *m );
    void addNormal( const Eigen::Vector3d & );

    bool addFace( int a, int b, int c );

    std::string doubleCheck() const;
    
    void generateNormals();
};

class TrimeshFace : public MaterialSceneObject
{
    Trimesh *parent;
    int ids[3]{};
public:
    TrimeshFace( Scene *scene, Material *mat, Trimesh *parent, const int a, const int b, const int c)
        : MaterialSceneObject( scene, mat )
    {
        this->parent = parent;
        ids[0] = a;
        ids[1] = b;
        ids[2] = c;
    }

    int operator[](const int i ) const
    {
        return ids[i];
    }

    bool intersectLocal( const Ray& r, ISect& i ) const override;

    bool hasBoundingBoxCapability() const override { return true; }

    BoundingBox computeLocalBoundingBox() override
    {
        BoundingBox localBounds;
        localBounds.max = parent->vertices[ids[0]].cwiseMax(parent->vertices[ids[1]]);
		localBounds.min = parent->vertices[ids[0]].cwiseMin(parent->vertices[ids[1]]);
        
        localBounds.max = parent->vertices[ids[2]].cwiseMax(localBounds.max);
		localBounds.min = parent->vertices[ids[2]].cwiseMin(localBounds.min);
        return localBounds;
    }
    
};


#endif // TRIMESH_H_
