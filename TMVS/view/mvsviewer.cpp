#include "mvsviewer.h"

void keyBoardEvent(const pcl::visualization::KeyboardEvent &event, void* viewer_void) {
	if (!event.keyUp()) return;

	MvsViewer &viewer = *((MvsViewer *) viewer_void);
	
	switch (event.getKeyCode()) {
	case 'H':
	case 'h':
		printf("\tA, a toggle point color\n");
		printf("\tB, b toggle background color\n");
		printf("\tN, n toggle normal display\n");
		break;
	case 'A':
	case 'a':
		viewer.toggleColor();
		break;
	case 'B':
	case 'b':
		viewer.toggleBackground();
		break;
	case 'N':
	case 'n':
		viewer.toggleNormal();
		break;
	}
}

void pointPickEvent(const pcl::visualization::PointPickingEvent &event, void* viewer_void) {
	MvsViewer &viewer = *((MvsViewer *) viewer_void);
	viewer.showPickedPoint(event.getPointIndex());
}

MvsViewer::MvsViewer(const MVS &mvs, bool show) {
	// set instance holder
	this->mvs = &mvs;

	// initialize viewer
	init();

	// add MVS cameras
	addCameras();
	// add MVS patches
	addPatches();

	if (show) open();
}

MvsViewer::~MvsViewer(void) {
	
}

void MvsViewer::init() {
	// set viewer
	pclViewer.setBackgroundColor(0, 0, 0);
	pclViewer.initCameraParameters();
	pclViewer.addCoordinateSystem (0.1);

	// set event listener
	pclViewer.registerKeyboardCallback(keyBoardEvent, (void*) this);
	pclViewer.registerPointPickingCallback(pointPickEvent, (void*) this);

	// set viewer flags
	normalEnable    = false;
	backgroundColor = false;
	colorEnable     = true;
}

void MvsViewer::addCameras() {
	const vector<Camera> &cameras = mvs->getCameras();
	const int camNum = (int) cameras.size();

	// camera position container
    PointCloud<PointXYZ>::Ptr centers(new PointCloud<PointXYZ>);
    // camera normal container
    PointCloud<pcl::Normal>::Ptr normals(new PointCloud<pcl::Normal>);

	// fill containers
	for (int i = 0; i < camNum; i++) { 
		const Camera &cam = cameras[i];
		const Vec3d  &c   = cam.getCenter();
		const Vec3d  &n   = cam.getOpticalNormal();

		PointXYZ    pt(c[0], c[1], c[2]);
        pcl::Normal nt(n[0], n[1], n[2]);

		centers->push_back(pt);
		normals->push_back(nt);
	}

	// remove old points
	pclViewer.removePointCloud("cameras");
	pclViewer.removePointCloud("cameraNormals");

	// add new points
	pclViewer.addPointCloud(centers, "cameras");
	pclViewer.addPointCloudNormals<pcl::PointXYZ, pcl::Normal>(centers, normals, 1, 0.1, "cameraNormals");

	// set camera visualize properities
	pclViewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 5, "cameras");
    pclViewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 1.0, 0.0, 0.0, "cameras");
    pclViewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 1.0, 1.0, 0.0, "cameraNormals");
    pclViewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_OPACITY, 0.5, "cameraNormals");
}

void MvsViewer::addPatches() {
	const map<int, Patch> &patches = mvs->getPatches();
	map<int, Patch>::const_iterator it;

	PointCloud<PointXYZRGB>::Ptr centers(new PointCloud<PointXYZRGB>);
	PointCloud<pcl::Normal>::Ptr normals(new PointCloud<pcl::Normal>);

	for (it = patches.begin(); it != patches.end(); ++it) {
		// patch center
		const Vec3d &p = it->second.getCenter();
		// patch normal
		const Vec3d &n = it->second.getNormal();
		// patch color
		const Vec3b &c = it->second.getColor();
		
		
		PointXYZRGB pt;
		pt.x = p[0];
		pt.y = p[1];
		pt.z = p[2];
		pt.r = c[0];
		pt.g = c[1];
		pt.b = c[2];
		pcl::Normal nt(n[0], n[1], n[2]);

		centers->push_back(pt);
		normals->push_back(nt);
	}

	// remove old points
	pclViewer.removePointCloud(NAME_PATCH);
	pclViewer.removePointCloud(NAME_NORMAL);

	// add new points
    pclViewer.addPointCloud(centers, NAME_PATCH);
	pclViewer.addPointCloudNormals<pcl::PointXYZRGB, pcl::Normal>(centers, normals, 1, 0.1, NAME_NORMAL);
	
	// set normal color
	pclViewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 1.0, 1.0, 0.0, NAME_NORMAL);
    pclViewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_OPACITY, 0.0, NAME_NORMAL);
}

void MvsViewer::open() {
	while ( !pclViewer.wasStopped() ) {
		pclViewer.spin();
	}
}

void MvsViewer::toggleBackground() {
	if (backgroundColor) {
		pclViewer.setBackgroundColor(0, 0, 0);
		backgroundColor = false;
	} else {
		pclViewer.setBackgroundColor(1, 1, 1);
		backgroundColor = true;
	}

	pclViewer.spinOnce(1, true);
}

void MvsViewer::toggleNormal() {
	if (normalEnable) {
		pclViewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_OPACITY, 0.0, NAME_NORMAL);
		normalEnable = false;
	} else {
		pclViewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_OPACITY, 0.2, NAME_NORMAL);
		normalEnable = true;
	}

	pclViewer.spinOnce(1, true);
}

void MvsViewer::toggleColor() {
	if (colorEnable) {
		pclViewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 0.5, 0, 0, NAME_PATCH);
		colorEnable = false;
	} else {
		addPatches();
		colorEnable = true;
	}

	pclViewer.spinOnce(1, true);
}

void MvsViewer::showPickedPoint(const int idx) {
	// skip out of index boundary
	if (idx > mvs->getPatches().size() || idx < 0) {
		return;
	}

	map<int, Patch>::const_iterator it;
	it = mvs->getPatches().begin();
	for (int i = 0; i < idx; ++it, ++i);

	// patch center
	const Vec3d &p = it->second.getCenter();
	// patch normal
	const Vec3d &n = it->second.getNormal();

	PointCloud<PointXYZ>::Ptr cloud(new PointCloud<PointXYZ>);
	PointCloud<pcl::Normal>::Ptr normal(new PointCloud<pcl::Normal>);

	PointXYZ pt;
	pt.x = p[0];
	pt.y = p[1];
	pt.z = p[2];
	pcl::Normal nt(n[0], n[1], n[2]);

	cloud->push_back(pt);
	normal->push_back(nt);

	// remove old points
	pclViewer.removePointCloud(NAME_PICKED_PATCH);
	pclViewer.removePointCloud(NAME_PICKED_NORMAL);

	// add new points
    pclViewer.addPointCloud(cloud, NAME_PICKED_PATCH);
	// add point normals
	pclViewer.addPointCloudNormals<pcl::PointXYZ, pcl::Normal>(cloud, normal, 1, 0.1, NAME_PICKED_NORMAL);
	// set color
    pclViewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 0.0, 1.0, 0.0, NAME_PICKED_PATCH);
	// set point size
	pclViewer.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 5, NAME_PICKED_PATCH);
}