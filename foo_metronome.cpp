#define _WIN32_WINNT _WIN32_WINNT_WIN7
#define WINVER _WIN32_WINNT_WIN7

#include <helpers/foobar2000+atl.h>

static constexpr const char* component_name = "Metronome";

DECLARE_COMPONENT_VERSION(
	component_name,
	"1.0",
	"grimes\n\n"
	"Build: " __TIME__ ", " __DATE__
);

VALIDATE_COMPONENT_FILENAME("foo_metronome.dll");

#define ID_TIMER4 1111

UINT_PTR ptr4 = 0;
bool menu_metronome_enabled = false;
pfc::string8 metronome;
UINT bpm;
int i = 1;

// {2A2F55E5-110A-43BF-8377-1DCB133EF19B}
static const GUID guid_cfg_branch = { 0x2eb4867c, 0x177c, 0x416d, { 0x9e, 0x7d, 0x41, 0x7d, 0xf9, 0x5f, 0xaa, 0xd4 } };
static advconfig_branch_factory cfg_branch("Metronome", guid_cfg_branch, advconfig_entry::guid_branch_playback, 0);

// {DD0DC9E7-DA12-4671-905A-A4340908CE5F}
static const GUID guid_cfg_bpm = { 0xdd0dc9e7, 0xda12, 0x4671, { 0x90, 0x5a, 0xa4, 0x34, 0x9, 0x8, 0xce, 0x5f } };
advconfig_string_factory cfg_bpm("bpm ", guid_cfg_bpm, guid_cfg_branch, 0, "60");

VOID CALLBACK MetronomeTimer(
	HWND hwnd,        // handle to window for timer messages
	UINT message,     // WM_TIMER message
	UINT idEvent1,     // timer identifier
	DWORD dwTime)     // current system time
{
	if (menu_metronome_enabled)
	{
		FB2K_console_formatter() << "bpm: " << bpm << " beat: " << i++;
		if (i & 1L) console::info("");
	}
	else
	{
		KillTimer(NULL, idEvent1);
		i = 1;
	}
}

class mainmenu_commands_metronome : public mainmenu_commands
{

public:

	// Return the number of commands we provide.
	virtual t_uint32 get_command_count()
	{
		return 1;
	}

	// All commands are identified by a GUID.
	virtual GUID get_command(t_uint32 p_index)
	{
		static const GUID guid_main_metronome =
		{ 0xf4f24e27, 0xd17c, 0x4402, { 0xae, 0x94, 0x2b, 0xca, 0xfd, 0x16, 0x11, 0x7b } };

		if (p_index == 0)
			return guid_main_metronome;
		return pfc::guid_null;
	}

	// Set p_out to the name of the n-th command.
	// This name is used to identify the command and determines
	// the default position of the command in the menu.
	virtual void get_name(t_uint32 p_index, pfc::string_base& p_out)
	{
		if (p_index == 0)
			p_out = "Metronome";
	}

	// Set p_out to the description for the n-th command.
	virtual bool get_description(t_uint32 p_index, pfc::string_base& p_out)
	{
		if (p_index == 0)
			p_out = "Metronome to console";
		else
			return false;
		return true;
	}

	// Every set of commands needs to declare which group it belongs to.
	virtual GUID get_parent()
	{
		return mainmenu_groups::playback;
	}

	// Execute n-th command.
	// p_callback is reserved for future use.
	virtual void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback)
	{
		if (p_index == 0)
		{
			cfg_bpm.get(metronome);
			bpm = atoi(metronome);
			menu_metronome_enabled = !menu_metronome_enabled;
			if (menu_metronome_enabled)
			{
				ptr4 = SetTimer(NULL, ID_TIMER4, 60 * 1000 / bpm, (TIMERPROC)MetronomeTimer);
			}
		}
	}

	// The standard version of this command does not support checked or disabled
	// commands, so we use our own version.
	virtual bool get_display(t_uint32 p_index, pfc::string_base& p_text, t_uint32& p_flags)
	{
		if (p_index == 0) {
			if (menu_metronome_enabled)
			{
				p_flags |= flag_checked;
			}
			else {
				p_flags = 0;
			}
			get_name(p_index, p_text);
		}
		return true;
	}
	virtual t_uint32 get_sort_priority()
	{
		return sort_priority_dontcare;
	}
	bool is_checked(t_uint32 p_index)
	{
		if (p_index == 0)
			return menu_metronome_enabled;
	}
};

static mainmenu_commands_factory_t<mainmenu_commands_metronome> g_mainmenu_commands_metronome;