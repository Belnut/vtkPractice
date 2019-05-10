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