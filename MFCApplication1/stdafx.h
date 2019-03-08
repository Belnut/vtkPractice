
// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.


#include <afxdisp.h>        // MFC 자동화 클래스입니다.



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC의 리본 및 컨트롤 막대 지원









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif




////////////////////////////////////////////////////////
//VTK 
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)



//////////
//standard
#include <vtkSmartPointer.h>
#include <vtkMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkDataSetMapper.h>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>

#include <vtkActor2D.h>
#include <vtkImageViewer2.h>

//////////
//Camera
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkInteractorStyleJoystickCamera.h>
//////////
//data Type
#include <vtkPolyData.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkCylinderSource.h>
#include <vtkImageShiftScale.h>

//////////
// struct Type
#include <vtkHexahedron.h>
#include <vtkPlane.h>
#include <vtkCellArray.h>

#include <vtkCellArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkImageData.h>
#include <vtkClipDataSet.h>
#include <vtkUnstructuredGrid.h>

#include <vtkPolyDataNormals.h>
#include <vtkStringArray.h>
//////////
//Filter
#include <vtkOutlineFilter.h>
#include <vtkExtractEdges.h>
#include <vtkGeometryFilter.h>

//////////
//data Reader
#include <vtkDICOMImageReader.h>
#include <vtkTIFFReader.h>

#include <vtkTIFFWriter.h>

//////////
//Volume oftion(DCM)
#include <vtkPiecewiseFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkColorTransferFunction.h>

//////////
//Volume Standard
#include <vtkSmartVolumeMapper.h>
#include<vtkMapper2D.h>
#include<vtkPolyDataMapper2D.h>
#include <vtkGPUVolumeRayCastMapper.h>

//////////
//widget
#include <vtkAbstractWidget.h>
#include <vtkAngleWidget.h>
#include <vtkImagePlaneWidget.h>
#include <vtkCaptionWidget.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkTextWidget.h>

//////////
//Picking0
#include <vtkVolumePicker.h>
#include <vtkCellPicker.h>

//////////
//etc vtk
#include <vtkAnnotatedCubeActor.h>
#include <vtkInformation.h>
#include <vtkImageImport.h>

#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkImageResliceMapper.h>

#include <vtkImageSlice.h>
#include <vtkInteractorStyleImage.h>