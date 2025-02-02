#include "stdafx.h"
#include "VTKCustomResliceWidget.h"

#include <vtkActor.h>
#include <vtkAlgorithmOutput.h>
#include <vtkAssemblyNode.h>
#include <vtkAssemblyPath.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCellPicker.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkImageReslice.h>
#include <vtkLookupTable.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkPickingManager.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
//#include <vtkTexture.h>
#include <vtkTransform.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformation.h>

vtkStandardNewMacro(VTKCustomResliceWidget);

//----------------------------------------------------------------------------
VTKCustomResliceWidget::VTKCustomResliceWidget()
{
	//상태 초기화 및 이벤트 등록
	this->State = VTKCustomResliceWidget::Start;
	this->EventCallbackCommand->SetCallback(VTKCustomResliceWidget::ProcessEvents);

	// 초기화, enum으로 표기할 것을 추천합
	this->Interaction = 1;  
	this->PlaneOrientation = 0;
	//this->PlaceFactor = 1.0;

	this->RestrictPlaneToVolumeOn();
	//=> this->RestrictPlaneToVolume = 1;
	
	this->OriginalWindow = 1.0; //이미지 밝기용 nn
	this->OriginalLevel = 0.5;	
	this->CurrentWindow = 1.0;
	this->TextureInterpolate = 1; // nn
	this->ResliceInterpolate = VTK_LINEAR_RESLICE;
	
	this->UserControlledLookupTableOff();
	//=> this->UserControlledLookupTable = 0;

	this->DisplayText = false; // 0;
	this->CurrentCursorPosition[0] = 0;
	this->CurrentCursorPosition[1] = 0;
	this->CurrentCursorPosition[2] = 0;
	
	this->CurrentImageValue = VTK_UNSIGNED_SHORT_MAX;
	//this->CurrentImageValue = VTK_DOUBLE_MAX;

	this->MarginSelectMode = 8; //뭔데 8임?
	this->UseContinuousCursorOff();
	this->UseContinuousCursor = 0;

	this->MarginSizeX = 0.05;	//nn
	this->MarginSizeY = 0.05;	//nn

	//Outline 설정
	this->PlaneSource = vtkPlaneSource::New();
	this->PlaneSource->SetXResolution(1);
	this->PlaneSource->SetYResolution(1);

	this->PlaneOutlinePolyData = vtkPolyData::New();
	this->PlaneOutlineActor = vtkActor::New();

	//Image Reslice Part
	this->ColorMap = vtkImageMapToColors::New();
	this->Reslice = vtkImageReslice::New();
	this->Reslice->TransformInputSamplingOff();
	//this->Reslice->TransformInputSampling = 0;
	this->ResliceAxes = vtkMatrix4x4::New();


	this->Texture = vtkTexture::New();
	this->TexturePlaneActor = vtkActor::New();
	this->Transform = vtkTransform::New();


	this->ImageData = nullptr;
	this->LookupTable = nullptr;


	//크로스헤어 커서
	this->CursorPolyData = vtkPolyData::New();
	this->CursorActor = vtkActor::New();

	//기울기 이벤트 영역 표시
	this->MarginPolyData = vtkPolyData::New();
	this->MarginActor = vtkActor::New();

	//텍스트 엑터
	this->TextActor = vtkTextActor::New();


	// 디폴트 포인트 옵션
	//
	double bounds[6];
	bounds[0] = -0.5;
	bounds[1] = 0.5;
	bounds[2] = -0.5;
	bounds[3] = 0.5;
	bounds[4] = -0.5;
	bounds[5] = 0.5;

	// 위젯 생성 및 초기화
//
	this->PlaceWidget(bounds);

	//this->GenerateTexturePlane();
	this->GenerateCursor();
	this->GenerateMargins();
	this->GenerateText();

	// 피커 설정
	//
	this->PlanePicker = nullptr;
	vtkCellPicker* picker = vtkCellPicker::New();
	picker->SetTolerance(0.005); //need soem fluff 값을 조금만 줄것
								 //오차율 설정
	this->SetPicker(picker);
	picker->Delete();

	//초기 Property 설정
	//
	this->PlaneProperty = nullptr;
	this->SelectedPlaneProperty = nullptr;
	//this->TexturePlaneProperty = nullptr;
	this->CursorProperty = nullptr;
	this->MarginProperty = nullptr;
	this->CreateDefaultProperties();


	// 초기 액션 설정

//this->LeftButtonAction = pxikVolumeSliceWidget::VTK_CURSOR_ACTION;
//this->MiddleButtonAction = pxikVolumeSliceWidget::VTK_SLICE_MOTION_ACTION;
//this->RightButtonAction = pxikVolumeSliceWidget::VTK_WINDOW_LEVEL_ACTION;

	this->LeftButtonAction = VTKCustomResliceWidget::VTK_SLICE_MOTION_ACTION;
	this->MiddleButtonAction = -1;
	this->RightButtonAction = -1;

	// Set up modifier
	// 모디파이어 설정

	this->LeftButtonAutoModifier = VTKCustomResliceWidget::VTK_NO_MODIFIER;
	this->MiddleButtonAutoModifier = VTKCustomResliceWidget::VTK_NO_MODIFIER;
	this->RightButtonAutoModifier = VTKCustomResliceWidget::VTK_NO_MODIFIER;

	this->LastButtonPressed = VTKCustomResliceWidget::VTK_NO_BUTTON;

	//this->TextureVisibility = 1;
}

//----------------------------------------------------------------------------
VTKCustomResliceWidget::~VTKCustomResliceWidget()
{
	//삭제는 연결의 반대순으로 진행하는듯 
	//Delete Outline Data, Actor 
	this->PlaneOutlineActor->Delete();
	this->PlaneOutlinePolyData->Delete();
	this->PlaneSource->Delete();

	//Delete와 비슷하다는데 음.... 정확히는 모르겠음
	//연결되어있는 경우 (ref count > 1) 일 때 쓰는 것 같음 햇갈리지 말라고
	if (this->PlanePicker)
	{
		this->PlanePicker->UnRegister(this);
	}

	if (this->PlaneProperty)
	{
		this->PlaneProperty->Delete();
	}

	if (this->SelectedPlaneProperty)
	{
		this->SelectedPlaneProperty->Delete();
	}

	if (this->CursorProperty)
	{
		this->CursorProperty->Delete();
	}

	if (this->MarginProperty)
	{
		this->MarginProperty->Delete();
	}

	this->ResliceAxes->Delete();
	this->Transform->Delete();
	this->Reslice->Delete();

	if (this->LookupTable)
	{
		this->LookupTable->UnRegister(this);
	}

	this->TexturePlaneActor->Delete();
	this->ColorMap->Delete();
	this->Texture->Delete();

	if (this->TexturePlaneProperty)
	{
		this->TexturePlaneProperty->Delete();
	}

	if (this->ImageData)
	{
		this->ImageData = nullptr;
	}

	this->CursorActor->Delete();
	this->CursorPolyData->Delete();

	this->MarginActor->Delete();
	this->MarginPolyData->Delete();

	this->TextActor->Delete();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::SetTextureVisibility(int vis)
{
	if (this->TextureVisibility == vis)
	{
		return;
	}

	this->TextureVisibility = vis;

	if (this->Enabled)
	{
		if (this->TextureVisibility && this->ImageData)
		{
			this->CurrentRenderer->AddViewProp(this->TexturePlaneActor);
		}
		else
		{
			this->CurrentRenderer->RemoveViewProp(this->TexturePlaneActor);
		}
	}

	this->Modified();
}


//----------------------------------------------------------------------------
void VTKCustomResliceWidget::SetEnabled(int enabling)
{

	if (!this->Interactor)
	{
		vtkErrorMacro(<< "The interactor must be set prior to enabling/disabling widget");
		return;
	}

	if (enabling) //----------------------------------------------------------
	{
		vtkDebugMacro(<< "Enabling plane widget");

		if (this->Enabled) //already enabled, just return
		{
			return;
		}

		if (!this->CurrentRenderer)
		{
			this->SetCurrentRenderer(this->Interactor->FindPokedRenderer(
				this->Interactor->GetLastEventPosition()[0],
				this->Interactor->GetLastEventPosition()[1]));
			if (this->CurrentRenderer == nullptr)
			{
				return;
			}
		}

		this->Enabled = 1;

		// we have to honour this ivar: it could be that this->Interaction was
		// set to off when we were disabled
		if (this->Interaction)
		{
			this->AddObservers();
		}

		// Add the plane
		this->CurrentRenderer->AddViewProp(this->PlaneOutlineActor);
		this->PlaneOutlineActor->SetProperty(this->PlaneProperty);

		//add the TexturePlaneActor
		if (this->TextureVisibility && this->ImageData)
		{
			this->CurrentRenderer->AddViewProp(this->TexturePlaneActor);
		}
		this->TexturePlaneActor->SetProperty(this->TexturePlaneProperty);

		// Add the cross-hair cursor
		this->CurrentRenderer->AddViewProp(this->CursorActor);
		this->CursorActor->SetProperty(this->CursorProperty);

		// Add the margins
		this->CurrentRenderer->AddViewProp(this->MarginActor);
		this->MarginActor->SetProperty(this->MarginProperty);

		// Add the image data annotation
		this->CurrentRenderer->AddViewProp(this->TextActor);

		this->TexturePlaneActor->PickableOn();

		this->InvokeEvent(vtkCommand::EnableEvent, nullptr);

	}

	else //disabling----------------------------------------------------------
	{
		vtkDebugMacro(<< "Disabling plane widget");

		if (!this->Enabled) //already disabled, just return
		{
			return;
		}

		this->Enabled = 0;

		// don't listen for events any more
		this->Interactor->RemoveObserver(this->EventCallbackCommand);

		// turn off the plane
		this->CurrentRenderer->RemoveViewProp(this->PlaneOutlineActor);

		//turn off the texture plane
		this->CurrentRenderer->RemoveViewProp(this->TexturePlaneActor);

		//turn off the cursor
		this->CurrentRenderer->RemoveViewProp(this->CursorActor);

		//turn off the margins
		this->CurrentRenderer->RemoveViewProp(this->MarginActor);

		//turn off the image data annotation
		this->CurrentRenderer->RemoveViewProp(this->TextActor);

		this->TexturePlaneActor->PickableOff();

		this->InvokeEvent(vtkCommand::DisableEvent, nullptr);
		this->SetCurrentRenderer(nullptr);
	}

	this->Interactor->Render();
}


//----------------------------------------------------------------------------
void VTKCustomResliceWidget::ProcessEvents(vtkObject* vtkNotUsed(object),
	unsigned long event,
	void* clientdata,
	void* vtkNotUsed(calldata))
{
	VTKCustomResliceWidget* self =
		reinterpret_cast<VTKCustomResliceWidget *>(clientdata);

	self->LastButtonPressed = VTKCustomResliceWidget::VTK_NO_BUTTON;

	//okay, let's do the right thing
	//TODO: We need to add Mouse Wheel Event
	//		change Event Role and sequence
	switch (event)
	{
	case vtkCommand::LeftButtonPressEvent:
		self->LastButtonPressed = VTKCustomResliceWidget::VTK_LEFT_BUTTON;
		self->OnLeftButtonDown();
		break;
	case vtkCommand::LeftButtonReleaseEvent:
		self->LastButtonPressed = VTKCustomResliceWidget::VTK_LEFT_BUTTON;
		self->OnLeftButtonUp();
		break;


	case vtkCommand::MouseWheelForwardEvent:
		break;
	case vtkCommand::MouseWheelBackwardEvent:
		break;



	case vtkCommand::MouseMoveEvent:
		self->OnMouseMove();
		break;
	case vtkCommand::CharEvent:
		self->OnChar();
		break;
	}

}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::OnChar()
{
	vtkRenderWindowInteractor *i = this->Interactor;

	if (i->GetKeyCode() == 'r' || i->GetKeyCode() == 'R')
	{
		if (i->GetShiftKey() || i->GetControlKey())
		{
			this->SetWindowLevel(this->OriginalWindow, this->OriginalLevel);
			double wl[2] = { this->CurrentWindow, this->CurrentLevel };

			this->EventCallbackCommand->SetAbortFlag(1);
			this->InvokeEvent(vtkCommand::ResetWindowLevelEvent, wl);
		}
		else
		{
			this->Interactor->GetInteractorStyle()->OnChar();
		}
	}
	else
	{
		this->Interactor->GetInteractorStyle()->OnChar();
	}
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::AddObservers(void)
{
	// listen for the following events
	vtkRenderWindowInteractor *i = this->Interactor;
	if (i)
	{
		i->AddObserver(vtkCommand::MouseMoveEvent, this->EventCallbackCommand,
			this->Priority);
		i->AddObserver(vtkCommand::LeftButtonPressEvent,
			this->EventCallbackCommand, this->Priority);
		i->AddObserver(vtkCommand::LeftButtonReleaseEvent,
			this->EventCallbackCommand, this->Priority);
		i->AddObserver(vtkCommand::MouseWheelForwardEvent,
			this->EventCallbackCommand, this->Priority);
		i->AddObserver(vtkCommand::MouseWheelBackwardEvent,
			this->EventCallbackCommand, this->Priority);
		i->AddObserver(vtkCommand::CharEvent,
			this->EventCallbackCommand, this->Priority);
	}
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::SetInteraction(int interact)
{
	if (this->Interactor && this->Enabled)
	{
		if (this->Interaction == interact)
		{
			return;
		}
		if (interact == 0)
		{
			this->Interactor->RemoveObserver(this->EventCallbackCommand);
		}
		else
		{
			this->AddObservers();
		}
		this->Interaction = interact;
	}
	else
	{
		vtkGenericWarningMacro(<< "set interactor and Enabled before changing interaction...");
	}
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);

	if (this->PlaneProperty)
	{
		os << indent << "Plane Property:\n";
		this->PlaneProperty->PrintSelf(os, indent.GetNextIndent());
	}
	else
	{
		os << indent << "Plane Property: (none)\n";
	}

	if (this->SelectedPlaneProperty)
	{
		os << indent << "Selected Plane Property:\n";
		this->SelectedPlaneProperty->PrintSelf(os, indent.GetNextIndent());
	}
	else
	{
		os << indent << "Selected Plane Property: (none)\n";
	}

	if (this->LookupTable)
	{
		os << indent << "LookupTable:\n";
		this->LookupTable->PrintSelf(os, indent.GetNextIndent());
	}
	else
	{
		os << indent << "LookupTable: (none)\n";
	}

	if (this->CursorProperty)
	{
		os << indent << "Cursor Property:\n";
		this->CursorProperty->PrintSelf(os, indent.GetNextIndent());
	}
	else
	{
		os << indent << "Cursor Property: (none)\n";
	}

	if (this->MarginProperty)
	{
		os << indent << "Margin Property:\n";
		this->MarginProperty->PrintSelf(os, indent.GetNextIndent());
	}
	else
	{
		os << indent << "Margin Property: (none)\n";
	}

	if (this->TexturePlaneProperty)
	{
		os << indent << "TexturePlane Property:\n";
		this->TexturePlaneProperty->PrintSelf(os, indent.GetNextIndent());
	}
	else
	{
		os << indent << "TexturePlane Property: (none)\n";
	}

	if (this->ColorMap)
	{
		os << indent << "ColorMap:\n";
		this->ColorMap->PrintSelf(os, indent.GetNextIndent());
	}
	else
	{
		os << indent << "ColorMap: (none)\n";
	}

	if (this->Reslice)
	{
		os << indent << "Reslice:\n";
		this->Reslice->PrintSelf(os, indent.GetNextIndent());
	}
	else
	{
		os << indent << "Reslice: (none)\n";
	}

	if (this->ResliceAxes)
	{
		os << indent << "ResliceAxes:\n";
		this->ResliceAxes->PrintSelf(os, indent.GetNextIndent());
	}
	else
	{
		os << indent << "ResliceAxes: (none)\n";
	}

	double *o = this->PlaneSource->GetOrigin();
	double *pt1 = this->PlaneSource->GetPoint1();
	double *pt2 = this->PlaneSource->GetPoint2();

	os << indent << "Origin: (" << o[0] << ", "
		<< o[1] << ", "
		<< o[2] << ")\n";
	os << indent << "Point 1: (" << pt1[0] << ", "
		<< pt1[1] << ", "
		<< pt1[2] << ")\n";
	os << indent << "Point 2: (" << pt2[0] << ", "
		<< pt2[1] << ", "
		<< pt2[2] << ")\n";

	os << indent << "Current Cursor Position: ("
		<< this->CurrentCursorPosition[0] << ", "
		<< this->CurrentCursorPosition[1] << ", "
		<< this->CurrentCursorPosition[2] << ")\n";

	os << indent << "Current Image Value: "
		<< this->CurrentImageValue << "\n";

	os << indent << "Plane Orientation: " << this->PlaneOrientation << "\n";
	os << indent << "Reslice Interpolate: " << this->ResliceInterpolate << "\n";
	os << indent << "Texture Interpolate: "
		<< (this->TextureInterpolate ? "On\n" : "Off\n");
	os << indent << "Texture Visibility: "
		<< (this->TextureVisibility ? "On\n" : "Off\n");
	os << indent << "Restrict Plane To Volume: "
		<< (this->RestrictPlaneToVolume ? "On\n" : "Off\n");
	os << indent << "Display Text: "
		<< (this->DisplayText ? "On\n" : "Off\n");
	os << indent << "Interaction: "
		<< (this->Interaction ? "On\n" : "Off\n");
	os << indent << "User Controlled Lookup Table: "
		<< (this->UserControlledLookupTable ? "On\n" : "Off\n");
	os << indent << "LeftButtonAction: " << this->LeftButtonAction << endl;
	os << indent << "MiddleButtonAction: " << this->MiddleButtonAction << endl;
	os << indent << "RightButtonAction: " << this->RightButtonAction << endl;
	os << indent << "LeftButtonAutoModifier: " <<
		this->LeftButtonAutoModifier << endl;
	os << indent << "MiddleButtonAutoModifier: " <<
		this->MiddleButtonAutoModifier << endl;
	os << indent << "RightButtonAutoModifier: " <<
		this->RightButtonAutoModifier << endl;
	os << indent << "UseContinuousCursor: "
		<< (this->UseContinuousCursor ? "On\n" : "Off\n");

	os << indent << "MarginSizeX: "
		<< this->MarginSizeX << "\n";
	os << indent << "MarginSizeY: "
		<< this->MarginSizeY << "\n";
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::BuildRepresentation()
{
	//TODO : SLCIE Plane
	//크기 조절시 여기를 참조할것
	this->PlaneSource->Update();

	double *o	= this->PlaneSource->GetOrigin();
	double *pt1 = this->PlaneSource->GetPoint1();
	double *pt2 = this->PlaneSource->GetPoint2();

	double x[3];
	x[0] = o[0] + (pt1[0] - o[0]) + (pt2[0] - o[0]);
	x[1] = o[1] + (pt1[1] - o[1]) + (pt2[1] - o[1]);
	x[2] = o[2] + (pt1[2] - o[2]) + (pt2[2] - o[2]);

	//whj : Modified를 호출함 중요함
	vtkPoints* points = this->PlaneOutlinePolyData->GetPoints();
	points->SetPoint(0, o);
	points->SetPoint(1, pt1);
	points->SetPoint(2, x);
	points->SetPoint(3, pt2);
	points->GetData()->Modified();
	this->PlaneOutlinePolyData->Modified();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::HighlightPlane(int highlight)
{
	//TODO : 선택받을때 사용하는 함수인듯
	if (highlight)
	{
		this->PlaneOutlineActor->SetProperty(this->SelectedPlaneProperty);
		this->PlanePicker->GetPickPosition(this->LastPickPosition);
	}
	else
	{
		this->PlaneOutlineActor->SetProperty(this->PlaneProperty);
	}
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::OnLeftButtonDown()
{
	switch (this->LeftButtonAction)
	{
	case VTKCustomResliceWidget::VTK_CURSOR_ACTION:
		this->StartCursor();
		break;
	case VTKCustomResliceWidget::VTK_SLICE_MOTION_ACTION:
		this->StartSliceMotion();
		break;
	case VTKCustomResliceWidget::VTK_WINDOW_LEVEL_ACTION:
		this->StartWindowLevel();
		break;
	}
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::OnLeftButtonUp()
{
	switch (this->LeftButtonAction)
	{
	case VTKCustomResliceWidget::VTK_CURSOR_ACTION:
		this->StopCursor();
		break;
	case VTKCustomResliceWidget::VTK_SLICE_MOTION_ACTION:
		this->StopSliceMotion();
		break;
	case VTKCustomResliceWidget::VTK_WINDOW_LEVEL_ACTION:
		this->StopWindowLevel();
		break;
	}
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::StartCursor()
{
	//TODO : 수정할 것!
	//		 해당 이미지를 선택하고 옵션을 선택할 시에 작동하도록 설정할것
	//		 하이라이트 이벤트 삭제할 것 => 해당 이벤트는 해당 슬라이스를 선택했을때 사용할 것

	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	// Okay, make sure that the pick is in the current renderer
	if (!this->CurrentRenderer || !this->CurrentRenderer->IsInViewport(X, Y))
	{
		this->State = VTKCustomResliceWidget::Outside;
		return;
	}

	// Okay, we can process this. If anything is picked, then we
	// can start pushing the plane.
	vtkAssemblyPath* path = this->GetAssemblyPath(X, Y, 0., this->PlanePicker);

	int found = 0;
	int i;
	if (path != nullptr)
	{
		// Deal with the possibility that we may be using a shared picker
		vtkCollectionSimpleIterator sit;
		path->InitTraversal(sit);
		vtkAssemblyNode *node;
		for (i = 0; i < path->GetNumberOfItems() && !found; i++)
		{
			node = path->GetNextNode(sit);
			if (node->GetViewProp() == vtkProp::SafeDownCast(this->TexturePlaneActor))
			{
				found = 1;
			}
		}
	}

	if (!found || path == nullptr)
	{
		this->State = VTKCustomResliceWidget::Outside;
		this->HighlightPlane(0);
		this->ActivateCursor(0);
		this->ActivateText(0);
		return;
	}
	else
	{
		this->State = VTKCustomResliceWidget::Cursoring;
		this->HighlightPlane(1);
		this->ActivateCursor(1);
		this->ActivateText(1);
		this->UpdateCursor(X, Y);
		this->ManageTextDisplay();
	}

	this->EventCallbackCommand->SetAbortFlag(1);
	this->StartInteraction();
	this->InvokeEvent(vtkCommand::StartInteractionEvent, nullptr);
	this->Interactor->Render();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::StopCursor()
{
	//TDOO : 하이라이트 이벤트 제거할것 
	if (this->State == VTKCustomResliceWidget::Outside ||
		this->State == VTKCustomResliceWidget::Start)
	{
		return;
	}

	this->State = VTKCustomResliceWidget::Start;
	this->HighlightPlane(0);
	this->ActivateCursor(0);
	this->ActivateText(0);

	this->EventCallbackCommand->SetAbortFlag(1);
	this->EndInteraction();
	this->InvokeEvent(vtkCommand::EndInteractionEvent, nullptr);
	this->Interactor->Render();
}


//----------------------------------------------------------------------------
void VTKCustomResliceWidget::StartSliceMotion()
{
	//TODO : 하이라이트 이벤트 재설정

	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	// Okay, make sure that the pick is in the current renderer
	if (!this->CurrentRenderer || !this->CurrentRenderer->IsInViewport(X, Y))
	{
		this->State = VTKCustomResliceWidget::Outside;
		return;
	}

	// Okay, we can process this. If anything is picked, then we
	// can start pushing or check for adjusted states.
	vtkAssemblyPath* path = this->GetAssemblyPath(X, Y, 0., this->PlanePicker);

	int found = 0;
	int i;
	if (path != nullptr)
	{
		// Deal with the possibility that we may be using a shared picker
		vtkCollectionSimpleIterator sit;
		path->InitTraversal(sit);
		vtkAssemblyNode *node;
		for (i = 0; i < path->GetNumberOfItems() && !found; i++)
		{
			node = path->GetNextNode(sit);
			if (node->GetViewProp() == vtkProp::SafeDownCast(this->TexturePlaneActor))
			{
				found = 1;
			}
		}
	}

	if (!found || path == nullptr)
	{
		this->State = VTKCustomResliceWidget::Outside;
		this->HighlightPlane(0);
		this->ActivateMargins(0);
		return;
	}
	else
	{
		this->State = VTKCustomResliceWidget::Pushing;
		this->HighlightPlane(1);
		this->ActivateMargins(1);
		this->AdjustState();
		this->UpdateMargins();
	}

	this->EventCallbackCommand->SetAbortFlag(1);
	this->StartInteraction();
	this->InvokeEvent(vtkCommand::StartInteractionEvent, nullptr);
	this->Interactor->Render();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::StopSliceMotion()
{
	//TODO : 작동원리 수정할것
	if (this->State == VTKCustomResliceWidget::Outside ||
		this->State == VTKCustomResliceWidget::Start)
	{
		return;
	}

	this->State = VTKCustomResliceWidget::Start;
	this->HighlightPlane(0);
	this->ActivateMargins(0);

	this->EventCallbackCommand->SetAbortFlag(1);
	this->EndInteraction();
	this->InvokeEvent(vtkCommand::EndInteractionEvent, nullptr);
	this->Interactor->Render();
}


#pragma region WINDOW LEVEL ACTION - 슬라이스 이미지 소명설정 (Not USE)


//----------------------------------------------------------------------------
void VTKCustomResliceWidget::StartWindowLevel()
{
	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	// Okay, make sure that the pick is in the current renderer
	if (!this->CurrentRenderer || !this->CurrentRenderer->IsInViewport(X, Y))
	{
		this->State = VTKCustomResliceWidget::Outside;
		return;
	}

	// Okay, we can process this. If anything is picked, then we
	// can start window-levelling.
	vtkAssemblyPath* path = this->GetAssemblyPath(X, Y, 0., this->PlanePicker);

	int found = 0;
	int i;
	if (path != nullptr)
	{
		// Deal with the possibility that we may be using a shared picker
		vtkCollectionSimpleIterator sit;
		path->InitTraversal(sit);
		vtkAssemblyNode *node;
		for (i = 0; i < path->GetNumberOfItems() && !found; i++)
		{
			node = path->GetNextNode(sit);
			if (node->GetViewProp() == vtkProp::SafeDownCast(this->TexturePlaneActor))
			{
				found = 1;
			}
		}
	}

	this->InitialWindow = this->CurrentWindow;
	this->InitialLevel = this->CurrentLevel;

	if (!found || path == nullptr)
	{
		this->State = VTKCustomResliceWidget::Outside;
		this->HighlightPlane(0);
		this->ActivateText(0);
		return;
	}
	else
	{
		this->State = VTKCustomResliceWidget::WindowLevelling;
		this->HighlightPlane(1);
		this->ActivateText(1);
		this->StartWindowLevelPositionX = X;
		this->StartWindowLevelPositionY = Y;
		this->ManageTextDisplay();
	}

	this->EventCallbackCommand->SetAbortFlag(1);
	this->StartInteraction();

	double wl[2] = { this->CurrentWindow, this->CurrentLevel };
	this->InvokeEvent(vtkCommand::StartWindowLevelEvent, wl);

	this->Interactor->Render();
}



//----------------------------------------------------------------------------
void VTKCustomResliceWidget::StopWindowLevel()
{
	if (this->State == VTKCustomResliceWidget::Outside ||
		this->State == VTKCustomResliceWidget::Start)
	{
		return;
	}

	this->State = VTKCustomResliceWidget::Start;
	this->HighlightPlane(0);
	this->ActivateText(0);

	this->EventCallbackCommand->SetAbortFlag(1);
	this->EndInteraction();

	double wl[2] = { this->CurrentWindow, this->CurrentLevel };
	this->InvokeEvent(vtkCommand::EndWindowLevelEvent, wl);

	this->Interactor->Render();
}

#pragma endregion

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::OnMouseMove()
{
	// See whether we're active
	//
	if (this->State == VTKCustomResliceWidget::Outside ||
		this->State == VTKCustomResliceWidget::Start)
	{
		return;
	}

	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	// Do different things depending on state
	// Calculations everybody does
	//
	double focalPoint[4], pickPoint[4], prevPickPoint[4];
	double z, vpn[3];

	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	if (!camera)
	{
		return;
	}

	// Compute the two points defining the motion vector
	//
	this->ComputeWorldToDisplay(this->LastPickPosition[0],
		this->LastPickPosition[1],
		this->LastPickPosition[2], focalPoint);
	z = focalPoint[2];

	this->ComputeDisplayToWorld(
		double(this->Interactor->GetLastEventPosition()[0]),
		double(this->Interactor->GetLastEventPosition()[1]),
		z, prevPickPoint);

	this->ComputeDisplayToWorld(double(X), double(Y), z, pickPoint);


	switch (this->State)
	{
	case VTKCustomResliceWidget::WindowLevelling : 
		this->WindowLevel(X, Y);
		this->ManageTextDisplay();
		break;
	
	case VTKCustomResliceWidget::Pushing :
		this->Push(prevPickPoint, pickPoint);
		this->UpdatePlane();
		this->UpdateMargins();
		this->BuildRepresentation();
		break;

	case VTKCustomResliceWidget::Spinning :
		this->Spin(prevPickPoint, pickPoint);
		this->UpdatePlane();
		this->UpdateMargins();
		this->BuildRepresentation();
		break;

	case VTKCustomResliceWidget::Rotating:
		camera->GetViewPlaneNormal(vpn);
		this->Rotate(prevPickPoint, pickPoint, vpn);
		this->UpdatePlane();
		this->UpdateMargins();
		this->BuildRepresentation();

	case  VTKCustomResliceWidget::Scaling:
		this->Scale(prevPickPoint, pickPoint, X, Y);
		this->UpdatePlane();
		this->UpdateMargins();
		this->BuildRepresentation();
		break;
	
	case VTKCustomResliceWidget::Moving:
		this->Translate(prevPickPoint, pickPoint);
		this->UpdatePlane();
		this->UpdateMargins();
		this->BuildRepresentation();
		break;
	
	case VTKCustomResliceWidget::Cursoring:
		this->UpdateCursor(X, Y);
		this->ManageTextDisplay();
		break;

	default:
		break;
	}
	
	// Interact, if desired
	//
	this->EventCallbackCommand->SetAbortFlag(1);

	if (this->State == VTKCustomResliceWidget::WindowLevelling)
	{
		double wl[2] = { this->CurrentWindow, this->CurrentLevel };
		this->InvokeEvent(vtkCommand::WindowLevelEvent, wl);
	}
	else
	{
		this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
	}

	this->Interactor->Render();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::InvertTable()
{
	//TODO : for WINDOW LEVEL
	//		 사용 안할듯
	int index = this->LookupTable->GetNumberOfTableValues();
	unsigned char swap[4];
	size_t num = 4 * sizeof(unsigned char);
	vtkUnsignedCharArray* table = this->LookupTable->GetTable();
	for (int count = 0; count < --index; count++)
	{
		unsigned char *rgba1 = table->GetPointer(4 * count);
		unsigned char *rgba2 = table->GetPointer(4 * index);
		memcpy(swap, rgba1, num);
		memcpy(rgba1, rgba2, num);
		memcpy(rgba2, swap, num);
	}

	// force the lookuptable to update its InsertTime to avoid
	// rebuilding the array
	double temp[4];
	this->LookupTable->GetTableValue(0, temp);
	this->LookupTable->SetTableValue(0, temp);
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::SetWindowLevel(double window, double level, int copy)
{
	//TODO : for WINDOW LEVEL
	//		 사용 안할듯
	if (copy)
	{
		this->CurrentWindow = window;
		this->CurrentLevel = level;
		return;
	}

	if (this->CurrentWindow == window && this->CurrentLevel == level)
	{
		return;
	}

	// if the new window is negative and the old window was positive invert table
	if (((window < 0 && this->CurrentWindow > 0) ||
		(window > 0 && this->CurrentWindow < 0)) &&
		!this->UserControlledLookupTable)
	{
		this->InvertTable();
	}

	this->CurrentWindow = window;
	this->CurrentLevel = level;

	if (!this->UserControlledLookupTable)
	{
		double rmin = this->CurrentLevel - 0.5*fabs(this->CurrentWindow);
		double rmax = rmin + fabs(this->CurrentWindow);
		this->LookupTable->SetTableRange(rmin, rmax);
	}

	if (this->Enabled)
	{
		this->Interactor->Render();
	}
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::GetWindowLevel(double wl[2])
{
	wl[0] = this->CurrentWindow;
	wl[1] = this->CurrentLevel;
}

//----------------------------------------------------------------------------
int VTKCustomResliceWidget::GetCursorData(double xyzv[4])
{
	if (this->State != VTKCustomResliceWidget::Cursoring || \
		this->CurrentImageValue == VTK_UNSIGNED_SHORT_MAX)
	{
		return 0;
	}

	xyzv[0] = this->CurrentCursorPosition[0];
	xyzv[1] = this->CurrentCursorPosition[1];
	xyzv[2] = this->CurrentCursorPosition[2];
	xyzv[3] = this->CurrentImageValue;

	return 1;
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::ManageTextDisplay()
{
	if (!this->DisplayText)
	{
		return;
	}

	if (this->State == VTKCustomResliceWidget::WindowLevelling)
	{
		snprintf(this->TextBuff,
			VTK_IMAGE_PLANE_WIDGET_MAX_TEXTBUFF,
			"Window, Level: ( %g, %g )",
			this->CurrentWindow,
			this->CurrentLevel);
	}
	else if (this->State == VTKCustomResliceWidget::Cursoring)
	{
		if (this->CurrentImageValue == VTK_DOUBLE_MAX)
		{
			snprintf(this->TextBuff,
				VTK_IMAGE_PLANE_WIDGET_MAX_TEXTBUFF,
				"Off Image");
		}
		else
		{
			snprintf(this->TextBuff,
				VTK_IMAGE_PLANE_WIDGET_MAX_TEXTBUFF,
				"( %g, %g, %g ): %g",
				this->CurrentCursorPosition[0],
				this->CurrentCursorPosition[1],
				this->CurrentCursorPosition[2],
				this->CurrentImageValue);
		}
	}

	this->TextActor->SetInput(this->TextBuff);
	this->TextActor->Modified();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::Push(double *p1, double *p2)
{
	// Get the motion vector
	//
	double v[3];
	v[0] = p2[0] - p1[0];
	v[1] = p2[1] - p1[1];
	v[2] = p2[2] - p1[2];

	//Scala 곱으로 이동시키는 작업
	//A·B = C   => C = |A|cosΘ
	//방향은 B과 평행하게 가짐
	this->PlaneSource->Push(vtkMath::Dot(v, this->PlaneSource->GetNormal()));
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::CreateDefaultProperties()
{
	if (!this->PlaneProperty)
	{
		this->PlaneProperty = vtkProperty::New();
		this->PlaneProperty->SetAmbient(1);
		this->PlaneProperty->SetColor(1, 1, 1);
		this->PlaneProperty->SetRepresentationToWireframe();
		this->PlaneProperty->SetInterpolationToFlat();
	}

	if (!this->SelectedPlaneProperty)
	{
		this->SelectedPlaneProperty = vtkProperty::New();
		this->SelectedPlaneProperty->SetAmbient(1);
		this->SelectedPlaneProperty->SetColor(1, 0, 0);
		this->SelectedPlaneProperty->SetRepresentationToWireframe();
		this->SelectedPlaneProperty->SetInterpolationToFlat();
	}

	if (!this->CursorProperty)
	{
		this->CursorProperty = vtkProperty::New();
		this->CursorProperty->SetAmbient(1);
		this->CursorProperty->SetColor(1, 0, 0);
		this->CursorProperty->SetRepresentationToWireframe();
		this->CursorProperty->SetInterpolationToFlat();
	}
#pragma region  maybe Not USE
	//TODO : 사용하지 않으므로 해당 연결 내용을 전부 제거해주세요
	if (!this->MarginProperty)
	{
		this->MarginProperty = vtkProperty::New();
		this->MarginProperty->SetAmbient(1);
		this->MarginProperty->SetColor(0, 0, 1);
		this->MarginProperty->SetRepresentationToWireframe();
		this->MarginProperty->SetInterpolationToFlat();
	}

	if (!this->TexturePlaneProperty)
	{
		this->TexturePlaneProperty = vtkProperty::New();
		this->TexturePlaneProperty->SetAmbient(1);
		this->TexturePlaneProperty->SetDiffuse(0);
		this->TexturePlaneProperty->SetInterpolationToFlat();
	}
#pragma endregion
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::PlaceWidget(double bds[6])
{
	double bounds[6], center[3];

	this->AdjustBounds(bds, bounds, center);

	if (this->PlaneOrientation == 1)
	{
		this->PlaneSource->SetOrigin(bounds[0], center[1], bounds[4]);
		this->PlaneSource->SetPoint1(bounds[1], center[1], bounds[4]);
		this->PlaneSource->SetPoint2(bounds[0], center[1], bounds[5]);
	}
	else if (this->PlaneOrientation == 2)
	{
		this->PlaneSource->SetOrigin(bounds[0], bounds[2], center[2]);
		this->PlaneSource->SetPoint1(bounds[1], bounds[2], center[2]);
		this->PlaneSource->SetPoint2(bounds[0], bounds[3], center[2]);
	}
	else //default or x-normal
	{
		this->PlaneSource->SetOrigin(center[0], bounds[2], bounds[4]);
		this->PlaneSource->SetPoint1(center[0], bounds[3], bounds[4]);
		this->PlaneSource->SetPoint2(center[0], bounds[2], bounds[5]);
	}

	this->UpdatePlane();
	this->BuildRepresentation();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::SetPlaneOrientation(int i)
{
	// Generate a XY plane if i = 2, z-normal
	// or a YZ plane if i = 0, x-normal
	// or a ZX plane if i = 1, y-normal
	//
	this->PlaneOrientation = i;

	// This method must be called _after_ SetInput
	//
	if (!this->ImageData)
	{
		vtkErrorMacro(<< "SetInput() before setting plane orientation.");
		return;
	}

	vtkAlgorithm* inpAlg = this->Reslice->GetInputAlgorithm();
	inpAlg->UpdateInformation();
	vtkInformation* outInfo = inpAlg->GetOutputInformation(0);
	int extent[6];
	outInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent);
	double origin[3];
	outInfo->Get(vtkDataObject::ORIGIN(), origin);
	double spacing[3];
	outInfo->Get(vtkDataObject::SPACING(), spacing);

	// Prevent obscuring voxels by offsetting the plane geometry
	//
	double xbounds[] = { origin[0] + spacing[0] * (extent[0] - 0.5),
					   origin[0] + spacing[0] * (extent[1] + 0.5) };
	double ybounds[] = { origin[1] + spacing[1] * (extent[2] - 0.5),
					   origin[1] + spacing[1] * (extent[3] + 0.5) };
	double zbounds[] = { origin[2] + spacing[2] * (extent[4] - 0.5),
					   origin[2] + spacing[2] * (extent[5] + 0.5) };

	if (spacing[0] < 0.0)
	{
		double t = xbounds[0];
		xbounds[0] = xbounds[1];
		xbounds[1] = t;
	}
	if (spacing[1] < 0.0)
	{
		double t = ybounds[0];
		ybounds[0] = ybounds[1];
		ybounds[1] = t;
	}
	if (spacing[2] < 0.0)
	{
		double t = zbounds[0];
		zbounds[0] = zbounds[1];
		zbounds[1] = t;
	}

	if (i == 2) //XY, z-normal
	{
		this->PlaneSource->SetOrigin(xbounds[0], ybounds[0], zbounds[0]);
		this->PlaneSource->SetPoint1(xbounds[1], ybounds[0], zbounds[0]);
		this->PlaneSource->SetPoint2(xbounds[0], ybounds[1], zbounds[0]);
	}
	else if (i == 0) //YZ, x-normal
	{
		this->PlaneSource->SetOrigin(xbounds[0], ybounds[0], zbounds[0]);
		this->PlaneSource->SetPoint1(xbounds[0], ybounds[1], zbounds[0]);
		this->PlaneSource->SetPoint2(xbounds[0], ybounds[0], zbounds[1]);
	}
	else  //ZX, y-normal
	{
		this->PlaneSource->SetOrigin(xbounds[0], ybounds[0], zbounds[0]);
		this->PlaneSource->SetPoint1(xbounds[0], ybounds[0], zbounds[1]);
		this->PlaneSource->SetPoint2(xbounds[1], ybounds[0], zbounds[0]);
	}

	this->UpdatePlane();
	this->BuildRepresentation();
	this->Modified();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::SetInputConnection(vtkAlgorithmOutput* aout)
{
	this->Superclass::SetInputConnection(aout);

	this->ImageData = vtkImageData::SafeDownCast(
		aout->GetProducer()->GetOutputDataObject(
			aout->GetIndex()));

	if (!this->ImageData)
	{
		// If nullptr is passed, remove any reference that Reslice had
		// on the old ImageData
		//
		this->Reslice->SetInputData(nullptr);
		return;
	}

	double range[2] = { 0, 65535 };

	//TODO: 수정할것 여기 시간이 대부분임
	//whj : 오래 걸리는 구간 히스토그램에서 넘겨준다고 가정하면 될듯하다
	//this->ImageData->GetScalarRange(range);


	if (!this->UserControlledLookupTable)
	{
		this->LookupTable->SetTableRange(range[0], range[1]);
		this->LookupTable->Build();
	}

	this->OriginalWindow = range[1] - range[0];
	this->OriginalLevel = 0.5*(range[0] + range[1]);

	if (fabs(this->OriginalWindow) < 0.001)
	{
		this->OriginalWindow = 0.001 * (this->OriginalWindow < 0.0 ? -1 : 1);
	}
	if (fabs(this->OriginalLevel) < 0.001)
	{
		this->OriginalLevel = 0.001 * (this->OriginalLevel < 0.0 ? -1 : 1);
	}

	this->SetWindowLevel(this->OriginalWindow, this->OriginalLevel);

	this->Reslice->SetInputConnection(aout);
	int interpolate = this->ResliceInterpolate;
	this->ResliceInterpolate = -1; // Force change
	this->SetResliceInterpolate(interpolate);

	this->ColorMap->SetInputConnection(this->Reslice->GetOutputPort());

	this->Texture->SetInputConnection(this->ColorMap->GetOutputPort());
	this->Texture->SetInterpolate(this->TextureInterpolate);

	this->SetPlaneOrientation(this->PlaneOrientation);
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::UpdatePlane()
{
	if (!this->Reslice || !this->ImageData)
	{
		return;
	}

	// Calculate appropriate pixel spacing for the reslicing
	//
	vtkAlgorithm* inpAlg = this->Reslice->GetInputAlgorithm();
	inpAlg->UpdateInformation();
	vtkInformation* outInfo = inpAlg->GetOutputInformation(0);
	double spacing[3];
	outInfo->Get(vtkDataObject::SPACING(), spacing);
	double origin[3];
	outInfo->Get(vtkDataObject::ORIGIN(), origin);
	int extent[6];
	outInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent);

	int i;

	for (i = 0; i < 3; i++)
	{
		if (extent[2 * i] > extent[2 * i + 1])
		{
			vtkErrorMacro("Invalid extent ["
				<< extent[0] << ", " << extent[1] << ", "
				<< extent[2] << ", " << extent[3] << ", "
				<< extent[4] << ", " << extent[5] << "]."
				<< " Perhaps the input data is empty?");
			break;
		}
	}
	//TODO: 이건 뭔 값이지?

	if (this->RestrictPlaneToVolume)
	{
		double bounds[] = { origin[0] + spacing[0] * extent[0], //xmin
						   origin[0] + spacing[0] * extent[1], //xmax
						   origin[1] + spacing[1] * extent[2], //ymin
						   origin[1] + spacing[1] * extent[3], //ymax
						   origin[2] + spacing[2] * extent[4], //zmin
						   origin[2] + spacing[2] * extent[5] };//zmax

		for (i = 0; i <= 4; i += 2) // reverse bounds if necessary
		{
			if (bounds[i] > bounds[i + 1])
			{
				double t = bounds[i + 1];
				bounds[i + 1] = bounds[i];
				bounds[i] = t;
			}
		}

		double abs_normal[3];
		this->PlaneSource->GetNormal(abs_normal);
		double planeCenter[3];
		this->PlaneSource->GetCenter(planeCenter);
		double nmax = 0.0;
		int k = 0;
		for (i = 0; i < 3; i++)
		{
			abs_normal[i] = fabs(abs_normal[i]);
			if (abs_normal[i] > nmax)
			{
				nmax = abs_normal[i];
				k = i;
			}
		}
		// Force the plane to lie within the true image bounds along its normal
		//
		if (planeCenter[k] > bounds[2 * k + 1])
		{
			planeCenter[k] = bounds[2 * k + 1];
		}
		else if (planeCenter[k] < bounds[2 * k])
		{
			planeCenter[k] = bounds[2 * k];
		}

		this->PlaneSource->SetCenter(planeCenter);
	}

	double planeAxis1[3];
	double planeAxis2[3];

	this->GetVector1(planeAxis1);
	this->GetVector2(planeAxis2);

	// The x,y dimensions of the plane
	//
	double planeSizeX = vtkMath::Normalize(planeAxis1);
	double planeSizeY = vtkMath::Normalize(planeAxis2);

	double normal[3];
	this->PlaneSource->GetNormal(normal);

	// Generate the slicing matrix
	//

	this->ResliceAxes->Identity();
	for (i = 0; i < 3; i++)
	{
		this->ResliceAxes->SetElement(0, i, planeAxis1[i]);
		this->ResliceAxes->SetElement(1, i, planeAxis2[i]);
		this->ResliceAxes->SetElement(2, i, normal[i]);
	}

	double planeOrigin[4];
	this->PlaneSource->GetOrigin(planeOrigin);

	planeOrigin[3] = 1.0;

	this->ResliceAxes->Transpose();
	this->ResliceAxes->SetElement(0, 3, planeOrigin[0]);
	this->ResliceAxes->SetElement(1, 3, planeOrigin[1]);
	this->ResliceAxes->SetElement(2, 3, planeOrigin[2]);

	this->Reslice->SetResliceAxes(this->ResliceAxes);

	double spacingX = fabs(planeAxis1[0] * spacing[0]) +
		fabs(planeAxis1[1] * spacing[1]) +
		fabs(planeAxis1[2] * spacing[2]);

	double spacingY = fabs(planeAxis2[0] * spacing[0]) +
		fabs(planeAxis2[1] * spacing[1]) +
		fabs(planeAxis2[2] * spacing[2]);


	// Pad extent up to a power of two for efficient texture mapping

	// make sure we're working with valid values
	double realExtentX = (spacingX == 0) ? VTK_INT_MAX : planeSizeX / spacingX;

	int extentX;
	// Sanity check the input data:
	// * if realExtentX is too large, extentX will wrap
	// * if spacingX is 0, things will blow up.
	if (realExtentX > (VTK_INT_MAX >> 1))
	{
		vtkErrorMacro(<< "Invalid X extent: " << realExtentX);
		extentX = 0;
	}
	else
	{
		extentX = 1;
		while (extentX < realExtentX)
		{
			extentX = extentX << 1;
		}
	}

	// make sure extentY doesn't wrap during padding
	double realExtentY = (spacingY == 0) ? VTK_INT_MAX : planeSizeY / spacingY;

	int extentY;
	if (realExtentY > (VTK_INT_MAX >> 1))
	{
		vtkErrorMacro(<< "Invalid Y extent: " << realExtentY);
		extentY = 0;
	}
	else
	{
		extentY = 1;
		while (extentY < realExtentY)
		{
			extentY = extentY << 1;
		}
	}

	double outputSpacingX = (extentX == 0) ? 1.0 : planeSizeX / extentX;
	double outputSpacingY = (extentY == 0) ? 1.0 : planeSizeY / extentY;
	this->Reslice->SetOutputSpacing(outputSpacingX, outputSpacingY, 1);
	this->Reslice->SetOutputOrigin(0.5*outputSpacingX, 0.5*outputSpacingY, 0);
	this->Reslice->SetOutputExtent(0, extentX - 1, 0, extentY - 1, 0, 0);
}

//----------------------------------------------------------------------------
vtkImageData* VTKCustomResliceWidget::GetResliceOutput()
{
	if (!this->Reslice)
	{
		return nullptr;
	}
	return this->Reslice->GetOutput();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::SetResliceInterpolate(int i)
{
	if (this->ResliceInterpolate == i)
	{
		return;
	}
	this->ResliceInterpolate = i;
	this->Modified();

	if (!this->Reslice)
	{
		return;
	}

	if (i == VTK_NEAREST_RESLICE)
	{
		this->Reslice->SetInterpolationModeToNearestNeighbor();
	}
	else if (i == VTK_LINEAR_RESLICE)
	{
		this->Reslice->SetInterpolationModeToLinear();
	}
	else
	{
		this->Reslice->SetInterpolationModeToCubic();
	}
	this->Texture->SetInterpolate(this->TextureInterpolate);
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::SetPicker(vtkAbstractPropPicker* picker)
{
	// we have to have a picker for slice motion, window level and cursor to work
	if (this->PlanePicker != picker)
	{
		// to avoid destructor recursion
		vtkAbstractPropPicker *temp = this->PlanePicker;
		this->PlanePicker = picker;
		if (temp != nullptr)
		{
			temp->UnRegister(this);
		}

		int delPicker = 0;
		if (this->PlanePicker == nullptr)
		{
			this->PlanePicker = vtkCellPicker::New();
			vtkCellPicker::SafeDownCast(this->PlanePicker)->SetTolerance(0.005);
			delPicker = 1;
		}

		this->PlanePicker->Register(this);
		this->PlanePicker->AddPickList(this->TexturePlaneActor);
		this->PlanePicker->PickFromListOn();

		if (delPicker)
		{
			this->PlanePicker->Delete();
		}
	}
}

//------------------------------------------------------------------------------
void VTKCustomResliceWidget::RegisterPickers()
{
	this->Interactor->GetPickingManager()->AddPicker(this->PlanePicker, this);
}

//----------------------------------------------------------------------------
vtkLookupTable* VTKCustomResliceWidget::CreateDefaultLookupTable()
{
	vtkLookupTable* lut = vtkLookupTable::New();
	lut->Register(this);
	lut->Delete();
	lut->SetNumberOfColors(256);
	lut->SetHueRange(0, 0);
	lut->SetSaturationRange(0, 0);
	lut->SetValueRange(0, 1);
	lut->SetAlphaRange(1, 1);
	lut->Build();
	return lut;
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::SetLookupTable(vtkLookupTable* table)
{
	if (this->LookupTable != table)
	{
		// to avoid destructor recursion
		vtkLookupTable *temp = this->LookupTable;
		this->LookupTable = table;
		if (temp != nullptr)
		{
			temp->UnRegister(this);
		}
		if (this->LookupTable != nullptr)
		{
			this->LookupTable->Register(this);
		}
		else  //create a default lut
		{
			this->LookupTable = this->CreateDefaultLookupTable();
		}
	}

	this->ColorMap->SetLookupTable(this->LookupTable);
	this->Texture->SetLookupTable(this->LookupTable);

	if (this->ImageData && !this->UserControlledLookupTable)
	{
		double range[2];
		this->ImageData->GetScalarRange(range);

		this->LookupTable->SetTableRange(range[0], range[1]);
		this->LookupTable->Build();

		this->OriginalWindow = range[1] - range[0];
		this->OriginalLevel = 0.5*(range[0] + range[1]);

		if (fabs(this->OriginalWindow) < 0.001)
		{
			this->OriginalWindow = 0.001 * (this->OriginalWindow < 0.0 ? -1 : 1);
		}
		if (fabs(this->OriginalLevel) < 0.001)
		{
			this->OriginalLevel = 0.001 * (this->OriginalLevel < 0.0 ? -1 : 1);
		}

		this->SetWindowLevel(this->OriginalWindow, this->OriginalLevel);
	}
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::SetSlicePosition(double position)
{
	double amount = 0.0;
	double planeOrigin[3];
	this->PlaneSource->GetOrigin(planeOrigin);

	if (this->PlaneOrientation == 2) // z axis
	{
		amount = position - planeOrigin[2];
	}
	else if (this->PlaneOrientation == 0) // x axis
	{
		amount = position - planeOrigin[0];
	}
	else if (this->PlaneOrientation == 1)  //y axis
	{
		amount = position - planeOrigin[1];
	}
	else
	{
		vtkGenericWarningMacro("only works for ortho planes: set plane orientation first");
		return;
	}

	this->PlaneSource->Push(amount);
	this->UpdatePlane();
	this->BuildRepresentation();
	this->Modified();
}


//----------------------------------------------------------------------------
void VTKCustomResliceWidget::SetSliceIndex(int index)
{
	if (!this->Reslice)
	{
		return;
	}
	if (!this->ImageData)
	{
		return;
	}
	vtkAlgorithm* inpAlg = this->Reslice->GetInputAlgorithm();
	inpAlg->UpdateInformation();
	vtkInformation* outInfo = inpAlg->GetOutputInformation(0);
	double origin[3];
	outInfo->Get(vtkDataObject::ORIGIN(), origin);
	double spacing[3];
	outInfo->Get(vtkDataObject::SPACING(), spacing);
	double planeOrigin[3];
	this->PlaneSource->GetOrigin(planeOrigin);
	double pt1[3];
	this->PlaneSource->GetPoint1(pt1);
	double pt2[3];
	this->PlaneSource->GetPoint2(pt2);

	if (this->PlaneOrientation == 2)
	{
		planeOrigin[2] = origin[2] + index * spacing[2];
		pt1[2] = planeOrigin[2];
		pt2[2] = planeOrigin[2];
	}
	else if (this->PlaneOrientation == 1)
	{
		planeOrigin[1] = origin[1] + index * spacing[1];
		pt1[1] = planeOrigin[1];
		pt2[1] = planeOrigin[1];
	}
	else if (this->PlaneOrientation == 0)
	{
		planeOrigin[0] = origin[0] + index * spacing[0];
		pt1[0] = planeOrigin[0];
		pt2[0] = planeOrigin[0];
	}
	else
	{
		vtkGenericWarningMacro("only works for ortho planes: set plane orientation first");
		return;
	}

	this->PlaneSource->SetOrigin(planeOrigin);
	this->PlaneSource->SetPoint1(pt1);
	this->PlaneSource->SetPoint2(pt2);
	this->UpdatePlane();
	this->BuildRepresentation();
	this->Modified();
}

//----------------------------------------------------------------------------
int VTKCustomResliceWidget::GetSliceIndex()
{
	if (!this->Reslice)
	{
		return 0;
	}
	if (!this->ImageData)
	{
		return 0;
	}
	vtkAlgorithm* inpAlg = this->Reslice->GetInputAlgorithm();
	inpAlg->UpdateInformation();
	vtkInformation* outInfo = inpAlg->GetOutputInformation(0);
	double origin[3];
	outInfo->Get(vtkDataObject::ORIGIN(), origin);
	double spacing[3];
	outInfo->Get(vtkDataObject::SPACING(), spacing);
	double planeOrigin[3];
	this->PlaneSource->GetOrigin(planeOrigin);

	if (this->PlaneOrientation == 2)
	{
		return vtkMath::Round((planeOrigin[2] - origin[2]) / spacing[2]);
	}
	else if (this->PlaneOrientation == 1)
	{
		return vtkMath::Round((planeOrigin[1] - origin[1]) / spacing[1]);
	}
	else if (this->PlaneOrientation == 0)
	{
		return vtkMath::Round((planeOrigin[0] - origin[0]) / spacing[0]);
	}
	else
	{
		vtkGenericWarningMacro("only works for ortho planes: set plane orientation first");
	}

	return 0;
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::ActivateCursor(int i)
{

	if (!this->CurrentRenderer)
	{
		return;
	}

	if (i == 0)
	{
		this->CursorActor->VisibilityOff();
	}
	else
	{
		this->CursorActor->VisibilityOn();
	}
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::ActivateMargins(int i)
{

	if (!this->CurrentRenderer)
	{
		return;
	}

	if (i == 0)
	{
		this->MarginActor->VisibilityOff();
	}
	else
	{
		this->MarginActor->VisibilityOn();
	}
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::ActivateText(int i)
{
	if (!this->CurrentRenderer || !this->DisplayText)
	{
		return;
	}

	if (i == 0)
	{
		this->TextActor->VisibilityOff();
	}
	else
	{
		this->TextActor->VisibilityOn();
	}
}


//----------------------------------------------------------------------------
void VTKCustomResliceWidget::UpdateCursor(int X, int Y)
{
	if (!this->ImageData)
	{
		return;
	}
	// We're going to be extracting values with GetScalarComponentAsDouble(),
	// we might as well make sure that the data is there.  If the data is
	// up to date already, this call doesn't cost very much.  If we don't make
	// this call and the data is not up to date, the GetScalar... call will
	// cause a segfault.
	this->Reslice->GetInputAlgorithm()->Update();

	vtkAssemblyPath* path = this->GetAssemblyPath(X, Y, 0., this->PlanePicker);

	this->CurrentImageValue = VTK_DOUBLE_MAX;

	int found = 0;
	int i;
	if (path)
	{
		// Deal with the possibility that we may be using a shared picker
		vtkCollectionSimpleIterator sit;
		path->InitTraversal(sit);
		vtkAssemblyNode *node;
		for (i = 0; i < path->GetNumberOfItems() && !found; i++)
		{
			node = path->GetNextNode(sit);
			if (node->GetViewProp() == vtkProp::SafeDownCast(this->TexturePlaneActor))
			{
				found = 1;
			}
		}
	}

	if (!found || path == nullptr)
	{
		this->CursorActor->VisibilityOff();
		return;
	}
	else
	{
		this->CursorActor->VisibilityOn();
	}

	double q[3];
	this->PlanePicker->GetPickPosition(q);

	if (this->UseContinuousCursor)
	{
		found = this->UpdateContinuousCursor(q);
	}
	else
	{
		found = this->UpdateDiscreteCursor(q);
	}

	if (!found)
	{
		this->CursorActor->VisibilityOff();
		return;
	}

	double o[3];
	this->PlaneSource->GetOrigin(o);

	// q relative to the plane origin
	//
	double qro[3];
	qro[0] = q[0] - o[0];
	qro[1] = q[1] - o[1];
	qro[2] = q[2] - o[2];

	double p1o[3];
	double p2o[3];

	this->GetVector1(p1o);
	this->GetVector2(p2o);

	double Lp1 = vtkMath::Dot(qro, p1o) / vtkMath::Dot(p1o, p1o);
	double Lp2 = vtkMath::Dot(qro, p2o) / vtkMath::Dot(p2o, p2o);

	double p1[3];
	this->PlaneSource->GetPoint1(p1);
	double p2[3];
	this->PlaneSource->GetPoint2(p2);

	double a[3];
	double b[3];
	double c[3];
	double d[3];

	for (i = 0; i < 3; i++)
	{
		a[i] = o[i] + Lp2 * p2o[i];   // left
		b[i] = p1[i] + Lp2 * p2o[i];   // right
		c[i] = o[i] + Lp1 * p1o[i];   // bottom
		d[i] = p2[i] + Lp1 * p1o[i];   // top
	}

	vtkPoints* cursorPts = this->CursorPolyData->GetPoints();

	cursorPts->SetPoint(0, a);
	cursorPts->SetPoint(1, b);
	cursorPts->SetPoint(2, c);
	cursorPts->SetPoint(3, d);
	cursorPts->GetData()->Modified();

	this->CursorPolyData->Modified();
}



//----------------------------------------------------------------------------
int VTKCustomResliceWidget::UpdateContinuousCursor(double *q)
{
	double tol2;
	vtkCell *cell;
	vtkPointData *pd;
	int subId;
	double pcoords[3], weights[8];

	this->CurrentCursorPosition[0] = q[0];
	this->CurrentCursorPosition[1] = q[1];
	this->CurrentCursorPosition[2] = q[2];

	pd = this->ImageData->GetPointData();

	vtkPointData* outPD = vtkPointData::New();
	outPD->InterpolateAllocate(pd, 1, 1);

	// Use tolerance as a function of size of source data
	//
	tol2 = this->ImageData->GetLength();
	tol2 = tol2 ? tol2 * tol2 / 1000.0 : 0.001;

	// Find the cell that contains q and get it
	//
	cell = this->ImageData->FindAndGetCell(q, nullptr, -1, tol2, subId, pcoords, weights);
	int found = 0;
	if (cell)
	{
		// Interpolate the point data
		//
		outPD->InterpolatePoint(pd, 0, cell->PointIds, weights);
		this->CurrentImageValue = outPD->GetScalars()->GetTuple1(0);
		found = 1;
	}

	outPD->Delete();
	return found;
}

//----------------------------------------------------------------------------
int VTKCustomResliceWidget::UpdateDiscreteCursor(double *q)
{
	// vtkImageData will find the nearest implicit point to q
	//
	vtkIdType ptId = this->ImageData->FindPoint(q);

	if (ptId == -1)
	{
		return 0;
	}

	double closestPt[3];
	this->ImageData->GetPoint(ptId, closestPt);

	double origin[3];
	this->ImageData->GetOrigin(origin);
	double spacing[3];
	this->ImageData->GetSpacing(spacing);
	int extent[6];
	this->ImageData->GetExtent(extent);

	int iq[3];
	int iqtemp;
	for (int i = 0; i < 3; i++)
	{
		// compute world to image coords
		iqtemp = vtkMath::Round((closestPt[i] - origin[i]) / spacing[i]);

		// we have a valid pick already, just enforce bounds check
		iq[i] = (iqtemp < extent[2 * i]) ? extent[2 * i] : ((iqtemp > extent[2 * i + 1]) ? extent[2 * i + 1] : iqtemp);

		// compute image to world coords
		q[i] = iq[i] * spacing[i] + origin[i];

		this->CurrentCursorPosition[i] = iq[i];
	}

	this->CurrentImageValue = this->ImageData->GetScalarComponentAsDouble(\
		static_cast<int>(this->CurrentCursorPosition[0]),
		static_cast<int>(this->CurrentCursorPosition[1]),
		static_cast<int>(this->CurrentCursorPosition[2]), 0);
	return 1;
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::SetOrigin(double x, double y, double z)
{
	this->PlaneSource->SetOrigin(x, y, z);
	this->Modified();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::SetOrigin(double xyz[3])
{
	this->PlaneSource->SetOrigin(xyz);
	this->Modified();
}

//----------------------------------------------------------------------------
double* VTKCustomResliceWidget::GetOrigin()
{
	return this->PlaneSource->GetOrigin();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::GetOrigin(double xyz[3])
{
	this->PlaneSource->GetOrigin(xyz);
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::SetPoint1(double x, double y, double z)
{
	this->PlaneSource->SetPoint1(x, y, z);
	this->Modified();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::SetPoint1(double xyz[3])
{
	this->PlaneSource->SetPoint1(xyz);
	this->Modified();
}

//----------------------------------------------------------------------------
double* VTKCustomResliceWidget::GetPoint1()
{
	return this->PlaneSource->GetPoint1();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::GetPoint1(double xyz[3])
{
	this->PlaneSource->GetPoint1(xyz);
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::SetPoint2(double x, double y, double z)
{
	this->PlaneSource->SetPoint2(x, y, z);
	this->Modified();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::SetPoint2(double xyz[3])
{
	this->PlaneSource->SetPoint2(xyz);
	this->Modified();
}

//----------------------------------------------------------------------------
double* VTKCustomResliceWidget::GetPoint2()
{
	return this->PlaneSource->GetPoint2();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::GetPoint2(double xyz[3])
{
	this->PlaneSource->GetPoint2(xyz);
}

//----------------------------------------------------------------------------
double* VTKCustomResliceWidget::GetCenter()
{
	return this->PlaneSource->GetCenter();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::GetCenter(double xyz[3])
{
	this->PlaneSource->GetCenter(xyz);
}

//----------------------------------------------------------------------------
double* VTKCustomResliceWidget::GetNormal()
{
	return this->PlaneSource->GetNormal();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::GetNormal(double xyz[3])
{
	this->PlaneSource->GetNormal(xyz);
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::GetPolyData(vtkPolyData *pd)
{
	pd->ShallowCopy(this->PlaneSource->GetOutput());
}

//----------------------------------------------------------------------------
vtkPolyDataAlgorithm *VTKCustomResliceWidget::GetPolyDataAlgorithm()
{
	return this->PlaneSource;
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::UpdatePlacement(void)
{
	this->UpdatePlane();
	this->UpdateMargins();
	this->BuildRepresentation();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::SetTextProperty(vtkTextProperty* tprop)
{
	this->TextActor->SetTextProperty(tprop);
}

//----------------------------------------------------------------------------
vtkTextProperty* VTKCustomResliceWidget::GetTextProperty()
{
	return this->TextActor->GetTextProperty();
}

//----------------------------------------------------------------------------
vtkTexture* VTKCustomResliceWidget::GetTexture()
{
	return this->Texture;
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::GetVector1(double v1[3])
{
	double* p1 = this->PlaneSource->GetPoint1();
	double* o = this->PlaneSource->GetOrigin();
	v1[0] = p1[0] - o[0];
	v1[1] = p1[1] - o[1];
	v1[2] = p1[2] - o[2];
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::GetVector2(double v2[3])
{
	double* p2 = this->PlaneSource->GetPoint2();
	double* o = this->PlaneSource->GetOrigin();
	v2[0] = p2[0] - o[0];
	v2[1] = p2[1] - o[1];
	v2[2] = p2[2] - o[2];
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::AdjustState()
{
	int *auto_modifier = nullptr;
	switch (this->LastButtonPressed)
	{
	case VTKCustomResliceWidget::VTK_LEFT_BUTTON:
		auto_modifier = &this->LeftButtonAutoModifier;
		break;
	case VTKCustomResliceWidget::VTK_MIDDLE_BUTTON:
		auto_modifier = &this->MiddleButtonAutoModifier;
		break;
	case VTKCustomResliceWidget::VTK_RIGHT_BUTTON:
		auto_modifier = &this->RightButtonAutoModifier;
		break;
	}

	if (this->Interactor->GetShiftKey() ||
		(auto_modifier &&
		(*auto_modifier & VTKCustomResliceWidget::VTK_SHIFT_MODIFIER)))
	{
		this->State = VTKCustomResliceWidget::Scaling;
		return;
	}

	double v1[3];
	this->GetVector1(v1);
	double v2[3];
	this->GetVector2(v2);
	double planeSize1 = vtkMath::Normalize(v1);
	double planeSize2 = vtkMath::Normalize(v2);
	double* planeOrigin = this->PlaneSource->GetOrigin();

	double ppo[3] = { this->LastPickPosition[0] - planeOrigin[0],
					this->LastPickPosition[1] - planeOrigin[1],
					this->LastPickPosition[2] - planeOrigin[2] };

	double x2D = vtkMath::Dot(ppo, v1);
	double y2D = vtkMath::Dot(ppo, v2);

	if (x2D > planeSize1) { x2D = planeSize1; }
	else if (x2D < 0.0) { x2D = 0.0; }
	if (y2D > planeSize2) { y2D = planeSize2; }
	else if (y2D < 0.0) { y2D = 0.0; }

	// Divide plane into three zones for different user interactions:
	// four corners -- spin around the plane's normal at its center
	// four edges   -- rotate around one of the plane's axes at its center
	// center area  -- push
	//
	double marginX = planeSize1 * this->MarginSizeX;
	double marginY = planeSize2 * this->MarginSizeY;

	double x0 = marginX;
	double y0 = marginY;
	double x1 = planeSize1 - marginX;
	double y1 = planeSize2 - marginY;

	if (x2D < x0)       // left margin
	{
		if (y2D < y0)        // bottom left corner
		{
			this->MarginSelectMode = 0;
		}
		else if (y2D > y1)   // top left corner
		{
			this->MarginSelectMode = 3;
		}
		else                 // left edge
		{
			this->MarginSelectMode = 4;
		}
	}
	else if (x2D > x1)   // right margin
	{
		if (y2D < y0)        // bottom right corner
		{
			this->MarginSelectMode = 1;
		}
		else if (y2D > y1)   // top right corner
		{
			this->MarginSelectMode = 2;
		}
		else                 // right edge
		{
			this->MarginSelectMode = 5;
		}
	}
	else                   // middle or on the very edge
	{
		if (y2D < y0)        // bottom edge
		{
			this->MarginSelectMode = 6;
		}
		else if (y2D > y1)   // top edge
		{
			this->MarginSelectMode = 7;
		}
		else                 // central area
		{
			this->MarginSelectMode = 8;
		}
	}

	if (this->Interactor->GetControlKey() ||
		(auto_modifier &&
		(*auto_modifier & VTKCustomResliceWidget::VTK_CONTROL_MODIFIER)))
	{
		this->State = VTKCustomResliceWidget::Moving;
	}
	else
	{
		if (this->MarginSelectMode >= 0 && this->MarginSelectMode < 4)
		{
			this->State = VTKCustomResliceWidget::Spinning;
			return;
		}
		else if (this->MarginSelectMode == 8)
		{
			this->State = VTKCustomResliceWidget::Pushing;
			return;
		}
		else
		{
			this->State = VTKCustomResliceWidget::Rotating;
		}
	}

	double *raPtr = nullptr;
	double *rvPtr = nullptr;
	double rvfac = 1.0;
	double rafac = 1.0;

	switch (this->MarginSelectMode)
	{
		// left bottom corner
	case 0: raPtr = v2; rvPtr = v1; rvfac = -1.0; rafac = -1.0; break;
		// right bottom corner
	case 1: raPtr = v2; rvPtr = v1;               rafac = -1.0; break;
		// right top corner
	case 2: raPtr = v2; rvPtr = v1;               break;
		// left top corner
	case 3: raPtr = v2; rvPtr = v1; rvfac = -1.0; break;
	case 4: raPtr = v2; rvPtr = v1; rvfac = -1.0; break; // left
	case 5: raPtr = v2; rvPtr = v1;               break; // right
	case 6: raPtr = v1; rvPtr = v2; rvfac = -1.0; break; // bottom
	case 7: raPtr = v1; rvPtr = v2;               break; // top
	default: raPtr = v1; rvPtr = v2; break;
	}

	for (int i = 0; i < 3; i++)
	{
		this->RotateAxis[i] = *raPtr++ * rafac;
		this->RadiusVector[i] = *rvPtr++ * rvfac;
	}
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::Spin(double *p1, double *p2)
{
	// Disable cursor snap
	//
	this->PlaneOrientation = 3;

	// Get the motion vector, in world coords
	//
	double v[3];
	v[0] = p2[0] - p1[0];
	v[1] = p2[1] - p1[1];
	v[2] = p2[2] - p1[2];

	// Plane center and normal before transform
	//
	double* wc = this->PlaneSource->GetCenter();
	double* wn = this->PlaneSource->GetNormal();

	// Radius vector from center to cursor position
	//
	double rv[3] = { p2[0] - wc[0], p2[1] - wc[1], p2[2] - wc[2] };

	// Distance between center and cursor location
	//
	double rs = vtkMath::Normalize(rv);

	// Spin direction
	//
	double wn_cross_rv[3];
	vtkMath::Cross(wn, rv, wn_cross_rv);

	// Spin angle
	//
	double dw = vtkMath::DegreesFromRadians(vtkMath::Dot(v, wn_cross_rv) / rs);

	this->Transform->Identity();
	this->Transform->Translate(wc[0], wc[1], wc[2]);
	this->Transform->RotateWXYZ(dw, wn);
	this->Transform->Translate(-wc[0], -wc[1], -wc[2]);

	double newpt[3];
	this->Transform->TransformPoint(this->PlaneSource->GetPoint1(), newpt);
	this->PlaneSource->SetPoint1(newpt);
	this->Transform->TransformPoint(this->PlaneSource->GetPoint2(), newpt);
	this->PlaneSource->SetPoint2(newpt);
	this->Transform->TransformPoint(this->PlaneSource->GetOrigin(), newpt);
	this->PlaneSource->SetOrigin(newpt);
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::Rotate(double *p1, double *p2, double *vpn)
{
	// Disable cursor snap
	//
	this->PlaneOrientation = 3;

	// Get the motion vector, in world coords
	//
	double v[3];
	v[0] = p2[0] - p1[0];
	v[1] = p2[1] - p1[1];
	v[2] = p2[2] - p1[2];

	// Plane center and normal
	//
	double* wc = this->PlaneSource->GetCenter();

	// Radius of the rotating circle of the picked point
	//
	double radius = fabs(this->RadiusVector[0] * (p2[0] - wc[0]) +
		this->RadiusVector[1] * (p2[1] - wc[1]) +
		this->RadiusVector[2] * (p2[2] - wc[2]));

	// Rotate direction ra_cross_rv
	//
	double rd[3];
	vtkMath::Cross(this->RotateAxis, this->RadiusVector, rd);

	// Direction cosin between rotating direction and view normal
	//
	double rd_dot_vpn = rd[0] * vpn[0] + rd[1] * vpn[1] + rd[2] * vpn[2];

	// 'push' plane edge when mouse moves away from plane center
	// 'pull' plane edge when mouse moves toward plane center
	//
	double dw = vtkMath::DegreesFromRadians(vtkMath::Dot(this->RadiusVector, v) / radius) * -rd_dot_vpn;

	this->Transform->Identity();
	this->Transform->Translate(wc[0], wc[1], wc[2]);
	this->Transform->RotateWXYZ(dw, this->RotateAxis);
	this->Transform->Translate(-wc[0], -wc[1], -wc[2]);

	double newpt[3];
	this->Transform->TransformPoint(this->PlaneSource->GetPoint1(), newpt);
	this->PlaneSource->SetPoint1(newpt);
	this->Transform->TransformPoint(this->PlaneSource->GetPoint2(), newpt);
	this->PlaneSource->SetPoint2(newpt);
	this->Transform->TransformPoint(this->PlaneSource->GetOrigin(), newpt);
	this->PlaneSource->SetOrigin(newpt);
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::GeneratePlaneOutline()
{
	vtkPoints* points = vtkPoints::New(VTK_DOUBLE);
	points->SetNumberOfPoints(4);
	int i;
	for (i = 0; i < 4; i++)
	{
		points->SetPoint(i, 0.0, 0.0, 0.0);
	}

	vtkCellArray *cells = vtkCellArray::New();
	cells->Allocate(cells->EstimateSize(4, 2));
	vtkIdType pts[2];
	pts[0] = 3; pts[1] = 2;       // top edge
	cells->InsertNextCell(2, pts);
	pts[0] = 0; pts[1] = 1;       // bottom edge
	cells->InsertNextCell(2, pts);
	pts[0] = 0; pts[1] = 3;       // left edge
	cells->InsertNextCell(2, pts);
	pts[0] = 1; pts[1] = 2;       // right edge
	cells->InsertNextCell(2, pts);

	this->PlaneOutlinePolyData->SetPoints(points);
	points->Delete();
	this->PlaneOutlinePolyData->SetLines(cells);
	cells->Delete();

	vtkPolyDataMapper* planeOutlineMapper = vtkPolyDataMapper::New();
	planeOutlineMapper->SetInputData(this->PlaneOutlinePolyData);
	planeOutlineMapper->SetResolveCoincidentTopologyToPolygonOffset();
	this->PlaneOutlineActor->SetMapper(planeOutlineMapper);
	this->PlaneOutlineActor->PickableOff();
	planeOutlineMapper->Delete();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::GenerateTexturePlane()
{
	this->SetResliceInterpolate(this->ResliceInterpolate);

	this->LookupTable = this->CreateDefaultLookupTable();

	this->ColorMap->SetLookupTable(this->LookupTable);
	this->ColorMap->SetOutputFormatToRGBA();
	this->ColorMap->PassAlphaToOutputOn();

	vtkPolyDataMapper* texturePlaneMapper = vtkPolyDataMapper::New();
	texturePlaneMapper->SetInputConnection(
		this->PlaneSource->GetOutputPort());

	this->Texture->SetQualityTo32Bit();
	this->Texture->SetColorMode(VTK_COLOR_MODE_DEFAULT);
	this->Texture->SetInterpolate(this->TextureInterpolate);
	this->Texture->RepeatOff();
	this->Texture->SetLookupTable(this->LookupTable);

	this->TexturePlaneActor->SetMapper(texturePlaneMapper);
	this->TexturePlaneActor->SetTexture(this->Texture);
	this->TexturePlaneActor->PickableOn();
	texturePlaneMapper->Delete();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::GenerateMargins()
{
	// Construct initial points
	vtkPoints* points = vtkPoints::New(VTK_DOUBLE);
	points->SetNumberOfPoints(8);
	int i;
	for (i = 0; i < 8; i++)
	{
		points->SetPoint(i, 0.0, 0.0, 0.0);
	}

	vtkCellArray *cells = vtkCellArray::New();
	cells->Allocate(cells->EstimateSize(4, 2));
	vtkIdType pts[2];
	pts[0] = 0; pts[1] = 1;       // top margin
	cells->InsertNextCell(2, pts);
	pts[0] = 2; pts[1] = 3;       // bottom margin
	cells->InsertNextCell(2, pts);
	pts[0] = 4; pts[1] = 5;       // left margin
	cells->InsertNextCell(2, pts);
	pts[0] = 6; pts[1] = 7;       // right margin
	cells->InsertNextCell(2, pts);

	this->MarginPolyData->SetPoints(points);
	points->Delete();
	this->MarginPolyData->SetLines(cells);
	cells->Delete();

	vtkPolyDataMapper* marginMapper = vtkPolyDataMapper::New();
	marginMapper->SetInputData(this->MarginPolyData);
	marginMapper->SetResolveCoincidentTopologyToPolygonOffset();
	this->MarginActor->SetMapper(marginMapper);
	this->MarginActor->PickableOff();
	this->MarginActor->VisibilityOff();
	marginMapper->Delete();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::GenerateCursor()
{
	// Construct initial points
	//
	vtkPoints* points = vtkPoints::New(VTK_DOUBLE);
	points->SetNumberOfPoints(4);
	int i;
	for (i = 0; i < 4; i++)
	{
		points->SetPoint(i, 0.0, 0.0, 0.0);
	}

	vtkCellArray *cells = vtkCellArray::New();
	cells->Allocate(cells->EstimateSize(2, 2));
	vtkIdType pts[2];
	pts[0] = 0; pts[1] = 1;       // horizontal segment
	cells->InsertNextCell(2, pts);
	pts[0] = 2; pts[1] = 3;       // vertical segment
	cells->InsertNextCell(2, pts);

	this->CursorPolyData->SetPoints(points);
	points->Delete();
	this->CursorPolyData->SetLines(cells);
	cells->Delete();

	vtkPolyDataMapper* cursorMapper = vtkPolyDataMapper::New();
	cursorMapper->SetInputData(this->CursorPolyData);
	cursorMapper->SetResolveCoincidentTopologyToPolygonOffset();
	this->CursorActor->SetMapper(cursorMapper);
	this->CursorActor->PickableOff();
	this->CursorActor->VisibilityOff();
	cursorMapper->Delete();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::GenerateText()
{
	snprintf(this->TextBuff, VTK_IMAGE_PLANE_WIDGET_MAX_TEXTBUFF, "NA");
	this->TextActor->SetInput(this->TextBuff);
	this->TextActor->SetTextScaleModeToNone();

	vtkTextProperty* textprop = this->TextActor->GetTextProperty();
	textprop->SetColor(1, 1, 1);
	textprop->SetFontFamilyToArial();
	textprop->SetFontSize(18);
	textprop->BoldOff();
	textprop->ItalicOff();
	textprop->ShadowOff();
	textprop->SetJustificationToLeft();
	textprop->SetVerticalJustificationToBottom();

	vtkCoordinate* coord = this->TextActor->GetPositionCoordinate();
	coord->SetCoordinateSystemToNormalizedViewport();
	coord->SetValue(.01, .01);

	this->TextActor->VisibilityOff();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::UpdateMargins()
{
	double v1[3];
	this->GetVector1(v1);
	double v2[3];
	this->GetVector2(v2);
	double o[3];
	this->PlaneSource->GetOrigin(o);
	double p1[3];
	this->PlaneSource->GetPoint1(p1);
	double p2[3];
	this->PlaneSource->GetPoint2(p2);

	double a[3];
	double b[3];
	double c[3];
	double d[3];

	double s = this->MarginSizeX;
	double t = this->MarginSizeY;

	int i;
	for (i = 0; i < 3; i++)
	{
		a[i] = o[i] + v2[i] * (1 - t);
		b[i] = p1[i] + v2[i] * (1 - t);
		c[i] = o[i] + v2[i] * t;
		d[i] = p1[i] + v2[i] * t;
	}

	vtkPoints* marginPts = this->MarginPolyData->GetPoints();

	marginPts->SetPoint(0, a);
	marginPts->SetPoint(1, b);
	marginPts->SetPoint(2, c);
	marginPts->SetPoint(3, d);

	for (i = 0; i < 3; i++)
	{
		a[i] = o[i] + v1[i] * s;
		b[i] = p2[i] + v1[i] * s;
		c[i] = o[i] + v1[i] * (1 - s);
		d[i] = p2[i] + v1[i] * (1 - s);
	}

	marginPts->SetPoint(4, a);
	marginPts->SetPoint(5, b);
	marginPts->SetPoint(6, c);
	marginPts->SetPoint(7, d);
	marginPts->GetData()->Modified();

	this->MarginPolyData->Modified();
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::Translate(double *p1, double *p2)
{
	// Get the motion vector
	//
	double v[3];
	v[0] = p2[0] - p1[0];
	v[1] = p2[1] - p1[1];
	v[2] = p2[2] - p1[2];

	double *o = this->PlaneSource->GetOrigin();
	double *pt1 = this->PlaneSource->GetPoint1();
	double *pt2 = this->PlaneSource->GetPoint2();
	double origin[3], point1[3], point2[3];

	double vdrv = this->RadiusVector[0] * v[0] + \
		this->RadiusVector[1] * v[1] + \
		this->RadiusVector[2] * v[2];
	double vdra = this->RotateAxis[0] * v[0] + \
		this->RotateAxis[1] * v[1] + \
		this->RotateAxis[2] * v[2];

	int i;
	if (this->MarginSelectMode == 8)       // everybody comes along
	{
		for (i = 0; i < 3; i++)
		{
			origin[i] = o[i] + v[i];
			point1[i] = pt1[i] + v[i];
			point2[i] = pt2[i] + v[i];
		}
		this->PlaneSource->SetOrigin(origin);
		this->PlaneSource->SetPoint1(point1);
		this->PlaneSource->SetPoint2(point2);
	}
	else if (this->MarginSelectMode == 4) // left edge
	{
		for (i = 0; i < 3; i++)
		{
			origin[i] = o[i] + vdrv * this->RadiusVector[i];
			point2[i] = pt2[i] + vdrv * this->RadiusVector[i];
		}
		this->PlaneSource->SetOrigin(origin);
		this->PlaneSource->SetPoint2(point2);
	}
	else if (this->MarginSelectMode == 5) // right edge
	{
		for (i = 0; i < 3; i++)
		{
			point1[i] = pt1[i] + vdrv * this->RadiusVector[i];
		}
		this->PlaneSource->SetPoint1(point1);
	}
	else if (this->MarginSelectMode == 6) // bottom edge
	{
		for (i = 0; i < 3; i++)
		{
			origin[i] = o[i] + vdrv * this->RadiusVector[i];
			point1[i] = pt1[i] + vdrv * this->RadiusVector[i];
		}
		this->PlaneSource->SetOrigin(origin);
		this->PlaneSource->SetPoint1(point1);
	}
	else if (this->MarginSelectMode == 7) // top edge
	{
		for (i = 0; i < 3; i++)
		{
			point2[i] = pt2[i] + vdrv * this->RadiusVector[i];
		}
		this->PlaneSource->SetPoint2(point2);
	}
	else if (this->MarginSelectMode == 3) // top left corner
	{
		for (i = 0; i < 3; i++)
		{
			origin[i] = o[i] + vdrv * this->RadiusVector[i];
			point2[i] = pt2[i] + vdrv * this->RadiusVector[i] +
				vdra * this->RotateAxis[i];
		}
		this->PlaneSource->SetOrigin(origin);
		this->PlaneSource->SetPoint2(point2);
	}
	else if (this->MarginSelectMode == 0) // bottom left corner
	{
		for (i = 0; i < 3; i++)
		{
			origin[i] = o[i] + vdrv * this->RadiusVector[i] +
				vdra * this->RotateAxis[i];
			point1[i] = pt1[i] + vdra * this->RotateAxis[i];
			point2[i] = pt2[i] + vdrv * this->RadiusVector[i];
		}
		this->PlaneSource->SetOrigin(origin);
		this->PlaneSource->SetPoint1(point1);
		this->PlaneSource->SetPoint2(point2);
	}
	else if (this->MarginSelectMode == 2) // top right corner
	{
		for (i = 0; i < 3; i++)
		{
			point1[i] = pt1[i] + vdrv * this->RadiusVector[i];
			point2[i] = pt2[i] + vdra * this->RotateAxis[i];
		}
		this->PlaneSource->SetPoint1(point1);
		this->PlaneSource->SetPoint2(point2);
	}
	else                                   // bottom right corner
	{
		for (i = 0; i < 3; i++)
		{
			origin[i] = o[i] + vdra * this->RotateAxis[i];
			point1[i] = pt1[i] + vdrv * this->RadiusVector[i] +
				vdra * this->RotateAxis[i];
		}
		this->PlaneSource->SetPoint1(point1);
		this->PlaneSource->SetOrigin(origin);
	}
}

//----------------------------------------------------------------------------
void VTKCustomResliceWidget::Scale(double *p1, double *p2,
	int vtkNotUsed(X), int Y)
{
	// Get the motion vector
	//
	double v[3];
	v[0] = p2[0] - p1[0];
	v[1] = p2[1] - p1[1];
	v[2] = p2[2] - p1[2];

	double *o = this->PlaneSource->GetOrigin();
	double *pt1 = this->PlaneSource->GetPoint1();
	double *pt2 = this->PlaneSource->GetPoint2();
	double* center = this->PlaneSource->GetCenter();

	// Compute the scale factor
	//
	double sf = vtkMath::Norm(v) /
		sqrt(vtkMath::Distance2BetweenPoints(pt1, pt2));
	if (Y > this->Interactor->GetLastEventPosition()[1])
	{
		sf = 1.0 + sf;
	}
	else
	{
		sf = 1.0 - sf;
	}

	// Move the corner points
	//
	double origin[3], point1[3], point2[3];

	for (int i = 0; i < 3; i++)
	{
		origin[i] = sf * (o[i] - center[i]) + center[i];
		point1[i] = sf * (pt1[i] - center[i]) + center[i];
		point2[i] = sf * (pt2[i] - center[i]) + center[i];
	}

	this->PlaneSource->SetOrigin(origin);
	this->PlaneSource->SetPoint1(point1);
	this->PlaneSource->SetPoint2(point2);
}


