#pragma once
#include "pxikVTKUIPanelRepresentation.h"
class pxikVTKUIPanelRepresentation :
	public pxikVTKUIWidgetAbstractRepresentation
{

public:
	static pxikVTKUIPanelRepresentation *New();
	//@{
		/**
		 * Standard VTK methods.
		 */
	vtkTypeMacro(pxikVTKUIPanelRepresentation, pxikVTKUIWidgetAbstractRepresentation);
	void PrintSelf(ostream& os, vtkIndent indent) override;
	//@}

	//@{
	/**
	 * Set/get the frame property (relevant only if text is shown).
	 * The frame lies behind the text.
	 */
	virtual void SetFrameProperty(vtkProperty2D *p);
	vtkGetObjectMacro(FrameProperty, vtkProperty2D);
	//@}

	//@{
	/**
	 * Set/Get the offset from the mouse pointer from which to place the
	 * balloon. The representation will try and honor this offset unless there
	 * is a collision with the side of the renderer, in which case the balloon
	 * will be repositioned to lie within the rendering window.
	 */
	vtkSetVector2Macro(Offset, int);
	vtkGetVector2Macro(Offset, int);
	//@}

	//@{
	/**
	 * These are methods that satisfy vtkWidgetRepresentation's API.
	 */
	void StartWidgetInteraction(double e[2]) override;
	void EndWidgetInteraction(double e[2]) override;
	void BuildRepresentation() override;
	int ComputeInteractionState(int X, int Y, int modify = 0) override;
	//@}

	//@{
	/**
	 * Methods required by vtkProp superclass.
	 */
	void ReleaseGraphicsResources(vtkWindow *w) override;
	int RenderOverlay(vtkViewport *viewport) override;
	//@}

	/**
	 * State is either outside, or inside (on the text portion of the image).
	 */
	enum _InteractionState { Outside = 0, Onside };

	//@{
	/**
	* Frame 크기 등을 결정하는 부분
	*/

	////배경 관련 part
	//vtkImageData *m_backgroundImage;
	//void setBackgroundImage(vtkImageData * src);
	//void removeBackgroundImage();
	//
	//struct color_t {
	//	double red, green, blue;
	//
	//	color_t()
	//	{
	//		red = 0;
	//		green = 0;
	//		blue = 0;
	//	}
	//
	//	void setcolor(double _red, double _green, double _blue)
	//	{
	//		red = _red;
	//		green = _green;
	//		blue = _blue;
	//	}
	//
	//};
	//
	//struct color_t m_bactgroundColor;
	//void setBackgroundColor(color_t);
	//void getBackgroundColor();
	//
	//void createImageData(vtkImageData *);
	//
	//
	//vtkActor2D *m_UIActor;
	//vtkImageMapper *m_UIMapper;

	int m_margin = 0;
	int m_marginTopValue = 0;
	int m_marginBtmValue = 0;
	int m_marginLeftValue = 0;
	int m_marginRightValue = 0;
	enum : int
	{
		marginTop = 1,
		marginBtm = 2,
		marginLeft = 4,
		marginRight = 8
	};

	void setMargin(int value, int margin);
	//please use single margin (2^n, 0 <= n <= 3)
	int getMargin(int margin);
	void removeAllMargin();

	int m_PositionX, m_PositionY;
	void setPosition(int x, int y);
	int* getPosition();

	int m_width, m_height;
	void setSize(int width, int height);
	int* getSize();

	/*struct color_t m_borderColor;
	void setBorder(int);
	void setBorderColor();
	void getBorderColor();*/

	//please Call, When UI Layout geometry or marign is Changed
	void AdjustFrameSize(double* frameSize);
	void AdjustFramePosition(double * framePosition);

	//{@
	/**
	* UI의 실제 그려지는 위치,
	* 해당 값을 사용하여 자식 UI의 location을 결정할 것
	*/
	int* getUIPosition();
	int* getUISize();
	//@}



	int m_lastButtonPressed;
	enum
	{
		VTK_NO_BUTTON = 0,
		VTK_LEFT_BUTTON = 1,
		VTK_MIDDLE_BUTTON = 2,
		VTK_RIGHT_BUTTON = 3
	};

	int m_state;
	enum WidgetState
	{
		Outside111 = 0,
		Click,
		Moving,
		Hovering
	};

protected:
	pxikVTKUIPanelRepresentation();
	~pxikVTKUIPanelRepresentation() override;

	//// Frame image (temp value. It will maybe be deleted)
	//vtkImageData *BalloonImage;


	// Controlling placement
	int Offset[2];
	//int ImageSize[2];
	int FrameSize[2];

	pxikVTKUIPanelRepresentation *m_parent = nullptr;
	//// Represent the image
	//vtkTexture          *Texture;
	//vtkPolyData         *TexturePolyData;
	//vtkPoints           *TexturePoints;
	//vtkPolyDataMapper2D *TextureMapper;
	//vtkTexturedActor2D  *TextureActor;
	//vtkProperty2D       *ImageProperty;

	// The frame
	vtkPoints           *FramePoints;
	vtkCellArray        *FramePolygon;
	vtkPolyData         *FramePolyData;
	vtkPolyDataMapper2D *FrameMapper;
	vtkActor2D          *FrameActor;
	vtkProperty2D       *FrameProperty;

	// Internal variable controlling rendering process
	int FrameVisible;

	double Color[3];
	double Opacity;


	// Helper methods
	void ScaleImage(double imageSize[2], double scale);

private:
	pxikVTKUIPanelRepresentation(const pxikVTKUIPanelRepresentation&) = delete;
	void operator=(const pxikVTKUIPanelRepresentation&) = delete;
};

