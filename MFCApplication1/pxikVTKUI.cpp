#include "stdafx.h"
#include "pxikVolumeSliceWidget.h"

#include <vtkActor.h>
#include <vtkAlgorithmOutput.h>
#include <vtkAssemblyNode.h>
#include <vtkAssemblyPath.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCellPicker.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkImageReslice.h>
#include <vtkLookupTable.h>
#include <vtkMath.h>
//#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkPickingManager.h>

#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include "vtkRendererCollection.h"
#include <vtkRenderer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

#include <vtkTexture.h>
#include <vtkTransform.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformation.h>
#include "pxikVTKUI.h"

#include "vtkImageMapper.h"

#include "pxikVTKUIWidgetRepresentation.h"
#include <vector>

vtkStandardNewMacro(pxikVTKUI);
//----------------------------------------------------------------------------
void pxikVTKUI::PrintSelf(ostream & os, vtkIndent indent)
{
}
//----------------------------------------------------------------------------
void pxikVTKUI::CreateDefaultRepresentation()
{
	if (!this->WidgetRep)
		this->WidgetRep = pxikVTKUIWidgetRepresentation::New();
}
//----------------------------------------------------------------------------
pxikVTKUI::pxikVTKUI()
{
	
}
//----------------------------------------------------------------------------
pxikVTKUI::~pxikVTKUI()
{
}
//----------------------------------------------------------------------------
void pxikVTKUI::addUI( pxikVTKUI *ui)
{
	m_uiList.push_back(ui);
}
//----------------------------------------------------------------------------
void pxikVTKUI::removeUI(pxikVTKUI *)
{
	
}
//----------------------------------------------------------------------------
void pxikVTKUI::setParentUI(pxikVTKUI *)
{
}
//----------------------------------------------------------------------------
void pxikVTKUI::removeParentUI()
{
}
//----------------------------------------------------------------------------
void pxikVTKUI::setBackgroundImage(vtkImageData * src)
{
	int *dims = src->GetDimensions();
	int type = src->GetExtentType();

	m_backgroundImage->SetDimensions(dims);
	m_backgroundImage->AllocateScalars(type, 3);
}
//----------------------------------------------------------------------------
void pxikVTKUI::removeBackgroundImage()
{
}
//----------------------------------------------------------------------------
void pxikVTKUI::createImageData(vtkImageData * src)
{

}
//----------------------------------------------------------------------------
void pxikVTKUI::ProcessEvents(vtkObject* object,
	unsigned long event,
	void* clientdata,
	void* vtkNotUsed(calldata))
{
	pxikVTKUI *self = reinterpret_cast<pxikVTKUI*>(clientdata);

	self->m_lastButtonPressed = pxikVTKUI::VTK_NO_BUTTON;

	/*switch (event)
	{
	case vtkCommand::LeftButtonPressEvent:
		self->m_lastButtonPressed = pxikVTKUI::VTK_LEFT_BUTTON;
		self->OnLeftButtonDown();
		break;

	case vtkCommand::LeftButtonReleaseEvent:
		self->m_lastButtonPressed = pxikVTKUI::VTK_LEFT_BUTTON;
		self->OnLeftButtonUp();
		break;

	case vtkCommand::RightButtonPressEvent:
		self->m_lastButtonPressed = pxikVTKUI::VTK_RIGHT_BUTTON;
		self->OnRightButtonDown();
		break;

	case vtkCommand::RightButtonReleaseEvent:
		self->m_lastButtonPressed = pxikVTKUI::VTK_RIGHT_BUTTON;
		self->OnRightButtonUp();
		break;
	
	case vtkCommand::MiddleButtonPressEvent:
		self->m_lastButtonPressed = pxikVTKUI::VTK_MIDDLE_BUTTON;
		self->OnMiddleButtonDown();
		break;
	
	case vtkCommand::MiddleButtonReleaseEvent:
		self->m_lastButtonPressed = pxikVTKUI::VTK_MIDDLE_BUTTON;
		self->OnMiddleButtonUp();
		break;

	
	case vtkCommand::HoverEvent:
		self->OnMouseHover();
		break;
	
	case vtkCommand::CharEvent:
		self->OnChar();
		break;

	}*/

}

void pxikVTKUI::OnMouseMove()
{
}

void pxikVTKUI::OnLeftButtonDown()
{
}

void pxikVTKUI::OnLeftButtonUp()
{
}

void pxikVTKUI::OnMiddleButtonDown()
{
}

void pxikVTKUI::OnMiddleButtonUp()
{
}

void pxikVTKUI::OnRightButtonDown()
{
}

void pxikVTKUI::OnRightButtonUp()
{
}

void pxikVTKUI::OnMouseHover()
{
}

void pxikVTKUI::OnChar()
{
}

void pxikVTKUI::SetEnabled(int enabling)
{
	this->Superclass::SetEnabled(enabling);

	if (this->Interactor &&
		this->Interactor->GetRenderWindow())
	{
		this->SetCurrentRenderer( this->Interactor->GetRenderWindow()->
			GetRenderers()->GetFirstRenderer());
	}
	if (!this->CurrentRenderer)
	{
		return;
	}

	if (enabling)
	{
		double loc[2] = { 0,0 };
		this->CreateDefaultRepresentation();
		this->WidgetRep->SetRenderer(this->CurrentRenderer);
		this->WidgetRep->BuildRepresentation();
		this->CurrentRenderer->AddViewProp(this->WidgetRep);
		WidgetRep->StartWidgetInteraction(loc);
	}
	else
	{
		this->CurrentRenderer->RemoveViewProp(this->WidgetRep);
		this->SetCurrentRenderer(nullptr);
	}

	
}