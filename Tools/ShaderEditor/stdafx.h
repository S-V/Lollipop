// Qt headers

#include <QtGui>
#include <QRegExp>

// Engine headers

#include <Base/Base.h>

MX_USING_NAMESPACE;

#include <Core/Core.h>

#include <Network/Network.h>
#include <Network/Protocol.h>
#include <Network/src/TCPUtil.h>
using namespace Net;

#if MX_AUTOLINK
#pragma comment( lib, "Base.lib" )
#pragma comment( lib, "Core.lib" )
#pragma comment( lib, "Network.lib" )
#pragma comment( lib, "Graphics.lib" )
#endif //MX_AUTOLINK
