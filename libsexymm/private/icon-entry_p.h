// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _LIBSEXYMM_ICON_ENTRY_P_H
#define _LIBSEXYMM_ICON_ENTRY_P_H
#include <gtkmm/private/entry_p.h>

#include <glibmm/class.h>

namespace Sexy
{

class IconEntry_Class : public Glib::Class
{
public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  typedef IconEntry CppObjectType;
  typedef SexyIconEntry BaseObjectType;
  typedef SexyIconEntryClass BaseClassType;
  typedef Gtk::Entry_Class CppClassParent;
  typedef GtkEntryClass BaseClassParent;

  friend class IconEntry;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  const Glib::Class& init();

  static void class_init_function(void* g_class, void* class_data);

  static Glib::ObjectBase* wrap_new(GObject*);

protected:

  //Callbacks (default signal handlers):
  //These will call the *_impl member methods, which will then call the existing default signal callbacks, if any.
  //You could prevent the original default signal handlers being called by overriding the *_impl method.
  static void icon_pressed_callback(SexyIconEntry* self, SexyIconEntryPosition icon_pos, int button);
  static void icon_released_callback(SexyIconEntry* self, SexyIconEntryPosition icon_pos, int button);

  //Callbacks (virtual functions):
};


} // namespace Sexy

#endif /* _LIBSEXYMM_ICON_ENTRY_P_H */

