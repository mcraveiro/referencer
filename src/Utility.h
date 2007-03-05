
/*
 * Referencer is released under the GNU General Public License v2
 * See the COPYING file for licensing details.
 *
 * Copyright 2007 John Spray
 * (Exceptions listed in README)
 *
 */




#ifndef UTILITY_H
#define UTILITY_H

#include <utility>
#include <vector>
#include <sstream>

#include <gtkmm.h>
#include <libglademm.h>
#include <libgnomevfsmm.h>

namespace Utility {
	typedef std::pair <Glib::ustring, Glib::ustring> StringPair;

	bool DOIURLValid (
		Glib::ustring const &url);

	StringPair twoWaySplit (
		Glib::ustring const &str,
		Glib::ustring const &divider);

	Glib::ustring strip (
		Glib::ustring const &victim,
		Glib::ustring const &unwanted);

	Glib::ustring ensureExtension (
		Glib::ustring const &filename,
		Glib::ustring const &extension);

	Glib::RefPtr<Gnome::Glade::Xml> openGlade (
		Glib::ustring const &filename);

	Glib::ustring findDataFile (
		Glib::ustring const &filename);

	bool fileExists (
		Glib::ustring const &filename);

	void exceptionDialog (
		Glib::Exception const *ex, Glib::ustring const &context);

	std::vector<Glib::ustring> recurseFolder (
		Glib::ustring const &rootfoldername);

	void moveToTrash (
		Glib::ustring const &uri);
		
	void deleteFile (
		Glib::ustring const &target_uri_str);

	void writeBibKey (
		std::ostringstream &out,
		Glib::ustring key,
		Glib::ustring const & value,
		bool usebraces);

	std::string escapeBibtexAccents (
		Glib::ustring target);

	Glib::ustring firstCap (
		Glib::ustring original);

	Glib::ustring relPath (
		Glib::ustring parent,
		Glib::ustring child);

	Glib::RefPtr<Gdk::Pixbuf> getThemeIcon(Glib::ustring const &iconname);

	int wvConvertUnicodeToLaTeX(
		gunichar char16,
		Glib::ustring &out);

	Glib::RefPtr<Gdk::Pixbuf> eelEmbedImageInFrame (
		Glib::RefPtr<Gdk::Pixbuf> source,
		Glib::RefPtr<Gdk::Pixbuf> frame,
		int left, int top, int right, int bottom);
}

#endif
