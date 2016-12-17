/***********************************************************

Built from starter code

***********************************************************/


#include "raytracer.h"
#include "bmp_io.h"
#include "material.h"
#include "light_source.h"
#include "the_scene.h"
#include "sampler.h"
#include <cmath>
#include <iostream>
#include <ctime>

Raytracer::Raytracer() : _worldToModel(matrix4x4_id), _modelToWorld(matrix4x4_id), _lightSource(NULL) {
	_root = new SceneDagNode();
}

Raytracer::~Raytracer() {
	delete _root;
}

SceneDagNode* Raytracer::addObject( SceneDagNode* parent, 
		SceneObject* obj, Material* mat ) {
	SceneDagNode* node = new SceneDagNode( obj, mat );
	node->parent = parent;
	node->next = NULL;
	node->child = NULL;
	
	// Add the object to the parent's child list, this means
	// whatever transformation applied to the parent will also
	// be applied to the child.
	if (parent->child == NULL) {
		parent->child = node;
	}
	else {
		parent = parent->child;
		while (parent->next != NULL) {
			parent = parent->next;
		}
		parent->next = node;
	}
	
	return node;
}

LightListNode* Raytracer::addLightSource( LightSource* light ) {
	LightListNode* tmp = _lightSource;
	_lightSource = new LightListNode( light, tmp );
	return _lightSource;
}

LightListNode* Raytracer::addObjectLight(ObjectLight* objLight) {
	LightListNode* ll = addLightSource(objLight);
	SceneDagNode* node = ll->scenerep = addObject(objLight->pt_obj, objLight->objmat);
	node->trans = node->trans * objLight->trans;
	node->invtrans = objLight->invtrans * node->invtrans;
	return ll;
}

// Memoize the transformation matrices
void Raytracer::buildMatrices(SceneDagNode* node) {
	SceneDagNode *childPtr;
	node = node ? node : _root;

	// Applies transformation of the current node to the global
	// transformation matrices.
	_modelToWorld = _modelToWorld*node->trans;
	_worldToModel = node->invtrans*_worldToModel;
	// Traverse the children.
	childPtr = node->child;
	while (childPtr) {
		buildMatrices(childPtr);
		childPtr = childPtr->next;
	}
	node->model_world = _modelToWorld;
	node->world_model = _worldToModel;
	// Removes transformation of the current node from the global
	// transformation matrices.
	_worldToModel = node->trans*_worldToModel;
	_modelToWorld = _modelToWorld*node->invtrans;
}

SceneDagNode* Raytracer::traverseScene(Ray3D& ray, SceneDagNode* ignore1, SceneDagNode* ignore2, SceneDagNode* node) {
	SceneDagNode *resultNode = NULL;
	SceneDagNode *resultNode1 = NULL;
	SceneDagNode *childPtr;
	if (!node) {
		node = _root;
	}
	// Traverse the children.
	childPtr = node->child;
	while (childPtr) {
		resultNode1 = traverseScene(ray, ignore1, ignore2, childPtr);
		if (resultNode1) {
			resultNode = resultNode1;
		}
		childPtr = childPtr->next;
	}
	if (node->obj && node != ignore1 && node != ignore2 && (!ray.isShadow || node->mat->hasShadow())) {
		// Perform intersection.
		if (node->obj->intersect(ray, node->world_model, node->model_world)) {
			resultNode = node;
		}
	}
	return resultNode;
}

Colour Raytracer::shadeRay(Ray3D& ray, SceneDagNode* fromNode) {
	// PRECONDITION ray is normalized
	if (ray.hp < 1) {
		Sampler sampler;
		sampler.beginSampling(SAMPLE_CUBE, 2);
		sampler.sample();
		Point3D val = sampler.getSampled();
		return the_Colour(1 - val[0] * val[0], 1 - val[1] * val[1], 1 - val[2] * val[2]);
	}

	SceneDagNode* node = traverseScene(ray, fromNode);

	// Don't bother shading if the ray didn't hit anything
	if (!node) {
		return colour_0;
	}

	Material* mat = node->mat;
	mat->setup(this, ray);
	return mat->shader(this, ray, _lightSource, node);
}

void Raytracer::render( int width, int height, Point3D eye, Vector3D view, 
		Vector3D up, double fov, char* fileName ) {
	Matrix4x4 viewToWorld;
	_scrWidth = width;
	_scrHeight = height;
	double factor = (double(height)/2)/tan(fov*M_PI/360.0);

	int num = _scrWidth * _scrHeight;
	Colour *colbuffer = new Colour[num];

	initPixelBuffer();
	viewToWorld = initInvViewMatrix(eye, view, up);
	Point3D origin = point3D_0;
	Point3D imagePlane;

	const int numSample = aa_pixel; // anti aliasing sample per axis per pixel
	Colour *pixelColBuf = new Colour[numSample * numSample];


	Sampler sampler;

	// Construct a ray for each pixel.
	for (int i = 0; i < _scrHeight; ++i) { for (int j = 0; j < _scrWidth; ++j) {
		// Anti aliasing oversampling per pixel
		sampler.beginSampling(SAMPLE_SQUARE, numSample);
		int ii = 0;
		while (sampler.sample()) {
			if (j == 119 && i == 5) {
				RAND01();
			}
			// Sets up ray origin and direction in view space, 
			// image plane is at z = -1.
			imagePlane[0] = (-double(width)/2 + sampler.getSampled()[0] + j)/factor;
			imagePlane[1] = (-double(height) / 2 + sampler.getSampled()[1] + i) / factor;
			imagePlane[2] = -1;

			// DONE: Convert ray to world space and call 
			// shadeRay(ray) to generate pixel colour. 	
			
			Ray3D ray = Ray3D_ray(origin, imagePlane - origin, ray_hp);
			ray.origin = viewToWorld * ray.origin;
			viewToWorld.applyNorm1(ray.dir);

			pixelColBuf[ii] = shadeRay(ray);
			colbuffer[i*width+j] += pixelColBuf[ii];
			++ii;
		}
		colbuffer[i*width + j] /= numSample * numSample;

		if (j % 2 == 0 && (int)(i * ((double)width/(double)height)) == j) {
			std::cout << std::endl << "(" << i << "," << j << ") ";
			std::cout << colbuffer[i*width + j];
		}
	}} 
	
	for (int i = 0; i < _scrHeight; ++i) { for (int j = 0; j < _scrWidth; ++j) {
		Colour col;
		col = colbuffer[i*width + j];
		col.clamp();
		_rbuffer[i*width+j] = int(col[0]*255);
		_gbuffer[i*width+j] = int(col[1]*255);
		_bbuffer[i*width+j] = int(col[2]*255);
	}}
	std::cout << std::endl;
	flushPixelBuffer(fileName);
	delete [] colbuffer;
	delete [] pixelColBuf;
}

int main(int argc, char* argv[])
{
	fast_srand((unsigned int)time(NULL));
	// Build your scene and setup your camera here, by calling 
	// functions from Raytracer.  The code here sets up an example
	// scene and renders it from two different view points, DO NOT
	// change this if you're just implementing part one of the 
	// assignment.  

	TheScene scene;

	int width = scene.screen_width;
	int height = scene.screen_height;

	Raytracer &raytracer = scene.rt;

	if (argc == 3) {
		width = atoi(argv[1]);
		height = atoi(argv[2]);
	}

	int fov = scene.fov;

	raytracer.buildMatrices();

	std::ofstream timingLog("timing_log.txt");

	// Render the scene, feel free to make the image smaller for
	// testing purposes.
	clock_t time0, time1;
	time0 = clock();
	scene.take(1);
	time1 = clock();
	timingLog << time0 << " -- " << time1 << " : " << time1-time0 << std::endl;

	// Render it from a different point of view.
	time0 = clock();
	scene.take(2);
	time1 = clock();
	timingLog << time0 << " -- " << time1 << " : " << time1-time0 << std::endl;

	time0 = clock();
	scene.take(3);
	time1 = clock();
	timingLog << time0 << " -- " << time1 << " : " << time1 - time0 << std::endl;


	return 0;
}


  ////////////////////////////////////////////////////////////////////////////////
 //            Transformations
///////////////////////////////////////////////////////////////////////////////




void Raytracer::rotate(SceneDagNode* node, char axis, double angle) {
	Matrix4x4 rotation = matrix4x4_id;
	double toRadian = 2 * M_PI / 360.0;
	int i;

	for (i = 0; i < 2; i++) {
		switch (axis) {
		case 'x':
			rotation(0, 0) = 1;
			rotation(1, 1) = cos(angle*toRadian);
			rotation(1, 2) = -sin(angle*toRadian);
			rotation(2, 1) = sin(angle*toRadian);
			rotation(2, 2) = cos(angle*toRadian);
			rotation(3, 3) = 1;
			break;
		case 'y':
			rotation(0, 0) = cos(angle*toRadian);
			rotation(0, 2) = sin(angle*toRadian);
			rotation(1, 1) = 1;
			rotation(2, 0) = -sin(angle*toRadian);
			rotation(2, 2) = cos(angle*toRadian);
			rotation(3, 3) = 1;
			break;
		case 'z':
			rotation(0, 0) = cos(angle*toRadian);
			rotation(0, 1) = -sin(angle*toRadian);
			rotation(1, 0) = sin(angle*toRadian);
			rotation(1, 1) = cos(angle*toRadian);
			rotation(2, 2) = 1;
			rotation(3, 3) = 1;
			break;
		}
		if (i == 0) {
			node->trans = node->trans*rotation;
			angle = -angle;
		}
		else {
			node->invtrans = rotation*node->invtrans;
		}
	}
}

void Raytracer::translate(SceneDagNode* node, Vector3D trans) {
	Matrix4x4 translation = matrix4x4_id;

	translation(0, 3) = trans[0];
	translation(1, 3) = trans[1];
	translation(2, 3) = trans[2];
	node->trans = node->trans*translation;
	translation(0, 3) = -trans[0];
	translation(1, 3) = -trans[1];
	translation(2, 3) = -trans[2];
	node->invtrans = translation*node->invtrans;
}

void Raytracer::scale(SceneDagNode* node, Point3D origin, double factor[3]) {
	Matrix4x4 scale = matrix4x4_id;

	scale(0, 0) = factor[0];
	scale(0, 3) = origin[0] - factor[0] * origin[0];
	scale(1, 1) = factor[1];
	scale(1, 3) = origin[1] - factor[1] * origin[1];
	scale(2, 2) = factor[2];
	scale(2, 3) = origin[2] - factor[2] * origin[2];
	if (factor[0] == factor[1] && factor[1] == factor[2]) {
		scale = Similarity4x4(scale, 1.0 / factor[0]);
	}
	node->trans = node->trans*scale;
	scale(0, 0) = 1 / factor[0];
	scale(0, 3) = origin[0] - 1 / factor[0] * origin[0];
	scale(1, 1) = 1 / factor[1];
	scale(1, 3) = origin[1] - 1 / factor[1] * origin[1];
	scale(2, 2) = 1 / factor[2];
	scale(2, 3) = origin[2] - 1 / factor[2] * origin[2];
	if (factor[0] == factor[1] && factor[1] == factor[2]) {
		scale = Similarity4x4(scale, factor[0]);
	}
	node->invtrans = scale*node->invtrans;
}

void Raytracer::scale(SceneDagNode* node, Point3D origin, Vector3D factor) {
	Matrix4x4 scale = matrix4x4_id;

	scale(0, 0) = factor[0];
	scale(0, 3) = origin[0] - factor[0] * origin[0];
	scale(1, 1) = factor[1];
	scale(1, 3) = origin[1] - factor[1] * origin[1];
	scale(2, 2) = factor[2];
	scale(2, 3) = origin[2] - factor[2] * origin[2];
	if (factor[0] == factor[1] && factor[1] == factor[2]) {
		scale = Similarity4x4(scale, 1.0 / factor[0]);
	}
	node->trans = node->trans*scale;
	scale(0, 0) = 1 / factor[0];
	scale(0, 3) = origin[0] - 1 / factor[0] * origin[0];
	scale(1, 1) = 1 / factor[1];
	scale(1, 3) = origin[1] - 1 / factor[1] * origin[1];
	scale(2, 2) = 1 / factor[2];
	scale(2, 3) = origin[2] - 1 / factor[2] * origin[2];
	if (factor[0] == factor[1] && factor[1] == factor[2]) {
		scale = Similarity4x4(scale, factor[0]);
	}
	node->invtrans = scale*node->invtrans;
}

Matrix4x4 Raytracer::initInvViewMatrix(Point3D eye, Vector3D view, Vector3D up) {
	Matrix4x4 mat = matrix4x4_id;
	Vector3D w;
	view.normalize();
	up = up - up.dot(view)*view;
	up.normalize();
	w = view.cross(up);
	w.normalize();

	mat(0, 0) = w[0];
	mat(1, 0) = w[1];
	mat(2, 0) = w[2];
	mat(0, 1) = up[0];
	mat(1, 1) = up[1];
	mat(2, 1) = up[2];
	mat(0, 2) = -view[0];
	mat(1, 2) = -view[1];
	mat(2, 2) = -view[2];
	mat(0, 3) = eye[0];
	mat(1, 3) = eye[1];
	mat(2, 3) = eye[2];

	return mat;
}





void Raytracer::initPixelBuffer() {
	int numbytes = _scrWidth * _scrHeight * sizeof(unsigned char);
	_rbuffer = new unsigned char[numbytes];
	_gbuffer = new unsigned char[numbytes];
	_bbuffer = new unsigned char[numbytes];
	for (int i = 0; i < _scrHeight; i++) {
		for (int j = 0; j < _scrWidth; j++) {
			_rbuffer[i*_scrWidth + j] = 0;
			_gbuffer[i*_scrWidth + j] = 0;
			_bbuffer[i*_scrWidth + j] = 0;
		}
	}
}

void Raytracer::flushPixelBuffer(char *file_name) {
	bmp_write(file_name, _scrWidth, _scrHeight, _rbuffer, _gbuffer, _bbuffer);
	delete _rbuffer;
	delete _gbuffer;
	delete _bbuffer;
}