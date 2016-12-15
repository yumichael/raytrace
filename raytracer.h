/***********************************************************
     From starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		This file contains the interface and 
		datastructures of the raytracer.  
		Simple traversal and addition code to 
		the datastructures are from starter code.

***********************************************************/

#ifndef _RAYTRACER_
#define _RAYTRACER_

#include "util.h"
#include "scene_object.h"
#include "light_source.h"

// The scene graph, containing objects in the scene.
struct SceneDagNode {
	SceneDagNode() : 
		obj(NULL), mat(NULL), 
		next(NULL), parent(NULL), child(NULL),
		trans(matrix4x4_id), invtrans(matrix4x4_id) {
	}	

	SceneDagNode( SceneObject* obj, Material* mat ) : 
		obj(obj), mat(mat), next(NULL), parent(NULL), child(NULL),
		trans(matrix4x4_id), invtrans(matrix4x4_id) {
		}
	
	~SceneDagNode() {
		if (!obj) delete obj;
		//if (!mat) delete mat;
	}

	// Pointer to geometry primitive, used for intersection.
	SceneObject* obj;
	// Pointer to material of the object, used in shading.
	Material* mat;
	// Each node maintains a transformation matrix, which maps the 
	// geometry from object space to world space and the inverse.
	Matrix4x4 trans;
	Matrix4x4 invtrans;
	// Memoized global transformation matrices
	Matrix4x4 model_world;
	Matrix4x4 world_model;

	// Internal structure of the tree, you shouldn't have to worry 
	// about them.
	SceneDagNode* next;
	SceneDagNode* parent;
	SceneDagNode* child;
};

// Linked list containing light sources in the scene.
struct LightListNode {
	LightListNode() : light(NULL), next(NULL), scenerep(NULL) {}
	LightListNode( LightSource* light, LightListNode* next=NULL) : 
		light(light), next(next), scenerep(NULL) {}
	~LightListNode() { 
		if (!light) delete light; 
	}
	LightSource* light;
	LightListNode* next;
	SceneDagNode* scenerep; // the scene node representing this light
};

class Raytracer {
public:
	Raytracer();
	~Raytracer();

	// Renders an image fileName with width and height and a camera
	// positioned at eye, with view vector view, up vector up, and 
	// field of view fov.
	void render( int width, int height, Point3D eye, Vector3D view, 
			Vector3D up, double fov, char* fileName );

	// Add an object into the scene, with material mat.  The function
	// returns a handle to the object node you just added, use the 
	// handle to apply transformations to the object.
	SceneDagNode* addObject( SceneObject* obj, Material* mat ) {
		return addObject(_root, obj, mat);
	}
	
	// Add an object into the scene with a specific parent node, 
	// don't worry about this unless you want to do hierarchical 
	// modeling.  You could create nodes with NULL obj and mat, 
	// in which case they just represent transformations.  
	SceneDagNode* addObject( SceneDagNode* parent, SceneObject* obj, 
		Material* mat);

	// Add a light object into the scene 
	LightListNode* addObjectLight(ObjectLight* objLight);

	// Add a light source.
	LightListNode* addLightSource( LightSource* light );

	// Transformation functions are implemented by right-multiplying 
	// the transformation matrix to the node's transformation matrix.
	
	// Apply rotation about axis 'x', 'y', 'z' angle degrees to node.
	void rotate( SceneDagNode* node, char axis, double angle );

	// Apply translation in the direction of trans to node.
	void translate( SceneDagNode* node, Vector3D trans );

	// Apply scaling about a fixed point origin.
	void scale(SceneDagNode* node, Point3D origin, double factor[3]);
	void scale(SceneDagNode* node, Point3D origin, Vector3D factor);

	// Return the colour of the ray after intersection and shading, call 
	// this function recursively for reflection and refraction.
	// When called recursively, fromNode has the reflective object
	// generating the recursive ray.
	Colour shadeRay(Ray3D& ray, SceneDagNode* fromNode = NULL);

	// Traversal code for the scene graph, the ray is transformed into 
	// the object space of each node where intersection is performed.
	// Return the intersected node.
	SceneDagNode* traverseScene(Ray3D& ray, SceneDagNode* ignore1 = NULL, SceneDagNode* ignore2 = NULL,
		SceneDagNode* node = NULL);

	// After profiling my code it turns out the starter code scene traversal
	// wastes a lot of time multiplying the same matrices for every ray.
	// So now this method computes all transformation matrices beforehand.
	void buildMatrices(SceneDagNode* node=NULL);

	// Rendering configuration
	int aa_pixel; // each pixel will sample aa_pixel * aa_pixel rays
	int ray_hp; // how many intersections before we draw black
private:
	// Allocates and initializes the pixel buffer for rendering, you
	// could add an interesting background to your scene by modifying 
	// this function.
	void initPixelBuffer();

	// Saves the pixel buffer to a file and deletes the buffer.
	void flushPixelBuffer(char *file_name);

	// Constructs a view to world transformation matrix based on the
	// camera parameters.
	Matrix4x4 initInvViewMatrix( Point3D eye, Vector3D view, Vector3D up );
	
	// Width and height of the viewport.
	int _scrWidth;
	int _scrHeight;

	// Light list and scene graph.
	LightListNode *_lightSource;
	SceneDagNode *_root;

	// Pixel buffer.
	unsigned char* _rbuffer;
	unsigned char* _gbuffer;
	unsigned char* _bbuffer;

	// Maintain global transformation matrices similar to OpenGL's matrix
	// stack.  These are used during scene traversal. 
	Matrix4x4 _modelToWorld;
	Matrix4x4 _worldToModel;
};

#endif