#include "stdafx.h"
#include "pxikVTKUITabPanelRepresentation.h"
#include "pxikVTKUIWidgetAbstractRepresentation.h"

#include "vtkImageMapper.h"
#include "vtkTexturedActor2D.h"


//----------------------------------------------------------------------------------
pxikVTKUITabPanelRepresentation::pxikVTKUITabPanelRepresentation()
{


}

//----------------------------------------------------------------------------------
pxikVTKUITabPanelRepresentation::~pxikVTKUITabPanelRepresentation()
{
}

//----------------------------------------------------------------------------------
void pxikVTKUITabPanelRepresentation::PrintSelf(ostream & os, vtkIndent indent)
{
}

//----------------------------------------------------------------------------------
void pxikVTKUITabPanelRepresentation::SetFrameProperty(vtkProperty2D * p)
{
}

//----------------------------------------------------------------------------------
void pxikVTKUITabPanelRepresentation::StartWidgetInteraction(double e[2])
{
}

//----------------------------------------------------------------------------------
void pxikVTKUITabPanelRepresentation::EndWidgetInteraction(double e[2])
{
}

//----------------------------------------------------------------------------------
void pxikVTKUITabPanelRepresentation::BuildRepresentation()
{
}

//----------------------------------------------------------------------------------
int pxikVTKUITabPanelRepresentation::ComputeInteractionState(int X, int Y, int modify)
{
	return 0;
}

//----------------------------------------------------------------------------------
void pxikVTKUITabPanelRepresentation::ReleaseGraphicsResources(vtkWindow * w)
{
}

//----------------------------------------------------------------------------------
int pxikVTKUITabPanelRepresentation::RenderOverlay(vtkViewport * viewport)
{
	return 0;
}

//----------------------------------------------------------------------------------
void pxikVTKUITabPanelRepresentation::setMargin(int value, int margin)
{
}

//----------------------------------------------------------------------------------
int pxikVTKUITabPanelRepresentation::getMargin(int margin)
{
	return 0;
}

//----------------------------------------------------------------------------------
void pxikVTKUITabPanelRepresentation::removeAllMargin()
{
}

//----------------------------------------------------------------------------------
int * pxikVTKUITabPanelRepresentation::getPosition()
{
	return nullptr;
}

//----------------------------------------------------------------------------------
void pxikVTKUITabPanelRepresentation::setSize(int width, int height)
{
}

//----------------------------------------------------------------------------------
int * pxikVTKUITabPanelRepresentation::getSize()
{
	return nullptr;
}

//----------------------------------------------------------------------------------
void pxikVTKUITabPanelRepresentation::AdjustFrameSize(double * frameSize)
{
}

//----------------------------------------------------------------------------------
void pxikVTKUITabPanelRepresentation::AdjustFramePosition(double * framePosition)
{
}

//----------------------------------------------------------------------------------
int * pxikVTKUITabPanelRepresentation::getUIPosition()
{
	return nullptr;
}

//----------------------------------------------------------------------------------
int * pxikVTKUITabPanelRepresentation::getUISize()
{
	return nullptr;
}

//----------------------------------------------------------------------------------
void pxikVTKUITabPanelRepresentation::updateFrameColor(double red, double green, double blue)
{
}

//----------------------------------------------------------------------------------
void pxikVTKUITabPanelRepresentation::ScaleImage(double imageSize[2], double scale)
{
}