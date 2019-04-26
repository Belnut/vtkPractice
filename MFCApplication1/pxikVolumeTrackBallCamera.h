#pragma once
#include <vtkInteractorStyleTrackballCamera.h>
class vtkImageCanvasSource2D;
class vtkImageMapper;

class pxikVolumeTrackBallCamera : public vtkInteractorStyleTrackballCamera
{
public:
	static pxikVolumeTrackBallCamera* New();
	vtkTypeMacro(pxikVolumeTrackBallCamera, vtkInteractorStyleTrackballCamera);
	void PrintSelf(ostream& os, vtkIndent indent) override;


protected:
	pxikVolumeTrackBallCamera();
	~pxikVolumeTrackBallCamera();

	vtkImageCanvasSource2D* mouseClickEffect;
	vtkImageMapper* mapper;
	vtkActor2D* actor;

public:
	void OnLeftButtonDown() override;
	void OnLeftButtonUp() override;
	void OnMiddleButtonDown() override;
	void OnMiddleButtonUp() override;
	void OnRightButtonDown() override;
	void OnRightButtonUp() override;

private:
	pxikVolumeTrackBallCamera(const pxikVolumeTrackBallCamera&) = delete;
	void operator=(const pxikVolumeTrackBallCamera&) = delete;
};

