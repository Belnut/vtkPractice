#include "stdafx.h"

//#include "vtkRenderWindowInteractor.h"
//#include "vtkInteractorObserver.h"
//#include "vtkWindow.h"
#include "vtkRendererCollection.h"
#include "vtkImageMapper.h"

#include "pxikVTKUIAbstractWidget.h"
#include "pxikVTKUIWidgetAbstractRepresentation.h"




//----------------------------------------------------------------------------
void pxikVTKUIAbstractWidget::PrintSelf(ostream & os, vtkIndent indent)
{
	//Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
	this->Superclass::PrintSelf(os, indent);

	os << indent << "ProcessEvents: "
		<< (this->ProcessEvents ? "On" : "Off") << "\n";

	if (this->WidgetRep)
	{
		os << indent << "Widget Representation: " << this->WidgetRep << "\n";
	}
	else
	{
		os << indent << "Widget Representation: (none)\n";
	}

	os << indent << "Manages Cursor: "
		<< (this->ManagesCursor ? "On" : "Off") << "\n";

	os << indent << "Parent: " << this->Parent << "\n";

}

//----------------------------------------------------------------------------
pxikVTKUIAbstractWidget::pxikVTKUIAbstractWidget()
{
}

//----------------------------------------------------------------------------
pxikVTKUIAbstractWidget::~pxikVTKUIAbstractWidget()
{
}

//----------------------------------------------------------------------------
void pxikVTKUIAbstractWidget::addUI( pxikVTKUIAbstractWidget *w)
{
	this->m_uiList.emplace_back(w);
	w->setParentUI(this);
}

//----------------------------------------------------------------------------
void pxikVTKUIAbstractWidget::removeUI(pxikVTKUIAbstractWidget *w)
{
	std::list<pxikVTKUIAbstractWidget *>::iterator iter = std::find(m_uiList.begin(), m_uiList.end(), w);
	if (iter == m_uiList.end())
	{
		//삭제할 UI 없음
		return;
	}
	(*iter)->removeParentUI();
	this->m_uiList.erase(iter);
}

//----------------------------------------------------------------------------
void pxikVTKUIAbstractWidget::setParentUI(pxikVTKUIAbstractWidget *w)
{
	m_parentUI = w;
}

//----------------------------------------------------------------------------
void pxikVTKUIAbstractWidget::removeParentUI()
{
	m_parentUI = nullptr;
}

//----------------------------------------------------------------------------
void pxikVTKUIAbstractWidget::setBackgroundImage(vtkImageData * src)
{
	int *dims = src->GetDimensions();
	int type = src->GetExtentType();

	m_backgroundImage->SetDimensions(dims);
	m_backgroundImage->AllocateScalars(type, 3);
}
//----------------------------------------------------------------------------
void pxikVTKUIAbstractWidget::removeBackgroundImage()
{
}
//----------------------------------------------------------------------------
void pxikVTKUIAbstractWidget::createImageData(vtkImageData * src)
{

}
//----------------------------------------------------------------------------
void pxikVTKUIAbstractWidget::ProcessEvents(vtkObject* object,
	unsigned long event,
	void* clientdata,
	void* vtkNotUsed(calldata))
{
	pxikVTKUIAbstractWidget *self = reinterpret_cast<pxikVTKUIAbstractWidget*>(clientdata);

	self->m_lastButtonPressed = pxikVTKUIAbstractWidget::VTK_NO_BUTTON;

	/*switch (event)
	{
	case vtkCommand::LeftButtonPressEvent:
		self->m_lastButtonPressed = pxikVTKUIAbstractWidget::VTK_LEFT_BUTTON;
		self->OnLeftButtonDown();
		break;

	case vtkCommand::LeftButtonReleaseEvent:
		self->m_lastButtonPressed = pxikVTKUIAbstractWidget::VTK_LEFT_BUTTON;
		self->OnLeftButtonUp();
		break;

	case vtkCommand::RightButtonPressEvent:
		self->m_lastButtonPressed = pxikVTKUIAbstractWidget::VTK_RIGHT_BUTTON;
		self->OnRightButtonDown();
		break;

	case vtkCommand::RightButtonReleaseEvent:
		self->m_lastButtonPressed = pxikVTKUIAbstractWidget::VTK_RIGHT_BUTTON;
		self->OnRightButtonUp();
		break;
	
	case vtkCommand::MiddleButtonPressEvent:
		self->m_lastButtonPressed = pxikVTKUIAbstractWidget::VTK_MIDDLE_BUTTON;
		self->OnMiddleButtonDown();
		break;
	
	case vtkCommand::MiddleButtonReleaseEvent:
		self->m_lastButtonPressed = pxikVTKUIAbstractWidget::VTK_MIDDLE_BUTTON;
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

void pxikVTKUIAbstractWidget::OnMouseMove()
{
}

void pxikVTKUIAbstractWidget::OnLeftButtonDown()
{
}

void pxikVTKUIAbstractWidget::OnLeftButtonUp()
{
}

void pxikVTKUIAbstractWidget::OnMiddleButtonDown()
{
}

void pxikVTKUIAbstractWidget::OnMiddleButtonUp()
{
}

void pxikVTKUIAbstractWidget::OnRightButtonDown()
{
}

void pxikVTKUIAbstractWidget::OnRightButtonUp()
{
}

void pxikVTKUIAbstractWidget::OnMouseHover()
{
}

void pxikVTKUIAbstractWidget::OnChar()
{
}

void pxikVTKUIAbstractWidget::SetEnabled(int enabling)
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