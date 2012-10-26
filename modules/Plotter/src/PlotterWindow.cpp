#include "IVALib/Plotter/PlotterWindow.hpp"


PlotterWindow::PlotterWindow(int argc,char **argv) :
  mx_(-1000),my_(-1000),n_Curves(0),
  m_box0(/*homogeneous*/false, /*spacing*/5), 
  m_box1(false, 5), m_box2(false, 5), m_box3(false, 5),
  m_button1("Dialog"), m_button2("Quit"), 
  m_sb(), 
  m_plot()
{

  button_[0]= button_[1]= button_[2]= false;
  // box2
#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 4)
  m_button2.signal_clicked().connect(sigc::mem_fun(*this, &Gtk::Widget::hide));
#else
  m_button2.signal_clicked().connect(SigC::slot(*this, &Gtk::Widget::hide));
#endif
  m_box2.pack_start(m_box3, Gtk::PACK_EXPAND_WIDGET, /*padding*/5);
  //m_box2.pack_start(m_button1, Gtk::PACK_SHRINK, 5);
  m_box2.pack_start(m_button2, Gtk::PACK_SHRINK, 5);

  // box1
  m_plot.set_size_request(500, 300);
  m_box1.pack_start(m_plot, Gtk::PACK_EXPAND_WIDGET, 5);
  m_box1.pack_start(m_box2, Gtk::PACK_SHRINK, 5);
    
  // box0
  m_box0.pack_start(m_box1, Gtk::PACK_EXPAND_WIDGET, 5);
  m_box0.pack_start(m_sb, Gtk::PACK_SHRINK, 5);
  
  set_border_width(0);
  add(m_box0);
  show_all();


#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 4)
  m_plot.signal_plot_mouse_press().
    connect(sigc::mem_fun(*this,&PlotterWindow::on_plot_mouse_press));
  m_plot.signal_plot_mouse_release().
    connect(sigc::mem_fun(*this,&PlotterWindow::on_plot_mouse_release));
  m_plot.signal_plot_mouse_move().
    connect(sigc::mem_fun(*this,&PlotterWindow::on_plot_mouse_move));
  m_button2.signal_clicked().connect(sigc::mem_fun(*this, &Gtk::Widget::hide));
#else
  m_plot.signal_plot_mouse_press().
    connect(SigC::slot(*this,&PlotterWindow::on_plot_mouse_press));
  m_plot.signal_plot_mouse_release().
    connect(SigC::slot(*this,&PlotterWindow::on_plot_mouse_release));
  m_plot.signal_plot_mouse_move().
    connect(SigC::slot(*this,&PlotterWindow::on_plot_mouse_move));
  m_button2.signal_clicked().connect(SigC::slot(*this, &Gtk::Widget::hide));
#endif

#if (GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION >= 4)
#define CHECKBOX_SCALE_HELPER(AXIS,CHECKBOX) \
  m_plot.scale(AXIS)->signal_enabled.connect(sigc::mem_fun(CHECKBOX,&Gtk::CheckButton::set_active));\
  CHECKBOX.signal_toggled().connect(sigc::bind(sigc::mem_fun(*this,&PlotTest::on_scale_toggled),AXIS,&CHECKBOX));\
  m_plot.scale(AXIS)->set_enabled(true)
#else
#define CHECKBOX_SCALE_HELPER(AXIS,CHECKBOX) \
  m_plot.scale(AXIS)->signal_enabled.connect(SigC::slot(CHECKBOX,&Gtk::CheckButton::set_active));\
  CHECKBOX.signal_toggled().connect(SigC::bind(SigC::slot(*this,&PlotTest::on_scale_toggled),AXIS,&CHECKBOX));\
  m_plot.scale(AXIS)->set_enabled(true)
#endif

  /* set some axes to linear and others to logarithmic scale */
  m_plot.scale(PlotMM::AXIS_BOTTOM)->set_range(-10,1000,false);
  m_plot.scale(PlotMM::AXIS_LEFT)->set_range(-10,1000,false);
  /* allow for autoscaling on all axes */
  m_plot.scale(PlotMM::AXIS_BOTTOM)->set_autoscale(true);
  m_plot.scale(PlotMM::AXIS_LEFT)->set_autoscale(true);
  /* set a plot title and some axis labels */
  m_plot.label(PlotMM::AXIS_BOTTOM)->set_text("bottom axis");
  m_plot.label(PlotMM::AXIS_LEFT)->set_text("left axis");
  m_plot.scale(PlotMM::AXIS_RIGHT)->set_enabled(false);
  m_plot.label(PlotMM::AXIS_RIGHT)->set_enabled(false);

}


void PlotterWindow::add_Curve(arma::vec const &dataX,arma::vec const &dataY, Glib::ustring const &color /*= "blue"*/, PlotMM::CurveStyleID const &cStyle /*= PlotMM::CURVE_LINES*/ ,PlotMM::SymbolStyleID const &s /*= PlotMM::SYMBOL_NONE*/){

  if(dataX.n_elem != dataY.n_elem){
    throw std::runtime_error("The size of dataX does not equal dataY");
    exit(0);
  }

  Glib::RefPtr<PlotMM::Symbol> p = Glib::RefPtr<PlotMM::Symbol>(new PlotMM::Symbol(s));

  this->Curves.push_back(Glib::RefPtr<PlotMM::Curve>(new PlotMM::Curve()));
  this->Curves[n_Curves]->set_data(dataX.memptr(),dataY.memptr(),dataY.n_elem);
  this->Curves[n_Curves]->paint()->set_pen_color(Gdk::Color(color));
  this->Curves[n_Curves]->set_curve_style(cStyle);
  this->Curves[n_Curves]->set_symbol(p);
  m_plot.add_curve(Curves[n_Curves]);
  n_Curves++;
}

void PlotterWindow::add_Curve(arma::vec const &dataY, Glib::ustring const &color /* = "blue"*/, PlotMM::CurveStyleID const &cStyle /* = PlotMM::CURVE_LINES*/,PlotMM::SymbolStyleID const &s /*= PlotMM::SYMBOL_NONE*/){


  Glib::RefPtr<PlotMM::Symbol> p = Glib::RefPtr<PlotMM::Symbol>(new PlotMM::Symbol(s));
  arma::vec dataX = arma::linspace(0,dataY.n_elem-1,dataY.n_elem);

  this->Curves.push_back(Glib::RefPtr<PlotMM::Curve>(new PlotMM::Curve()));
  this->Curves[n_Curves]->set_data(dataX.memptr(),dataY.memptr(),dataY.n_elem);
  this->Curves[n_Curves]->paint()->set_pen_color(Gdk::Color(color));
  this->Curves[n_Curves]->set_curve_style(cStyle);
  this->Curves[n_Curves]->set_symbol(p);
  m_plot.add_curve(Curves[n_Curves]);
  n_Curves++;

}

inline void PlotterWindow::set_Xrange(double const &start, double const &end,bool lg /*= false*/){
    m_plot.scale(PlotMM::AXIS_BOTTOM)->set_range(start,end,lg);
}

inline void PlotterWindow::set_Yrange(double const &start, double const &end,bool lg /*= false*/){
    m_plot.scale(PlotMM::AXIS_LEFT)->set_range(start,end,lg);
}

void PlotterWindow::run(int argc, char **argv){
  Gtk::Main::run(*this);
}
