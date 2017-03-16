#!/usr/bin/env python

# example table.py

import pygtk
pygtk.require('2.0')
import gtk

class Table:
    # Our callback.
    # The data passed to this method is printed to stdout
    def callback(self, widget, data=None):
        print "%s was pressed" % data

    # This callback quits the program
    def delete_event(self, widget, event, data=None):
        gtk.main_quit()
        return False

    def __init__(self):
        # Create a new window
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)

        # Set the window title
        self.window.set_title("Table")

        # Set a handler for delete_event that immediately
        # exits GTK.
        self.window.connect("delete_event", self.delete_event)

        # Sets the border width of the window.
        self.window.set_border_width(20)

	num_lines = sum(1 for line in open('mp_result.log'))
        table = gtk.Table(num_lines+3, 3, True)
        self.window.add(table)

        button = gtk.Button("MP Test Result")
        button.connect("clicked", self.callback, "line 0")
        table.attach(button, 0, 3, 0, 1)
        button.show()

	fo = open("mp_result.log","r")
	for k in range(1,num_lines+1):
		line=fo.tell()
		fo.seek(line)
		str = fo.readline().split(" ")[0]        
        	button = gtk.Button(str)
        	button.connect("clicked", self.callback, str)
        	table.attach(button, 0, 1, k, k+1)
        	button.show()
        	fo.seek(line)
		str = fo.readline().split(" ")[2].strip()
        	button = gtk.Button(str)
        	button.connect("clicked", self.callback, str)
        	table.attach(button, 1, 3, k, k+1)
        	button.show()

	fo.close                                
        button = gtk.Button("Quit")
        button.connect("clicked", lambda w: gtk.main_quit())
        table.attach(button, 1, 2, num_lines+2, num_lines+3)
        button.show()

        table.show()
        self.window.show()

def main():
    gtk.main()
    return 0

if __name__ == "__main__":
    Table()
    main()


