
// MFCApplication1Dlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"

#include <vtkCaptionActor2D.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkAnnotatedCubeActor.h>

#ifdef _DEBUG
#define new DEBUG_NEW
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
		this->InitVtkWindow(this->GetDlgItem(IDC_STATIC_RENDERWINDOW)->GetSafeHwnd());
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

		vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
		renderer->SetBackground(.0, .0, .0);

		//원래는 책에서도 말하듯 이게 맞는거 같긴한데
		//아마 이렇게 하면 이 메서드를 빠저나가는 순간 문제가 생기지 않을까?
		//실제로는 문제없음
		//interactor->SetRenderWindow(m_vtkWindow);

		m_vtkWindow->SetParentId(hWnd);
		m_vtkWindow->AddRenderer(renderer);
		m_vtkWindow->SetInteractor(interactor);
		m_vtkWindow->Render();
	}
}

//설정한 FRAME에 맞게 사이즈가 조절됨
//this->Onsize()가 호출, 프래임을 바꿀시 hWnd 변경할 것
void CMFCApplication1Dlg::ResizeVtkWindow()
{
	CRect rc;
	GetDlgItem(IDC_STATIC_RENDERWINDOW)->GetClientRect(rc);

	m_vtkWindow->SetSize(rc.Width(), rc.Height());
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

	this->VtkDCMTest();
	vtkSmartPointer<vtkDICOMImageReader> RCMreader = vtkSmartPointer<vtkDICOMImageReader>::New();
	RCMreader->Modified();
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

//dcm Test
void CMFCApplication1Dlg::VtkDCMTest()
{
	//////////////////////////////////
	// DCM READ
	m_DCMReader = vtkSmartPointer<vtkDICOMImageReader>::New();
	m_DCMReader->SetDirectoryName("D:/PCB_CT");
	m_DCMReader->Update();

	//////////////////////////////////
	// DCM Info
	m_DCMReader->GetOutput()->GetBounds(m_dcmBounds);
	m_DCMReader->GetDataOrigin(m_dcmOrigin);
	m_DCMReader->GetDataExtent(m_dcmExtent);

	//////////////////////////////////
	// add Plane
	m_zTopClipPlane = vtkSmartPointer<vtkPlane>::New();
	m_zTopClipPlane->SetOrigin(m_dcmExtent[1], m_dcmExtent[3], m_dcmExtent[5]);
	m_zTopClipPlane->SetNormal(0, 0, -1);

	m_zBtmClipPlane = vtkSmartPointer<vtkPlane>::New();
	m_zBtmClipPlane->SetOrigin(m_dcmExtent[1], m_dcmExtent[3], m_dcmExtent[4]);
	m_zBtmClipPlane->SetNormal(0, 0, 1);


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
	compositeOpacity->AddPoint(15277, 0);
	compositeOpacity->AddPoint(21823, 1);

	////////////////////////////////////////////////////////////////
	//color setting
	vtkSmartPointer<vtkColorTransferFunction> color
		= vtkSmartPointer<vtkColorTransferFunction>::New();

	color->AddRGBPoint(-15000, 1, 0, 0);
	color->AddRGBPoint(-18000, 1, 0, 0);
	color->AddRGBPoint(-22000, 0, 1, 0);

	////////////////////////////////////////////////////////////////
	//Mapper
	m_smartV_Mapper	= vtkSmartPointer<vtkSmartVolumeMapper>::New();

	//Conncet with Mapper - dcmReader
	m_smartV_Mapper->SetInputData(m_DCMReader->GetOutput());



	////////////////////////////////////////////////////////////////
	//Setting clip plane;
	m_smartV_Mapper->SetBlendModeToComposite();//이게 먼저
	m_smartV_Mapper->AddClippingPlane(m_zTopClipPlane);	// +Z clip
	m_smartV_Mapper->AddClippingPlane(m_zBtmClipPlane);	// -Z clip

	////////////////////////////////////////////////////////////////
	//Connect with Property - color and Opacity
	vtkSmartPointer<vtkVolumeProperty> volumeProperty
		= vtkSmartPointer<vtkVolumeProperty>::New();

	volumeProperty->ShadeOn();
	volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);
	volumeProperty->SetColor(color);
	volumeProperty->SetScalarOpacity(compositeOpacity);

	////////////////////////////////////////////////////////////////
	//Conncet with Volume  - Mapper and Property
	vtkSmartPointer<vtkVolume> volume
		= vtkSmartPointer<vtkVolume> ::New();
	volume->SetMapper(m_smartV_Mapper);
	volume->SetProperty(volumeProperty);


	////////////////////////////////////////////////////////////////
	//Connect With Renderer - Volume
	vtkSmartPointer<vtkRenderer> renderer
		= vtkSmartPointer<vtkRenderer>::New();

	renderer->AddVolume(volume);
	renderer->SetBackground(.8, .8, .8);
	renderer->ResetCamera();

	////////////////////////////////////////////////////////////////
	//Connect With vtkWindow - Renderer 
	m_vtkWindow->AddRenderer(renderer);
	this->SetCilpBox(m_dcmBounds, renderer);
	
	m_vtkWindow->Render();


	///////////////////////////////////////////////////////////////
	// another setting
	SetClipSlide(m_dcmBounds[5], m_dcmExtent[5]);




	///////////////////////////////////////////////////////////////
	//Setting widget
	this->SettingOrientationWidget();

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
		
	/////////////////////////////////
	//hexahedron Setting
//	vtkSmartPointer<vtkHexahedron> clipHex = vtkSmartPointer<vtkHexahedron>::New();
//
//	//Setting 8 points
//	double P0[3] = { 0, 0, 0 };
//	double P1[3] = { m_dcmBounds[1], 0, 0 };
//	double P2[3] = { m_dcmBounds[1], m_dcmBounds[3], 0 };
//	double P3[3] = { 0, m_dcmBounds[3], 0 };
//	double P4[3] = { 0, 0, m_dcmBounds[5] };
//	double P5[3] = { m_dcmBounds[1], 0, m_dcmBounds[5] };
//	double P6[3] = { m_dcmBounds[1], m_dcmBounds[3], m_dcmBounds[5] };
//	double P7[3] = { 0, m_dcmBounds[3], m_dcmBounds[5] };
//
//	//create Points
//	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
//	points->InsertNextPoint(P0);
//	points->InsertNextPoint(P1);
//	points->InsertNextPoint(P2);
//	points->InsertNextPoint(P3);
//	points->InsertNextPoint(P4);
//	points->InsertNextPoint(P5);
//	points->InsertNextPoint(P6);
//	points->InsertNextPoint(P7);
//
//	//create a hexahedon
//	clipHex->GetPointIds()->SetId(0, 0);
//	clipHex->GetPointIds()->SetId(1, 1);
//	clipHex->GetPointIds()->SetId(2, 2);
//	clipHex->GetPointIds()->SetId(3, 3);
//	clipHex->GetPointIds()->SetId(4, 4);
//	clipHex->GetPointIds()->SetId(5, 5);
//	clipHex->GetPointIds()->SetId(6, 6);
//	clipHex->GetPointIds()->SetId(7, 7);
//
//	//add hexahedron to a cell array
//	vtkSmartPointer<vtkCellArray> hexs = vtkSmartPointer<vtkCellArray>::New();
//	hexs->InsertNextCell(clipHex);
//
//	// Add the points and hexahedron to an unstructured grid
//	vtkSmartPointer<vtkUnstructuredGrid> uGrid =
//		vtkSmartPointer<vtkUnstructuredGrid>::New();
//	uGrid->SetPoints(points);
//	uGrid->InsertNextCell(clipHex->GetCellType(), clipHex->GetPointIds());
//
//	// Visualize
//	vtkSmartPointer<vtkDataSetMapper> mapper =
//		vtkSmartPointer<vtkDataSetMapper>::New();
//#if VTK_MAJOR_VERSION <= 5
//	mapper->SetInputConnection(uGrid->GetProducerPort());
//#else
//	mapper->SetInputData(uGrid);
//#endif
//
//	vtkSmartPointer<vtkActor> actor =
//		vtkSmartPointer<vtkActor>::New();
//	actor->SetMapper(mapper);
//	actor->GetProperty()->SetOpacity(0.3);
//	actor->GetProperty()->SetLineWidth(1.2);
//	actor->GetProperty()->BackfaceCullingOff();
//
//	renderer->AddActor(actor);
//
//	//outline actor 추가
//	//////////////////////////////////////////////////
//	//PolyData 전용
//	//vtkSmartPointer<vtkPolyDataNormals> outlineNomal =
//	//	vtkSmartPointer<vtkPolyDataNormals>::New();
//	//outlineNomal->SetInputData(uGrid);
//	//
//	//vtkSmartPointer<vtkOutlineFilter> outlineFilter =
//	//	vtkSmartPointer<vtkOutlineFilter>::New();
//	//outlineFilter->SetInputData(outlineNomal->GetOutput());
//	/////////////////////////////////////////////////////
//
//	vtkSmartPointer<vtkGeometryFilter> geoFilter =
//		vtkSmartPointer<vtkGeometryFilter>::New();
//	geoFilter->SetInputData(uGrid);
//
//	vtkSmartPointer<vtkExtractEdges> edgesFilter =
//		vtkSmartPointer<vtkExtractEdges>::New();
//
//	edgesFilter->SetInputData(geoFilter->GetOutput());
//
//	vtkSmartPointer<vtkPolyDataMapper> outlineMapper =
//		vtkSmartPointer<vtkPolyDataMapper>::New();
//	outlineMapper->SetInputData(outlineFilter->GetOutput());
//
//	vtkSmartPointer<vtkActor> outlineActor =
//		vtkSmartPointer<vtkActor>::New();
//
//	outlineActor->GetProperty()->SetColor(0, 0, 0);
//
//	renderer->AddActor(outlineActor);

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


void CMFCApplication1Dlg::DCMSilder()
{
	m_DCMReader->GetOutput()->GetScalarPointer();
	
	//imgData->GetScalarPointer();
	//imgData->SetDimensions;
	//imgData->SetExtent;
	//imgData->SetOrigin;
	//imgData->SetInformation;
	//imgData->AllocateScalars
}