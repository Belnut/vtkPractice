
#include "stdafx.h"
#ifndef vtkResliceCustomBtnWidget_h
#define vtkResliceCustomBtnWidget_h

#include "vtkInteractionWidgetsModule.h" // For export macro
#include "vtkAbstractWidget.h"
#include "TestClass.h"


#pragma once
class pxikVTKButtonWidget : public vtkButtonWidget
{
protected:
	bool m_isSelected;
	TestClass* m_dlg;
	typedef void (TestClass::*Func)(void);
	Func m_func;
public:
	static pxikVTKButtonWidget *New();

	vtkTypeMacro(pxikVTKButtonWidget, vtkButtonWidget);
	void PrintSelf(ostream& os, vtkIndent indent) override;
	void setEventFunction(TestClass *dlg, Func func);

	void startFunc();
public:


	// These are the events that are handled
	static void SelectAction(vtkAbstractWidget*);
	static void MoveAction(vtkAbstractWidget*);
	static void EndSelectAction(vtkAbstractWidget*);

protected:
	pxikVTKButtonWidget();
	~pxikVTKButtonWidget() override {}

private:
	pxikVTKButtonWidget(const pxikVTKButtonWidget&) = delete;
	void operator=(const pxikVTKButtonWidget&) = delete;
};

#endif