
#ifndef DOCUMENTLIST_H
#define DOCUMENTLIST_H

#include <gtkmm.h>
#include <sstream>

#include "BibData.h"

class Document {
	private:
	Glib::ustring filename_;
	Glib::ustring displayname_;
	std::vector<int> tagUids_;
	Glib::RefPtr<Gdk::Pixbuf> thumbnail_;
	
	void setupThumbnail ();
	static Glib::RefPtr<Gdk::Pixbuf> getThemeIcon(Glib::ustring const &iconname);

	BibData bib_;

	public:
	Document ();
	Document (Glib::ustring const &filename);
	Document (
		Glib::ustring const &filename,
		Glib::ustring const &displayname,
		std::vector<int> const &tagUids);
	Glib::ustring& getDisplayName();
	Glib::ustring& getFileName();
	// Unused?
	//void setFileName (Glib::ustring &filename);
	void setDisplayName (Glib::ustring const &displayname);
	Glib::RefPtr<Gdk::Pixbuf> getThumbnail () {return thumbnail_;}
	std::vector<int>& getTags ();
	void setTag (int uid);
	void clearTag (int uid);
	void clearTags ();
	bool hasTag (int uid);
	
	void writeBibtex (std::ostringstream& out);
	void readPDF ();
	
	BibData& getBibData () {return bib_;}
	void setBibData (BibData& bib){bib_ = bib;}

	static Glib::RefPtr<Gdk::Pixbuf> defaultthumb_;
	
	Glib::ustring displayNameFromFileName ();
};

class DocumentList {
	private:
	std::vector<Document> docs_;
	
	public:
	std::vector<Document>& getDocs ();
	Document* getDoc (Glib::ustring const &name);
	Document* newDocWithFile (Glib::ustring const &filename);
	Document* newDocWithName (Glib::ustring const &displayname);
	Document* newDocWithDoi (Glib::ustring const &doi);
	Document* newDocUnnamed ();
	Glib::ustring uniqueDisplayName (Glib::ustring const &basename);
	void removeDoc (Glib::ustring const &displayname);
	void loadDoc (
		Glib::ustring const &filename,
		Glib::ustring const &displayname,
		std::vector<int> const &taguids);
	void print ();
	bool test ();
	void clearTag (int uid);
	void writeXML (std::ostringstream& out);
	void clear () {docs_.clear ();}
	void writeBibtex (std::ostringstream &out);
};

#endif
