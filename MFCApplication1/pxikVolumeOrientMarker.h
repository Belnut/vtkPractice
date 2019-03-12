#pragma once

#ifndef pxikVolumeOrientMarker_h
#define pxikVolumeOrientMarker_h

#include <vtkObject.h>
#include <vtkInteractionWidgetsModule.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkInteractorObserver.h>

class pxikVTKVolumeOrientMarker : public vtkOrientationMarkerWidget
{

public:
	static pxikVTKVolumeOrientMarker* New();
	vtkTypeMacro(pxikVTKVolumeOrientMarker, vtkOrientationMarkerWidget);
	void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
	vtkProp     *OrientationMarker;
public:
	pxikVTKVolumeOrientMarker();
	~pxikVTKVolumeOrientMarker();

	void SetCursor(int state) override;
	//virtual void UpdateOutline();


	virtual void MoveWidget(int X, int Y);
	//virtual void ResizeTopLeft(int X, int Y);
	//virtual void ResizeTopRight(int X, int Y);
	//virtual void ResizeBottomLeft(int X, int Y);
	//virtual void ResizeBottomRight(int X, int Y);

	virtual void ExecuteCameraUpdateEvent(vtkObject *o, unsigned long event, void *calldata);


	void OnLeftButtonDown() override;
	void OnLeftButtonUp() override;
	void OnMouseMove() override;
	virtual void SquareRenderer();
};

#endif