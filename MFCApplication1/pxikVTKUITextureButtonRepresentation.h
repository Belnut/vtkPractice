#pragma once
#ifndef pxikVTKUITextureButtonRepresentation_h
#define pxikVTKUITextureButtonRepresentation_h

#include "vtkInteractionWidgetsModule.h" // For export macro
#include "pxikVTKUIButtonRepresentation.h"

class vtkProperty2D;
class vtkImageData;
class vtkTextureArray; //PIMPLd
class vtkProperty2D;
class vtkAlgorithmOutput;
class vtkBalloonRepresentation;
class vtkCoordinate;

class pxikVTKUITextureButtonRepresentation : public pxikVTKUIButtonRepresentation
{
public:
	/**
  * Instantiate the class.
  */
	static pxikVTKUITextureButtonRepresentation *New();

	//@{
	/**
	 * Standard methods for the class.
	 */
	vtkTypeMacro(pxikVTKUITextureButtonRepresentation, pxikVTKUIButtonRepresentation);
	void PrintSelf(ostream& os, vtkIndent indent) override;
	//@}

	//@{
	/**
	 * Specify the property to use when the button is to appear "normal"
	 * i.e., the mouse pointer is not hovering or selecting the button.
	 */
	virtual void SetProperty(vtkProperty2D *p);
	vtkGetObjectMacro(Property, vtkProperty2D);
	//@}

	//@{
	/**
	 * Specify the property to use when the hovering over the button.
	 */
	virtual void SetHoveringProperty(vtkProperty2D *p);
	vtkGetObjectMacro(HoveringProperty, vtkProperty2D);
	//@}

	//@{
	/**
	 * Specify the property to use when selecting the button.
	 */
	virtual void SetSelectingProperty(vtkProperty2D *p);
	vtkGetObjectMacro(SelectingProperty, vtkProperty2D);
	//@}

	//@{
	/**
	 * Add the ith texture corresponding to the ith button state.
	 * The parameter i should be 0<=i<NumberOfStates.
	 */
	void SetButtonTexture(int i, vtkImageData *image);
	vtkImageData *GetButtonTexture(int i);
	//@}

	/**
	 * Grab the underlying vtkBalloonRepresentation used to position and display
	 * the button texture.
	 */
	vtkBalloonRepresentation *GetBalloon() { return this->Balloon; }

	//@{
	/**
	 * Provide the necessary methods to satisfy the vtkWidgetRepresentation API.
	 */
	int ComputeInteractionState(int X, int Y, int modify = 0) override;
	void BuildRepresentation() override;
	void Highlight(int state) override;
	//@}

	/**
	 * Conventional PlaceWidget() method to satisfy the vtkWidgetRepresentation API.
	 * In this version, bounds[6] specifies a rectangle in *display* coordinates
	 * in which to place the button. The values for bounds[4] and bounds[5] can be
	 * set to zero. Note that PlaceWidget() is typically called at the end of configuring
	 * the button representation.
	 */
	void PlaceWidget(double bounds[6]) override;

	/**
	 * This alternative PlaceWidget() method can be used to anchor the button
	 * to a 3D point. In this case, the button representation will move around
	 * the screen as the camera moves around the world space. The first
	 * parameter anchor[3] is the world point anchor position (attached to the
	 * lower left portion of the button by default); and the size[2] parameter
	 * defines a x-y box in display coordinates in which the button will
	 * fit. Note that you can grab the vtkBalloonRepresentation and set an
	 * offset value if the anchor point is to be elsewhere on the button.
	 */
	virtual void PlaceWidget(double anchor[3], int size[2]);

	//@{
	/**
	 * Provide the necessary methods to satisfy the rendering API.
	 */
	void ShallowCopy(vtkProp *prop) override;
	double *GetBounds() override;
	void GetActors(vtkPropCollection *pc) override;
	void ReleaseGraphicsResources(vtkWindow*) override;
	int RenderOverlay(vtkViewport*) override;
	int HasTranslucentPolygonalGeometry() override;
	//@}

protected:
	pxikVTKUITextureButtonRepresentation();
	~pxikVTKUITextureButtonRepresentation() override;

	// Representing the button
	vtkBalloonRepresentation *Balloon;

	// Properties of the button
	vtkProperty2D *Property;
	vtkProperty2D *HoveringProperty;
	vtkProperty2D *SelectingProperty;
	void CreateDefaultProperties();

	// Keep track of the images (textures) associated with the N
	// states of the button.
	vtkTextureArray *TextureArray;

	// Tracking world position
	vtkCoordinate *Anchor;

private:
	pxikVTKUITextureButtonRepresentation(const pxikVTKUITextureButtonRepresentation&) = delete;
	void operator=(const pxikVTKUITextureButtonRepresentation&) = delete;


};

#endif