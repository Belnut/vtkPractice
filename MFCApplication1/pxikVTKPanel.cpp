#include "vtkObjectFactory.h"
#include "vtkCommand.h"
#include "vtkRenderer.h"
#include "vtkWidgetEventTranslator.h"
#include "vtkWidgetCallbackMapper.h"
#include "vtkEvent.h"
#include "vtkWidgetEvent.h"
#include "vtkRenderWindow.h"
#include "vtkTexturedButtonRepresentation.h"
#include "vtkInteractorObserver.h"
#include "vtkAssemblyPath.h"

#include "vtkActor2D.h"
#include "vtkProperty2D.h"

#include "vtkImageMapper.h"

#include "pxikVTKPanel.h"

vtkStandardNewMacro(pxikVTKPanel);

void pxikVTKPanel::CreateDefaultRepresentation()
{
	if (!this->WidgetRep)
	{
		this->WidgetRep = vtkTexturedButtonRepresentation::New();
	}
}
#pragma region pre Event Setting
//----------------------------------------------------------------------------
void pxikVTKPanel::ProcessEvents(vtkObject * object, unsigned long event, void * clientdata, void * calldata)
{
	pxikVTKPanel* self =
		reinterpret_cast<pxikVTKPanel *>(clientdata);

	self->LastButtonPressed = pxikVTKPanel::VTK_NO_BUTTON;

	//okay, let's do the right thing
	switch (event)
	{
	case vtkCommand::LeftButtonPressEvent:
		self->LastButtonPressed = pxikVTKPanel::VTK_LEFT_BUTTON;
		self->OnLeftButtonDown();
		break;
	case vtkCommand::LeftButtonReleaseEvent:
		self->LastButtonPressed = pxikVTKPanel::VTK_LEFT_BUTTON;
		self->OnLeftButtonUp();
		break;
	case vtkCommand::MiddleButtonPressEvent:
		self->LastButtonPressed = pxikVTKPanel::VTK_MIDDLE_BUTTON;
		self->OnMiddleButtonDown();
		break;
	case vtkCommand::MiddleButtonReleaseEvent:
		self->LastButtonPressed = pxikVTKPanel::VTK_MIDDLE_BUTTON;
		self->OnMiddleButtonUp();
		break;
	case vtkCommand::RightButtonPressEvent:
		self->LastButtonPressed = pxikVTKPanel::VTK_RIGHT_BUTTON;
		self->OnRightButtonDown();
		break;
	case vtkCommand::RightButtonReleaseEvent:
		self->LastButtonPressed = pxikVTKPanel::VTK_RIGHT_BUTTON;
		self->OnRightButtonUp();
		break;
	case vtkCommand::MouseMoveEvent:
		self->OnMouseMove();
		break;
	case vtkCommand::HoverEvent:
		self->OnMouseHover();
		
	case vtkCommand::CharEvent:
		self->OnChar();
		break;
	case vtkCommand::KeyPressEvent:
		self->OnChar();
		break;
	}
}

//----------------------------------------------------------------------------
void pxikVTKPanel::SetPicker(vtkAbstractPropPicker* picker)
{
	// we have to have a picker for slice motion, window level and cursor to work
	if (this->PlanePicker != picker)
	{
		// to avoid destructor recursion
		vtkAbstractPropPicker *temp = this->PlanePicker;
		this->PlanePicker = picker;
		if (temp != nullptr)
		{
			temp->UnRegister(this);
		}

		int delPicker = 0;
		if (this->PlanePicker == nullptr)
		{
			this->PlanePicker = vtkCellPicker::New();
			vtkCellPicker::SafeDownCast(this->PlanePicker)->SetTolerance(0.005);
			delPicker = 1;
		}

		this->PlanePicker->Register(this);
		this->PlanePicker->AddPickList(this->m_panelActor);
		this->PlanePicker->PickFromListOn();

		if (delPicker)
		{
			this->PlanePicker->Delete();
		}
	}
}
#pragma endregion
//----------------------------------------------------------------------------
void pxikVTKPanel::SetEnabled(int enabling)
{
	if (enabling) //----------------
	{
		if (this->Interactor)
		{
			if (!this->CurrentRenderer)
			{
				int X = this->Interactor->GetEventPosition()[0];
				int Y = this->Interactor->GetEventPosition()[1];
				this->SetCurrentRenderer(this->Interactor->FindPokedRenderer(X, Y));
			}
			this->CreateDefaultRepresentation();
			this->WidgetRep->SetRenderer(this->CurrentRenderer);
		}
	}
	else
	{
		this->SetCurrentRenderer(nullptr);
	}
	Superclass::SetEnabled(enabling);
}
//----------------------------------------------------------------------------
void pxikVTKPanel::testActorFunction()
{
	m_panelActor->SetPosition(100, 200);
	m_panelActor->GetProperty()->SetOpacity(0.2);
	
}
//----------------------------------------------------------------------------
#pragma region EventFrame
//----------------------------------------------------------------------------
void pxikVTKPanel::OnMouseMove()
{
}
//----------------------------------------------------------------------------
void pxikVTKPanel::OnLeftButtonDown()
{
}
//----------------------------------------------------------------------------
void pxikVTKPanel::OnLeftButtonUp()
{
}
//----------------------------------------------------------------------------
void pxikVTKPanel::OnMiddleButtonDown()
{
}
//----------------------------------------------------------------------------
void pxikVTKPanel::OnMiddleButtonUp()
{
}
//----------------------------------------------------------------------------
void pxikVTKPanel::OnRightButtonDown()
{
}
//----------------------------------------------------------------------------
void pxikVTKPanel::OnRightButtonUp()
{
}
//----------------------------------------------------------------------------
void pxikVTKPanel::OnMouseHover()
{
}
//----------------------------------------------------------------------------
void pxikVTKPanel::OnChar()
{
}
#pragma endregion
//----------------------------------------------------------------------------
pxikVTKPanel::pxikVTKPanel()
{
	MarginSizeX = 3;
	MarginSizeY = 3;
}
//----------------------------------------------------------------------------
void pxikVTKPanel::RegisterPickers()
{
}
//----------------------------------------------------------------------------
