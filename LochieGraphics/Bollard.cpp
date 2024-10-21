#include "Bollard.h"
#include "ExtraEditorGUI.h"
#include "Serialisation.h"

Bollard::Bollard(toml::table table)
{
	startsUp = Serialisation::LoadAsBool(table["startsUp"]);
	timeToLower = Serialisation::LoadAsFloat(table["timeToLower"]);
	timeToRaise = Serialisation::LoadAsFloat(table["timeToRaise"]);
	triggerTag = Serialisation::LoadAsString(table["triggerTag"]);
}


void Bollard::GUI()
{

}

toml::table Bollard::Serialise(unsigned long long guid)
{
	return toml::table
	{
		{"guid", Serialisation::SaveAsUnsignedLongLong(guid)},
		{"startsUp", startsUp},
		{"timeToRaise", timeToRaise},
		{"timeToLower", timeToLower},
		{"triggerTag", triggerTag},
	};
}
