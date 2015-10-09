#ifndef __SHADER_EDITOR_UTILS_H__
#define __SHADER_EDITOR_UTILS_H__

template< class COMMAND >
bool ExecuteCommand(
	COMMAND & command )
{
	ShaderEditor & app = ShaderEditor::Get();
	return app.cmdExec.ExecuteCommand( app.GetSocket(), command );
}


#endif // __SHADER_EDITOR_UTILS_H__
