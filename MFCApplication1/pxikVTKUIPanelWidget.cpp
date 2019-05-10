#include "stdafx.h"

#include "vtkRendererCollection.h"

#include "pxikVTKUIPanelWidget.h"
#include "pxikVTKUIPanelRepresentation.h"


vtkStandardNewMacro(pxikVTKUIPanelWidget);

void pxikVTKUIPanelWidget::PrintSelf(ostream & os, vtkIndent indent)
{
}

void pxikVTKUIPanelWidget::CreateDefaultRepresentation()
{
	if (!this->WidgetRep)
		this->WidgetRep = pxikVTKUIPanelRepresentation::New();
}

//----------------------------------------------------------------------------
pxikVTKUIPanelWidget::pxikVTKUIPanelWidget()
{

}
//----------------------------------------------------------------------------
pxikVTKUIPanelWidget::~pxikVTKUIPanelWidget()
{
	
}

//----------------------------------------------------------------------------
void pxikVTKUIPanelWidget::setBackgroundColor(color_t src)
{
	m_backgroundColor = src;

	dynamic_cast<pxikVTKUIPanelRepresentation *>(this->WidgetRep)
		->updateFrameColor(m_backgroundColor.red, m_backgroundColor.green, m_backgroundColor.blue);
}

//----------------------------------------------------------------------------
void pxikVTKUIPanelWidget::setBackgroundColor(double _red, double _green, double _blue)
{
	m_backgroundColor.setcolor(_red, _green, _blue);

	dynamic_cast<pxikVTKUIPanelRepresentation *>(this->WidgetRep)
		->updateFrameColor(m_backgroundColor.red, m_backgroundColor.green, m_backgroundColor.blue);
}

//----------------------------------------------------------------------------
void pxikVTKUIPanelWidget::getBackgroundColor(double* red, double *green, double *blue)
{
	*red	= m_backgroundColor.red;
	*green	= m_backgroundColor.green;
	*blue	= m_backgroundColor.blue;
}

//----------------------------------------------------------------------------
void pxikVTKUIPanelWidget::SetEnabled(int enabling)
{
	this->Superclass::SetEnabled(enabling);

	if (this->Interactor &&
		this->Interactor->GetRenderWindow())
	{
		this->SetCurrentRenderer(this->Interactor->GetRenderWindow()->
			GetRenderers()->GetFirstRenderer());
	}
	if (!this->CurrentRenderer)
	{
		return;
	}

	if (enabling)
	{
		//위치 설정 필요 loc이 위치를 변경함, Margin값을 가져와야 하는가? 
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

	//자신소유의 모든 UI 호출
	std::list<pxikVTKUIAbstractWidget *>::iterator iter = this->m_uiList.begin();
	while (iter != m_uiList.end())
	{
		(*iter)->SetEnabled(enabling);
		iter++;
	}

}