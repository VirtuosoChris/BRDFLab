/*
	Copyright (c) 2009.
		Adria Fores
		Sumanta N. Pattanaik
		Carles Bosch
		Xavier Pueyo

	Contact: adriafores@gmail.com	

	This file is part of BRDFLab.

    BRDFLab is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    BRDFLab is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with BRDFLab.  If not, see <http://www.gnu.org/licenses/>.
*/

//
// --- Includes ---
//
#include "BRDFLab.h"

#include <QApplication>

// output to console includes.
#include <iostream>
#include <fcntl.h>
#include <io.h>


/*
Open a console in windows for debug purposes.

*/
void showWin32Console(void)
   {
       static const WORD MAX_CONSOLE_LINES = 500;
       int hConHandle;
       long lStdHandle;
       CONSOLE_SCREEN_BUFFER_INFO coninfo;
       FILE *fp;
       // allocate a console for this app
       AllocConsole();
       // set the screen buffer to be big enough to let us scroll text
       GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
       coninfo.dwSize.Y = MAX_CONSOLE_LINES;
       SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),
       coninfo.dwSize);
       // redirect unbuffered STDOUT to the console
       lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
       hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
       fp = _fdopen( hConHandle, "w" );
       *stdout = *fp;
       setvbuf( stdout, NULL, _IONBF, 0 );
       // redirect unbuffered STDIN to the console
       lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
       hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
       fp = _fdopen( hConHandle, "r" );
       *stdin = *fp;
       setvbuf( stdin, NULL, _IONBF, 0 );
       // redirect unbuffered STDERR to the console
       lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
       hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
       fp = _fdopen( hConHandle, "w" );
       *stderr = *fp;
       setvbuf( stderr, NULL, _IONBF, 0 );
       // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
       // point to console as well
       std::ios::sync_with_stdio();
   }

//
// --- Main Function ---
//
int main(int argc, char **argv) {

	//open console
	//showWin32Console();


	QApplication app(argc, argv);

	app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

	BRDFLab *window = new BRDFLab();
	window->resize(1000,750);	
	window->show();

	
	return app.exec();
}
