#ifndef GENERICDEFINES_H
#define GENERICDEFINES_H

#ifdef SANANGELES
#define IMS(ios, mac, sanangeles) sanangeles
#define FREETYPE
#define APL_(a)
#define AMZ_(a) a
#define DRD_(a) a
#define _QT(a)
#include <android/log.h>
#endif

#ifdef LINUX
#define AMZ_(a)
#define DRD_(a)
#define LNX_(a) a
#define APL_(a)
#define LINUX_(a) a
#else
#define LINUX_(a)
#endif

#ifdef _QT_
#define _QT(a) a
#define QT_(a) a
#define IMS(ios, mac, sanangeles)
#define EGL
#endif

#ifdef IOS
#define IMS(ios, mac, sanangeles) ios
#define AMZ_(a)
#define DRD_(a)
#endif

#ifdef MAC
#define IMS(ios, mac, sanangeles) mac
#define AMZ_(a)
#define DRD_(a)
#define ITUNES
#endif

#define IOS_(a) IMS(a, , )
#define MAC_(a) IMS(, a, )
#define SAN_(a) IMS(, , a)

#ifdef APPLE // Apple does not permit FreeType rendering just because we don't have an international Font
#define APPLERENDERING
#endif

#ifndef APPLERENDERING
#define FREETYPE
#define GLTEXTURE
#endif

#ifndef ArchitectureSpace
#define ArchitectureSpace DefaultArchitecture
#endif

#ifdef DeBuG
#define USE_SAMPLE_RECEIPT
#define DEBUG__(a) a
#else
#define DEBUG__(a)
#endif

#ifdef FREETYPE
#define FREETYPE_(a) a
#define NOTFREETYPE_(a)
#else
#define NOTFREETYPE
#define FREETYPE_(a)
#define NOTFREETYPE_(a) a
#endif
#define FREETYPE_NOT(freetype, notfreetype) FREETYPE_(freetype) NOTFREETYPE_(notfreetype)

#ifdef GLTEXTURE
#define GLTEXTURE_(a) a
#define NOTGLTEXTURE_(a)
#else
#define GLTEXTURE_(a)
#define NOTGLTEXTURE_(a) a
#endif
#define GLTEXTURE_NOT(gltexture, notgltexture) GLTEXTURE_(gltexture) NOTGLTEXTURE_(notgltexture)

#ifdef APPLE
#define _QT(a)
#define APL_(a) a
#else
#ifdef _QT_
#define assert(a)
#endif
#endif

#define ensure(a) { if (!(a)) DBGX("[Premise %s is false!]", #a); }
#define ensureHard(a) { if (!(a)) { DBGX("[Premise %s is false!]", #a); throw std::exception(); } }

#define IM(ios, mac) IOS_(ios) MAC_(mac)

#ifdef APPLE
#define Rect IM(CGRect, NSRect)
#define MakeRect IM(CGRectMake, NSMakeRect)
#define TextAlignment IM(UITextAlignment, NSTextAlignment)
#define TextAlignmentCenter IM(UITextAlignmentCenter, NSCenterTextAlignment)
#define Color IM(UIColor, NSColor)
#define Window IM(UIWindow, NSWindow)
#define View IM(UIView, NSView)
#define ViewController IM(UIViewController, NSViewController)
#define ImageOrientation IM(UIImageOrientation, bool)
#define ApplicationDelegate IM(UIApplicationDelegate, NSApplicationDelegate)
//#define Application IM(UIApplication, NSApplication)
#define AppleFont IM(UIFont, NSFont)
#define AlertView IM(UIAlertView, NSAlert)
#define ImageView IM(UIImageView, NSImageView)
#define TableViewCell IM(UITableViewCell, NSTableViewCell)
#define TextField IM(UITextField, NSTextField)
#define Switch IM(UISwitch, NSSwitch)
#define ViewController IM(UIViewController, NSViewController)
#define WindowDelegate IM(UIWindowDelegate, NSWindowDelegate)
#define GLContext IM(EAGLContext, NSOpenGLContext)
//#define Application IM(UIApplication, NSApplication)
#define NavigationController IM(UINavigationController, NSNavigationController)
#define Image IM(UIImage, NSImage)
#endif

#ifdef _QT_
#ifdef DeBuG
#include <QDebug>
#endif
#endif

#define IM(ios, mac) IOS_(ios) MAC_(mac)
#define IMQ(ios, mac, qt) IOS_(ios) MAC_(mac) _QT(qt)
#define IMQS(ios, mac, qt, sanangeles) IOS_(ios) MAC_(mac) _QT(qt) SAN_(sanangeles)
#define QO(qt, others) IMQS(others, others, qt, others)
#define Font FREETYPE_NOT(FreeTypeFontWrapper, IMQS(UIFont, NSFont, QFont, FreeTypeFontWrapper))
#define SizeType FREETYPE_NOT(SANSize, IMQS(CGSize, CGSize, SANSize, SANSize))
//#define MMM IMQS(;, DenyDebuggerAttachment, ;, ;)
#define GLF(a) IMQS((a), (a), (a), (GLFixed) (65536*(a)))

#define bAPL IMQS(true, true, false, false)
#define bSAN IMQS(false, false, false, true)
#define bQT IMQS(false, false, true, false)

#define Afbeelding IMQS(UIImage, NSImage, QImage, void)

#define PRI IMQ(printf, printf, qDebug)

#ifdef PROFILE
#define DEBOUT(a)
#define DBGX(...)
#else
#ifdef SANANGELES
#define SDBG(a) DEBUG__(__android_log_print(ANDROID_LOG_INFO, "SanAngeles", (a).c_str(), __FILE__, __LINE__))
#define SDBGX(a, ...) DEBUG__(__android_log_print(ANDROID_LOG_INFO, "SanAngeles", (a).c_str(), __VA_ARGS__, __FILE__, __LINE__))
#define DEBOUT(a) SDBG(streng(a) + " {%s:%i}");
#define DBG1(a) SDBG(streng(a) + " {%s:%i}")
#define DBGX(a, ...) SDBGX(streng(a) + " {%s:%i}", __VA_ARGS__)
#else
#define DEBOUT(a) DEBUG__(PRI(a);)
#define DBGX(...) DEBUG__(PRI(__VA_ARGS__);)
#endif
#define DBG(a) APL_(DBGX(a)) SAN_(DBG1(a))
#define DBG2(a, b) DBGX(a, b)
#define DBG3(a, b, c) DBGX(a, b, c)
#endif

#define vaporize(a) { delete a; a = NULL; }

#define UNUSED(a) (void) a;

#endif

