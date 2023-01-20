#pragma once
#include "windows.h"
#include <functional>
#include <psapi.h>
#include <vector>
#include <tlhelp32.h>
#include <DbgHelp.h>
#include <comdef.h>  // you will need this
#pragma comment(lib, "DbgHelp.lib")

namespace protocol
{
    using namespace std;

    struct protocol_t;
    struct proto_cmd_t
    {
        string protocol_name;
        vector<string> args;

        proto_cmd_t( string str_in )
        {
            string post_processed = "";

            int step = 0;
            for ( int i = 0; i < str_in.size(); i++ )
            {
                if ( step == 0 && str_in[i] == ':' )
                {
                    protocol_name = str_in.substr( 1, i-1 );
                    step++;
                }
                else if ( step == 1 && str_in[i] == '/' )
                    step++;
                else if ( step == 2 && str_in[i] == '/' )
                    step++;
                else if ( step == 3 )
                {
                    if ( str_in[i] == '/' )
                    {
                        if ( i != str_in.size() - 2 )
                        {
                            this->args.push_back( post_processed );
                            post_processed = "";
                        }
                    }
                    else if ( str_in[i] == '"' )
                    {
                        break;
                    }
                    else
                    {
                        post_processed += str_in[i];
                    }
                }

            }
            this->args.push_back( post_processed );
        }
    };
    using callback_handler_t = function<void( proto_cmd_t& )>;


    static CRITICAL_SECTION proto_cs;
    static bool proto_cs_init = false;
    static vector<protocol_t*> protos_int;

    struct protocol_t
    {
        string protocol_name;
        vector<callback_handler_t> functions;

        void add_callback( callback_handler_t fn )
        {
            functions.push_back( fn );
        }
        void handle( proto_cmd_t& cmd )
        {
            if ( cmd.protocol_name != this->protocol_name )
                return;
            for ( auto fn : functions )
                fn( cmd );
        }

        ~protocol_t()
        {
            EnterCriticalSection( &proto_cs );
            for ( int i = 0; i < protos_int.size(); i++ )
                if ( protos_int[i] == this )
                    protos_int.erase( protos_int.begin() + i );
            LeaveCriticalSection( &proto_cs );
        }
    };

    static protocol_t* create_protocol( string protocol_name )
    {
        if ( !proto_cs_init )
            InitializeCriticalSection( &proto_cs );

        static const auto open_key = []( HKEY root_key, const char* sub_key ) ->HKEY
        {
            HKEY key_handle;
            if ( RegOpenKeyExA( root_key, sub_key, NULL, KEY_ALL_ACCESS, &key_handle ) == ERROR_FILE_NOT_FOUND )
                RegCreateKeyExA( root_key, sub_key, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key_handle, NULL );
            return key_handle;
        };
        static const auto set_val = []( HKEY key_handle, const char* value_name, string& data ) -> void
        {
            RegSetValueExA( key_handle, value_name, NULL, REG_SZ, (BYTE*)data.c_str(), data.size() + 1 );
        };
        static const auto close_key = RegCloseKey;

        char path[2048];
        char command[4096];

        GetModuleFileNameA( GetModuleHandleA( NULL ), path, 2048 );
        sprintf_s( command, R"("%s" "%%1")", path );

        HKEY key = open_key( HKEY_CLASSES_ROOT, protocol_name.c_str() );
        string t1 = "URL:srift protocol";
        string t2 = "";
        set_val( key, "", t1 );
        set_val( key, "URL Protocol", t2 );
        close_key( key );
        close_key( open_key( HKEY_CLASSES_ROOT, (protocol_name + "\\Shell").c_str() ) );
        close_key( open_key( HKEY_CLASSES_ROOT, (protocol_name + "\\Shell\\Open").c_str() ) );
        close_key( open_key( HKEY_CLASSES_ROOT, (protocol_name + "\\Shell\\Open\\Command").c_str() ) );


        key = open_key( HKEY_CLASSES_ROOT, (protocol_name + "\\Shell\\Open\\Command").c_str() );
        string s = command;
        set_val( key, "", s);
        close_key( key );

        protocol_t * p = new protocol_t{ protocol_name };
        EnterCriticalSection( &proto_cs );
        protos_int.push_back( p );
        LeaveCriticalSection( &proto_cs );
        return p;
    }

    static void handle_cmd_internal( const char * cmd_raw )
    {
        proto_cmd_t cmd( cmd_raw );

        EnterCriticalSection( &proto_cs );
        for ( protocol_t* proto : protos_int )
            proto->handle( cmd );
        LeaveCriticalSection( &proto_cs );
    }

    static void handle_cmd( const char * cmd_raw )
    {
        static const auto get_img_name = []( HANDLE h = GetCurrentProcess() ) -> string
        {
            char image_name[4096];
            GetProcessImageFileNameA( h, image_name, 4096 );
            return string( image_name );
        };
        static const auto get_module_base = []( DWORD process_id, const char * module_name ) -> BYTE*
        {
            HANDLE snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, process_id );
            BYTE* module = nullptr;
            if ( snapshot != INVALID_HANDLE_VALUE )
            {
                MODULEENTRY32 mod_entry = { 0 };
                mod_entry.dwSize = sizeof( MODULEENTRY32 );
                if ( Module32First( snapshot, &mod_entry ) )
                {
                    do
                    {

                        char output[256];
                        sprintf(output, "%ws", mod_entry.szModule);

                        if ( _stricmp( output, module_name ) == 0 )
                        {
                            module = mod_entry.modBaseAddr;
                            break;
                        }
                    } while ( Module32Next( snapshot, &mod_entry ) );
                }
                CloseHandle( snapshot );
            }
            return module;
        };

        string img_name = get_img_name();
        const char* exe_name = img_name.c_str() + img_name.find_last_of( '\\' ) + 1;

        DWORD process_list[1024];
        DWORD byte_needed;
        EnumProcesses( process_list, sizeof( process_list ), &byte_needed );

        for ( int i = 0; i < byte_needed / sizeof( DWORD ); i++ )
        {
            if ( process_list[i] != 0 )
            {

                HANDLE handle_process = OpenProcess( PROCESS_QUERY_INFORMATION |
                                                     PROCESS_VM_READ,
                                                     FALSE, process_list[i] );

                if ( handle_process != NULL && GetProcessId( handle_process ) != GetCurrentProcessId() )
                {
                    if ( img_name == get_img_name( handle_process ) )
                    {
                        // Found already open parent, get a more privileged handle
                        HANDLE handle_process_priv = OpenProcess( PROCESS_ALL_ACCESS, FALSE, process_list[i] );
                        CloseHandle( handle_process );

                        // Get pointer
                        void* handler_ext = (void*)((size_t)get_module_base( process_list[i], exe_name ) + ((size_t)&handle_cmd_internal - (size_t)GetModuleHandle( NULL )));

                        // Write arguments
                        void* arg_space = VirtualAllocEx( handle_process_priv, 0, strlen( cmd_raw ) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
                        WriteProcessMemory( handle_process_priv, arg_space, cmd_raw, strlen( cmd_raw ) + 1, 0 );

                        // Call the function
                        HANDLE thread = CreateRemoteThread( handle_process_priv, NULL, 0, (LPTHREAD_START_ROUTINE)handler_ext, arg_space, 0, NULL );
                        WaitForSingleObject( thread, INFINITE );
                        VirtualFreeEx( handle_process_priv, arg_space, 0, MEM_RELEASE );
                        CloseHandle( thread );

                        CloseHandle( handle_process_priv );
                        exit( 0 );
                    }
                }

                CloseHandle( handle_process );
            }
        }

        handle_cmd_internal( cmd_raw );
    };
}
