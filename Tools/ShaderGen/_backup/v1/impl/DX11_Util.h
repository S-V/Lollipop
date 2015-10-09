
#pragma once

wrBuffer ToUpper( const wrName& filter )
{
	wrBuffer	buf( filter );
	buf.ToUpper();
	return buf;
}
wrBuffer get_boolean_constant_string( const wrName& s )
{
	if( s.IsEmpty() ) {
		return "FALSE";
	}

	wrBuffer	tmp = ToUpper( s );

	wrBuffer	buf;
	if( tmp == "0" || tmp == "FALSE" ) {
		buf = "FALSE";
	}
	else if( tmp == "1" || tmp == "TRUE" ) {
		buf = "TRUE";
	}
	else {
		Unimplemented;
	}
	return buf;
}
wrBuffer get_integer_constant_string( const wrName& s )
{
	if( s.IsEmpty() ) {
		return "0";
	}
//	Assert( ParseInteger(s.ToChars()) );

	wrBuffer	buf = ToUpper( s );
	return buf;
}
wrBuffer get_float_constant_string( const wrName& s )
{
	if( s.IsEmpty() ) {
		return "0.0f";
	}
//	Assert( ParseFloat(s.ToChars()) );

	wrBuffer	buf = ToUpper( s );
	return buf;
}
wrBuffer get_rgba_color_string( const FColor& s )
{
	wrBuffer	buf;
	buf.Format("FColor( %ff, %ff, %ff, %ff )",s.R,s.G,s.B,s.A);
	return buf;
}
wrBuffer get_write_mask_string( const wrName& s )
{
	if( s.IsEmpty() ) {
		return "0";
	}
	if( mxAnsiCharIsDigit( s.ToChars()[0] ) )
	{
		return s;
	}

	wrBuffer	buf = ( "D3D11_" );
	buf += ToUpper( s );
	return buf;
}

wrBuffer DXGI_Format_From_Script_Format( const wrName& origFormat )
{
	if( origFormat.IsEmpty() ) {
		return wrName("DXGI_FORMAT_UNKNOWN");
	}
	wrBuffer	dxgiFormat("DXGI_FORMAT_");
	dxgiFormat += origFormat;
	return dxgiFormat;
}

wrBuffer Get_RT_Size( const wrRTSize& size, ConstCharPtr& relativeToWhat = MXC("backbufferSize") )
{
	wrBuffer	sizeStr;
	if ( size.sizeMode == Size_Absolute )
	{
		sizeStr.Format( "%u", size.absoluteSize );
	}
	else if ( size.sizeMode == Size_Relative )
	{
		if( size.relativeSize == 1.0f ) {
			sizeStr = relativeToWhat;
		} else {
			sizeStr.Format( "%s * %ff", relativeToWhat.ToChars(), size.relativeSize );
		}
	} 
	else
	{
		Unimplemented;
		sizeStr = "?";
	}
	return sizeStr;
}


wrBuffer GetTextureFilter( const wrName& filter )
{
	if( filter.IsEmpty() ) {
		return wrBuffer("D3D11_FILTER_MIN_MAG_MIP_LINEAR");
	}
	wrBuffer	buf("D3D11_FILTER_");
	buf += ToUpper( filter );
	return buf;
}
wrBuffer GetTextureAddressMode( const wrName& addrMode )
{
	if( addrMode.IsEmpty() ) {
		return wrBuffer("D3D11_TEXTURE_ADDRESS_CLAMP");
	}
	wrBuffer	buf("D3D11_TEXTURE_ADDRESS_");
	buf += ToUpper( addrMode );
	return buf;
}

wrBuffer get_depth_write_mask_string( const wrName& s )
{
	if( s.IsEmpty() ) {
		return wrBuffer("D3D11_DEPTH_WRITE_MASK_ALL");
	}
	if( s == "0" ) {
		return wrBuffer("D3D11_DEPTH_WRITE_MASK_ZERO");
	}
	if( s == "1" ) {
		return wrBuffer("D3D11_DEPTH_WRITE_MASK_ALL");
	}
	wrBuffer	buf("D3D11_DEPTH_WRITE_MASK_");
	buf += ToUpper( s );
	return buf;
}
wrBuffer get_depth_func_string( const wrName& s )
{
	if( s.IsEmpty() ) {
		return wrBuffer("D3D11_COMPARISON_LESS");
	}
	wrBuffer	buf("D3D11_COMPARISON_");
	buf += ToUpper( s );
	return buf;
}
wrBuffer get_fill_mode_string( const wrName& s )
{
	if( s.IsEmpty() ) {
		return wrBuffer("D3D11_FILL_SOLID");
	}
	wrBuffer	buf("D3D11_FILL_");
	buf += ToUpper( s );
	return buf;
}
wrBuffer get_cull_mode_string( const wrName& s )
{
	if( s.IsEmpty() ) {
		return wrBuffer("D3D11_CULL_BACK");
	}
	wrBuffer	buf("D3D11_CULL_");
	buf += ToUpper( s );
	return buf;
}

wrBuffer get_blend_mode_string( const wrName& s )
{
	if( s.IsEmpty() ) {
		return wrBuffer("D3D11_BLEND_ZERO");
	}
	wrBuffer	buf("D3D11_BLEND_");
	buf += ToUpper( s );
	return buf;
}

wrBuffer get_blend_op_string( const wrName& s )
{
	if( s.IsEmpty() ) {
		return wrBuffer("D3D11_BLEND_OP_ADD");
	}
	wrBuffer	buf("D3D11_BLEND_OP_");
	buf += ToUpper( s );
	return buf;
}
