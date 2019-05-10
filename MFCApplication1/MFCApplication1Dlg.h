
// MFCApplication1Dlg.h: 헤더 파일
//

#pragma once
#include "pxikVolumeOrientMarker.h"
#include "pxikVolumeTrackBallCamera.h"
#include "pxikVolumeSliceWidget.h"
#include "pxikVTKButtonWidget.h"

#include "pxikVTKUIPanelWidget.h"
#include "pxikVTKUIPanelRepresentation.h"


// CMFCApplication1Dlg 대화 상자
class CMFCApplication1Dlg : public CDialogEx
{
	// 생성입니다.
public:
	CMFCApplication1Dlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	~CMFCApplication1Dlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION1_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


public:
	vtkSmartPointer<vtkRenderWindow>				m_vtkWindow;
	vtkSmartPointer<vtkAngleWidget>					m_angleWidget;
	vtkSmartPointer<vtkImagePlaneWidget>			m_imageWidget;
	vtkSmartPointer<vtkCaptionWidget>				m_captionWidget;
	vtkSmartPointer<vtkOrientationMarkerWidget>		m_orientationWidget;
	vtkSmartPointer<vtkTextWidget>					m_textWidget;
	vtkSmartPointer<vtkDICOMImageReader>			m_DCMReader;
	vtkSmartPointer<vtkGPUVolumeRayCastMapper>			m_smartV_Mapper;
	vtkSmartPointer<vtkRenderer>					m_vtkRenderer;


	vtkSmartPointer<vtkRenderWindowInteractor>		m_vtkInteractor;
	vtkSmartPointer<vtkGPUVolumeRayCastMapper>		m_vtkVolumeMapper;
	vtkSmartPointer<vtkPiecewiseFunction>			m_vtkVolumePiecewiseFunction;
	vtkSmartPointer<vtkColorTransferFunction>		m_vtkVolumeColorTransferFunction;
	vtkSmartPointer<vtkVolumeProperty>				m_vtkVolumeProperty;
	vtkSmartPointer<vtkImageData>					m_vtkVolumeImageData;
	vtkSmartPointer<vtkVolume>						m_vtkVolume;


	vtkSmartPointer<vtkOutlineFilter>				m_vtkOutlineFilter;
	vtkSmartPointer<vtkPolyDataMapper>				m_vtkPolyDataMapper;
	vtkSmartPointer<vtkActor>						m_vtkActor;
	vtkSmartPointer<vtkImageResliceMapper>			m_vtkImageResliceMapper;
	vtkSmartPointer<vtkImageProperty>				m_vtkImageProp;
	vtkSmartPointer< vtkImageSlice>					m_vtkImageSlice;


	vtkSmartPointer<vtkImagePlaneWidget>			m_vtkimagePlaneWidget;
	





	//custom made vtkObjects
	vtkSmartPointer<pxikVTKVolumeOrientMarker>		m_pxikOrientationMarker = nullptr;
	vtkSmartPointer<pxikVolumeTrackBallCamera>		m_pxikVolumeTrackBallCamera = nullptr;
	vtkSmartPointer<pxikVolumeSliceWidget>			m_pxikVolumeSliceWidget = nullptr;
	vtkSmartPointer<pxikVTKButtonWidget>			m_buttonWidget = nullptr;
	vtkSmartPointer<pxikVTKUIPanelWidget>			m_pxikVTKUI = nullptr;
	vtkSmartPointer<pxikVTKUIPanelRepresentation>	m_pxikVTKUIRepresentation = nullptr;

	//TestObject;
	vtkCubeSource*				m_vtkTestCubeSource = NULL;
	vtkPolyDataMapper*			m_vtkTestPolyDataMapper = NULL;
	vtkActor*					m_vtkTestActor = NULL;

	vtkSmartPointer<vtkLookupTable> table;


	vtkSmartPointer<vtkPlane>						m_zTopClipPlane;
	vtkSmartPointer<vtkPlane>						m_zBtmClipPlane;

	double m_dcmBounds[6];
	double m_dcmOrigin[3];
	int m_dcmExtent[6];


	unsigned int*	m_histOriginData = nullptr;
	unsigned int*	m_histResizeData = nullptr;
	int m_maxHeight = 0;

	enum PlaneLoc
	{
		TOP = 0,
		BOTTOM = 1
	};


	void InitVtkWindow(void* hWnd);
	void ResizeVtkWindow();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnSize(UINT nType, int cx, int cy);

private:
	void VtkConeTest();
	void VtkCubeTest();
	void SmartVtkCubeTest();
	void VtkDCMTest();
	void SettingOrientationWidget();
	void SetPicking();
public:
	void SetCilpBox(double * m_dcmBounds, vtkSmartPointer<vtkRenderer> renderer);
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
private:
	void SetClipSlide(double z_Bound, int Range);
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void ChangePlaneOrigin(int Pos, PlaneLoc PlaneLoc);
	void DCMSilder(vtkSmartPointer<vtkImageData> slider, vtkSmartPointer<vtkImageData> origin);
	vtkSmartPointer<vtkImageData> TIFFReader();
	vtkSmartPointer<vtkImageData> TIFFReaderOpenCV();
	vtkSmartPointer<vtkImageImport> TIFFReaderOpenCVImport();
	void convertDICOMToTIFF();
	afx_msg void OnNMCustomdrawSliderBottom(NMHDR *pNMHDR, LRESULT *pResult);


	void newCreate();
	void initialize(HWND parent);
	void onSize(int width, int height);
	void volumePropInitialize();
	void setROIVolumeDataOpenCV(int topLeftX, int topLeftY, int btmRightX, int btmRightY, int sliceTopZ, int sliceBtmZ);

	void setVtkOutLine();


	void setSliceImage();

	void setSliceImage2();

	void setOrientAxesActor();
	void setCustomOrientAxesActor();


	void setSliceImageWidgetPreset();

	void setSliceImageWidgetCustom();

	void addButton();
	void addCustomButton();

	void CreateImage(vtkSmartPointer<vtkImageData> image, void* data, int x, int y);
	void ResliceOn();
	void ResliceOff();

	void set2DPlane();
	void set2DPlaneimage();

	void balloonTest();

	void addPanel();
	void addChart();
	
	
	void addHistogram();
	double getSingleHistogramValue(double x);
	double getSingleHistogramValue(int x);
	void setResizeHistogram(int pitch);

	void setSilderWiget();
};
