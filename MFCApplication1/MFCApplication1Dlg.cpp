

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
	clock_t start, end;
	double result;

	start = clock();

	this->VtkDCMTest();
	
	end = clock();
	result = (double)(end - start);
	CEditView *view = (CEditView *)this->GetDlgItem(IDC_EDIT1);
	
	CString str;
	str.Format(_T("%f"), result);

	view->SetWindowTextW(str);
	//this->convertDICOMToTIFF();
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
	//compositeOpacity->AddPoint(15277, 0);
	//compositeOpacity->AddPoint(21823, 1);
	
	//compositeOpacity->AddPoint(30000, 0);
	//compositeOpacity->AddPoint(40000, 1);
	
	
	compositeOpacity->AddPoint(40000, 0);
	compositeOpacity->AddPoint(70000, 1);
	compositeOpacity->ClampingOff();

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

	//Conncet with Mapper - dcmReader(FullMap)
	//m_smartV_Mapper->SetInputData(m_DCMReader->GetOutput());
	////////////////////////////////////////////////////////////////
	//slider
	//vtkSmartPointer<vtkImageData> originalImage = m_DCMReader->GetOutput();
	//vtkSmartPointer<vtkImageData> sliderImage = vtkSmartPointer<vtkImageData>::New();
	//this->DCMSilder(sliderImage, originalImage);
	//
	//m_smartV_Mapper->SetInputData(sliderImage);


	////////////////////////////////////////////////////////////////
	//TIFF READER
	//vtkSmartPointer<vtkImageData> x = TIFFReader();
	////////////////////////////////////////////////////////////////
	//TIFF READER OPENCV
	vtkSmartPointer<vtkImageData> x = TIFFReaderOpenCV();
	////////////////////////////////////////////////////////////////
	//TIFF READER OPENCV vtk Image Import
	//vtkSmartPointer<vtkImageImport> x = TIFFReaderOpenCVImport();
	
	//vtkSmartPointer<vtkImageShiftScale> convertScaleType = vtkSmartPointer<vtkImageShiftScale>::New();
	//convertScaleType->SetInputData(x);
	//convertScaleType->SetOutputScalarTypeToShort();
	//convertScaleType->Update();
	m_smartV_Mapper->SetInputData(x);
	//m_smartV_Mapper->SetInputConnection(x->GetOutputPort());
	////////////////////////////////////////////////////////////////
	//Setting clip plane;
	//m_smartV_Mapper->SetBlendModeToComposite();//이게 먼저
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
		memcpy(MainStackData, src, imgOffSetX * imgOffSetY * sizeof(USHORT));
		MainStackData += (imgOffSetX * imgOffSetY);
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
	m_vtkVolumeImageImport->Update();

	int type = m_vtkVolumeImageImport->GetDataScalarType();


	m_vtkVolumeImageImport->SetNumberOfScalarComponents(1);
	m_vtkVolumeImageImport->SetImportVoidPointer((void*) matPart.data);
	m_vtkVolumeImageImport->Update();

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