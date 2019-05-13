#pragma once
#ifndef pxikVTKUIButtonRepresentation_h
#define pxikVTKUIButtonRepresentation_h

#include "vtkInteractionWidgetsModule.h" // For export macro
#include "pxikVTKUIWidgetAbstractRepresentation.h"

class pxikVTKUIButtonRepresentation : public pxikVTKUIWidgetAbstractRepresentation
{
public:
	//@{
	/**
	 * Standard methods for the class.
	 */
	vtkTypeMacro(pxikVTKUIButtonRepresentation, pxikVTKUIWidgetAbstractRepresentation);
	void PrintSelf(ostream& os, vtkIndent indent) override;
	//@}

	//@{
	/**
	 * Retrieve the current button state.
	 */
	vtkSetClampMacro(NumberOfStates, int, 1, VTK_INT_MAX);
	//@}

	//@{
	/**
	 * Retrieve the current button state.
	 */
	vtkGetMacro(State, int);
	//@}

	//@{
	/**
	 * Manipulate the state. Note that the NextState() and PreviousState() methods
	 * use modulo traveral. The "state" integral value will be clamped within
	 * the possible state values (0<=state<NumberOfStates). Note that subclasses
	 * will override these methods in many cases.
	 */
	virtual void SetState(int state);
	virtual void NextState();
	virtual void PreviousState();
	//@}

	enum _InteractionState
	{
		Outside = 0,
		Inside
	};

	//@{
	/**
	 * These methods control the appearance of the button as it is being
	 * interacted with. Subclasses will behave differently depending on their
	 * particulars.  HighlightHovering is used when the mouse pointer moves
	 * over the button. HighlightSelecting is set when the button is selected.
	 * Otherwise, the HighlightNormal is used. The Highlight() method will throw
	 * a vtkCommand::HighlightEvent.
	 */
	enum _HighlightState { HighlightNormal, HighlightHovering, HighlightSelecting };
	void Highlight(int) override;
	vtkGetMacro(HighlightState, int);
	//@}

	/**
	 * Satisfy some of vtkProp's API.
	 */
	void ShallowCopy(vtkProp *prop) override;

	

protected:
	pxikVTKUIButtonRepresentation();
	~pxikVTKUIButtonRepresentation() override;

	// Values
	int NumberOfStates;
	int State;
	int HighlightState;



private:
	pxikVTKUIButtonRepresentation(const pxikVTKUIButtonRepresentation&) = delete;
	void operator=(const pxikVTKUIButtonRepresentation&) = delete;
};

#endif
