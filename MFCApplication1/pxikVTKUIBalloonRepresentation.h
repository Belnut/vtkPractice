#pragma once

#ifndef pxikVTKUIBalloonRepresentation_h
#define pxikVTKUIBalloonRepresentation_h
#include "pxikVTKUIWidgetAbstractRepresentation.h"

class vtkTextMapper;
class vtkTextActor;
class vtkTextProperty;
class vtkPoints;
class vtkCellArray;
class vtkPolyData;
class vtkPolyDataMapper2D;
class vtkActor2D;
class vtkProperty2D;
class vtkImageData;
class vtkTexture;
class vtkPoints;
class vtkPolyData;
class vtkPolyDataMapper2D;
class vtkTexturedActor2D;


class pxikVTKUIBalloonRepresentation :
	public pxikVTKUIWidgetAbstractRepresentation
{
public:
	/**
	 * Instantiate the class.
	 */
	static pxikVTKUIBalloonRepresentation *New();

	//@{
	/**
	 * Standard VTK methods.
	 */
	vtkTypeMacro(pxikVTKUIBalloonRepresentation, vtkWidgetRepresentation);
	void PrintSelf(ostream& os, vtkIndent indent) override;
	//@}

	//@{
	/**
	 * Specify/retrieve the image to display in the balloon.
	 */
	virtual void SetBalloonImage(vtkImageData *img);
	vtkGetObjectMacro(BalloonImage, vtkImageData);
	//@}

	//@{
	/**
	 * Specify/retrieve the text to display in the balloon.
	 */
	vtkGetStringMacro(BalloonText);
	vtkSetStringMacro(BalloonText);
	//@}

	//@{
	/**
	 * Specify the minimum size for the image. Note that this is a bounding
	 * rectangle, the image will fit inside of it. However, if the balloon
	 * consists of text plus an image, then the image may be bigger than
	 * ImageSize[2] to fit into the balloon frame.
	 */
	vtkSetVector2Macro(ImageSize, int);
	vtkGetVector2Macro(ImageSize, int);
	//@}

	//@{
	/**
	 * Set/get the text property (relevant only if text is shown).
	 */
	virtual void SetTextProperty(vtkTextProperty *p);
	vtkGetObjectMacro(TextProperty, vtkTextProperty);
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
	 * Set/get the image property (relevant only if an image is shown).
	 */
	virtual void SetImageProperty(vtkProperty2D *p);
	vtkGetObjectMacro(ImageProperty, vtkProperty2D);
	//@}

	enum { ImageLeft = 0, ImageRight, ImageBottom, ImageTop };

	//@{
	/**
	 * Specify the layout of the image and text within the balloon. Note that
	 * there are reduncies in these methods, for example
	 * SetBalloonLayoutToImageLeft() results in the same effect as
	 * SetBalloonLayoutToTextRight(). If only text is specified, or only an
	 * image is specified, then it doesn't matter how the layout is specified.
	 */
	vtkSetMacro(BalloonLayout, int);
	vtkGetMacro(BalloonLayout, int);
	void SetBalloonLayoutToImageLeft() { this->SetBalloonLayout(ImageLeft); }
	void SetBalloonLayoutToImageRight() { this->SetBalloonLayout(ImageRight); }
	void SetBalloonLayoutToImageBottom() { this->SetBalloonLayout(ImageBottom); }
	void SetBalloonLayoutToImageTop() { this->SetBalloonLayout(ImageTop); }
	void SetBalloonLayoutToTextLeft() { this->SetBalloonLayout(ImageRight); }
	void SetBalloonLayoutToTextRight() { this->SetBalloonLayout(ImageLeft); }
	void SetBalloonLayoutToTextTop() { this->SetBalloonLayout(ImageBottom); }
	void SetBalloonLayoutToTextBottom() { this->SetBalloonLayout(ImageTop); }
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
	 * Set/Get the padding (in pixels) that is used between the text and the
	 * frame.
	 */
	vtkSetClampMacro(Padding, int, 0, 100);
	vtkGetMacro(Padding, int);
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
	enum _InteractionState { Outside = 0, OnText, OnImage, OnFrame };

	//@{
	/**
	 * 프래임을 보이게 할지 말지 설정 default = 0;
	 */
	void setFrameVisible(int );
	int getFrameVisible();
	//@}
protected:
	pxikVTKUIBalloonRepresentation();
	~pxikVTKUIBalloonRepresentation() override;

	// The balloon text and image
	char         *BalloonText;
	vtkImageData *BalloonImage;

	// The layout of the balloon
	int BalloonLayout;

	// Controlling placement
	int Padding;
	int Offset[2];
	int ImageSize[2];

	// Represent the text
	vtkTextMapper       *TextMapper;
	vtkActor2D          *TextActor;
	vtkTextProperty     *TextProperty;

	// Represent the image
	vtkTexture          *Texture;
	vtkPolyData         *TexturePolyData;
	vtkPoints           *TexturePoints;
	vtkPolyDataMapper2D *TextureMapper;
	vtkTexturedActor2D  *TextureActor;
	vtkProperty2D       *ImageProperty;

	// The frame
	vtkPoints           *FramePoints;
	vtkCellArray        *FramePolygon;
	vtkPolyData         *FramePolyData;
	vtkPolyDataMapper2D *FrameMapper;
	vtkActor2D          *FrameActor;
	vtkProperty2D       *FrameProperty;

	// Internal variable controlling rendering process
	int TextVisible;
	int ImageVisible;
	int m_frameVisible;

	// Helper methods
	void AdjustImageSize(double imageSize[2]);
	void ScaleImage(double imageSize[2], double scale);

	void createRectangleFrame();
	void createCircleFrame();

private:
	pxikVTKUIBalloonRepresentation(const pxikVTKUIBalloonRepresentation&) = delete;
	void operator=(const pxikVTKUIBalloonRepresentation&) = delete;
};

#endif

