/*
=============================================================================
	File:	URI.h
	Desc:	Uniform Resource Identifier.
	Note:

	An URI object can split a Uniform Resource Identifier string into
	its components or build a string from URI components. Please note
	that the memory footprint of an URI object is always bigger then
	a pure String object, so if memory usage is of concern, it is advised
	to keep paths as String objects around, and only use URI objects
	to encode and decode them.

	An URI is made of the following components, where most of them
	are optional:

	Scheme://UserInfo@Host:Port/LocalPath#Fragment?Query

	Example URIs:

	http://user:password@www.myserver.com:8080/index.html#main
	http://www.myserver.com/query?user=bla
	ftp://ftp.myserver.com/pub/bla.zip
	file:///c:/temp/bla.txt
	file://SambaServer/temp/blub.txt

	Note that assigns will be resolved before splitting a URI into its
	components, for instance the assign "textures" could be defined
	as:

	Assign("textures", "http://www.dataserv.com/myapp/textures/");

	So a path to a texture URI could be defined as:

	URI("textures:mytex.dds")

	Which would actually resolve into:

	http://www.dataserv.com/myapp/textures/mytex.dds

	Decoding into components happens in the init constructor or the 
	Set() method in the following steps:

	- resolve any assigns in the original string
	- split into Scheme, Host and Path blocks
	- resolve Host and Path blocks further

	Enconding from components into string happens in the AsString()
	method in the following steps:

	- concatenate URI string from components
	- convert part of the string back into an existing assign

	(C) 2006 Radon Labs GmbH
=============================================================================
*/

#ifndef __MX_URI_H__
#define __MX_URI_H__

mxSWIPED("Nebula3");

mxNAMESPACE_BEGIN

//
//	URI
//
class URI
{
public:
	URI();
	explicit URI( const String& s );
	explicit URI( const ConstCharPtr& s );
	explicit URI( const URI& other );
	URI( EInitSlow, const char* s );
	~URI();

	void operator = ( const URI& other );

	bool operator == ( const URI& other ) const;
	bool operator != ( const URI& other ) const;

	bool IsEmpty() const;
	bool IsValid() const;

	void Clear();
	
	/// set complete URI string
	void Set( const String& s );
	/// return as concatenated string
	String AsString() const;

	/// set Scheme component (ftp, http, etc...)
	void SetScheme(const String& s);
	/// get Scheme component (default is file)
	const String& Scheme() const;
	/// set UserInfo component
	void SetUserInfo(const String& s);
	/// get UserInfo component (can be empty)
	const String& UserInfo() const;
	/// set Host component
	void SetHost(const String& s);
	/// get Host component (can be empty)
	const String& Host() const;
	/// set Port component
	void SetPort(const String& s);
	/// get Port component (can be empty)
	const String& Port() const;
	/// set LocalPath component
	void SetLocalPath(const String& s);
	/// get LocalPath component (can be empty)
	const String& LocalPath() const;
	/// append an element to the local path component
	void AppendLocalPath(const String& pathComponent);
	/// set Fragment component
	void SetFragment(const String& s);
	/// get Fragment component (can be empty)
	const String& Fragment() const;
	/// set Query component
	void SetQuery(const String& s);
	/// get Query component (can be empty)
	const String& Query() const;

	/// get the "tail" (path, query and fragment)
	String GetTail() const;
	/// get the host and path without scheme
	String GetHostAndLocalPath() const;

private:
	/// split string into components
	bool Split(const String& s);
	/// build string from components
	String Build() const;

private:
	String	scheme;
	String	userInfo;
	String	host;
	String	port;
	String	localPath;
	String	fragment;
	String	query;
	bool	isEmpty;
};

inline
URI::URI() :
isEmpty(true)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
inline
URI::URI(const String& s) :
isEmpty(true)
{
	bool validUri = this->Split(s);
	AssertX(validUri, s.ToChars());
}

//------------------------------------------------------------------------------
/**
*/
inline
URI::URI( EInitSlow, const char* s) :
isEmpty(true)
{
	bool validUri = this->Split(s);
	AssertX(validUri, s);
}

//------------------------------------------------------------------------------
/**
*/
inline
URI::URI(const URI& rhs) :
isEmpty(rhs.isEmpty),
scheme(rhs.scheme),
userInfo(rhs.userInfo),
host(rhs.host),
port(rhs.port),
localPath(rhs.localPath),
fragment(rhs.fragment),
query(rhs.query)
{
	// empty
}    

//------------------------------------------------------------------------------
/**
*/
inline
void
URI::operator=(const URI& rhs)
{
	this->isEmpty = rhs.isEmpty;
	this->scheme = rhs.scheme;
	this->userInfo = rhs.userInfo;
	this->host = rhs.host;
	this->port = rhs.port;
	this->localPath = rhs.localPath;
	this->fragment = rhs.fragment;
	this->query = rhs.query;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
URI::operator==(const URI& rhs) const
{
	if (this->isEmpty && rhs.isEmpty)
	{
		return true;
	}
	else
	{
		return ((this->scheme == rhs.scheme) &&
			(this->userInfo == rhs.userInfo) &&
			(this->host == rhs.host) &&
			(this->port == rhs.port) &&
			(this->localPath == rhs.localPath) &&
			(this->fragment == rhs.fragment) &&
			(this->query == rhs.query));
	}
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
URI::operator!=(const URI& rhs) const
{
	return !(*this == rhs);
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
URI::IsEmpty() const
{
	return this->isEmpty;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
URI::IsValid() const
{
	return !(this->isEmpty);
}

//------------------------------------------------------------------------------
/**
*/
inline
void
URI::Clear() 
{
	this->isEmpty = true;
	this->scheme.Clear();
	this->userInfo.Clear();
	this->host.Clear();
	this->port.Clear();
	this->localPath.Clear();
	this->fragment.Clear();
	this->query.Clear();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
URI::Set(const String& s)
{
	this->Split(s);
}

//------------------------------------------------------------------------------
/**
*/
inline
String
URI::AsString() const
{
	return this->Build();
}

//------------------------------------------------------------------------------
/**
*/
inline
void
URI::SetScheme(const String& s)
{
	this->isEmpty = false;
	this->scheme = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const String&
URI::Scheme() const
{
	return this->scheme;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
URI::SetUserInfo(const String& s)
{
	this->isEmpty = false;
	this->userInfo = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const String&
URI::UserInfo() const
{
	return this->userInfo;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
URI::SetHost(const String& s)
{
	this->isEmpty = false;
	this->host = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const String&
URI::Host() const
{
	return this->host;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
URI::SetPort(const String& s)
{
	this->isEmpty = false;
	this->port = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const String&
URI::Port() const
{
	return this->port;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
URI::SetLocalPath(const String& s)
{
	this->isEmpty = false;
	this->localPath = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const String&
URI::LocalPath() const
{
	return this->localPath;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
URI::SetFragment(const String& s)
{
	this->isEmpty = false;
	this->fragment = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const String&
URI::Fragment() const
{
	return this->fragment;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
URI::SetQuery(const String& s)
{
	this->isEmpty = false;
	this->query = s;
}

//------------------------------------------------------------------------------
/**
*/
inline
const String&
URI::Query() const
{
	return this->query;
}

mxNAMESPACE_END

#endif // !__MX_URI_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
