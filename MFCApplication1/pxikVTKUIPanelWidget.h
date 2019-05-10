#pragma once
#ifndef pxikVTKUIPanelWidget_h
#define pxikVTKUIPanelWidget_h


#include "pxikVTKUIAbstractWidget.h"
class pxikVTKUIPanelRepresentation;

class pxikVTKUIPanelWidget :
	public pxikVTKUIAbstractWidget
{
public:
	/*!
	 * Instantiate the object.
	 */
	static pxikVTKUIPanelWidget * New();
	vtkTypeMacro(pxikVTKUIPanelWidget, pxikVTKUIAbstractWidget);
	void PrintSelf(ostream& os, vtkIndent indent) override;

	virtual void CreateDefaultRepresentation() override;

	void SetRepresentation(pxikVTKUIPanelRepresentation *r)
	{
		this->Superclass::SetWidgetRepresentation(reinterpret_cast<pxikVTKUIWidgetAbstractRepresentation*>(r));
	}

	vtkImageData *m_backgroundImage;

	struct color_t {
		double red, green, blue;

		color_t()
		{
			red = 0;
			green = 0;
			blue = 0;
		}

		void setcolor(double _red, double _green, double _blue)
		{
			red = _red;
			green = _green;
			blue = _blue;
		}

	};


	struct color_t m_backgroundColor;
	void setBackgroundColor(color_t);
	void setBackgroundColor(double _red, double _green, double _blue);
	void getBackgroundColor(double*, double*, double*);


	void SetEnabled(int) override;

protected:
	pxikVTKUIPanelWidget();
	~pxikVTKUIPanelWidget() override;

private:
	pxikVTKUIPanelWidget(const pxikVTKUIPanelWidget&) = delete;
	void operator=(const pxikVTKUIPanelWidget&) = delete;
};
#endif

