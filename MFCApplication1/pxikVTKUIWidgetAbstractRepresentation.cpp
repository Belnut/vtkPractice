#include "stdafx.h"
#include "pxikVTKUIWidgetAbstractRepresentation.h"
#include "vtkBalloonRepresentation.h"
#include "vtkImageMapper.h"
#include "vtkActor2D.h"
#include "vtkProperty2D.h"

//----------------------------------------------------------------------
pxikVTKUIWidgetAbstractRepresentation::pxikVTKUIWidgetAbstractRepresentation()
{
	// Initially we are not visible
	this->Visibility = 1;

	// controlling Opacity
	this->m_opacity = 0.5;

	m_Geometry.X = -1;
	m_Geometry.Y = -1;
	m_Geometry.Width = -1;
	m_Geometry.Height = -1;
}

//----------------------------------------------------------------------
pxikVTKUIWidgetAbstractRepresentation::~pxikVTKUIWidgetAbstractRepresentation()
{
}


//----------------------------------------------------------------------
void pxikVTKUIWidgetAbstractRepresentation::StartWidgetInteraction(double e[2])
{
	this->StartEventPosition[0] = e[0];
	this->StartEventPosition[1] = e[1];
	this->VisibilityOn();
}

//----------------------------------------------------------------------
void pxikVTKUIWidgetAbstractRepresentation::EndWidgetInteraction(double e[2])
{
	this->VisibilityOff();
}

//----------------------------------------------------------------------
void pxikVTKUIWidgetAbstractRepresentation::BuildRepresentation()
{
}

//----------------------------------------------------------------------
void pxikVTKUIWidgetAbstractRepresentation::ReleaseGraphicsResources(vtkWindow * w)
{
}

//----------------------------------------------------------------------
int pxikVTKUIWidgetAbstractRepresentation::RenderOverlay(vtkViewport * v)
{
}

//----------------------------------------------------------------------
void pxikVTKUIWidgetAbstractRepresentation::setMargin(double value, int margin)
{
	int cmpMargin = 0;

	if (margin <= 0)
	{
		return;
	}

	m_margin = margin;
	for (cmpMargin = 1; cmpMargin&cmpMargin; cmpMargin *= 2)
	{
		switch (cmpMargin)
		{
		case marginTop:
			m_marginTopValue = value;
			break;
		case marginBtm:
			m_marginBtmValue = value;
			break;
		case marginLeft:
			m_marginLeftValue = value;
			break;
		case marginRight:
			m_marginRightValue = value;
			break;
		default:
			break;
		}
	}
}

//----------------------------------------------------------------------
int pxikVTKUIWidgetAbstractRepresentation::getMargin()
{
	return m_margin;
}

//----------------------------------------------------------------------
void pxikVTKUIWidgetAbstractRepresentation::removeAllMargin()
{
	m_margin = 0;
	m_marginTopValue = 0;
	m_marginBtmValue = 0;
	m_marginLeftValue = 0;
	m_marginRightValue = 0;

	//this->Modified();
}

//----------------------------------------------------------------------
void pxikVTKUIWidgetAbstractRepresentation::setPosition(double x, double y)
{

	if (x < 0 || y < 0)
	{
		return;
	}

	m_Geometry.X = x;
	m_Geometry.Y = y;

	this->Modified();
}

//----------------------------------------------------------------------
double * pxikVTKUIWidgetAbstractRepresentation::getPosition()
{
	double* position = new double[2];
	position[0] = m_Geometry.X;
	position[1] = m_Geometry.Y;

	return position;
}

//----------------------------------------------------------------------
void pxikVTKUIWidgetAbstractRepresentation::setSize(double width, double height)
{
	m_Geometry.Width	= width;
	m_Geometry.Height	= height;

	this->Modified();
}

//----------------------------------------------------------------------
int * pxikVTKUIWidgetAbstractRepresentation::getSize()
{
	int* size = new int[2];
	size[0] = m_Geometry.Width;
	size[1] = m_Geometry.Height;

	return size;
}

//----------------------------------------------------------------------
void pxikVTKUIWidgetAbstractRepresentation::AdjustSize(int * Size)
{
	Size[0] = m_Geometry.Width;
	Size[1] = m_Geometry.Height;

	int parentSize[2] = { 0,0 };
	int *tmp = nullptr;


	if (m_parent == nullptr)
	{
		//아님 연결되고 시작할때 Bulid되는 걸 확인함
		tmp = this->Renderer->GetSize();
		parentSize[0] = tmp[0];
		parentSize[1] = tmp[1];
	}
	else
	{
		tmp = m_parent->getSize();
		parentSize[0] = tmp[0];
		parentSize[1] = tmp[1];
		delete tmp;

		if (m_parent->getMargin() != 0)
			m_parent->AdjustSize(parentSize);
	}


	if (m_margin & marginLeft && m_margin & marginRight)
	{
		Size[0] = parentSize[0] - (m_marginLeftValue + m_marginRightValue);
	}

	if (m_margin & marginTop && m_margin & marginBtm)
	{
		Size[1] = parentSize[1] - (m_marginTopValue + m_marginBtmValue);
	}
}

//----------------------------------------------------------------------
void pxikVTKUIWidgetAbstractRepresentation::AdjustPosition(double * Position)
{
	Position[0] = m_Geometry.X;
	Position[1] = m_Geometry.Y;

	double parentPostion[2] = { 0,0 };
	double *tmp = nullptr;
	if (m_parent == nullptr)
	{
		//아님 연결되고 시작할때 Bulid되는 걸 확인함
		parentPostion[0] = 0;
		parentPostion[1] = 0;
	}
	else
	{
		
		tmp = m_parent->getPosition();
		parentPostion[0] = tmp[0];
		parentPostion[1] = tmp[1];
		delete tmp;

		if (m_parent->getMargin() != 0)
			m_parent->AdjustPosition(parentPostion);

	}

	int parentSize[2];
	if (m_parent == nullptr)
	{
		int* tmp = this->Renderer->GetSize();
		parentSize[0] = tmp[0];
		parentSize[1] = tmp[1];
	}
	else
		m_parent->AdjustSize(parentSize);


	if (m_margin & marginLeft)
		Position[0] = parentPostion[0] + m_marginLeftValue;
	else if(m_margin & marginRight)
		Position[0] = parentPostion[0] + (parentSize[0] - m_marginRightValue);

	if (m_margin & marginBtm)
		Position[1] = parentPostion[1] + (m_marginBtmValue);
	else if(m_margin & marginTop)
		Position[1] = parentPostion[1] + (parentSize[1] - m_marginTopValue);
}

void pxikVTKUIWidgetAbstractRepresentation::setOpacity(double opacity)
{
	m_opacity = opacity;
}

void pxikVTKUIWidgetAbstractRepresentation::PrintSelf(ostream & os, vtkIndent indent)
{
}

