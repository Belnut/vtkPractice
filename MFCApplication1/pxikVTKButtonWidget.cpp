#include "stdafx.h"
#include "pxikVTKButtonWidget.h"
#include "vtkButtonRepresentation.h"
#include "vtkTexturedButtonRepresentation.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkWidgetEventTranslator.h"
#include "vtkWidgetCallbackMapper.h"
#include "vtkEvent.h"
#include "vtkWidgetEvent.h"
#include "vtkRenderWindow.h"

vtkStandardNewMacro(pxikVTKButtonWidget);



pxikVTKButtonWidget::pxikVTKButtonWidget()
{
	// Set the initial state
	this->WidgetState = vtkButtonWidget::Start;

	// Okay, define the events
	this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
		vtkWidgetEvent::Select,
		this, pxikVTKButtonWidget::SelectAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent,
		vtkWidgetEvent::Move,
		this, pxikVTKButtonWidget::MoveAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent,
		vtkWidgetEvent::EndSelect,
		this, pxikVTKButtonWidget::EndSelectAction);
}


//----------------------------------------------------------------------------------
void pxikVTKButtonWidget::PrintSelf(ostream& os, vtkIndent indent)
{
	//Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
	this->Superclass::PrintSelf(os, indent);

}


void pxikVTKButtonWidget::setEventFunction( TestClass *dlg , Func func)
{
	m_dlg = dlg;
	m_func = func;
}

void pxikVTKButtonWidget::startFunc()
{
	(m_dlg->*m_func)();
}

 
void pxikVTKButtonWidget::SelectAction(vtkAbstractWidget *s)
{
	pxikVTKButtonWidget *self = reinterpret_cast<pxikVTKButtonWidget*>(s);
	vtkButtonWidget::SelectAction(s);
	if (self->m_isSelected)
	{ 
		//다른 클래스의 함수(외부의 객체 메서드 포인터)
		//self->startFunc();
		
	}
	else
	{
		self->m_dlg->goodbyeWorld();
	}

	self->m_isSelected = !(self->m_isSelected);

}

void pxikVTKButtonWidget::MoveAction(vtkAbstractWidget *s)
{
	vtkButtonWidget::MoveAction(s);
}

void pxikVTKButtonWidget::EndSelectAction(vtkAbstractWidget *s)
{
	vtkButtonWidget::EndSelectAction(s);
}
