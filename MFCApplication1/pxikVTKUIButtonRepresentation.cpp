#include "stdafx.h"
#include "pxikVTKUIButtonRepresentation.h"
#include "pxikVTKUIWidgetAbstractRepresentation.h"

//----------------------------------------------------------------------
pxikVTKUIButtonRepresentation::pxikVTKUIButtonRepresentation()
{
	this->NumberOfStates = 0;
	this->State = 0;
	this->HighlightState = pxikVTKUIButtonRepresentation::HighlightNormal;
}

//----------------------------------------------------------------------
pxikVTKUIButtonRepresentation::~pxikVTKUIButtonRepresentation()
{
}

//----------------------------------------------------------------------
void pxikVTKUIButtonRepresentation::SetState(int state)
{
	if (this->NumberOfStates < 1)
	{
		return;
	}

	int remain = state % this->NumberOfStates;
	if (remain < 0)
	{
		remain += this->NumberOfStates;
	}
	state = remain;

	// Modify if necessary
	if (state != this->State)
	{
		this->State = state;
		this->Modified();
	}
}

//----------------------------------------------------------------------
void pxikVTKUIButtonRepresentation::NextState()
{
	this->SetState(this->State + 1);
}

//----------------------------------------------------------------------
void pxikVTKUIButtonRepresentation::PreviousState()
{
	this->SetState(this->State - 1);
}

//----------------------------------------------------------------------
void pxikVTKUIButtonRepresentation::Highlight(int state)
{
	int newState;
	if (state == pxikVTKUIButtonRepresentation::HighlightNormal)
	{
		newState = pxikVTKUIButtonRepresentation::HighlightNormal;
	}
	else if (state == pxikVTKUIButtonRepresentation::HighlightHovering)
	{
		newState = pxikVTKUIButtonRepresentation::HighlightHovering;
	}
	else //if ( state == vtkButtonRepresentation::HighlightSelecting )
	{
		newState = pxikVTKUIButtonRepresentation::HighlightSelecting;
	}

	if (newState != this->HighlightState)
	{
		this->HighlightState = newState;
		this->InvokeEvent(vtkCommand::HighlightEvent, &(this->HighlightState));
		this->Modified();
	}
}

//----------------------------------------------------------------------
void pxikVTKUIButtonRepresentation::ShallowCopy(vtkProp * prop)
{
	pxikVTKUIButtonRepresentation *rep =
		pxikVTKUIButtonRepresentation::SafeDownCast(prop);

	if (rep)
	{
		this->NumberOfStates = rep->NumberOfStates;
		this->State = rep->State;
		this->HighlightState = rep->HighlightState;
	}

	this->Superclass::ShallowCopy(prop);
}

//----------------------------------------------------------------------
void pxikVTKUIButtonRepresentation::PrintSelf(ostream & os, vtkIndent indent)
{
	//Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
	this->Superclass::PrintSelf(os, indent);

	os << indent << "Number Of States: " << this->NumberOfStates << "\n";
	os << indent << "State: " << this->State << "\n";
	os << indent << "Highlight State: " << this->HighlightState << "\n";
}
