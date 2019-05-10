#pragma once
#include "vtkInteractionWidgetsModule.h" // For export macro
#include "pxikVTKUIAbstractWidget.h"
//#include "pxikVTKUIButtonRepresentation.h"

class pxikVTKUIButtonRepresentation;

class pxikVTKUIButtonWidget : public pxikVTKUIAbstractWidget
{
public:
	/**
	* Instantiate the class.
	*/
	static pxikVTKUIButtonWidget* New();

	//@{
	 /**
	 * Standard macros.
	 */
	vtkTypeMacro(pxikVTKUIButtonWidget, pxikVTKUIAbstractWidget);
	void PrintSelf(ostream& os, vtkIndent indent) override;
	//@}

	/**
	* Specify an instance of vtkWidgetRepresentation used to represent this
	* widget in the scene. Note that the representation is a subclass of vtkProp
	* so it can be added to the renderer independent of the widget.
	*/
	void SetRepresentation(pxikVTKUIButtonRepresentation *r)
	{
		this->Superclass::SetWidgetRepresentation(reinterpret_cast<pxikVTKUIWidgetAbstractRepresentation*>(r));}

	/**
 	 * Return the representation as a vtkButtonRepresentation.
	 */
	pxikVTKUIButtonRepresentation* GetSliderRepresentation()
	{return reinterpret_cast<pxikVTKUIButtonRepresentation*>(this->WidgetRep);}

	/**
	* Create the default widget representation if one is not set.
	*/
	void CreateDefaultRepresentation() override;

	/**
	 * The method for activating and deactivating this widget. This method
	 * must be overridden because it is a composite widget and does more than
	 * its superclasses' vtkAbstractWidget::SetEnabled() method. The
	 * method finds and sets the active viewport on the internal balloon
	 * representation.
	 */
	void SetEnabled(int) override;
protected:
	pxikVTKUIButtonWidget();
	~pxikVTKUIButtonWidget() override;

	// These are the events that are handled
	static void SelectAction(vtkAbstractWidget*);
	static void MoveAction(vtkAbstractWidget*);
	static void EndSelectAction(vtkAbstractWidget*);

	// Manage the state of the widget
	int WidgetState;
	enum _WidgetState
	{
		Start = 0,
		Hovering,
		Selecting
	};

private:
	pxikVTKUIButtonWidget(const pxikVTKUIButtonWidget&) = delete;
	void operator=(const pxikVTKUIButtonWidget&) = delete;
};

