
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

	this->vtkConeTest();
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


///////////////////////////////////////////////////////////
/*
*					vtk TEST 관련 함수
*/
///////////////////////////////////////////////////////////

//Cone, Cylinder TEST 
void CMFCApplication1Dlg::vtkConeTest()
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
	vtkSmartPointer<vtkAnnotatedCubeActor> cube = vtkSmartPointer<vtkAnnotatedCubeActor>::New();

	m_orientationWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
	m_orientationWidget->SetOrientationMarker(cube);
	m_orientationWidget->SetInteractor( m_vtkWindow->GetInteractor() );
	m_orientationWidget->SetViewport(0, 0, 0.2, 0.2);
	m_orientationWidget->SetEnabled(TRUE);
	m_orientationWidget->On();


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
	vtkSmartPointer<vtkHexahedron> clipHex = vtkSmartPointer<vtkHexahedron>::New();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

}
