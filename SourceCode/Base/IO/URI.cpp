/*
=============================================================================
	File:	URI.cpp
	Desc:	Uniform Resource Identifier.
=============================================================================
*/

#include <Base_PCH.h>
#pragma hdrstop
#include <Base.h>

#include <Base/IO/URI.h>

mxNAMESPACE_BEGIN

/*================================
			URI
================================*/

URI::~URI()
{
}
#if 0
//------------------------------------------------------------------------------
/**
    Resolve assigns and split URI string into its components.

    @todo: this is too complicated...
*/
bool
URI::Split(const String& s)
{
    Assert(s.IsValid());
    this->Clear();
    this->isEmpty = false;
    
    // resolve assigns
    String str;
    if (AssignRegistry::HasInstance())
    {
        str = AssignRegistry::Instance()->ResolveAssignsInString(s);
    }
    else
    {
        str = s;
    }

    // scheme is the first components and ends with a :
    IndexT schemeColonIndex = str.FindCharIndex(':');
    String potentialScheme;
    bool schemeIsDevice = false;
    if (INDEX_NONE != schemeColonIndex)
    {
        potentialScheme = str.ExtractRange(0, schemeColonIndex);
        if (FSWrapper::IsDeviceName(potentialScheme))
        {
            // there is either no scheme given, or the "scheme"
            // is actually a device name
            // in both cases we fall back to the default scheme "file", and
            // just set the whole string as local path, there will be no
            // other components
            schemeIsDevice = true;
        }
    }
    if ((INDEX_NONE == schemeColonIndex) || schemeIsDevice)
    {
        this->SetScheme(DEFAULT_IO_SCHEME);
        this->SetLocalPath(str);
        return true;
    }

    // check is a valid scheme was provided
    if (INDEX_NONE != schemeColonIndex)
    {
        // a valid scheme is given
        this->SetScheme(potentialScheme);

        // after the scheme, and before the host, there must be a double slash
        if (!((str[schemeColonIndex + 1] == '/') && (str[schemeColonIndex + 2] == '/')))
        {
            return false;
        }
    }

    // extract UserInfo, Host and Port components
    IndexT hostStartIndex = schemeColonIndex + 3;
    IndexT hostEndIndex = str.FindCharIndex('/', hostStartIndex);
    String userInfoHostPort;
    String path;
    if (INDEX_NONE == hostEndIndex)
    {
        userInfoHostPort = str.ExtractToEnd(hostStartIndex);
    }
    else
    {
        userInfoHostPort = str.ExtractRange(hostStartIndex, hostEndIndex - hostStartIndex);
        path = str.ExtractToEnd(hostEndIndex + 1);
    }

    // extract port number if exists
    IndexT portIndex = userInfoHostPort.FindCharIndex(':');
    IndexT atIndex = userInfoHostPort.FindCharIndex('@');
    if (INDEX_NONE != portIndex)
    {
        if (INDEX_NONE != atIndex)
        {
            n_assert(portIndex > atIndex);
        }
        this->SetPort(userInfoHostPort.ExtractToEnd(portIndex + 1));
        userInfoHostPort.TerminateAtIndex(portIndex);
    }
    if (INDEX_NONE != atIndex)
    {
        this->SetHost(userInfoHostPort.ExtractToEnd(atIndex + 1));
        userInfoHostPort.TerminateAtIndex(atIndex);
        this->SetUserInfo(userInfoHostPort);
    }
    else
    {
        this->SetHost(userInfoHostPort);
    }

    // split path part into components
    if (path.IsValid())
    {
        IndexT fragmentIndex = path.FindCharIndex('#');
        IndexT queryIndex = path.FindCharIndex('?');
        if (INDEX_NONE != queryIndex)
        {
            if (INDEX_NONE != fragmentIndex)
            {
                n_assert(queryIndex > fragmentIndex);
            }
            this->SetQuery(path.ExtractToEnd(queryIndex + 1));
            path.TerminateAtIndex(queryIndex);
        }
        if (INDEX_NONE != fragmentIndex)
        {
            this->SetFragment(path.ExtractToEnd(fragmentIndex + 1));
            path.TerminateAtIndex(fragmentIndex);
        }
        this->SetLocalPath(path);
    }
    return true;
}
#endif
//------------------------------------------------------------------------------
/**
    This builds an URI string from its components.
*/
String
URI::Build() const
{
    Assert(!this->IsEmpty());

    String str;
    str.Reserve(256);
    if (this->scheme.IsValid())
    {
        str.Append(this->scheme);
        str.Append("://");
    }
    if (this->userInfo.IsValid())
    {
        str.Append(this->userInfo);
        str.Append("@");
    }
    if (this->host.IsValid())
    {
        str.Append(this->host);
    }
    if (this->port.IsValid())
    {
        str.Append(":");
        str.Append(this->port);
    }
    if (this->localPath.IsValid())
    {
        str.Append("/");
        str.Append(this->localPath);
    }
    if (this->fragment.IsValid())
    {
        str.Append("#");
        str.Append(this->fragment);
    }
    if (this->query.IsValid())
    {
        str.Append("?");
        str.Append(this->query);
    }
    return str;
}

//------------------------------------------------------------------------------
/**
    This returns the "tail", which is the local path, the fragment and
    the query concatenated into one string.
*/
String
URI::GetTail() const
{   
    String str;
    str.Reserve(256);
    if (this->localPath.IsValid())
    {
        str.Append(this->localPath);
    }
    if (this->fragment.IsValid())
    {
        str.Append("#");
        str.Append(this->fragment);
    }
    if (this->query.IsValid())
    {
        str.Append("?");
        str.Append(this->query);
    }
    return str;
}

//------------------------------------------------------------------------------
/**
    Returns the host and local path in the form "//host/localpath".
    If no host has been set, only "/localpath" will be returned.
*/
String
URI::GetHostAndLocalPath() const
{
    String str;
    str.Reserve(this->host.Length() + this->localPath.Length() + 8);
    if (this->host.IsValid())
    {
        str.Append("//");
        str.Append(this->host);
        str.Append("/");
    }
    str.Append(this->localPath);
    return str;
}

//------------------------------------------------------------------------------
/**
    Appends an element to the local path. Automatically inserts
    a path delimiter "/".
*/
void
URI::AppendLocalPath(const String& pathComponent)
{
    Assert(pathComponent.IsValid());
    this->localPath.Append("/");
    this->localPath.Append(pathComponent);
}

mxNAMESPACE_END

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
