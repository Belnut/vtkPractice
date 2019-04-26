#include "stdafx.h"
#include "pxikVolumeOrientMarker.h"
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
vtkStandardNewMacro(pxikVTKVolumeOrientMarker);
vtkCxxSetObjectMacro(vtkOrientationMarkerWidget, OrientationMarker, vtkProp);

void pxikVTKVolumeOrientMarker::PrintSelf(ostream & os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);

	os << indent << "OrientationMarker: " << this->OrientationMarker << endl;
	os << indent << "Interactive: " << this->Interactive << endl;
	os << indent << "Tolerance: " << this->Tolerance << endl;
	os << indent << "Viewport: (" << this->Viewport[0] << ", "
		<< this->Viewport[1] << ", " << this->Viewport[2] << ", "
		<< this->Viewport[3] << ")\n";
}


pxikVTKVolumeOrientMarker::pxikVTKVolumeOrientMarker()
{
}


pxikVTKVolumeOrientMarker::~pxikVTKVolumeOrientMarker()
{
}

void pxikVTKVolumeOrientMarker::SetCursor(int state)
{
}

void pxikVTKVolumeOrientMarker::MoveWidget(int X, int Y)
{
}


void pxikVTKVolumeOrientMarker::ExecuteCameraUpdateEvent(vtkObject * o, unsigned long event, void * calldata)
{
	if (!this->CurrentRenderer)
	{
		return;
	}

	vtkCamera *cam = this->CurrentRenderer->GetActiveCamera();
	double pos[3], fp[3], viewup[3];
	cam->GetPosition(pos);
	cam->GetFocalPoint(fp);
	cam->GetViewUp(viewup);

	cam = this->Renderer->GetActiveCamera();
	cam->SetPosition(pos);
	cam->SetFocalPoint(fp);
	cam->SetViewUp(viewup);
	this->Renderer->ResetCamera();

}

void pxikVTKVolumeOrientMarker::OnLeftButtonDown()
{
	// We're only here if we are enabled
	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	// are we over the widget?
	double vp[4];
	this->Renderer->GetViewport(vp);

	this->Renderer->NormalizedDisplayToDisplay(vp[0], vp[1]);
	this->Renderer->NormalizedDisplayToDisplay(vp[2], vp[3]);

	int pos1[2] = { static_cast<int>(vp[0]), static_cast<int>(vp[1]) };
	int pos2[2] = { static_cast<int>(vp[2]), static_cast<int>(vp[3]) };

	this->StartPosition[0] = X;
	this->StartPosition[1] = Y;

	// flag that we are attempting to adjust or move the outline
	this->Moving = 1;
	this->State = this->ComputeStateBasedOnPosition(X, Y, pos1, pos2);

	if (this->State == vtkOrientationMarkerWidget::Outside)
	{
		this->Moving = 0;
		return;
	}

	this->EventCallbackCommand->SetAbortFlag(1);
	this->StartInteraction();
	this->InvokeEvent(vtkCommand::StartInteractionEvent, nullptr);
}

void pxikVTKVolumeOrientMarker::OnLeftButtonUp()
{
	if (this->State == vtkOrientationMarkerWidget::Outside)
	{
		return;
	}

	// finalize any corner adjustments
	//this->SquareRenderer();

	// stop adjusting
	this->State = vtkOrientationMarkerWidget::Outside;
	this->Moving = 0;

	this->RequestCursorShape(VTK_CURSOR_DEFAULT);
	this->EndInteraction();
	this->InvokeEvent(vtkCommand::EndInteractionEvent, nullptr);
	this->Interactor->Render();
}

void pxikVTKVolumeOrientMarker::OnMouseMove()
{
	// compute some info we need for all cases
	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	double vp[4];
	this->Renderer->GetViewport(vp);

	// compute display bounds of the widget to see if we are inside or outside
	this->Renderer->NormalizedDisplayToDisplay(vp[0], vp[1]);
	this->Renderer->NormalizedDisplayToDisplay(vp[2], vp[3]);

	int pos1[2] = { static_cast<int>(vp[0]), static_cast<int>(vp[1]) };
	int pos2[2] = { static_cast<int>(vp[2]), static_cast<int>(vp[3]) };

	int state = this->ComputeStateBasedOnPosition(X, Y, pos1, pos2);
	this->State = this->Moving ? this->State : state;
	this->SetCursor(this->State);

	if (this->State == vtkOrientationMarkerWidget::Outside || !this->Moving)
	{
		this->Interactor->Render();
		return;
	}

	// based on the state set when the left mouse button is clicked,
	// adjust the renderer's viewport


	this->EventCallbackCommand->SetAbortFlag(1);
	this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
	this->Interactor->Render();
}

void pxikVTKVolumeOrientMarker::SquareRenderer()
{
	int *size = this->Renderer->GetSize();
	if (size[0] == 0 || size[1] == 0)
	{
		return;
	}

	double vp[4];
	this->Renderer->GetViewport(vp);

	this->Renderer->NormalizedDisplayToDisplay(vp[0], vp[1]);
	this->Renderer->NormalizedDisplayToDisplay(vp[2], vp[3]);

	// get the minimum viewport edge size
	//
	double dx = vp[2] - vp[0];
	double dy = vp[3] - vp[1];

	if (dx != dy)
	{
		double delta = dx < dy ? dx : dy;

		switch (this->State)
		{
		case vtkOrientationMarkerWidget::AdjustingP1:
			vp[2] = vp[0] + delta;
			vp[3] = vp[1] + delta;
			break;
		case vtkOrientationMarkerWidget::AdjustingP2:
			vp[0] = vp[2] - delta;
			vp[3] = vp[1] + delta;
			break;
		case vtkOrientationMarkerWidget::AdjustingP3:
			vp[0] = vp[2] - delta;
			vp[1] = vp[3] - delta;
			break;
		case vtkOrientationMarkerWidget::AdjustingP4:
			vp[2] = vp[0] + delta;
			vp[1] = vp[3] - delta;
			break;
		case vtkOrientationMarkerWidget::Translating:
			delta = (dx + dy)*0.5;
			vp[0] = ((vp[0] + vp[2]) - delta)*0.5;
			vp[1] = ((vp[1] + vp[3]) - delta)*0.5;
			vp[2] = vp[0] + delta;
			vp[3] = vp[1] + delta;
			break;
		}
		this->Renderer->DisplayToNormalizedDisplay(vp[0], vp[1]);
		this->Renderer->DisplayToNormalizedDisplay(vp[2], vp[3]);
		this->Renderer->SetViewport(vp);
	}
}
