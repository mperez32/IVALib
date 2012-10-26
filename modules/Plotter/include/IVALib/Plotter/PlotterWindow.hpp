#ifndef PLOTTERWINDOW_H_
#define PLOTTERWINDOW_H_

/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * PlotterWindow class 
 * Copyright (C) 2012   Matias Perez
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the LGPL
 *****************************************************************************/
#include<armadillo_bits/config.hpp>
#include<armadillo>
#include <plotmm/plot.h>
#include <plotmm/scalediv.h>
#include <plotmm/curve.h>
#include <plotmm/errorcurve.h>
#include <plotmm/symbol.h>
#include <plotmm/paint.h>
#include <plotmm/rectangle.h>
#include <stdexcept> 
#include <gtkmm/checkbutton.h>
#include <gtkmm/button.h>
#include <gtkmm/box.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/label.h>

#include <gtkmm/window.h>
#include <gtkmm/main.h>

#include <iostream>

class PlotterWindow : public Gtk::Window
{
public:
    PlotterWindow(int argc = 1, char **argv = NULL);
    virtual ~PlotterWindow(){};
    
    void on_scale_toggled(PlotMM::PlotAxisID id,const Gtk::CheckButton *chk) 
	{
	    m_plot.scale(id)->set_enabled(chk->get_active());
	    m_plot.label(id)->set_enabled(chk->get_active());
	}
    
    void on_plot_mouse_press(int x,int y, GdkEventButton *ev)
	{
	    if (ev->button>0 && ev->button<4) button_[ev->button-1]= true;
	    print_coords(mx_=x,my_=y);
	    
    
	    if ((ev->button==1)) {  // zoom
		m_plot.scale(PlotMM::AXIS_BOTTOM)->set_autoscale(false);
		m_plot.scale(PlotMM::AXIS_LEFT)->set_autoscale(false);
		zoomRect_.set_rect(x,y,0,0);
		m_plot.set_selection(zoomRect_);
		m_plot.enable_selection();
	    } else if ((ev->button==3)) {  // unzoom
		m_plot.scale(PlotMM::AXIS_BOTTOM)->set_autoscale(true);
		m_plot.scale(PlotMM::AXIS_LEFT)->set_autoscale(true);
		m_plot.replot();
	    }
	}
    
    void on_plot_mouse_release(int x,int y, GdkEventButton *ev)
	{
	    if (ev->button>0 && ev->button<4) button_[ev->button-1]= false;
	    print_coords(mx_=x,my_=y);
	    
	    if ((ev->button==1)) {
		double x0,y0,x1,y1;
		int ix0,iy0,ix1,iy1;
		zoomRect_= m_plot.get_selection();
		ix0= zoomRect_.get_x_min(); 
		ix1= zoomRect_.get_x_max();
		iy0= zoomRect_.get_y_min(); 
		iy1= zoomRect_.get_y_max();
		x0= m_plot.scale(PlotMM::AXIS_BOTTOM)->scale_map().inv_transform(ix0);
		x1= m_plot.scale(PlotMM::AXIS_BOTTOM)->scale_map().inv_transform(ix1);
		y0= m_plot.scale(PlotMM::AXIS_LEFT)->scale_map().inv_transform(iy1);
		y1= m_plot.scale(PlotMM::AXIS_LEFT)->scale_map().inv_transform(iy0);
		m_plot.disable_selection();
		if (zoomRect_.get_width()==0 && zoomRect_.get_height()==0)
		    return;
		m_plot.scale(PlotMM::AXIS_BOTTOM)->set_range(x0,x1);
		m_plot.scale(PlotMM::AXIS_LEFT)->set_range(y0,y1);
		m_plot.replot();
	    }
	}

    void on_plot_mouse_move(int x,int y, GdkEventMotion *ev)
	{
	    print_coords(mx_=x,my_=y);
	    zoomRect_.set_destination(x,y);
	    m_plot.set_selection(zoomRect_);
	}
    
    void print_coords(int x, int y)
	{
	    char tmp[1000];
	    sprintf(tmp,"(%d,%d): b:%8g; t:%8g; l:%8g; r:%8g; %c%c%c",x,y,
		    m_plot.scale(PlotMM::AXIS_BOTTOM)->scale_map().inv_transform(x),
		    m_plot.scale(PlotMM::AXIS_TOP)->scale_map().inv_transform(x),
		    m_plot.scale(PlotMM::AXIS_LEFT)->scale_map().inv_transform(y),
		    m_plot.scale(PlotMM::AXIS_RIGHT)->scale_map().inv_transform(y),
		    button_[0]?'*':'O',button_[1]?'*':'O',button_[2]?'*':'O');
	    m_sb.pop();
	    if (x>-1000&&y>-1000)
		m_sb.push(tmp);
	}

    void add_Curve(arma::vec const &dataX,arma::vec const &dataY, Glib::ustring const &color = "blue", PlotMM::CurveStyleID const &cStyle = PlotMM::CURVE_LINES ,PlotMM::SymbolStyleID const &s = PlotMM::SYMBOL_NONE);
    void add_Curve(arma::vec const &dataY, Glib::ustring const &color = "blue", PlotMM::CurveStyleID const &cStyle = PlotMM::CURVE_LINES ,PlotMM::SymbolStyleID const &s = PlotMM::SYMBOL_NONE);
    inline void set_Xlabel(Glib::ustring const &xlabel){
	m_plot.label(PlotMM::AXIS_BOTTOM)->set_text(xlabel);
	m_plot.label(PlotMM::AXIS_BOTTOM)->set_enabled(true);
    };
    inline void set_Ylabel(Glib::ustring const &ylabel){
	m_plot.label(PlotMM::AXIS_LEFT)->set_text(ylabel);
	m_plot.label(PlotMM::AXIS_LEFT)->set_enabled(true);  
    };
    void set_Title(Glib::ustring const &title){
	m_plot.title()->set_text(title);
	m_plot.title()->set_enabled(true);  
    };
    inline std::vector<Glib::RefPtr<PlotMM::Curve> >& get_Curves(){ return Curves;};
    inline void set_Xrange(double const &start, double const &end, bool const lg = false);
    inline void set_Yrange(double const &start, double const &end, bool const lg = false);
    inline void replot(){  m_plot.replot();};
    inline void reset(){
	Curves.clear();
	m_plot.replot();
    }
    inline void set_CurveData( arma::vec Y, int curve = 0){

	arma::vec X = arma::linspace<arma::vec>(0,Y.n_elem - 1,Y.n_elem);
	Curves[curve]->set_data(X.memptr(),Y.memptr(),Y.n_elem);
	m_plot.replot();
    }

    inline void set_CurveData(arma::vec X, arma::vec Y, int curve = 0){
	if(X.n_elem != Y.n_elem){
	      throw std::runtime_error("The size of dataX does not equal dataY");
	      exit(0);
	}

	Curves[curve]->set_data(X.memptr(),Y.memptr(),Y.n_elem);
	m_plot.replot();
    }

    void run(int argc = 0, char **argv = NULL);
    
protected:
    bool button_[3];
    int mx_,my_;
    PlotMM::Rectangle zoomRect_;
    // Child widgets
    Gtk::Label l_fps;
    Gtk::VBox m_box0;
    Gtk::HBox m_box1;
    Gtk::VBox m_box2;
    Gtk::VBox m_box3; //empty box

    Gtk::Button m_button1, m_button2;
    Gtk::Statusbar m_sb;

    std::vector<Glib::RefPtr<PlotMM::Curve> > Curves;
    PlotMM::Plot m_plot;
    int loop;
    int cs;
    int n_Curves;
};

#endif 
