
#ifndef LIBRARYPARSER_H
#define LIBRARYPARSER_H

#include <gtkmm.h>
#include "DocumentList.h"
#include "TagList.h"

class LibraryParser : public Glib::Markup::Parser {

	DocumentList &doclist_;
	TagList &taglist_;
	
	bool inTag_;
	bool inUid_;
	bool inTagName_;

	Glib::ustring newTagUid_;
	Glib::ustring newTagName_;
	
	bool inDoc_;
	bool inDisplayName_;
	bool inFileName_;
	bool inTagged_;

	Glib::ustring newDocDisplayName_;
	Glib::ustring newDocFileName_;
	Glib::ustring newDocTag_;
	std::vector<int> newDocTags_;

	public:
	LibraryParser (TagList &taglist, DocumentList &doclist)
		: doclist_(doclist), taglist_(taglist)
	{
	}

	private:
	virtual void on_start_element (
		Glib::Markup::ParseContext& context,
		const Glib::ustring& element_name,
		const Glib::Markup::Parser::AttributeMap& attributes)
	{
		//std::cerr << "Started element " << element_name << "\n";
		if (element_name == "tag") {
			inTag_ = true;
			newTagUid_ = "";
			newTagName_ = "";
		} else if (inTag_ && element_name == "uid") {
			inUid_ = true;
		} else if (inTag_ && element_name == "name") {
			inTagName_ = true;
		}
		else if (element_name == "doc") {
			inDoc_ = true;
			newDocFileName_ = "";
			newDocDisplayName_ = "";
			newDocTag_ = "";
			newDocTags_.clear();
		} else if (inDoc_ && element_name == "filename") {
			inFileName_ = true;
		} else if (inDoc_ && element_name == "displayname") {
			inDisplayName_ = true;
		} else if (inDoc_ && element_name == "tagged") {
			inTagged_ = true;
		}
	}
	
	virtual void	on_end_element (
		Glib::Markup::ParseContext& context,
		const Glib::ustring& element_name)
	{
		//std::cerr << "Ended element " << element_name << "\n";
		if (element_name == "tag") {
			inTag_ = false;
			// Hardcoded for ATTACH tags only
			taglist_.loadTag (newTagName_, Tag::ATTACH, atoi(newTagUid_.c_str()));
		} else if (inTag_ && element_name == "uid") {
			inUid_ = false;
		} else if (inTag_ && element_name == "name") {
			inTagName_ = false;
		}
		else if (element_name == "doc") {
			inDoc_ = false;
			doclist_.loadDoc (newDocFileName_, newDocDisplayName_, newDocTags_);
		} else if (inDoc_ && element_name == "filename") {
			inFileName_ = false;
		} else if (inDoc_ && element_name == "displayname") {
			inDisplayName_ = false;
		} else if (inDoc_ && element_name == "tagged") {
			inTagged_ = false;
			newDocTags_.push_back(atoi(newDocTag_.c_str()));
			newDocTag_ = "";
		}
	}
	
 	// Called on error, including one thrown by an overridden virtual method. 
	virtual void on_error (
		Glib::Markup::ParseContext& context,
		const Glib::MarkupError& error)
	{
		std::cerr << "LibraryParser: Parse Error!\n";
	}	
 
	virtual void on_text (
		Glib::Markup::ParseContext& context,
		const Glib::ustring& text)
	{
		//std::cerr << "\"" << text << "\"\n";
		if (inUid_)
			newTagUid_ += text;
		else if (inTagName_)
			newTagName_ += text;
	
		else if (inFileName_)
			newDocFileName_ += text;
		else if (inDisplayName_)
			newDocDisplayName_ += text;
		else if (inTagged_)
			newDocTag_ += text;
	}
 	
 	/*
 	//Called for strings that should be re-saved verbatim in this same
 	//position, but are not otherwise interpretable.
	virtual void on_passthrough (
		ParseContext& context,
		const Glib::ustring& passthrough_text)
	{
	}
	*/
};

#endif
