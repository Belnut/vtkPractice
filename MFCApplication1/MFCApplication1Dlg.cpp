﻿

// MFCApplication1Dlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"
#include "vtkCTIFFImageReader.h"

#include <vtkCaptionActor2D.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkAnnotatedCubeActor.h>
#include <vtktiff/tiffio.h>
#include <vtkDirectory.h>

#include <vtkImageImport.h>

#include <vector>
#include <string>

#include <opencv2/opencv.hpp>

#include <vtkImageSliceMapper.h>
#include <vtkImageSlice.h>
#include <vtkInteractorStyleImage.h>
#include <vtkImageActor.h>


#include <vtkLookupTable.h>
#include <vtkMatrix4x4.h>
#include <vtkImageMapToColors.h>
#include <vtkImageProperty.h>

#include <vtkScalarBarActor.h>


#include <vtkAxesActor.h>
//#include <vtkOrientationMarkerWidget.h>


#include <vtkPlaneWidget.h>
#include <vtkImagePlaneWidget.h>

#include <vtkActor2D.h>
#include <vtkProperty2D.h>

#include <vtkImageMapper.h>

#include <vtkTexturedActor2D.h>

#include <vtkSphereSource.h>
#include <vtkRegularPolygonSource.h>

#include <vtkBalloonRepresentation.h>
#include <vtkBalloonWidget.h>

//#include "pxikVTKUI.h"
//#include "pxikVTKUIWidgetRepresentation.h"
#include "pxikVTKUIPanelWidget.h"
#include "pxikVTKUIPanelRepresentation.h"
#include "pxikVTKUITextureButtonRepresentation.h"
#include "pxikVTKUIButtonWidget.h"


#include <vtkNew.h>
#include <vtkChartXY.h>

#include <vtkNamedColors.h>
#include <vtkActor.h>
#include <vtkContextScene.h>
#include <vtkContextActor.h>
#include <vtkCubeSource.h>
#include <vtkFloatArray.h>
#include <vtkPlotPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTable.h>
#include <vtkCamera.h>
#include <vtkContextInteractorStyle.h>

#include <vtkSliderWidget.h>
#include <vtkSliderRepresentation2D.h>

#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>


class vtkSliderCallback : public vtkCommand
{
public:
	static vtkSliderCallback *New()
	{
		return new vtkSliderCallback;
	}
	virtual void Execute(vtkObject *caller, unsigned long, void*)
	{
		vtkSliderWidget *sliderWidget =
			reinterpret_cast<vtkSliderWidget*>(caller);
		this->represent->GetFrameProperty()->SetOpacity(static_cast<vtkSliderRepresentation *>(sliderWidget->GetRepresentation())->GetValue());
		//this->represent->BuildRepresentation();
	}
	vtkSliderCallback() :represent(0) {}
	pxikVTKUIPanelRepresentation *represent;
};



#ifdef _DEBUG
//#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCApplication1Dlg 대화 상자



CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CMFCApplication1Dlg::~CMFCApplication1Dlg()
{
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCApplication1Dlg::OnBnClickedButton1)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_BOTTOM, &CMFCApplication1Dlg::OnNMCustomdrawSliderBottom)
END_MESSAGE_MAP()


// CMFCApplication1Dlg 메시지 처리기

BOOL CMFCApplication1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	//seting VtkWindow - Window32
	if (this->GetDlgItem(IDC_STATIC_RENDERWINDOW))
	{
		
		//this->InitVtkWindow(this->GetDlgItem(IDC_STATIC_RENDERWINDOW)->GetSafeHwnd());
		this->initialize(this->GetDlgItem(IDC_STATIC_RENDERWINDOW)->GetSafeHwnd());
		this->ResizeVtkWindow();
		
		

	
	}


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMFCApplication1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCApplication1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//////////////////////////////////////////////////////////
/*
*					내부 Event
*/
//////////////////////////////////////////////////////////


//VTK 화면을 Window창 안에 집어넣음
//다른 FRAME 에 넣을거라면 호출에서 원하는 FRAME의 hWnd를 넣어줄 것
void CMFCApplication1Dlg::InitVtkWindow(void* hWnd)
{
	if (m_vtkWindow == NULL)
	{
		m_vtkWindow = vtkSmartPointer<vtkRenderWindow>::New();

		vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
		interactor->SetInteractorStyle(vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New());

		//vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
		//renderer->SetBackground(.0, .0, .0);

		m_vtkRenderer = vtkSmartPointer<vtkRenderer>::New();
		m_vtkRenderer->SetBackground(.0, .0, .0);


		//원래는 책에서도 말하듯 이게 맞는거 같긴한데
		//아마 이렇게 하면 이 메서드를 빠저나가는 순간 문제가 생기지 않을까?
		//실제로는 문제없음
		//interactor->SetRenderWindow(m_vtkWindow);

		m_vtkWindow->SetParentId(hWnd);
		m_vtkWindow->AddRenderer(m_vtkRenderer);
		//m_vtkWindow->SetInteractor(interactor);
		interactor->SetRenderWindow(m_vtkWindow);
		
		interactor->Initialize();
		m_vtkWindow->Render();
	}
}


//설정한 FRAME에 맞게 사이즈가 조절됨
//this->Onsize()가 호출, 프래임을 바꿀시 hWnd 변경할 것
void CMFCApplication1Dlg::ResizeVtkWindow()
{
	CRect rc;
	GetDlgItem(IDC_STATIC_RENDERWINDOW)->GetClientRect(rc);

	//m_vtkWindow->SetSize(rc.Width(), rc.Height());
	this->onSize(rc.Width(), rc.Height());
}

//////////////////////////////////////////////////////////
/*
*					MFC Event
*/
//////////////////////////////////////////////////////////

//vtk Button Event
void CMFCApplication1Dlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	clock_t start, end;
	double result;

	start = clock();

	//balloonTest();

	this->volumePropInitialize();
	//this->VtkDCMTest();
	this->setROIVolumeDataOpenCV(100, 100, 700, 700, 0, 100);
	//this->setVtkOutLine();

	//m_vtkRenderer->SetBackground(.0, .0, .0);
	

	m_vtkWindow->Render();
	//setOrientAxesActor();
	//setSliceImage();
	//setSliceImageWidgetCustom();
	//setCustomOrientAxesActor();
	//ddButton();

	//this->addCustomButton();
	this->pxikCircleButton();

	//m_vtkInteractor->SetDesiredUpdateRate(1000);

	//m_vtkInteractor->Start();

	//set2DPlane();
	//set2DPlaneimage();
	//m_vtkInteractor->Start();
	
	end = clock();
	result = (double)(end - start);
	CEditView *view = (CEditView *)this->GetDlgItem(IDC_EDIT1);
	
	CString str;
	str.Format(_T("%f"), result);

	view->SetWindowTextW(str);

	//balloonTest();
	addPanel();
	addHistogram();
	//addChart();
	setSilderWiget();

	
}

//Dlg Frame 크기 변경 시 발생하는 이벤트
void CMFCApplication1Dlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	this->ResizeVtkWindow();
}

//VSScroll 이벤트
void CMFCApplication1Dlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl * Slider[2];
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (pScrollBar)
	{
		Slider[TOP] = (CSliderCtrl*)this->GetDlgItem(IDC_SLIDER_TOP);
		Slider[BOTTOM] = (CSliderCtrl*)this->GetDlgItem(IDC_SLIDER_BOTTOM);

		int _nPosTop = Slider[TOP]->GetPos();
		int _nPosBtm = Slider[BOTTOM]->GetPos();

		
		if (pScrollBar == (CScrollBar*)Slider[TOP])
		{
			if (_nPosTop >= _nPosBtm)
			{
				Slider[TOP]->SetPos(_nPosBtm - 1);
				_nPosTop = _nPosBtm - 1;
			}
			this->ChangePlaneOrigin(100 - _nPosTop, TOP);
		}
		
		if (pScrollBar == (CScrollBar*)Slider[BOTTOM])
		{	
			if (_nPosTop >= _nPosBtm)
			{
				Slider[BOTTOM]->SetPos(_nPosTop + 1);
				_nPosBtm = _nPosTop +1;
			}
			this->ChangePlaneOrigin(100 - _nPosBtm, BOTTOM);
		}
	}
	else
	{
		// CScrollView를 상속받은 뷰의 경우 프래임의 스크롤롤 동작시 pScrollBar이 NULL된다.
	}

	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}


///////////////////////////////////////////////////////////
/*
*					vtk TEST 관련 함수
*/
///////////////////////////////////////////////////////////

#pragma region TEST_FUNC



//Cone, Cylinder TEST 
void CMFCApplication1Dlg::VtkConeTest()
{
	////////////////////////////////////////////////
	//콘 생성 및 Mapper actor 연결
	vtkSmartPointer<vtkConeSource> coneSource = vtkSmartPointer<vtkConeSource>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(0, coneSource->GetOutputPort(0));
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer <vtkActor>::New();
	actor->SetMapper(mapper);

	////////////////////////////////////////////////
	//Visualize
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(actor);
	renderer->SetBackground(.1, .2, .3);
	renderer->ResetCamera();

	m_vtkWindow->AddRenderer(renderer);
	m_vtkWindow->Render();

	////////////////////////////////////////////////
	//Angle Widget
	//m_angleWidget = vtkSmartPointer<vtkAngleWidget>::New();
	//m_angleWidget->SetInteractor(m_vtkWindow->GetInteractor());
	//m_angleWidget->On();


	////////////////////////////////////////////////
	//Orientation<arkerWidget
	this->SettingOrientationWidget();


	////////////////////////////////////////////////
	//Cylinder Testing
	//vtkSmartPointer<vtkCylinderSource> cylinder = vtkSmartPointer<vtkCylinderSource>::New();
	//cylinder->SetResolution(20);
	//mapper->AddInputConnection(1, cylinder->GetOutputPort(1));
	//mapper->RemoveInputConnection(0, 0);
	//mapper->SetInputConnection(0, cylinder->GetOutputPort());
	//actor->GetProperty()->SetColor(1, 0, 0);
	//actor->RotateX(30);
	//actor->RotateY(-45);
	//m_vtkWindow->Render();

	////////////////////////////////////////////////
	//Hex

	double height;
	double radius;
	double* center;
	height = coneSource->GetHeight();
	radius = coneSource->GetRadius();
	center = coneSource->GetCenter();

}

//Cube Test
void CMFCApplication1Dlg::VtkCubeTest()
{
	if (m_vtkTestCubeSource == NULL)
		m_vtkTestCubeSource = vtkCubeSource::New();
	else
		return ;

	m_vtkTestCubeSource->SetXLength(10);
	m_vtkTestCubeSource->SetYLength(10);
	m_vtkTestCubeSource->SetZLength(10);
	m_vtkTestCubeSource->SetCenter(0, 0, 0);
	m_vtkTestCubeSource->Update();

	m_vtkTestPolyDataMapper = vtkPolyDataMapper::New();
	m_vtkTestPolyDataMapper->SetInputData(m_vtkTestCubeSource->GetOutput());


	m_vtkTestActor = vtkActor::New();
	m_vtkTestActor->SetMapper(m_vtkTestPolyDataMapper);

	vtkRenderer* renderer = vtkRenderer::New();
	renderer->AddActor(m_vtkTestActor);
	renderer->SetBackground(.1, .1, .1);
	renderer->ResetCamera();

	m_vtkWindow->AddRenderer(renderer);
	m_vtkWindow->Render();
}


void CMFCApplication1Dlg::SmartVtkCubeTest()
{
	//TODO : smartPointer 실험
	vtkSmartPointer< vtkCubeSource> vtkTestCubeSource = vtkSmartPointer<vtkCubeSource>::New();
	vtkSmartPointer< vtkPolyDataMapper> vtkTestPolyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer< vtkActor> vtkTestActor = vtkSmartPointer<vtkActor>::New();
	vtkSmartPointer< vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

	vtkTestCubeSource->SetXLength(10);
	vtkTestCubeSource->SetYLength(10);
	vtkTestCubeSource->SetZLength(10);
	vtkTestCubeSource->SetCenter(0, 0, 0);
	vtkTestCubeSource->Update();

	//if (m_vtkTestCubeSource == NULL)
	//	m_vtkTestCubeSource = vtkCubeSource::New();
	//else
	//	return;

	//m_vtkTestPolyDataMapper = vtkPolyDataMapper::New();
	vtkTestPolyDataMapper->SetInputData(vtkTestCubeSource->GetOutput());


	//m_vtkTestActor = vtkActor::New();
	vtkTestActor->SetMapper(vtkTestPolyDataMapper);

	//vtkRenderer* renderer = vtkRenderer::New();
	renderer->AddActor(vtkTestActor);
	renderer->SetBackground(.1, .1, .1);
	renderer->ResetCamera();
	
	
	m_vtkWindow->AddRenderer(renderer);
	m_vtkWindow->Render();

	this->SettingOrientationWidget();
}

//dcm Test
void CMFCApplication1Dlg::VtkDCMTest()
{
	//////////////////////////////////
	// DCM READ
	//m_DCMReader = vtkSmartPointer<vtkDICOMImageReader>::New();
	//m_DCMReader->SetDirectoryName("D:/PCB_CT");
	//m_DCMReader->Update();

	//////////////////////////////////
	// DCM Info
	//m_DCMReader->GetOutput()->GetBounds(m_dcmBounds);
	//m_DCMReader->GetDataOrigin(m_dcmOrigin);
	//m_DCMReader->GetDataExtent(m_dcmExtent);

	//////////////////////////////////
	// add Plane
	//m_zTopClipPlane = vtkSmartPointer<vtkPlane>::New();
	//m_zTopClipPlane->SetOrigin(m_dcmExtent[1], m_dcmExtent[3], m_dcmExtent[5]);
	//m_zTopClipPlane->SetNormal(0, 0, -1);

	//m_zBtmClipPlane = vtkSmartPointer<vtkPlane>::New();
	//m_zBtmClipPlane->SetOrigin(m_dcmExtent[1], m_dcmExtent[3], m_dcmExtent[4]);
	//m_zBtmClipPlane->SetNormal(0, 0, 1);


	////////////////////////////////////////////////////////////////
	//ClipFilter
	//vtkSmartPointer<vtkClipDataSet> clipDataset = vtkSmartPointer<vtkClipDataSet>::New();
	//clipDataset->SetInputConnection(dcmReader->GetOutputPort());
	//clipDataset->SetClipFunction(m_panel);

	////////////////////////////////////////////////////////////////
	//Volume Rendering
	vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity
		= vtkSmartPointer<vtkPiecewiseFunction>::New();

	////////////////////////////////////////////////////////////////
	//PCB
	//compositeOpacity->AddPoint(15277, 0);
	//compositeOpacity->AddPoint(21823, 1);
	
	//compositeOpacity->AddPoint(30000, 0);
	//compositeOpacity->AddPoint(40000, 1);
	
	compositeOpacity->RemoveAllPoints();
	
	compositeOpacity->AddPoint(40000, 0);
	compositeOpacity->AddPoint(70000, 1);
	compositeOpacity->ClampingOff();

	////////////////////////////////////////////////////////////////
	//color setting
	vtkSmartPointer<vtkColorTransferFunction> color
		= vtkSmartPointer<vtkColorTransferFunction>::New();

	color->RemoveAllPoints();


	////////////////////////////////////////////////////////////////
	//Mapper
	m_smartV_Mapper	= vtkSmartPointer<vtkGPUVolumeRayCastMapper>::New();


	////////////////////////////////////////////////////////////////
	//slider



	////////////////////////////////////////////////////////////////
	//TIFF READER


	////////////////////////////////////////////////////////////////
	//TIFF READER OPENCV



	
	////////////////////////////////////////////////////////////////
	//Setting clip plane;


	////////////////////////////////////////////////////////////////
	//Connect with Property - color and Opacity
	vtkSmartPointer<vtkVolumeProperty> volumeProperty
		= vtkSmartPointer<vtkVolumeProperty>::New();

	volumeProperty->ShadeOn();
	volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

	volumeProperty->SetScalarOpacity(compositeOpacity);
	volumeProperty->SetColor(color);

	////////////////////////////////////////////////////////////////
	//Conncet with Volume  - Mapper and Property



	////////////////////////////////////////////////////////////////
	//Connect With Renderer - Volume
	

	////////////////////////////////////////////////////////////////
	//TIFF READER OPENCV vtk Image Import
	vtkSmartPointer<vtkImageData> data = TIFFReaderOpenCV();


	vtkSmartPointer<vtkVolume> volume
		= vtkSmartPointer<vtkVolume> ::New();
	

	////Prop 대체실험
	m_smartV_Mapper->SetInputData(data);
	m_smartV_Mapper->SetBlendModeToComposite();
	m_smartV_Mapper->Update();
	
	
	volume->SetMapper(m_smartV_Mapper);
	volume->SetProperty(volumeProperty);


	//에러테스트

	m_vtkRenderer->AddVolume(volume);
	m_vtkRenderer->SetBackground(.8, .8, .8);
	m_vtkRenderer->ResetCamera();

	////////////////////////////////////////////////////////////////
	//Connect With vtkWindow - Renderer 
	
	//에러테스트
	//m_vtkWindow->AddRenderer(renderer);
	
	
	//this->SetCilpBox(m_dcmBounds, renderer);

	//m_vtkWindow->GetInteractor()->Initialize();
	m_vtkWindow->Render();

	//renderer->Render();
	///////////////////////////////////////////////////////////////
	// another setting
	//SetClipSlide(m_dcmBounds[5], m_dcmExtent[5]);

	///////////////////////////////////////////////////////////////
	//Setting widget



}

//OrientationWidget Setting
void CMFCApplication1Dlg::SettingOrientationWidget()
{
	vtkSmartPointer<vtkAnnotatedCubeActor> cube = vtkSmartPointer<vtkAnnotatedCubeActor>::New();

	m_orientationWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
	m_orientationWidget->SetOrientationMarker(cube);
	m_orientationWidget->SetInteractor(m_vtkWindow->GetInteractor());
	m_orientationWidget->SetViewport(0, 0, 0.2, 0.2);
	m_orientationWidget->SetEnabled(TRUE);
	m_orientationWidget->On();
}

//SetingPicking
void CMFCApplication1Dlg::SetPicking()
{
	// TODO: 여기에 구현 코드 추가.
	vtkSmartPointer<vtkCellPicker> partPicker = vtkSmartPointer<vtkCellPicker>::New();

//	partPicker->AddObserver();

}

//clip용 박스 초기화
void CMFCApplication1Dlg::SetCilpBox(double * m_dcmBounds, vtkSmartPointer<vtkRenderer> renderer)
{
	// TODO: 여기에 구현 코드 추가.
	vtkSmartPointer<vtkCubeSource> clipCube 
		= vtkSmartPointer<vtkCubeSource>::New();

	clipCube->SetCenter(m_dcmOrigin);
	clipCube->SetBounds(m_dcmBounds);
	clipCube->Update();

	///////////////////////////////////////////////////
	//PolyData Outline 생성
	vtkSmartPointer<vtkPolyDataNormals> outlineNomal =
		vtkSmartPointer<vtkPolyDataNormals>::New();
	outlineNomal->SetInputConnection(clipCube->GetOutputPort());
	
	vtkSmartPointer<vtkOutlineFilter> outlineFilter =
		vtkSmartPointer<vtkOutlineFilter>::New();
	outlineFilter->SetInputConnection(outlineNomal->GetOutputPort());
	
	vtkSmartPointer<vtkPolyDataMapper> outlineMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	outlineMapper->SetInputConnection(outlineFilter->GetOutputPort());
		
	vtkSmartPointer<vtkActor> outlineActor =
	vtkSmartPointer<vtkActor>::New();
	
	outlineActor->SetMapper(outlineMapper);
	outlineActor->GetProperty()->SetColor(0, 0, 0);
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////
	//Cube Actor
	vtkSmartPointer<vtkPolyDataMapper> cubeMapper
		= vtkSmartPointer<vtkPolyDataMapper>::New();
	cubeMapper->SetInputConnection(clipCube->GetOutputPort());

	vtkSmartPointer<vtkActor> cubeActor =
		vtkSmartPointer<vtkActor>::New();
	cubeActor->SetMapper(cubeMapper);
	cubeActor->GetProperty()->SetOpacity(0.3);
	cubeActor->GetProperty()->SetColor(1, 1, 0);
	cubeActor->GetProperty()->BackfaceCullingOff();
	

	renderer->AddActor(cubeActor);
	renderer->AddActor(outlineActor);
}

//ClipSlider 초기화
void CMFCApplication1Dlg::SetClipSlide(double z_Bound, int Range)
{
	// TODO: 여기에 구현 코드 추가.
	CSliderCtrl* topSlider;
	topSlider = (CSliderCtrl*) this->GetDlgItem(IDC_SLIDER_TOP);
	topSlider->SetRange(0, Range, TRUE);
	topSlider->SetPos(0);

	CSliderCtrl* btmSlider;
	btmSlider = (CSliderCtrl*)this->GetDlgItem(IDC_SLIDER_BOTTOM);
	btmSlider->SetRange(0, Range, TRUE);
	btmSlider->SetPos(100);
}

//Clip용 Plane 위치 변경
void CMFCApplication1Dlg::ChangePlaneOrigin(int Pos, PlaneLoc planeLoc)
{
	// TODO: 여기에 구현 코드 추가.

	double zBound = m_dcmBounds[5] * ((double)Pos / m_dcmExtent[5]);

	//for DeBug
	if (Pos == 100)
		;

	switch (planeLoc)
	{
	case TOP:
		m_zTopClipPlane->SetOrigin(m_dcmOrigin[0], m_dcmOrigin[1], zBound);
		break;
	case BOTTOM:
		m_zBtmClipPlane->SetOrigin(m_dcmOrigin[0], m_dcmOrigin[1], zBound);
		break;
	default:
		return;
		break;
	}

	m_smartV_Mapper->Update();
	m_vtkWindow->Render();
}

// slider 함수
void CMFCApplication1Dlg::DCMSilder(vtkSmartPointer<vtkImageData> slider, vtkSmartPointer<vtkImageData> origin )
{
	
	vtkSmartPointer<vtkInformation> originInfo = origin->GetInformation();

	//크기 이미지 먼저 계산할것
	//Extent x * y * z
	//
	slider->SetOrigin(0,0,0);
	//잘라낼 크기 설정
	slider->SetDimensions(100, 100, 100);
	//slider 이미지 버퍼 설정
	slider->AllocateScalars(VTK_UNSIGNED_SHORT, 1);

	void* imageOrign = origin->GetScalarPointer();
	void* copyBuffer = slider->GetScalarPointer();

	unsigned char* originImgData = (unsigned char*)imageOrign;
	unsigned char* copyImgData = (unsigned char*)copyBuffer;

	int originType = origin->GetScalarType();
	unsigned long originSize = origin->GetActualMemorySize();
	int copysize = slider->GetActualMemorySize();
	//image 위치에 따른 OffSet 설정
	int imgOffSetX = (1024 - 100);
	int imgOffSetY = ((1024 - 100) / 2) * 2048;
	
	// Z
	for (int z = 0; z < 100; z++)
	{
		unsigned char* oneImageOrigin = originImgData + (1024 * 2) * 1024 * z;
		oneImageOrigin += imgOffSetX + imgOffSetY;
		for (int y = 0; y < 100; y++)
		{
			memcpy(copyImgData, oneImageOrigin, 200);
			copyImgData += 200;
			oneImageOrigin += 2048;
		}		
	}

	//imgData->GetScalarPointer();
	//imgData->SetDimensions;
	//imgData->SetExtent;
	//imgData->SetOrigin;
	//imgData->SetInformation;
	//imgData->AllocateScalars
}

// TIFF Reader
vtkSmartPointer<vtkImageData> CMFCApplication1Dlg::TIFFReader()
{
	//vtkSmartPointer<vtkCTIFFImageReader> CTIFFReader = vtkSmartPointer<vtkCTIFFImageReader>::New();
	//CTIFFReader->SetDirectoryName("D:/PCB_CT_TIFF");
	////CTIFFReader->SetFileName("D:/PCB_CT_TIFF/whj_0451.tiff");
	//
	//CTIFFReader->Update();

	//vtkSmartPointer<vtkStringArray> array = vtkSmartPointer<vtkStringArray>::New();
	////array->

	//vtkSmartPointer<vtkTIFFReader> TIFFReader = vtkSmartPointer<vtkTIFFReader>::New();
	//TIFFReader->SetFileName("D:/PCB_CT_TIFF/whj_0451.tiff");
	//TIFFReader->Update();
	//
	//vtkSmartPointer<vtkTIFFReader> TIFFWriter = vtkSmartPointer<vtkTIFFReader>::New();
	////TIFFReader->SetFileNames();

	vtkSmartPointer<vtkImageData> buffer = vtkSmartPointer<vtkImageData>::New();
	vtkSmartPointer<vtkImageData> slider = vtkSmartPointer<vtkImageData>::New();
	vtkSmartPointer<vtkTIFFReader> reader = vtkSmartPointer<vtkTIFFReader>::New();

	

	char DirectoryName[] = "D:/PCB_CT_TIFF";
	std::vector<std::string> * TIFFFileNames = new std::vector<std::string>;
	if (DirectoryName)
	{
		vtkDirectory* dir = vtkDirectory::New();
		int opened = dir->Open(DirectoryName);
		if (!opened)
		{
			dir->Delete();
			return NULL;
		}
		vtkIdType numFiles = dir->GetNumberOfFiles();

		TIFFFileNames->clear();
		//this->AppHelper->Clear();

		for (vtkIdType i = 0; i < numFiles; i++)
		{
			if (strcmp(dir->GetFile(i), ".") == 0 ||
				strcmp(dir->GetFile(i), "..") == 0)
			{
				continue;
			}

			std::string fileString = DirectoryName;
			fileString += "/";
			fileString += dir->GetFile(i);

			//int val = this->CanReadFile(fileString.c_str());

			//open 확인시 TURE = 3 FALSE = 0 리턴;
			//if (val == 3)
			//{
			//	vtkDebugMacro(<< "Adding " << fileString.c_str() << " to DICOMFileNames.");
			TIFFFileNames->push_back(fileString);
			//}
			//else
			//{
			//	vtkDebugMacro(<< fileString.c_str() << " - DICOMParser CanReadFile returned : " << val);
			//}

		}
		std::vector<std::string>::iterator iter;
		std::vector<std::pair<float, std::string> > sortedFiles;
	}



	
	buffer->SetOrigin(0, 0, 0);
	//잘라낼 크기 설정
	buffer->SetDimensions(1024, 1024, 100);
	//slider 이미지 버퍼 설정
	buffer->AllocateScalars(VTK_UNSIGNED_SHORT, 1);



	slider->SetOrigin(0, 0, 0);
	//잘라낼 크기 설정
	slider->SetDimensions(1024, 1024, 100);
	//slider 이미지 버퍼 설정
	slider->AllocateScalars(VTK_UNSIGNED_SHORT, 1);

	std::vector<std::string>::iterator fiter;

	int count = 0;
	vtkIdType numFiles = static_cast<int>( TIFFFileNames->size() );

	USHORT* MainStackData = (USHORT*)buffer->GetScalarPointer();

	//USHORT* MainStackData = (USHORT*)MainStack;

	//디버그 return 확인용
	int originType = buffer->GetScalarType();
	unsigned long originSize = buffer->GetActualMemorySize();
	//int copysize = slider->GetActualMemorySize();


	//image 위치에 따른 OffSet 설정
	int imgOffSetX = 1024;
	int imgOffSetY = 1024;

	int i = 0;

	for (fiter = TIFFFileNames->begin();
		fiter != TIFFFileNames->end();
		++fiter)
	{
		reader->SetFileName(fiter->c_str());
		reader->Update();
		
		vtkImageData * img = reader->GetOutput();
		//vtkSmartPointer<vtkImageShiftScale> changeFormat = vtkSmartPointer< vtkImageShiftScale>::New();
		//changeFormat->SetOutputScalarTypeToShort();
		//changeFormat->SetInputData(reader->GetOutput());
		//changeFormat->Update();

		USHORT* src = (USHORT*)reader->GetOutput()->GetScalarPointer();
		memcpy(MainStackData, src, imgOffSetX * imgOffSetY* sizeof(USHORT));
		MainStackData += (imgOffSetX * imgOffSetY);
		i++;
	}

	
	return buffer;

}


// TIFF Reader
vtkSmartPointer<vtkImageData> CMFCApplication1Dlg::TIFFReaderOpenCV()
{


	vtkSmartPointer<vtkImageData> buffer = vtkSmartPointer<vtkImageData>::New();
	vtkSmartPointer<vtkImageData> slider = vtkSmartPointer<vtkImageData>::New();
	//vtkSmartPointer<vtkTIFFReader> reader = vtkSmartPointer<vtkTIFFReader>::New();



	char DirectoryName[] = "D:/PCB_CT_TIFF";
	std::vector<std::string> * TIFFFileNames = new std::vector<std::string>;
	if (DirectoryName)
	{
		vtkDirectory* dir = vtkDirectory::New();
		int opened = dir->Open(DirectoryName);
		if (!opened)
		{
			dir->Delete();
			return NULL;
		}
		vtkIdType numFiles = dir->GetNumberOfFiles();

		TIFFFileNames->clear();
		//this->AppHelper->Clear();

		for (vtkIdType i = 0; i < numFiles; i++)
		{
			if (strcmp(dir->GetFile(i), ".") == 0 ||
				strcmp(dir->GetFile(i), "..") == 0)
			{
				continue;
			}

			std::string fileString = DirectoryName;
			fileString += "/";
			fileString += dir->GetFile(i);

			//int val = this->CanReadFile(fileString.c_str());

			//open 확인시 TURE = 3 FALSE = 0 리턴;
			//if (val == 3)
			//{
			//	vtkDebugMacro(<< "Adding " << fileString.c_str() << " to DICOMFileNames.");
			TIFFFileNames->push_back(fileString);
			//}
			//else
			//{
			//	vtkDebugMacro(<< fileString.c_str() << " - DICOMParser CanReadFile returned : " << val);
			//}

		}
		std::vector<std::string>::iterator iter;
		std::vector<std::pair<float, std::string> > sortedFiles;
	}




	buffer->SetOrigin(0, 0, 0);
	//잘라낼 크기 설정
	buffer->SetExtent(0, 1023, 0, 1023, 0, 99);
	//slider 이미지 버퍼 설정
	buffer->AllocateScalars(VTK_UNSIGNED_SHORT, 1);



	slider->SetOrigin(0, 0, 0);
	//잘라낼 크기 설정
	slider->SetExtent(0, 1023, 0, 1023, 0, 99);
	//slider 이미지 버퍼 설정
	slider->AllocateScalars(VTK_UNSIGNED_SHORT, 1);

	std::vector<std::string>::iterator fiter;

	int count = 0;
	vtkIdType numFiles = static_cast<int>(TIFFFileNames->size());

	void* MainStack = buffer->GetScalarPointer();

	USHORT* MainStackData = (USHORT*)MainStack;

	//디버그 return 확인용
	int originType = buffer->GetScalarType();
	unsigned long originSize = buffer->GetActualMemorySize();
	//int copysize = slider->GetActualMemorySize();


	//image 위치에 따른 OffSet 설정
	int imgOffSetX = 1024;
	int imgOffSetY = 1024;

	int i = 0;

	for (fiter = TIFFFileNames->begin();
		fiter != TIFFFileNames->end();
		++fiter)
	{
		cv::Mat cvImg = cv::imread(fiter->c_str(), cv::IMREAD_UNCHANGED);

		
		//vtkSmartPointer<vtkImageShiftScale> changeFormat = vtkSmartPointer< vtkImageShiftScale>::New();
		//changeFormat->SetOutputScalarTypeToShort();
		//changeFormat->SetInputData(reader->GetOutput());
		//changeFormat->Update();

		USHORT* src = (USHORT*) cvImg.data;
		memcpy(MainStackData, src, imgOffSetX * imgOffSetY * sizeof(USHORT));
		MainStackData += (imgOffSetX * imgOffSetY);
		i++;
	}


	return buffer;

}


vtkSmartPointer<vtkImageImport> CMFCApplication1Dlg::TIFFReaderOpenCVImport()
{
	vtkSmartPointer<vtkImageData> buffer = vtkSmartPointer<vtkImageData>::New();
	vtkSmartPointer<vtkImageData> slider = vtkSmartPointer<vtkImageData>::New();
	vtkSmartPointer<vtkTIFFReader> reader = vtkSmartPointer<vtkTIFFReader>::New();



	char DirectoryName[] = "D:/PCB_CT_TIFF";
	std::vector<std::string> * TIFFFileNames = new std::vector<std::string>;
	if (DirectoryName)
	{
		vtkDirectory* dir = vtkDirectory::New();
		int opened = dir->Open(DirectoryName);
		if (!opened)
		{
			dir->Delete();
			return NULL;
		}
		vtkIdType numFiles = dir->GetNumberOfFiles();

		TIFFFileNames->clear();
		//this->AppHelper->Clear();

		for (vtkIdType i = 0; i < numFiles; i++)
		{
			if (strcmp(dir->GetFile(i), ".") == 0 ||
				strcmp(dir->GetFile(i), "..") == 0)
			{
				continue;
			}

			std::string fileString = DirectoryName;
			fileString += "/";
			fileString += dir->GetFile(i);

			//int val = this->CanReadFile(fileString.c_str());

			//open 확인시 TURE = 3 FALSE = 0 리턴;
			//if (val == 3)
			//{
			//	vtkDebugMacro(<< "Adding " << fileString.c_str() << " to DICOMFileNames.");
			TIFFFileNames->push_back(fileString);
			//}
			//else
			//{
			//	vtkDebugMacro(<< fileString.c_str() << " - DICOMParser CanReadFile returned : " << val);
			//}

		}
		std::vector<std::string>::iterator iter;
		std::vector<std::pair<float, std::string> > sortedFiles;
	}



	std::vector<std::string>::iterator fiter;

	int count = 0;
	vtkIdType numFiles = static_cast<int>(TIFFFileNames->size());

	void* MainStack = buffer->GetScalarPointer();

	USHORT* MainStackData = (USHORT*)malloc(1024*1024*100 * 2);

	//디버그 return 확인용
	int originType = buffer->GetScalarType();
	unsigned long originSize = buffer->GetActualMemorySize();
	//int copysize = slider->GetActualMemorySize();


	//image 위치에 따른 OffSet 설정
	int imgOffSetX = 1024;
	int imgOffSetY = 1024;

	int i = 0;



	USHORT* picker = MainStackData;

	for (fiter = TIFFFileNames->begin();
		fiter != TIFFFileNames->end();
		++fiter)
	{
		cv::Mat cvImg = cv::imread(fiter->c_str(), cv::IMREAD_UNCHANGED);


		//vtkSmartPointer<vtkImageShiftScale> changeFormat = vtkSmartPointer< vtkImageShiftScale>::New();
		//changeFormat->SetOutputScalarTypeToShort();
		//changeFormat->SetInputData(reader->GetOutput());
		//changeFormat->Update();

		USHORT* src = (USHORT*)cvImg.data;
		memcpy(picker, src, imgOffSetX * imgOffSetY * sizeof(USHORT));
		picker += (imgOffSetX * imgOffSetY);
		i++;
	}


	int dims[] = { 100 ,1024, 1024 };

	cv::Mat fullImg = cv::Mat(3, dims, CV_16UC1, MainStackData);

	std::vector<cv::Range> ranges = { cv::Range(0, 99),
									  cv::Range(0, 1023),
									  cv::Range(0, 1023) };
	cv::Mat matPart = fullImg(ranges);

	vtkSmartPointer<vtkImageImport> m_vtkVolumeImageImport;
	m_vtkVolumeImageImport = vtkSmartPointer<vtkImageImport>::New();
	m_vtkVolumeImageImport->SetDataSpacing(1, 1, 1);
	m_vtkVolumeImageImport->SetDataOrigin(0, 0, 0);
	m_vtkVolumeImageImport->SetWholeExtent(0, 1023, 0, 1023, 0, 99);
	m_vtkVolumeImageImport->SetDataExtentToWholeExtent();
	m_vtkVolumeImageImport->SetDataScalarType(VTK_UNSIGNED_SHORT);
	//m_vtkVolumeImageImport->Update();

	int type = m_vtkVolumeImageImport->GetDataScalarType();


	m_vtkVolumeImageImport->SetNumberOfScalarComponents(1);
	m_vtkVolumeImageImport->SetImportVoidPointer((void*) matPart.data);
	m_vtkVolumeImageImport->Update();


	//잘린 이미지 확인
	USHORT * testPointer = (USHORT *)m_vtkVolumeImageImport->GetOutput()->GetScalarPointer();
	for (int i = 0; i < 100; i++)
	{
		cv::Mat img = cv::Mat(1024, 1024, CV_16UC1, testPointer);
		(USHORT *)testPointer += (1024 * 1024);
	}


	return m_vtkVolumeImageImport;


}
//임시 컨버트용
void CMFCApplication1Dlg :: convertDICOMToTIFF()
{
	vtkSmartPointer<vtkTIFFWriter> writer = vtkSmartPointer<vtkTIFFWriter>::New();
	
	char DirectoryName[] = "D:/PCB_CT";
	std::vector<std::string> * TIFFFileNames = new std::vector<std::string>;
	if (DirectoryName)
	{
		vtkDirectory* dir = vtkDirectory::New();
		int opened = dir->Open(DirectoryName);
		if (!opened)
		{
			dir->Delete();
			return ;
		}
		vtkIdType numFiles = dir->GetNumberOfFiles();

		TIFFFileNames->clear();
		//this->AppHelper->Clear();

		for (vtkIdType i = 0; i < numFiles; i++)
		{
			if (strcmp(dir->GetFile(i), ".") == 0 ||
				strcmp(dir->GetFile(i), "..") == 0)
			{
				continue;
			}

			std::string fileString = DirectoryName;
			fileString += "/";
			fileString += dir->GetFile(i);

			//int val = this->CanReadFile(fileString.c_str());

			//open 확인시 TURE = 3 FALSE = 0 리턴;
			//if (val == 3)
			//{
			//	vtkDebugMacro(<< "Adding " << fileString.c_str() << " to DICOMFileNames.");
			TIFFFileNames->push_back(fileString);
			//}
			//else
			//{
			//	vtkDebugMacro(<< fileString.c_str() << " - DICOMParser CanReadFile returned : " << val);
			//}

		}
		std::vector<std::string>::iterator iter;
		std::vector<std::pair<float, std::string> > sortedFiles;
	}

	std::vector<std::string>::iterator fiter;
	vtkSmartPointer <vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
	for (fiter = TIFFFileNames->begin();
		fiter != TIFFFileNames->end();
		++fiter)
	{
		reader->SetFileName(fiter->c_str());
		reader->Update();


		vtkSmartPointer<vtkImageShiftScale> changeFormat = vtkSmartPointer< vtkImageShiftScale>::New();
		changeFormat->SetOutputScalarTypeToUnsignedShort();
		changeFormat->SetInputData(reader->GetOutput());
		changeFormat->Update();

		fiter->replace(19, 22, "tiff");

		writer->SetFileName(fiter->c_str());
		writer->SetInputData(changeFormat->GetOutput());
		writer->Write();
	}
}


void CMFCApplication1Dlg::OnNMCustomdrawSliderBottom(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void CMFCApplication1Dlg::newCreate()
{
}
#pragma endregion

void CMFCApplication1Dlg::initialize(HWND parent)
{
	// TODO : smartPointer 제거 (예정, 삭제할지는 모르겠음)
	m_vtkWindow = vtkSmartPointer< vtkRenderWindow>::New();
	m_vtkRenderer = vtkSmartPointer< vtkRenderer>::New();
	m_vtkInteractor = vtkSmartPointer< vtkRenderWindowInteractor>::New();

	m_vtkInteractor->SetInteractorStyle(vtkSmartPointer<pxikVolumeTrackBallCamera>::New());

	m_vtkRenderer->SetBackground(0.1, 0.2, 0.4);

	m_vtkWindow->SetParentId(parent);
	m_vtkWindow->AddRenderer(m_vtkRenderer);
	m_vtkWindow->SetInteractor(m_vtkInteractor);

	m_vtkInteractor->Initialize();
	m_vtkWindow->Render();
}

void CMFCApplication1Dlg::onSize(int width, int height)
{
	m_vtkWindow->SetSize(width, height);
}

void CMFCApplication1Dlg::volumePropInitialize()
{
	m_vtkVolumeMapper = vtkSmartPointer<vtkGPUVolumeRayCastMapper>::New();
	m_vtkVolumePiecewiseFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
	m_vtkVolumeColorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
	m_vtkVolumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();


#pragma endregion

	m_vtkVolumePiecewiseFunction->RemoveAllPoints();
	//m_vtkVolumePiecewiseFunction->ClampingOff();
	m_vtkVolumePiecewiseFunction->AddPoint(40000, 0);
	m_vtkVolumePiecewiseFunction->AddPoint(70000, 1);
	m_vtkVolumePiecewiseFunction->ClampingOff();

	m_vtkVolumeColorTransferFunction->RemoveAllPoints();
	m_vtkVolumeColorTransferFunction->GetAboveRangeColor();

	//TODO: 나중에 사용자가 추가하는 형식으로 구현하자
	//this->addROIVolumeColorTransferFunctionPointer(-15000, 1, 0, 0);
	//this->addROIVolumeColorTransferFunctionPointer(-18000, 1, 0, 0);
	//this->addROIVolumeColorTransferFunctionPointer(-22000, 0, 1, 0);

	m_vtkVolumeProperty->ShadeOn();
	m_vtkVolumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

	m_vtkVolumeProperty->SetScalarOpacity(m_vtkVolumePiecewiseFunction);
	m_vtkVolumeProperty->SetColor(m_vtkVolumeColorTransferFunction);
}

void CMFCApplication1Dlg::setROIVolumeDataOpenCV(int topLeftX, int topLeftY, int btmRightX, int btmRightY, int sliceTopZ, int sliceBtmZ)
{
	//			TODO:
			//타입이 일치하여 이렇게 집어넣었습니다.  차후 보기 쉽게 수정해주세요 (=> m_fullImgType +1, +2) 

	if (m_vtkVolumeImageData != NULL)
	{
		m_vtkVolumeImageData->Delete();
	}
	m_vtkVolumeImageData = vtkSmartPointer<vtkImageData>::New();

	//TODO : setting 수정
	m_vtkVolumeImageData->SetOrigin(0, 0, 0);
	m_vtkVolumeImageData->SetDimensions(1024, 1024, 100);
	m_vtkVolumeImageData->AllocateScalars(VTK_UNSIGNED_SHORT, 1);

	char DirectoryName[] = "D:/PCB_CT_TIFF";
	std::vector<std::string> * TIFFFileNames = new std::vector<std::string>;
	if (DirectoryName)
	{
		vtkDirectory* dir = vtkDirectory::New();
		int opened = dir->Open(DirectoryName);
		if (!opened)
		{
			dir->Delete();
			return ;
		}
		vtkIdType numFiles = dir->GetNumberOfFiles();

		TIFFFileNames->clear();
		//this->AppHelper->Clear();

		for (vtkIdType i = 0; i < numFiles; i++)
		{
			if (strcmp(dir->GetFile(i), ".") == 0 ||
				strcmp(dir->GetFile(i), "..") == 0)
			{
				continue;
			}

			std::string fileString = DirectoryName;
			fileString += "/";
			fileString += dir->GetFile(i);

			//int val = this->CanReadFile(fileString.c_str());

			//open 확인시 TURE = 3 FALSE = 0 리턴;
			//if (val == 3)
			//{
			//	vtkDebugMacro(<< "Adding " << fileString.c_str() << " to DICOMFileNames.");
			TIFFFileNames->push_back(fileString);
			//}
			//else
			//{
			//	vtkDebugMacro(<< fileString.c_str() << " - DICOMParser CanReadFile returned : " << val);
			//}

		}
		std::vector<std::string>::iterator iter;
		std::vector<std::pair<float, std::string> > sortedFiles;
	}

	//int count = 0;
	//vtkIdType numFiles = static_cast<int>(TIFFFileNames->size());

	void* MainStack = m_vtkVolumeImageData->GetScalarPointer();

	USHORT* MainStackData = (USHORT*)MainStack;

	//디버그 return 확인용
	//int originType = m_vtkVolumeImageData->GetScalarType();
	//unsigned long originSize = m_vtkVolumeImageData->GetActualMemorySize();
	//int copysize = slider->GetActualMemorySize();


	//image 위치에 따른 OffSet 설정
	int imgOffSetX = 1024;
	int imgOffSetY = 1024;

	int i = 0;

	std::vector<std::string>::iterator fiter;

	for (fiter = TIFFFileNames->begin();
		fiter != TIFFFileNames->end();
		++fiter)
	{
		cv::Mat cvImg = cv::imread(fiter->c_str(), cv::IMREAD_UNCHANGED);

		USHORT* src = (USHORT*)cvImg.data;
		memcpy(MainStackData, src, imgOffSetX * imgOffSetY * sizeof(USHORT));
		MainStackData += (imgOffSetX * imgOffSetY);
		i++;
	}



	////원본
	//if (this->isBufferEmpty())
	//	return XIK_ERROR_NO_BUFFER;

	if (topLeftX < 0 || topLeftY < 0 ||
		btmRightX <= 0 || btmRightY <= 0 ||
		sliceTopZ < 0 || sliceBtmZ <= 0)
		return;

	int width = btmRightX - topLeftX;
	int height = btmRightY - topLeftY;
	int depth = sliceBtmZ - sliceTopZ;

	if (width <= 0 || height <= 0 || depth <= 0)
		return;

	//int dims[3] = { m_fullImgX, m_fullImgY, m_fullImgSliceCount };


	////TODO:
	////타입이 일치하여 이렇게 집어넣었습니다.  차후 보기 쉽게 수정해주세요 (=> m_fullImgType +1, +2) 

	////if (m_vtkVolumeImageData != NULL)
	////{
	////	m_vtkVolumeImageData->Delete();
	////}
	////m_vtkVolumeImageData = vtkSmartPointer<vtkImageData>::New();

	//////TODO : setting 수정
	////m_vtkVolumeImageData->SetOrigin(0, 0, 0);
	////m_vtkVolumeImageData->SetExtent(0, width - 1, 0, height - 1, 0, depth - 1);
	////m_vtkVolumeImageData->AllocateScalars(VTK_UNSIGNED_SHORT, 1);

	//int x = m_vtkVolumeImageData->GetScalarType();
	//char * dst = (char *)m_vtkVolumeImageData->GetScalarPointer();
	//char * picker = (char*)m_imageBuffer;

	//

	//cv::Range cuttingPlaneRange[] = { cv::Range(topLeftY, btmRightY),  cv::Range(topLeftX, btmRightX) };
	//int pitch = (width * height) * sizeof(unsigned short);

	//picker += (sliceTopZ * m_fullImgX * m_fullImgY * sizeof(unsigned short));
	//for (int i = sliceTopZ; i < sliceBtmZ; i++)
	//{
	//	cv::Mat slice = cv::Mat(m_fullImgY, m_fullImgX, CV_16UC1, picker);
	//	cv::Mat cutImg = slice(cuttingPlaneRange).clone();
	//	memcpy(dst, cutImg.data, pitch);
	//	picker += (m_fullImgX * m_fullImgY * sizeof(unsigned short));
	//	


	//	cv::Mat saveData = cv::Mat(m_fullImgY, m_fullImgX, CV_16UC1, dst);
	//	
	//	std::string saveName = "cuttingfile";
	//	saveName += std::string(i + "" );
	//	saveName += ".tiff";
	//	cv::imwrite(saveName, saveData);

	//	dst += pitch;
	//}
	//x = m_vtkVolumeImageData->GetScalarType();

	//#define XIK_DEBUG_SHOWSTACK_IMG
#ifdef XIK_DEBUG_SHOWSTACK_IMG
	//잘린 이미지 확인
	{
		USHORT * testPointer = (USHORT *)m_vtkImageData->GetScalarPointer();
		for (int i = 0; i < depth; i++)
		{
			cv::Mat img = cv::Mat(height, width, CV_16UC1, testPointer);
			(USHORT *)testPointer += (width * height);
		}
	}
#endif

	m_vtkVolumeMapper->SetInputData(m_vtkVolumeImageData);
	m_vtkVolumeMapper->SetBlendModeToComposite();

	m_vtkVolumeMapper->Update();
	//m_vtkVolumeMapper->SetRequestedRenderModeToGPU();
	m_vtkVolume = vtkSmartPointer< vtkVolume>::New();

	m_vtkVolume->SetMapper(m_vtkVolumeMapper);
	m_vtkVolume->SetProperty(m_vtkVolumeProperty);

	//m_vtkVolume->Update();

	m_vtkRenderer->AddVolume(m_vtkVolume);
	m_vtkRenderer->SetBackground(.8, .8, .8);
	m_vtkRenderer->ResetCamera();

	//this->setHistogramData();

	return;
}
//---------------------------------------------------------------------------------

void CMFCApplication1Dlg::setVtkOutLine()
{
	m_vtkOutlineFilter = vtkSmartPointer<vtkOutlineFilter>::New();
	m_vtkOutlineFilter->SetInputData(m_vtkVolumeImageData);

	m_vtkOutlineFilter->Update();

	m_vtkPolyDataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_vtkPolyDataMapper->SetInputData(m_vtkOutlineFilter->GetOutput());


	m_vtkActor = vtkSmartPointer<vtkActor>::New();
	m_vtkActor->SetMapper(m_vtkPolyDataMapper);

	m_vtkActor->GetProperty()->SetColor(1, 0, 0);

	m_vtkRenderer->AddActor(m_vtkActor);
}

void CMFCApplication1Dlg::setSliceImage()
{
	int extent[6];
	double spacing[3];
	double origin[3];
	m_vtkVolumeImageData->GetExtent(extent);
	m_vtkVolumeImageData->GetSpacing(spacing);
	m_vtkVolumeImageData->GetOrigin(origin);

	double center[3];
	center[0] = origin[0] + spacing[0] * 0.5 * (extent[0] + extent[1]);
	center[1] = origin[1] + spacing[1] * 0.5 * (extent[2] + extent[3]);
	center[2] = origin[2] + spacing[2] * 0.5 * (extent[4] + extent[5]);

	// Matrices for axial, coronal, sagittal, oblique view orientations
	static double axialElements[16] = {
			 1, 0, 0, 0,
			 0, 1, 0, 0,
			 0, 0, 1, 0,
			 0, 0, 0, 1 };

	static double coronalElements[16] = {
			 1, 0, 0, 0,
			 0, 0, 1, 0,
			 0,-1, 0, 0,
			 0, 0, 0, 1 };

	static double sagittalElements[16] = {
			 0, 0,-1, 0,
			 1, 0, 0, 0,
			 0,-1, 0, 0,
			 0, 0, 0, 1 };

	static double obliqueElements[16] = {
			 1, 0, 0, 0,
			 0, 0.866025, -0.5, 0,
			 0, 0.5, 0.866025, 0,
			 0, 0, 0, 1 };

	// Set the slice orientation
	vtkSmartPointer<vtkMatrix4x4> resliceAxes = vtkSmartPointer<vtkMatrix4x4>::New();
	resliceAxes->DeepCopy(sagittalElements);
	// Set the point through which to slice
	resliceAxes->SetElement(0, 3, 0);
	resliceAxes->SetElement(1, 3, 0);
	resliceAxes->SetElement(2, 3, 70);



	// Extract a slice in the desired orientation
	vtkSmartPointer<vtkImageReslice> reslice = vtkSmartPointer<vtkImageReslice>::New();
	reslice->SetInputData(m_vtkVolumeImageData);
	reslice->SetOutputDimensionality(2);
	reslice->SetResliceAxes(resliceAxes);
	reslice->SetInterpolationModeToLinear();
	reslice->Update();

	// Create a greyscale lookup table
	table = vtkSmartPointer<vtkLookupTable>::New();
	double *Range;
	Range = table->GetRange();
	Range[0];
	Range[1];


	Range = table->GetValueRange();
	Range[0];
	Range[1];


	Range = table->GetTableRange();
	Range[0];
	Range[1];

	Range = table->GetSaturationRange();
	Range[0];
	Range[1];

	//Range = table->SetAboveRangeColor();
	table->GetHueRange();

	Range = table->GetAlphaRange();
	Range[0];
	Range[1];
	//table->SetAlphaRange(0.3, 0.3);

	table->SetRange(0, 65535); // image intensity range
	//table->SetAlpha(0.0);
	//table->GetRange();
	table->SetNumberOfTableValues(65536);

	double red = 0, green = 0, blue = 0;
	for (int i = 0; i < table->GetNumberOfTableValues(); i++)
	{
		if (i < 20000)
			red += 1.0 / 20000;

		else if (i < 40000)
			green += 1.0 / 20000;

		else if (i < 60000)
			blue += 1.0 / 20000;

		table->SetTableValue(i, red, green, blue, 1);
	}


	//table->SetValueRange(0.0, 1.0); // from black to white
	//table->GetValueRange();

	//table->SetTableRange();
	//table->GetTableRange();

	//table->SetSaturationRange();
	//table->GetSaturationRange();
	//
	//table->SetAboveRangeColor();
	//table->GetAboveRangeColor();

	//table->SetHueRange();
	//table->GetHueRange();
	//
	//table->SetAlphaRange();
	//table->GetAlphaRange();
	//table->SetSaturationRange(0.0, 0.0); // no color saturation
	//table->SetRampToLinear();
	table->Build();


	//// Map the image through the lookup table
	vtkSmartPointer<vtkImageMapToColors> color = vtkSmartPointer<vtkImageMapToColors>::New();
	color->SetLookupTable(table);
	color->SetInputData(reslice->GetOutput());
	color->Update();

	//// Display the image
	//vtkSmartPointer<vtkImageActor> actor = vtkSmartPointer<vtkImageActor>::New();
	////actor->SetInputData(m_vtkVolumeImageData);
	//actor->SetInputData(color->GetOutput());
	//actor->SetOpacity(0.7);
	//
	////actor->GetProperty()->SetOpacity(0.2);
	//double volumeOrigin[3];
	////actor->SetDisplayExtent(0, 1023, 0, 1023, 0,70);
	////actor->SetOpacity(0.3);
	//actor->Modified();
	//actor->SetPosition(0,0,70);
	//actor->Update();
	//m_vtkRenderer->AddActor(actor);


	vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(color->GetOutput());

	double position[3] = { 0, 0, 70 };
	vtkSmartPointer<vtkActor> nomal_actor = vtkSmartPointer<vtkActor>::New();
	nomal_actor->SetMapper(mapper);
	nomal_actor->GetProperty()->SetOpacity(0.7);
	nomal_actor->SetPosition(position);
	m_vtkRenderer->AddActor(nomal_actor);



	vtkSmartPointer<vtkOutlineFilter> sliceOutlineFilter = vtkSmartPointer<vtkOutlineFilter>::New();
	sliceOutlineFilter->SetInputData(color->GetOutput());
	sliceOutlineFilter->Update();
	vtkSmartPointer<vtkPolyDataMapper> sliceOutlineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	sliceOutlineMapper->SetInputData(sliceOutlineFilter->GetOutput());
	vtkSmartPointer<vtkActor> vtksliceOutlineActor = vtkSmartPointer<vtkActor>::New();
	vtksliceOutlineActor->SetMapper(sliceOutlineMapper);
	vtksliceOutlineActor->GetProperty()->SetColor(1, 0, 0);
	vtksliceOutlineActor->SetPosition(position);
	m_vtkRenderer->AddActor(vtksliceOutlineActor);



	vtkSmartPointer<vtkScalarBarActor> scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
	scalarBar->SetLookupTable(table);
	scalarBar->SetTitle("value");
	scalarBar->SetNumberOfLabels(10);
	scalarBar->SetLabelFormat("%10.2f");
	scalarBar->SetHeight(.2);
	scalarBar->SetWidth(.2);
	//actor->SetOpacity(0.3);


	m_vtkRenderer->AddActor2D(scalarBar);
}

void CMFCApplication1Dlg::setSliceImage2()
{


	vtkSmartPointer<vtkImageResliceMapper> im = vtkSmartPointer<vtkImageResliceMapper>::New();
	im->SetInputData(m_vtkVolumeImageData);
	im->SliceFacesCameraOn();
	im->SliceAtFocalPointOn();
	im->BorderOff();


	vtkSmartPointer<vtkImageProperty> ip = vtkSmartPointer<vtkImageProperty>::New();
	ip->SetColorWindow(2000);
	ip->SetColorLevel(1000);
	ip->SetAmbient(0.0);
	ip->SetDiffuse(1.0);
	ip->SetOpacity(.3);
	ip->SetInterpolationTypeToLinear();
	vtkSmartPointer<vtkImageSlice> ia = vtkSmartPointer<vtkImageSlice>::New();
	
	ia->SetMapper(im);
	ia->SetProperty(ip);
	
	m_vtkRenderer->AddViewProp(ia);
}

void CMFCApplication1Dlg::setOrientAxesActor()
{
	vtkSmartPointer<vtkAxesActor> axesActor = vtkSmartPointer<vtkAxesActor>::New();
	//< vtkOrientationMarkerWidget> orientationMarkerWidget = vtkSmartPointer< vtkOrientationMarkerWidget>::New();

	
	m_orientationWidget = vtkSmartPointer< vtkOrientationMarkerWidget>::New();
	m_orientationWidget->SetOrientationMarker(axesActor);

	vtkSmartPointer< vtkRenderWindowInteractor> inter = m_vtkWindow->GetInteractor();
	

	m_orientationWidget->SetInteractor( inter );
	m_orientationWidget->SetViewport(0,0, 0.1,0.1);
	m_orientationWidget->SetEnabled(1);
	m_orientationWidget->InteractiveOn();
	
}

void CMFCApplication1Dlg::setCustomOrientAxesActor()
{

	vtkSmartPointer<vtkAxesActor> axesActor = vtkSmartPointer<vtkAxesActor>::New();
	//< vtkOrientationMarkerWidget> orientationMarkerWidget = vtkSmartPointer< vtkOrientationMarkerWidget>::New();

	
	m_pxikOrientationMarker = vtkSmartPointer<pxikVTKVolumeOrientMarker>::New();
	m_pxikOrientationMarker->SetOrientationMarker(axesActor);

	vtkSmartPointer< vtkRenderWindowInteractor> inter = m_vtkWindow->GetInteractor();


	m_pxikOrientationMarker->SetInteractor(inter);
	m_pxikOrientationMarker->SetViewport(0, 0, 0.1, 0.1);
	m_pxikOrientationMarker->SetEnabled(1);
	m_pxikOrientationMarker->InteractiveOn();
	m_pxikOrientationMarker->On();

}

void CMFCApplication1Dlg::setSliceImageWidgetPreset()
{
	m_vtkimagePlaneWidget = vtkSmartPointer<vtkImagePlaneWidget>::New();

	
	m_vtkimagePlaneWidget->TextureInterpolateOff();

	m_vtkimagePlaneWidget->SetInputData(m_vtkVolumeImageData);

	m_vtkimagePlaneWidget->SetInteractor(m_vtkWindow->GetInteractor());
	m_vtkimagePlaneWidget->SetOrigin(400, 0, 0);
	m_vtkimagePlaneWidget->SetPlaneOrientationToXAxes();
	m_vtkimagePlaneWidget->SetSlicePosition(400);

	m_vtkimagePlaneWidget->UpdatePlacement();
	m_vtkimagePlaneWidget->DisplayTextOn();
	m_vtkimagePlaneWidget->On();

	
	vtkSmartPointer<vtkImageData> reslice = m_vtkimagePlaneWidget->GetResliceOutput();
	int extent[6];
	reslice->GetExtent(extent);

	cv::Mat x = cv::Mat(extent[1], extent[3], CV_16UC1, reslice->GetScalarPointer());

	int a = x.rows;
}

void CMFCApplication1Dlg::setSliceImageWidgetCustom()
{
	m_pxikVolumeSliceWidget = vtkSmartPointer<pxikVolumeSliceWidget>::New();


	m_pxikVolumeSliceWidget->TextureInterpolateOff();

	m_pxikVolumeSliceWidget->SetInputData(m_vtkVolumeImageData);

	m_pxikVolumeSliceWidget->SetInteractor(m_vtkWindow->GetInteractor());
	m_pxikVolumeSliceWidget->SetOrigin(0, 0, 50);
	m_pxikVolumeSliceWidget->SetPlaneOrientationToZAxes();
	m_pxikVolumeSliceWidget->SetSlicePosition(70);

	m_pxikVolumeSliceWidget->SetLookupTable(table);
	//m_pxikVolumeSliceWidget->UpdatePlacement();
	m_pxikVolumeSliceWidget->DisplayTextOn();
	m_pxikVolumeSliceWidget->On();

	m_vtkWindow->DoubleBufferOn();
}
//ColorTransferFunction -> lookuptable
void MakeLUTFromCTF(size_t const & tableSize, vtkLookupTable *lut)
{
	vtkSmartPointer<vtkColorTransferFunction> ctf =
		vtkSmartPointer<vtkColorTransferFunction>::New();
	ctf->SetColorSpaceToDiverging();
	// Green to tan.
	ctf->AddRGBPoint(0.0, 0.085, 0.532, 0.201);
	ctf->AddRGBPoint(0.5, 0.865, 0.865, 0.865);
	ctf->AddRGBPoint(1.0, 0.677, 0.492, 0.093);

	lut->SetNumberOfTableValues(tableSize);
	lut->Build();

	for (size_t i = 0; i < tableSize; ++i)
	{
		double *rgb;
		rgb = ctf->GetColor(static_cast<double>(i) / tableSize);
		lut->SetTableValue(i, rgb);
	}



}


void CMFCApplication1Dlg::addButton()
{

	vtkSmartPointer<vtkImageData> image1 =
		vtkSmartPointer<vtkImageData>::New();
	vtkSmartPointer<vtkImageData> image2 =
		vtkSmartPointer<vtkImageData>::New();
	
	cv::Mat resliceOnImage	 = cv::imread("C:\\Users\\User1\\Downloads\\ResliceOn.png",  cv::IMREAD_GRAYSCALE);
	cv::Mat resliceOffImage  = cv::imread("C:\\Users\\User1\\Downloads\\ResliceOff.png", cv::IMREAD_GRAYSCALE);

	CreateImage(image1, resliceOnImage.data, 50, 50);
	CreateImage(image2, resliceOffImage.data, 50, 50);



	vtkSmartPointer<vtkTexturedButtonRepresentation2D> buttonRepresentation =
		vtkSmartPointer<vtkTexturedButtonRepresentation2D>::New();
	buttonRepresentation->SetNumberOfStates(2);
	buttonRepresentation->SetButtonTexture(0, image1);
	buttonRepresentation->SetButtonTexture(1, image2);


	vtkSmartPointer<vtkButtonWidget> buttonWidget =
		vtkSmartPointer<vtkButtonWidget>::New();
	buttonWidget->SetInteractor(m_vtkInteractor);
	buttonWidget->SetRepresentation(buttonRepresentation);


	// Place the widget. Must be done after a render so that the
	// viewport is defined.
	// Here the widget placement is in normalized display coordinates
	vtkSmartPointer<vtkCoordinate> upperRight =
		vtkSmartPointer<vtkCoordinate>::New();
	upperRight->SetCoordinateSystemToNormalizedDisplay();
	upperRight->SetValue(1.0, 1.0);

	double bds[6];
	double sz = 50.0;
	bds[0] = upperRight->GetComputedDisplayValue(m_vtkRenderer)[0] - sz -10;
	bds[1] = bds[0] + sz;
	bds[2] = upperRight->GetComputedDisplayValue(m_vtkRenderer)[1] - sz - 10;
	bds[3] = bds[2] + sz;
	bds[4] = bds[5] = 0.0;

	// Scale to 1, default is .5
	buttonRepresentation->SetPlaceFactor(0);
	buttonRepresentation->PlaceWidget(bds);

	buttonWidget->On();

	m_vtkInteractor->Start();

}

void CMFCApplication1Dlg::addCustomButton()
{
	TestClass* testclass = new TestClass();

	vtkSmartPointer<vtkImageData> image1 =
		vtkSmartPointer<vtkImageData>::New();
	vtkSmartPointer<vtkImageData> image2 =
		vtkSmartPointer<vtkImageData>::New();

	unsigned char banana[3] = { 227, 207, 87 };
	unsigned char tomato[3] = { 255, 99, 71 };

	cv::Mat resliceOnImage = cv::imread("C:\\Users\\User1\\Downloads\\ResliceOn.png", cv::IMREAD_GRAYSCALE);
	cv::Mat resliceOffImage = cv::imread("C:\\Users\\User1\\Downloads\\ResliceOff.png", cv::IMREAD_GRAYSCALE);

	CreateImage(image1, resliceOnImage.data, 50, 50);
	CreateImage(image2, resliceOffImage.data, 50, 50);

	vtkSmartPointer<vtkTexturedButtonRepresentation2D> buttonRepresentation =
		vtkSmartPointer<vtkTexturedButtonRepresentation2D>::New();
	buttonRepresentation->SetNumberOfStates(2);
	buttonRepresentation->SetButtonTexture(0, image1);
	buttonRepresentation->SetButtonTexture(1, image2);


	m_buttonWidget = vtkSmartPointer<pxikVTKButtonWidget>::New();
	m_buttonWidget->SetInteractor(m_vtkInteractor);
	m_buttonWidget->SetRepresentation(buttonRepresentation);
	//buttonWidget->setEventFunction(testclass, &TestClass::helloWorld );

	// Place the widget. Must be done after a render so that the
	// viewport is defined.
	// Here the widget placement is in normalized display coordinates
	vtkSmartPointer<vtkCoordinate> upperRight =
		vtkSmartPointer<vtkCoordinate>::New();
	upperRight->SetCoordinateSystemToNormalizedDisplay();
	upperRight->SetValue(1.0, 1.0);

	double bds[6];
	double sz = 100.0;
	bds[0] = upperRight->GetComputedDisplayValue(m_vtkRenderer)[0] - sz - 10;
	bds[1] = bds[0] + sz;
	bds[2] = upperRight->GetComputedDisplayValue(m_vtkRenderer)[1] - sz*2 - 10;
	bds[3] = bds[2] + sz*2;
	bds[4] = bds[5] = 0.0;

	// Scale to 1, default is .5
	buttonRepresentation->SetPlaceFactor(1);
	buttonRepresentation->PlaceWidget(bds);

	m_buttonWidget->On();

	
}

void CMFCApplication1Dlg::CreateImage(vtkSmartPointer<vtkImageData> image, void* data, int width, int height)
{
	// Specify the size of the image data
	image->SetDimensions(width, height, 1);
	image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

	int* dims = image->GetDimensions();

	unsigned char * dataPtr = (unsigned char *) data;
	// Fill the image with
	for (int y = 0; y < dims[1]; y++)
	{
		for (int x = 0; x < dims[0]; x++)
		{
			unsigned char* pixel =
				static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));

			pixel[0] = dataPtr[x + y * 50];
			pixel[1] = dataPtr[x + y * 50];
			pixel[2] = dataPtr[x + y * 50];
		}
	}
}

void CMFCApplication1Dlg::ResliceOn()
{
	m_pxikVolumeSliceWidget->On();
}

void CMFCApplication1Dlg::ResliceOff()
{
	m_pxikVolumeSliceWidget->Off();
}

void CMFCApplication1Dlg::set2DPlane()
{
	vtkImageData * image = vtkImageData::New();

	image->SetDimensions(100, 100, 1);
	image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

	int * dims = image->GetDimensions();

	for (int y = 0; y < 100; y++)
	{
		for (int x = 0; x < 100; x++)
		{
			unsigned char* pixel = static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));
			
			if (x < 10 || y < 10 || x>90 || y >90)
			{
				pixel[0] = 255;
				pixel[1] = 0;
				pixel[2] = 0;
			}
			else
			{
				pixel[0] = 0;
				pixel[1] = 0;
				pixel[2] = 255;
			}
		}
	}

	vtkImageMapper* imageMapper= vtkImageMapper::New();
	imageMapper->SetInputData(image);
	imageMapper->SetColorWindow(255);
	imageMapper->SetColorLevel(127.5);
	image->UnRegister(0);

	vtkActor2D* imageActor = vtkActor2D::New();
	imageActor->SetMapper(imageMapper);
	imageMapper->UnRegister(0);

	m_vtkRenderer->AddActor2D(imageActor);
	imageActor->SetPosition(100, 100);
	imageActor->GetProperty()->SetOpacity(1);
	imageActor->UnRegister(0);

	imageActor->SetLayerNumber(1);
}

void CMFCApplication1Dlg::set2DPlaneimage()
{
	vtkImageData * image = vtkImageData::New();

	image->SetDimensions(50, 50, 1);
	image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

	int * dims = image->GetDimensions();

	for (int y = 0; y < 50; y++)
	{
		for (int x = 0; x < 50; x++)
		{
			unsigned char* pixel = static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));

			if (x < 5 || y < 5 || x>45 || y >45)
			{
				pixel[0] = 0;
				pixel[1] = 255;
				pixel[2] = 0;
			}
			else
			{
				pixel[0] = 0;
				pixel[1] = 255;
				pixel[2] = 0;
			}
		}
	}

	vtkImageMapper* imageMapper = vtkImageMapper::New();
	imageMapper->SetInputData(image);
	imageMapper->SetColorWindow(255);
	imageMapper->SetColorLevel(127.5);
	image->UnRegister(0);

	vtkActor2D* imageActor = vtkActor2D::New();
	imageActor->SetMapper(imageMapper);
	imageMapper->UnRegister(0);


	m_vtkRenderer->AddActor2D(imageActor);
	imageActor->SetPosition(125, 125);
	imageActor->GetProperty()->SetOpacity(1);
	imageActor->UnRegister(0);

	int loc = imageActor->GetLayerNumber();

}

void CMFCApplication1Dlg::balloonTest()
{
	vtkSmartPointer<vtkSphereSource> sphereSource =
		vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetCenter(-4.0, 0.0, 0.0);
	sphereSource->SetRadius(4.0);
	sphereSource->Update();

	vtkSmartPointer<vtkPolyDataMapper> sphereMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	sphereMapper->SetInputConnection(sphereSource->GetOutputPort());

	vtkSmartPointer<vtkActor> sphereActor =
		vtkSmartPointer<vtkActor>::New();
	sphereActor->SetMapper(sphereMapper);

	// Regular Polygon
	vtkSmartPointer<vtkRegularPolygonSource> regularPolygonSource =
		vtkSmartPointer<vtkRegularPolygonSource>::New();
	regularPolygonSource->SetCenter(4.0, 0.0, 0.0);
	regularPolygonSource->SetRadius(4.0);
	regularPolygonSource->Update();

	vtkSmartPointer<vtkPolyDataMapper> regularPolygonMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	regularPolygonMapper->SetInputConnection(regularPolygonSource->GetOutputPort());

	vtkSmartPointer<vtkActor> regularPolygonActor =
		vtkSmartPointer<vtkActor>::New();
	regularPolygonActor->SetMapper(regularPolygonMapper);

	// A renderer and render window
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);

	// An interactor
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	// Create the widget
	vtkSmartPointer<vtkBalloonRepresentation> balloonRep =
		vtkSmartPointer<vtkBalloonRepresentation>::New();
	balloonRep->SetBalloonLayoutToImageRight();

	vtkSmartPointer<vtkBalloonWidget> balloonWidget =
		vtkSmartPointer<vtkBalloonWidget>::New();
	balloonWidget->SetInteractor(renderWindowInteractor);
	balloonWidget->SetRepresentation(balloonRep);
	balloonWidget->AddBalloon(sphereActor,
		"This is a sphere", NULL);
	balloonWidget->AddBalloon(regularPolygonActor,
		"This is a regular polygon", NULL);

	// Add the actors to the scene
	renderer->AddActor(sphereActor);
	renderer->AddActor(regularPolygonActor);

	// Render
	renderWindow->Render();
	balloonWidget->EnabledOn();

	// Begin mouse interaction
	renderWindowInteractor->Start();
}


void CMFCApplication1Dlg::addPanel()
{
	this->m_pxikVTKUI = vtkSmartPointer<pxikVTKUIPanelWidget>::New();
	this->m_pxikVTKUIRepresentation = vtkSmartPointer<pxikVTKUIPanelRepresentation>::New();

	m_pxikVTKUIRepresentation->setMargin(10,
		pxikVTKUIPanelRepresentation::marginRight |
		pxikVTKUIPanelRepresentation::marginLeft
		);

	m_pxikVTKUIRepresentation->setSize(250, 250);
	m_pxikVTKUIRepresentation->GetFrameProperty()->SetOpacity(1);

	m_pxikVTKUI->SetInteractor(m_vtkInteractor);
	m_pxikVTKUI->SetRepresentation(m_pxikVTKUIRepresentation);

	m_pxikVTKUI->EnabledOn();
	//m_vtkInteractor->Start();
}

void CMFCApplication1Dlg::addChart()
{
	vtkNew<vtkNamedColors> colors;

	//m_vtkRenderer->ResetCamera();
	//m_vtkRenderer->GetActiveCamera()->SetPosition(1.0, 1.0, -4.0);
	//m_vtkRenderer->GetActiveCamera()->Azimuth(40);


	// Now the chart
	vtkNew<vtkChartXY> chart;
	vtkNew<vtkContextScene> chartScene;
	vtkNew<vtkContextActor> chartActor;

	int* windowSize = m_vtkRenderer->GetSize();
	int size[2];

	size[0] = windowSize[0] - ((15 * 2) + 30);
	size[1] = 220;

	chart->SetAutoSize(false);
	chart->SetSize(vtkRectf(15.0, 15.0, size[0], size[1]));

	chartScene->AddItem(chart);
	chartActor->SetScene(chartScene);

	//both needed
	m_vtkRenderer->AddActor(chartActor);
	chartScene->SetRenderer(m_vtkRenderer);

	// Create a table with some points in it...
	vtkNew<vtkTable> table;

	vtkNew<vtkFloatArray> arrX;
	arrX->SetName("X Axis");
	table->AddColumn(arrX);

	vtkNew<vtkFloatArray> arrH;
	arrH->SetName("Histogram");
	table->AddColumn(arrH);

	// Test charting with a few more points...
	int numPoints = 65536;
	double inc = 65536 / size[0];
	table->SetNumberOfRows(numPoints);
	//table->SetNumberOfRows(numPoints);
	for (int i = 0; i < numPoints; ++i)
	{
		table->SetValue(i, 0, i);
		table->SetValue(i, 1, this->getSingleHistogramValue( i ));
	}

	// Add multiple line plots, setting the colors etc
	vtkColor3d color3d = colors->GetColor3d("Red");

	vtkPlot *points = chart->AddPlot(vtkChart::LINE);
	points->SetInputData(table, 0, 1);
	points->SetColor(1.0, .0, .0);
	points->SetWidth(1.0);

	m_vtkWindow->SetMultiSamples(2);
	m_vtkRenderer->Render();

	vtkSmartPointer<vtkContextInteractorStyle> iterStyle = vtkSmartPointer<vtkContextInteractorStyle>::New();
	iterStyle->SetScene(chartScene);
	iterStyle->SetInteractor(m_vtkInteractor);
	//m_vtkInteractor->SetInteractorStyle(iterStyle);
	m_vtkInteractor->Initialize();
	m_vtkInteractor->Start();
	
}

#pragma region Histogram

void CMFCApplication1Dlg::addHistogram()
{
	if (m_histOriginData != nullptr)
		free(m_histOriginData);

	if (m_histResizeData != nullptr)
		free(m_histResizeData);

	m_histOriginData = nullptr;
	m_histResizeData = nullptr;

	int m_startIdx, m_endIdx;

	switch (1)
	{
	case 0:
		m_startIdx = SHRT_MIN;
		m_endIdx = SHRT_MAX;

		m_histOriginData = (unsigned int*)malloc(sizeof(int) * (USHRT_MAX + 1));
		memset(m_histOriginData, 0, (USHRT_MAX + 1) * sizeof(int));
		break;
	case 1:
		m_startIdx = 0;
		m_endIdx = USHRT_MAX;

		m_histOriginData = (unsigned int*)malloc(sizeof(int) * (USHRT_MAX + 1));
		memset(m_histOriginData, 0, (USHRT_MAX + 1) * sizeof(int));
		break;
	default:
		break;
	}


	int size = 1024 * 1024 * 100;

	switch (1)
	{
	case 0:
	{
		short *Picker = (short*)m_vtkVolumeImageData->GetScalarPointer();
		for (int i = 0; i < size; i++)
			m_histOriginData[Picker[i] - m_startIdx]++;
	}
	break;
	case 1:
	{
		unsigned short	*Picker = (unsigned short*)m_vtkVolumeImageData->GetScalarPointer();
		for (int i = 0; i < size; i++)
			m_histOriginData[Picker[i] - m_startIdx]++;
	}
	break;
	default:
		break;
	}

	m_maxHeight = 0;
	for (int x = 0; x < USHRT_MAX + 1; x++)
	{
		if (m_maxHeight < (int)m_histOriginData[x])
			m_maxHeight = (int)m_histOriginData[x];
	}


}

double CMFCApplication1Dlg::getSingleHistogramValue(double x)
{
	return this->getSingleHistogramValue((int)round(x));
}

double CMFCApplication1Dlg::getSingleHistogramValue(int x)
{
	if (m_histOriginData[x] < 2)
		return 0;

	return log2(m_histOriginData[x]) / log2(m_maxHeight);
}

void CMFCApplication1Dlg::setResizeHistogram(int pitch)
{
	int m_pitch = (USHRT_MAX + 1) / pitch;
	int m_pitchStartIdx, m_pitchEndIdx;


	switch (1)
	{
	case 0:
	{
		m_pitchStartIdx = -pitch / 2;
		m_pitchEndIdx = +pitch / 2 - 1;
	}
	break;
	case 1:
	{
		m_pitchStartIdx = 0;
		m_pitchEndIdx = pitch - 1;
	}
	default:
		break;
	}

	if (m_histResizeData != nullptr)
		free(m_histResizeData);

	m_histResizeData = (unsigned int*)malloc(sizeof(int) * pitch);
	memset(m_histResizeData, 0, (pitch) * sizeof(int));

	int m_pitchMaxHeight = 0;
	for (int i = 0; i < pitch; i++)
	{
		for (int j = 0; j < m_pitch; j++)
			m_histResizeData[i] += m_histOriginData[(m_pitch * i) + j];

		m_histResizeData[i] /= m_pitch;

		if (m_pitchMaxHeight < (int)m_histResizeData[i])
			m_pitchMaxHeight = m_histResizeData[i];
	}
}

#pragma endregion

void CMFCApplication1Dlg::setSilderWiget()
{
	vtkSmartPointer<vtkSliderWidget> widget	= vtkSmartPointer<vtkSliderWidget>::New();
	vtkSmartPointer<vtkSliderRepresentation2D> represent = vtkSmartPointer<vtkSliderRepresentation2D>::New();
	represent->ShowSliderLabelOn();
	represent->SetMinimumValue(0);
	represent->SetMaximumValue(1);
	represent->SetValue(50);

	represent->GetPoint1Coordinate()->SetCoordinateSystemToDisplay();
	represent->GetPoint1Coordinate()->SetValue(650, 250);
	represent->GetPoint2Coordinate()->SetCoordinateSystemToDisplay();
	represent->GetPoint2Coordinate()->SetValue(850, 250);
	//represent->SetTubeWidth(0.05);
	represent->SetEndCapLength(0.0);
	//represent->set

	represent->SetEndCapLength(0);
	//represent->ShowSliderLabelOff();
	represent->Highlight(1);

	//represent->SetHandleSize();
	//represent->SetSliderLength();
	//represent->BuildRepresentation();

	widget->SetRepresentation(represent);
	widget->SetInteractor(m_vtkInteractor);
	widget->On();

	vtkSmartPointer<vtkSliderCallback> callback = vtkSmartPointer<vtkSliderCallback>::New();
	callback->represent = this->m_pxikVTKUIRepresentation;
	widget->AddObserver(vtkCommand::InteractionEvent, callback);

	

	m_vtkInteractor->Start();
}

void CMFCApplication1Dlg::pxikCircleButton()
{
	vtkSmartPointer<vtkImageData> image1 =
		vtkSmartPointer<vtkImageData>::New();
	vtkSmartPointer<vtkImageData> image2 =
		vtkSmartPointer<vtkImageData>::New();

	cv::Mat resliceOnImage = cv::imread("C:\\Users\\User1\\Downloads\\ResliceOn.png", cv::IMREAD_GRAYSCALE);
	cv::Mat resliceOffImage = cv::imread("C:\\Users\\User1\\Downloads\\ResliceOff.png", cv::IMREAD_GRAYSCALE);

	CreateImage(image1, resliceOnImage.data, 50, 50);
	CreateImage(image2, resliceOffImage.data, 50, 50);



	vtkSmartPointer<pxikVTKUITextureButtonRepresentation> buttonRepresentation =
		vtkSmartPointer<pxikVTKUITextureButtonRepresentation>::New();
	buttonRepresentation->SetNumberOfStates(2);
	buttonRepresentation->SetButtonTexture(0, image1);
	buttonRepresentation->SetButtonTexture(1, image2);
	buttonRepresentation->setShape(pxikVTKUIWidgetAbstractRepresentation::Circle);
	buttonRepresentation->SetFrameVisible(1);
	//buttonRepresentation->BuildRepresentation();


	m_pxikVTKUIButtonWidget =
		vtkSmartPointer<pxikVTKUIButtonWidget>::New();
	m_pxikVTKUIButtonWidget->SetInteractor(m_vtkInteractor);
	m_pxikVTKUIButtonWidget->SetRepresentation(buttonRepresentation);


	// Place the widget. Must be done after a render so that the
	// viewport is defined.
	// Here the widget placement is in normalized display coordinates
	vtkSmartPointer<vtkCoordinate> upperRight =
		vtkSmartPointer<vtkCoordinate>::New();
	upperRight->SetCoordinateSystemToNormalizedDisplay();
	upperRight->SetValue(1.0, 1.0);

	double bds[6];
	double sz = 50.0;
	bds[0] = upperRight->GetComputedDisplayValue(m_vtkRenderer)[0] - sz - 10;
	bds[1] = bds[0] + sz;
	bds[2] = upperRight->GetComputedDisplayValue(m_vtkRenderer)[1] - sz - 10;
	bds[3] = bds[2] + sz;
	bds[4] = bds[5] = 0.0;

	// Scale to 1, default is .5
	buttonRepresentation->SetPlaceFactor(1);
	buttonRepresentation->PlaceWidget(bds);

	m_pxikVTKUIButtonWidget->On();
}