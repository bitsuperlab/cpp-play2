/*
 * Copyright (c) 2015 Cryptonomex, Inc., and contributors.
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <graphene/game/game_plugin.hpp>

#include <graphene/chain/database.hpp>
#include <graphene/chain/asset_object.hpp>
#include <graphene/chain/game_object.hpp>
#include <graphene/time/time.hpp>
#include <graphene/game/v8_api.hpp>

#include <graphene/utilities/key_conversion.hpp>

#include <fc/smart_ref_impl.hpp>
#include <fc/thread/thread.hpp>

#include <iostream>

namespace graphene { namespace game_plugin {

    namespace detail
    {
        class ArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
        public:
           virtual void* Allocate(size_t length) {
              void* data = AllocateUninitialized(length);
              return data == NULL ? data : memset(data, 0, length);
           }
           virtual void* AllocateUninitialized(size_t length) { return malloc(length); }
           virtual void Free(void* data, size_t) { free(data); }
        };

        class game_plugin_impl
        {
        public:
            game_plugin&            _self;
            v8::Platform*           _platform;

            // For each game we should create a different isolate instance,
            // and do exit/dispose operation for each of them
            //
            // Here I just fixed the process hang issue.
            v8::Isolate* _isolate;

            ArrayBufferAllocator*   _allocator;

            exlib::Service*         _service;

            //fc::path                _data_dir;

            std::unordered_map<std::string, v8_game_engine_ptr > _engines;

            boost::signals2::scoped_connection   _http_callback_signal_connection;

            static uint32_t* ComputeStackLimit(uint32_t size) {
              uint32_t* answer = &size - (size / sizeof(size));
              // If the size is very large and the stack is very near the bottom of
              // memory then the calculation above may wrap around and give an address
              // that is above the (downwards-growing) stack.  In that case we return
              // a very low address.
              if (answer > &size) return reinterpret_cast<uint32_t*>(sizeof(size));
              return answer;
            }

            game_plugin_impl(game_plugin& _plugin)
            : _self( _plugin )
            {
               try {
                  v8::V8::InitializeICU();
                  exlib::Service::init();

                  _platform = v8::platform::CreateDefaultPlatform();
                  v8::V8::InitializePlatform(_platform);
                  v8::V8::Initialize();


                  _isolate = v8::Isolate::GetCurrent();
                  if ( _isolate == NULL )
                  {
                     /*
                      ResourceConstraints rc;
                      rc.set_max_old_space_size(10); //MB
                      rc.set_max_executable_size(10); //MB

                      params.constraints.set_stack_limit(reinterpret_cast<uint32_t*>((char*)&rc - 1024 * 512));
                      https://github.com/v8/v8/blob/master/test/cctest/test-api.cc#L18724
                      */
                     _service = exlib::Service::current();
                     _allocator = new ArrayBufferAllocator();
                     Isolate::CreateParams create_params;
                     create_params.array_buffer_allocator = _allocator;

                     ResourceConstraints rc;
                     rc.set_max_semi_space_size(40);
                     rc.set_max_old_space_size(60); //MB
                     rc.set_max_executable_size(60); //MB
                     static const int stack_breathing_room = 1024 * 1024;
                     //uint32_t* set_limit = ComputeStackLimit(stack_breathing_room);
                     rc.set_stack_limit(reinterpret_cast<uint32_t*>((char*)&rc - stack_breathing_room));

                     create_params.constraints = rc;

                     _isolate = v8::Isolate::New(create_params);

                     //_isolate->SetStackLimit(reinterpret_cast<uintptr_t>(set_limit));
                     _isolate->Enter();
                  }

                  v8::V8::SetCaptureStackTraceForUncaughtExceptions(true, 10, StackTrace::kDetailed);

                  ilog("Init class templat for game client" );

                  v8_api::init_class_template( _isolate );
                  // Refer: libraries/chain/include/graphene/chain/protocol/operations.hpp
                  //bts::blockchain::operation_factory::instance().register_operation<game_create_operation>();
                  //bts::blockchain::operation_factory::instance().register_operation<game_update_operation>();
                  //bts::blockchain::operation_factory::instance().register_operation<game_play_operation>();

                  /*
                  game_executors::instance().register_game_executor(
                     std::function<void( chain_database_ptr, uint32_t, const pending_chain_state_ptr&)>(
                     std::bind(&client::execute, self, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ) )
                                                                );
                  */
                  /*
                  _http_callback_signal_connection =
                  self->game_claimed_script.connect(
                                                [=]( std::string code, std::string name) { this->script_http_callback( code, name ); } );
                  */
               } catch (...) {
               }
            }

            virtual ~game_plugin_impl();

            graphene::chain::database& database()
            {
                return _self.database();
            }

            void   install_game_engine(const std::string& game_name, v8_game_engine_ptr engine_ptr )
            {
              FC_ASSERT( _engines.find( game_name ) == _engines.end(),
                        "Game Name already Registered ${name}", ("name", game_name) );

              if(engine_ptr != NULL)
              {
                  _engines[game_name] = engine_ptr;
              }
            }

            void install_game_engine_if_not(const std::string& game_name)
            {
               auto itr = _engines.find( game_name );

               if( itr == _engines.end() )
               {
                  try
                  {
                      install_game_engine(game_name, std::make_shared< v8_game_engine > (game_name, _self));
                  } catch ( const fc::exception& e )
                  {
                      wlog("game engine register failed: ${x}", ("x",e.to_detail_string()));
                  }
               }
            }

            // Debuging file from operation and save to data_dir
            /*
            void script_http_callback( const std::string code, std::string game_name )
            {
              ilog("Storing the game code in a directory for viewing: ${game_name}", ("game_name", game_name) );
              fc::async( [=]()
                      {

                         if ( !fc::exists(self->get_data_dir()) ) {
                            fc::create_directories(self->get_data_dir());
                         }

                         ilog ("The data dir is ${d}", ("d", self->get_data_dir() ));

                         fc::path script_filename = self->get_data_dir() / ( game_name + ".js");

                         ilog ("The script_filename ${d}", ("d", script_filename ));

                         std::ofstream script_file(script_filename.string());

                         script_file << code;
                      }
                      );
              }
              */
        };

        game_plugin_impl::~game_plugin_impl()
        {
           // explicitly release enginer obj here
           // before we release isolate things.

            for(auto itr=_engines.begin(); itr!=_engines.end(); itr++)
            //for(auto e: _engines) not work???
            {
               //int count = itr->second.use_count();
               itr->second.reset();
            }

            _isolate->Exit();
            _isolate->Dispose();

            v8::V8::Dispose();
            v8::V8::ShutdownPlatform();
            delete _platform;
            delete _allocator;
            return;
        }
    } // end namespace detail



}}

using namespace graphene::game_plugin;
using namespace graphene::chain;
using std::string;
using std::vector;

namespace bpo = boost::program_options;

// TODO: migrate from https://github.com/dacsunlimited/dac_play/blob/master/libraries/game/client.cpp

void game_plugin::plugin_set_program_options(
   boost::program_options::options_description& command_line_options,
   boost::program_options::options_description& config_file_options)
{
}

game_plugin::game_plugin() :
my( new detail::game_plugin_impl(*this) )
{
}

game_plugin::~game_plugin()
{
}

std::string game_plugin::plugin_name()const
{
   return "game";
}

void game_plugin::plugin_initialize(const boost::program_options::variables_map& options)
{ try {
   ilog("game plugin:  plugin_initialize() begin");
   _options = &options;
   
   auto& db = database();

   db.on_game_execution.connect( [&]( const signed_block& b){
      execute(b, db);
   } );
   
   db.on_game_play.connect([&]( game_id_type game_id) { /* play(game_id); // get_v8_engine(game_id)->play(...)*/ } );

   db.on_game_evaluate.connect([&]( const game_play_operation& op) {
      auto game_to_play = db.get(op.game_to_play);
      auto v8_game_engine = get_v8_engine( game_to_play.name );
      v8_game_engine->evaluate(op, db);
   } );

   db.on_game_apply.connect([&]( const game_play_operation& op, const game_play_object& obj) {
      auto game_to_play = db.get(op.game_to_play);
      auto v8_game_engine = get_v8_engine( game_to_play.name );
      v8_game_engine->apply(op, obj, db);
   } );

   ilog("game plugin:  plugin_initialize() end");
} FC_LOG_AND_RETHROW() }

void game_plugin::plugin_startup()
{ try {
   ilog("game plugin:  plugin_startup() begin");
   chain::database& d = database();
   //Start NTP time client
   graphene::time::now();


   ilog("game plugin:  plugin_startup() end");
} FC_CAPTURE_AND_RETHROW() }

void game_plugin::plugin_shutdown()
{
   graphene::time::shutdown_ntp_time();
   return;
}

const game_object& game_plugin::get_game( const string& name )
{
   const auto& idx = database().get_index_type<game_index>().indices().get<by_game_name>();
   const auto itr = idx.find(name);
   assert( itr != idx.end() );
   return *itr;
}

const asset_object& game_plugin::get_game_asset( const string& symbol )
{
   const auto& idx = database().get_index_type<asset_index>().indices().get<by_symbol>();
   const auto itr = idx.find(symbol);
   assert( itr != idx.end() );
   return *itr;
}

void game_plugin::execute( const signed_block& b, chain::database& db )
{ try {
    wlog("Start executing in game client at block ${b}", ("b", b.block_num()));
    const auto& games = database().get_index_type<game_index>().indices();
    for( auto itr = games.begin(); itr != games.end();
       ++itr )
    {
      try {
          auto v8_game_engine = get_v8_engine( itr->name );
          wlog("Start execute the game ${g}", ("g", itr->name));
          v8_game_engine->execute( b, db );
      }
      catch (const game_play_game_engine_not_found& e)
      {
          wlog("game engine note found, failed to init for unknown reason during chain execution");
      }
    }
} FC_CAPTURE_AND_RETHROW( (b) ) }

v8_game_engine_ptr game_plugin::get_v8_engine(const std::string& game_name)
{
        my->install_game_engine_if_not(game_name);

        auto itr = my->_engines.find( game_name );
        if( itr == my->_engines.end() )
           FC_THROW_EXCEPTION( game_play_game_engine_not_found, "", ("game_name", game_name) );
        return itr->second;
}

bool game_plugin::reinstall_game_engine(const std::string& game_name)
{
      auto itr = my->_engines.find( game_name );
      if( itr != my->_engines.end() )
          my->_engines.erase(itr);

      get_v8_engine( game_name );

      return true;
}

void* game_plugin::get_isolate(/*const std::string& game_name*/)
{
   return my->_isolate;
}
