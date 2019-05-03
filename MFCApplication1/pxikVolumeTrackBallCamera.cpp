#include "stdafx.h"
#include "pxikVolumeTrackBallCamera.h"

#include <vtkCamera.h>
#include <vtkCellPicker.h>
#include <vtkCallbackCommand.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkProp3D.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

#include <vtkImageCanvasSource2D.h>
#include <vtkImageMapper.h>


vtkStandardNewMacro(pxikVolumeTrackBallCamera);

void pxikVolumeTrackBallCamera::PrintSelf(ostream & os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}

pxikVolumeTrackBallCamera::pxikVolumeTrackBallCamera() : Superclass()
{
	//mouseClickEffect = vtkImageCanvasSource2D::New();
	//mouseClickEffect->SetNumberOfScalarComponents(4);


	//mouseClickEffect->SetExtent(0, 20, 0, 20, 0, 0);
	//mouseClickEffect->SetDrawColor(0, 0, 0, 1);
	//mouseClickEffect->FillBox(0, 20, 0, 20);

	//mouseClickEffect->SetDrawColor(255, 0, 0,0);
	//mouseClickEffect->DrawCircle(9, 9, 10);
	//mouseClickEffect->Update();
	//
	//mapper = vtkImageMapper::New();
	//mapper->SetInputData(mouseClickEffect->GetOutput());

	//actor = vtkActor2D::New();
	//actor->SetMapper(mapper);

}


pxikVolumeTrackBallCamera::~pxikVolumeTrackBallCamera()
{
}

void pxikVolumeTrackBallCamera::OnLeftButtonDown()
{
}

void pxikVolumeTrackBallCamera::OnLeftButtonUp()
{
}

void pxikVolumeTrackBallCamera::OnMiddleButtonDown()
{
}

void pxikVolumeTrackBallCamera::OnMiddleButtonUp()
{
}

void pxikVolumeTrackBallCamera::OnRightButtonDown()
{
	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
		this->Interactor->GetEventPosition()[1]);
	if (this->CurrentRenderer == nullptr)
	{
		return;
	}
	this->RequestCursorShape(VTK_CURSOR_HAND);
	this->GrabFocus(this->EventCallbackCommand);
	if (this->Interactor->GetShiftKey())
	{
		if (this->Interactor->GetControlKey())
		{
			this->StartDolly();
		}
		else
		{
			this->StartPan();
		}
	}
	else
	{
		if (this->Interactor->GetControlKey())
		{
			this->StartSpin();
		}
		else
		{
			this->StartRotate();
		}
	}
}

void pxikVolumeTrackBallCamera::OnRightButtonUp()
{
	switch (this->State)
	{
	case VTKIS_PAN:
		this->EndPan();
		break;

	case VTKIS_SPIN:
		this->EndSpin();
		break;

	case VTKIS_ROTATE:
		this->EndRotate();
		break;
	}

	if (this->Interactor)
	{
		this->ReleaseFocus();
	}

	this->RequestCursorShape(VTK_CURSOR_ARROW);
}
