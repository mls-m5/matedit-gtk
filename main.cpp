/*
 * main.cpp
 *
 *  Created on: 11 jan 2015
 *      Author: Mattias Larsson Sköld
 */

#include "mainwindow.h"
#include "completionwindow.h"
//#include <gtk/gtkwidget.h>
//#include <glib/gprintf.h>
#include <gdkmm-3.0/gdkmm.h>

using namespace std;
using namespace MatEdit;

int main(int argc, char **argv) {

	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv,
			"se.lasersköld.test");
	Gsv::init();

	MainWindow window;

	app->run(window);
}

