#ifndef __PAIS_PATCH__H__
#define __PAIS_PATCH__H__

// homography patch correlation threshold
#define CORRELATION_THRESHOLD 0.7
// minimul camera number threshold
#define MIN_CAMERA_NUMBER 3

#define _USE_MATH_DEFINES
#include <math.h>

#include "../pso/psosolver.h"
#include "cellmap.h"
#include "mvs.h"
#include "utility.h"
#include "abstractpatch.h"


using namespace PAIS;
using namespace cv;

namespace PAIS {
	class MVS;

	class Patch : public AbstractPatch {
	private:
		// check neighbor patch
		static bool isNeighbor(const Patch &pth1, const Patch &pth2);

		// normal setters
		void setNormal(const Vec3d &n);
		void setNormal(const Vec2d &n);
		// depth setters
		bool setDepth();
		// set level of detail
		bool setLOD();
		// set estimated normal using sum of unit vector from point to camera
		bool setEstimatedNormal();
		// set reference camera index using normal
		bool setReferenceCameraIndex();
		// set depth range
		bool setDepthRange();
		// set normalized homography patch correlation table
		bool setCorrelationTable();
		// set priority
        bool setPriority();
		// set image point and color
		bool setImagePoint();

		// remove invisible camera using texture correlation
		bool removeInvisibleCamera();
		// get normalized homography patch column vector
		bool getHomographyPatch(const Vec2d &pt, const Camera &cam, const Mat_<double> &H, Mat_<double> &hp) const;
		// check neighbor cell to expand
		bool checkNeighborCell(const CellMap &map, const int cx, const int cy) const;
		// expand cell (create expansion patch)
		bool expandCell(const Camera &cam, const int cx, const int cy) const;

		// show refined projection
		void showRefinedResult() const;
		// show multi-view error
		void showError() const;

	public:
		// constructors
		Patch(const MVS &mvs, const Vec3d &center, const Vec3b &color, const vector<int> &camIdx, const vector<Vec2d> &imgPoint, const int id = -1);
		Patch(const Patch &parent, const Vec3d &center);

		// descructor
		~Patch(void) {};

		void refine();
		void expand() const;
	};

	double getFitness(const Particle &p, void *obj);
};

#endif