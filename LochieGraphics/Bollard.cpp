#include "Bollard.h"
#include "ExtraEditorGUI.h"
#include "Serialisation.h"

void Bollard::GUI()
{

}

toml::table Bollard::Serialise(unsigned long long guid)
{
	return toml::table
	{
		{"guid", Serialisation::SaveAsUnsignedLongLong(guid)},
	};
}
