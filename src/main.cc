#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

#include <dlfcn.h>
#include <dirent.h>

#include <extdll.h> 
#include <meta_api.h> 
#include <iterator>

using namespace std;

plugin_info_t Plugin_info = {
    META_INTERFACE_VERSION, // ifvers
    "So Loader",  // name
    "1.0",        // version
    "2016/10/07", // date
    "Polarhigh",  // author
    "",           // url
    "SO LOADER",  // logtag, all caps please
    PT_ANYTIME,   // (when) loadable
    PT_STARTUP,   // (when) unloadable
};

int GetEntityAPI2(DLL_FUNCTIONS*, int*) { return(TRUE); }

C_DLLEXPORT void GiveFnptrsToDll(enginefuncs_t*, globalvars_t*) { }

C_DLLEXPORT int Meta_Query(const char* interfaceVersion, plugin_info_t** plinfo, mutil_funcs_t* pMetaUtilFuncs)
{
    *plinfo = &Plugin_info;


    string plugin_path = pMetaUtilFuncs->pfnGetPluginPath(PLID);
    plugin_path.replace(plugin_path.find_last_of("/"), plugin_path.length(), "");

    // 
    string libraries_folder;
    libraries_folder
        .append(plugin_path)
        .append("/libraries");

    // read config
    string libraries_config_path;
    libraries_config_path
        .append(plugin_path)
        .append("/libraries.cfg");


    vector<string> libraries_to_load;
    {
        ifstream config_file(libraries_config_path);

        if (config_file.is_open())
            copy(istream_iterator<string>(config_file), istream_iterator<string>(), back_inserter(libraries_to_load));
        else
        {
            pMetaUtilFuncs->pfnLogConsole(PLID, "[SO LOADER] Config not found: %s", libraries_config_path.c_str());
            return FALSE;
        }
    }

    // load libraries
    for_each(libraries_to_load.begin(), libraries_to_load.end(), [pMetaUtilFuncs, &libraries_folder](const string& library_name)
    {
        string res_msg;

        if (dlopen((libraries_folder + "/" + library_name).c_str(), RTLD_NOW | RTLD_GLOBAL) == NULL)
        {
            res_msg
                .append("Cannot load ")
                .append(library_name)
                .append(". Error: ").append(dlerror());

            pMetaUtilFuncs->pfnLogConsole(PLID, "[SO LOADER] %s", res_msg.c_str());
        }
        else
        {
            res_msg
                .append("Loaded: ")
                .append(library_name);

            pMetaUtilFuncs->pfnLogConsole(PLID, "[SO LOADER] %s", res_msg.c_str());
        }
    });

    return(TRUE);
}

C_DLLEXPORT int Meta_Attach(PLUG_LOADTIME now, META_FUNCTIONS* pFunctionTable,
    meta_globals_t* pMetaGlobals, gamedll_funcs_t* pGamedllFuncs)
{
    memset(pFunctionTable, 0, sizeof(META_FUNCTIONS));
    pFunctionTable->pfnGetEntityAPI2 = GetEntityAPI2;
    return(TRUE);
}

C_DLLEXPORT int Meta_Detach(PLUG_LOADTIME, PL_UNLOAD_REASON)
{
    return(TRUE);
}
