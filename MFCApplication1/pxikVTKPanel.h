#include "stdafx.h"
#ifndef pxikVTKPanelWidget_h
#define pxikVTKPanelWidget_h

#include "vtkInteractionWidgetsModule.h" // For export macro
#include "vtkAbstractWidget.h"

class vtkActor2D;
class vtkImageMapper;

#pragma once
class pxikVTKPanel : public vtkAbstractWidget
{
public:
	static pxikVTKPanel *New();
	vtkTypeMacro(pxikVTKPanel, vtkAbstractWidget);

	void CreateDefaultRepresentation() override;

	// These are the events that are handled
	static void ProcessEvents(vtkObject* object,
		unsigned long event,
		void* clientdata,
		void* calldata);

	void SetEnabled(int) override;

	int LeftButtonAction, MiddleButtonAction, RightButtonAction;
	enum
	{
		VTK_CURSOR_ACTION = 0,
		VTK_SLICE_MOTION_ACTION = 1,
		VTK_WINDOW_LEVEL_ACTION = 2
	};
	vtkSetClampMacro(LeftButtonAction, int, VTK_CURSOR_ACTION, VTK_WINDOW_LEVEL_ACTION);
	vtkGetMacro(LeftButtonAction, int);
	vtkSetClampMacro(MiddleButtonAction, int, VTK_CURSOR_ACTION, VTK_WINDOW_LEVEL_ACTION);
	vtkGetMacro(MiddleButtonAction, int);
	vtkSetClampMacro(RightButtonAction, int, VTK_CURSOR_ACTION, VTK_WINDOW_LEVEL_ACTION);
	vtkGetMacro(RightButtonAction, int);

protected:
	pxikVTKPanel();
	~pxikVTKPanel() override {}

	// Do the picking
	vtkAbstractPropPicker *PlanePicker;

	// Register internal Pickers within PickingManager
	void RegisterPickers() override;

	void SetPicker(vtkAbstractPropPicker* picker);

	enum
	{
		VTK_NO_BUTTON = 0,
		VTK_LEFT_BUTTON = 1,
		VTK_MIDDLE_BUTTON = 2,
		VTK_RIGHT_BUTTON = 3
	};
	int LastButtonPressed;

	enum _WidgetState
	{
		None = 0,
		BorderTop,		//수평 수직 경계선
		BorderBottom,
		BorderLeft,
		BorderRight,
		EdgeLT,			//왼쪽   위
		EdgeLB,			//왼쪽   아래 				
		EdgeRT,			//오른쪽 위
		EdegRB,			//오른쪽 아래
		Inside,
		Moving,
		Outside
		
	};
	_WidgetState WidgetState;
	bool isClicked = false;

	bool isFocused = false;

	int LeftButtonAction;
	int MiddleButtonAction;
	int RightButtonAction;

	virtual void OnMouseMove();
	virtual void OnLeftButtonDown();
	virtual void OnLeftButtonUp();
	virtual void OnMiddleButtonDown();
	virtual void OnMiddleButtonUp();
	virtual void OnRightButtonDown();
	virtual void OnRightButtonUp();
	virtual void OnMouseHover();
	void OnChar() override;


	vtkImageData		*m_backgroundImage;
	vtkImageMapper		*m_imageMapper;
	vtkActor2D			*m_panelActor;
	
	
	
	//Margin
	int             MarginSizeX;
	int             MarginSizeY;

	vtkGetMacro(MarginSizeX, int);
	vtkSetMacro(MarginSizeX, int);
	vtkGetMacro(MarginSizeY, int);
	vtkSetMacro(MarginSizeY, int);


	void testActorFunction();

private:
	pxikVTKPanel(const pxikVTKPanel&) = delete;
	void operator=(const pxikVTKPanel&) = delete;



};

#endif