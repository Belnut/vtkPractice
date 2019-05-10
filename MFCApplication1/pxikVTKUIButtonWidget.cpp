#include "stdafx.h"

#include "vtkWidgetEvent.h"
#include "vtkWidgetCallbackMapper.h"
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

#include "pxikVTKUIButtonWidget.h"
#include "pxikVTKUIAbstractWidget.h"
#include "pxikVTKUIButtonRepresentation.h"
#include "pxikVTKUIWidgetAbstractRepresentation.h"
#include "pxikVTKUITextureButtonRepresentation.h"


vtkStandardNewMacro(pxikVTKUIButtonWidget);

//----------------------------------------------------------------------------------
pxikVTKUIButtonWidget::pxikVTKUIButtonWidget()
{
	// Set the initial state
	this->WidgetState = pxikVTKUIButtonWidget::Start;

	// Okay, define the events
	this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
		vtkWidgetEvent::Select,
		this, pxikVTKUIButtonWidget::SelectAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent,
		vtkWidgetEvent::Move,
		this, pxikVTKUIButtonWidget::MoveAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent,
		vtkWidgetEvent::EndSelect,
		this, pxikVTKUIButtonWidget::EndSelectAction);
}

pxikVTKUIButtonWidget::~pxikVTKUIButtonWidget()
{
}

//----------------------------------------------------------------------
void pxikVTKUIButtonWidget::CreateDefaultRepresentation()
{
	if (!this->WidgetRep)
	{
		//에러 나중에 수정할것
		this->WidgetRep = pxikVTKUITextureButtonRepresentation::New();
	}
}

//----------------------------------------------------------------------
void pxikVTKUIButtonWidget::SetEnabled(int enabling)
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

//----------------------------------------------------------------------
void pxikVTKUIButtonWidget::SelectAction(vtkAbstractWidget *w)
{
	pxikVTKUIButtonWidget *self = reinterpret_cast<pxikVTKUIButtonWidget*>(w);

	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];

	// The state must be hovering for anything to happen. MoveAction sets the
	// state.
	if (self->WidgetState != pxikVTKUIButtonWidget::Hovering)
	{
		return;
	}

	// Okay, make sure that the selection is in the current renderer
	if (!self->CurrentRenderer || !self->CurrentRenderer->IsInViewport(X, Y))
	{
		self->WidgetState = pxikVTKUIButtonWidget::Start;
		return;
	}

	// We are definitely selected, Highlight as necessary.
	self->WidgetState = pxikVTKUIButtonWidget::Selecting;
	self->WidgetRep->Highlight(pxikVTKUIButtonRepresentation::HighlightSelecting);
	self->EventCallbackCommand->SetAbortFlag(1);
	self->Render();
}

//----------------------------------------------------------------------
void pxikVTKUIButtonWidget::MoveAction(vtkAbstractWidget *w)
{
	pxikVTKUIButtonWidget *self = reinterpret_cast<pxikVTKUIButtonWidget*>(w);

	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];

	// Motion while selecting is ignored
	if (self->WidgetState == pxikVTKUIButtonWidget::Selecting)
	{
		self->EventCallbackCommand->SetAbortFlag(1);
		return;
	}

	// Get the new state and compare it to the old
	int renderRequired = 0;
	int state = self->WidgetRep->ComputeInteractionState(X, Y);
	if (self->WidgetState == pxikVTKUIButtonWidget::Hovering)
	{
		if (state == pxikVTKUIButtonRepresentation::Outside)
		{
			renderRequired = 1;
			if (self->ManagesCursor)
			{
				self->RequestCursorShape(VTK_CURSOR_DEFAULT);
			}
			self->WidgetRep->Highlight(pxikVTKUIButtonRepresentation::HighlightNormal);
			self->WidgetState = pxikVTKUIButtonWidget::Start;
		}
	}
	else //state is Start
	{
		if (state == pxikVTKUIButtonRepresentation::Inside)
		{
			renderRequired = 1;
			if (self->ManagesCursor)
			{
				self->RequestCursorShape(VTK_CURSOR_HAND);
			}
			self->WidgetRep->Highlight(pxikVTKUIButtonRepresentation::HighlightHovering);
			self->WidgetState = pxikVTKUIButtonWidget::Hovering;
			self->EventCallbackCommand->SetAbortFlag(1);
		}
	}

	if (renderRequired)
	{
		self->Render();
	}
}

//----------------------------------------------------------------------
void pxikVTKUIButtonWidget::EndSelectAction(vtkAbstractWidget *w)
{
	pxikVTKUIButtonWidget *self = reinterpret_cast<pxikVTKUIButtonWidget*>(w);

	if (self->WidgetState != pxikVTKUIButtonWidget::Selecting)
	{
		return;
	}

	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];

	int state = self->WidgetRep->ComputeInteractionState(X, Y);
	if (state == pxikVTKUIButtonRepresentation::Outside)
	{
		if (self->ManagesCursor)
		{
			self->RequestCursorShape(VTK_CURSOR_DEFAULT);
		}
		self->WidgetRep->Highlight(pxikVTKUIButtonRepresentation::HighlightNormal);
		self->WidgetState = pxikVTKUIButtonWidget::Start;
	}
	else //state == vtkButtonRepresentation::Inside
	{
		if (self->ManagesCursor)
		{
			self->RequestCursorShape(VTK_CURSOR_HAND);
		}
		self->WidgetRep->Highlight(pxikVTKUIButtonRepresentation::HighlightHovering);
		self->WidgetState = pxikVTKUIButtonWidget::Hovering;
	}

	// Complete interaction
	self->EventCallbackCommand->SetAbortFlag(1);
	reinterpret_cast<pxikVTKUIButtonRepresentation*>(self->WidgetRep)->NextState();
	self->InvokeEvent(vtkCommand::StateChangedEvent, nullptr);
	self->Render();
}

//----------------------------------------------------------------------
void pxikVTKUIButtonWidget::PrintSelf(ostream & os, vtkIndent indent)
{
}
