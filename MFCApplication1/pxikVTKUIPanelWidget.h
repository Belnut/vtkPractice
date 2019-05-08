#pragma once
#ifndef pxikVTKUIPanelWidget_h
#define pxikVTKUIPanelWidget_h


#include "pxikVTKUIAbstractWidget.h"

class pxikVTKUIPanelWidget :
	public pxikVTKUIAbstractWidget
{
public:
	/*!
	 * Instantiate the object.
	 */
	static pxikVTKUIPanelWidget * New();
	vtkTypeMacro(pxikVTKUIPanelWidget, pxikVTKUIAbstractWidget);
	void PrintSelf(ostream& os, vtkIndent indent) override;

	virtual void CreateDefaultRepresentation() override;

	void SetRepresentation(vtkWidgetRepresentation *r)
	{
		this->Superclass::SetWidgetRepresentation(reinterpret_cast<vtkWidgetRepresentation*>(r));
	}


private:
	pxikVTKUIPanelWidget();
	~pxikVTKUIPanelWidget();
};
#endif

