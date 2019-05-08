#pragma once
#ifndef pxikVTKUIWidgetAbstractRepresentation_h
#define pxikVTKUIWidgetAbstractRepresentation_h
#include "vtkWidgetRepresentation.h"

class  pxikVTKUIWidgetAbstractRepresentation : public vtkWidgetRepresentation
{
public:

	//@{
	/**
	 * Standard VTK methods.
	 */
	vtkTypeMacro(pxikVTKUIWidgetAbstractRepresentation, vtkWidgetRepresentation);
	void PrintSelf(ostream& os, vtkIndent indent) override;
	//@}

	//@{
	/**
	 * These are methods that satisfy vtkWidgetRepresentation's API.
	 */
	void StartWidgetInteraction(double e[2]) override;
	void EndWidgetInteraction(double e[2]) override;
	void BuildRepresentation() override;
	int ComputeInteractionState(int X, int Y, int modify = 0) =0;
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

	/**
	 * 4 margin exist
	 */
	enum : int
	{
		marginTop = 1,
		marginBtm = 2,
		marginLeft = 4,
		marginRight = 8
	};

	//@{
	//please use single margin (2^n, 0 <= n <= 3)
	void setMargin(double value, int margin);
	int getMargin();
	//@}

	//remove all Margin
	//Margin state and value wiil be 0
	void removeAllMargin();


	struct geometryInfo_t
	{
		double X, Y;
		int Width, Height;
	};

	
	//{@
	/**
	* UI의 실제 그려지는 위치,
	* 해당 값을 사용하여 자식 UI의 location을 결정할 것
	*/

	void setPosition(double x, double y);
	double* getPosition();


	void setSize(double width, double height);
	int* getSize();
	//@}

	//please Call, When UI Layout geometry or marign is Changed
	void AdjustSize(int* Size);
	void AdjustPosition(double * Position);

	//Opacity 설정
	virtual void setOpacity(double opacity);

	

protected:
	pxikVTKUIWidgetAbstractRepresentation();
	~pxikVTKUIWidgetAbstractRepresentation() override;
	
	//Parent Represent
	pxikVTKUIWidgetAbstractRepresentation *m_parent = nullptr;

	//Geometry info
	geometryInfo_t m_Geometry;
	
	int m_margin = 0;
	double m_marginTopValue = 0;
	double m_marginBtmValue = 0;
	double m_marginLeftValue = 0;
	double m_marginRightValue = 0;

	//Prop Opacity
	double m_opacity;

	//Action
	int m_state;
	
private:
	pxikVTKUIWidgetAbstractRepresentation(const pxikVTKUIWidgetAbstractRepresentation&) = delete;
	void operator=(const pxikVTKUIWidgetAbstractRepresentation&) = delete;
};

#endif