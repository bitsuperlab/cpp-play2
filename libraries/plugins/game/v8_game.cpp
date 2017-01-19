#include <graphene/game/v8_helper.hpp>
#include <graphene/game/v8_api.hpp>
#include <graphene/game/v8_game.hpp>
#include <graphene/game/game_plugin.hpp>
#include <graphene/chain/protocol/game.hpp>

#include <fc/log/logger.hpp>

namespace graphene { namespace game_plugin {

   namespace detail {
      class v8_game_engine_impl {

      public:
         graphene::game_plugin::v8_game_engine*         self;
         graphene::game_plugin::game_plugin&            _game_plugin;
         std::string                                    _game_name;
         Isolate*                                       _isolate;
         v8::Persistent<Context>                        _context;

         v8_game_engine_impl(v8_game_engine* self, graphene::game_plugin::game_plugin& plugin)
         : self(self), _game_plugin(plugin)
         {
             //_context.Reset();
         }

         ~v8_game_engine_impl(){
            _context.Reset();
         }

         void init()
         {
            // Refer http://v8.googlecode.com/svn/trunk/samples/process.cc
            // Deprecated: fc::path script_path( _client->get_data_dir() / (_game_name + ".js") );

            //_isolate = v8::Isolate::GetCurrent();
            _isolate = (Isolate*)_game_plugin.get_isolate();

            v8::Locker locker(_isolate);
            Isolate::Scope isolate_scope(_isolate);
            HandleScope handle_scope(_isolate);
            v8::TryCatch try_catch( _isolate );
            v8::Handle<v8::Context> context = v8_helper::CreateShellContext(_isolate);
            if (context.IsEmpty()) {
                String::Utf8Value error(try_catch.Exception());
                // wlog("Error creating context in game ${name}, error is ${e}", ("name", _game_name)("e", *error));
                FC_CAPTURE_AND_THROW(game_play_failed_game_engine_init);
            }
            _context.Reset(_isolate, context);

            Context::Scope context_scope(context);

            //ilog("The game is ${s}", ("s", _game_name ));

            auto game = _game_plugin.get_game(_game_name);
             //FC_ASSERT( game.valid() );

             v8::Handle<v8::String> source = v8::String::NewFromUtf8( GetIsolate(), game.script_code.c_str() );

             if (game.script_code.empty()) {
                 //wlog("The souce is empty, error loading script code");
                 GetIsolate()->ThrowException( v8::String::NewFromUtf8(GetIsolate(), "Error loading file" ) );
                 String::Utf8Value error(try_catch.Exception());
                 FC_CAPTURE_AND_THROW(game_play_failed_load_source_file, (_game_name)(*error));
             }

            String::Utf8Value utf8_source(source);
            Handle<Script> script = Script::Compile(source);

            if ( script.IsEmpty() )
            {
                // The TryCatch above is still in effect and will have caught the error.
                FC_CAPTURE_AND_THROW(game_play_failed_compile_script, (*utf8_source)(v8_helper::ReportException(GetIsolate(), &try_catch)));
            } else
            {
                // Run the script to get the result.
                Handle<Value> result = script->Run();

                if ( result.IsEmpty() )
                {
                    FC_CAPTURE_AND_THROW(game_play_failed_run_script, (v8_helper::ReportException(GetIsolate(), &try_catch)));
                } else
                {
                    wlog("Successfull to init the game engine.");
                    wlog("Script init result is ${s}", ( "s",  v8_helper::ToCString(String::Utf8Value(result)) ));
                }
            }
         }

         Isolate* GetIsolate() { return _isolate; }
      };
   }

   v8_game_engine::v8_game_engine(std::string game_name, graphene::game_plugin::game_plugin& plugin): my(new detail::v8_game_engine_impl(this, plugin))
   {
      my->_game_name = game_name;
      my->init();

      auto game = my->_game_plugin.get_game( game_name );
      //FC_ASSERT( game.valid() );

      auto game_asset = my->_game_plugin.get_game_asset( "DICE" );

      global(game.get_id(), game_asset);
   }

   bool v8_game_engine::global( game_id_type game_id, asset_object game_asset)
   {
       auto isolate = my->GetIsolate();
       v8::Locker locker( isolate );
       Isolate::Scope isolate_scope(my->GetIsolate());
       v8::HandleScope handle_scope( isolate );
       v8::Local<v8::Context> context = v8::Local<v8::Context>::New(my->GetIsolate(), my->_context);

       // Entering the context
       Context::Scope context_scope(context);

       v8::TryCatch try_catch(my->GetIsolate());

       Local<Function> evaluate_func;
       Local<Value>  argv[2] ;

       auto play = context->Global()->Get( String::NewFromUtf8( my->GetIsolate(), "PLAY") );

       auto evaluate = play->ToObject()->Get( String::NewFromUtf8( my->GetIsolate(), "global") );

       if(!evaluate->IsFunction()) {
           FC_CAPTURE_AND_THROW( game_play_failed_compile_script );
       } else {
           evaluate_func = Handle<Function>::Cast(evaluate);
           argv[0] = v8_helper::cpp_to_json(isolate, game_id);
           argv[1] = v8_helper::cpp_to_json( isolate, game_asset );

           Local<Value> result = evaluate_func->Call(context->Global(), 2, argv);

           if ( result.IsEmpty() )
           {
               FC_CAPTURE_AND_THROW(game_play_failed_run_script, (v8_helper::ReportException(my->GetIsolate(), &try_catch)));
           } else
           {
               variant v = v8_helper::json_to_cpp<variant>(isolate, result);
               //wlog("The result of the running of script is ${s}", ( "s",  v) );
               return v.as_bool();
           }
       }
   }

   void v8_game_engine::evaluate( const game_play_operation& op, database& db )
   {
       auto isolate = my->GetIsolate();
       v8::Locker locker( isolate );
       Isolate::Scope isolate_scope(my->GetIsolate());
       v8::HandleScope handle_scope( isolate );
       v8::Local<v8::Context> context = v8::Local<v8::Context>::New(my->GetIsolate(), my->_context);

       // Entering the context
       Context::Scope context_scope(context);

       v8::TryCatch try_catch(my->GetIsolate());

       Local<Function> evaluate_func;
       Local<Value>  argv[3] ;

       auto play = context->Global()->Get( String::NewFromUtf8( my->GetIsolate(), "PLAY") );

       auto evaluate = play->ToObject()->Get( String::NewFromUtf8( my->GetIsolate(), "evaluate") );

       if(!evaluate->IsFunction()) {
           FC_CAPTURE_AND_THROW( game_play_failed_compile_script );
       } else {
           v8_database v8_database(&db);

           evaluate_func = Handle<Function>::Cast(evaluate);
           argv[0] = v8_helper::cpp_to_json(isolate, op);
           argv[1] = v8_database::New( isolate, &v8_database );

           Local<Value> result = evaluate_func->Call(context->Global(), 2, argv);

           //wlog("Start evaluating the game.. with var ${v}", ("v", var));

           if ( result.IsEmpty() )
           {
               FC_CAPTURE_AND_THROW(game_play_failed_run_script, (v8_helper::ReportException(my->GetIsolate(), &try_catch)));
           } else
           {
               auto result_obj = v8_helper::json_to_cpp<variant>( isolate, result );
               //wlog("The result of the running of script is ${s}", ( "s",  result_obj ));
              // TODO check result true or false
           }
       }
   }
   
   void v8_game_engine::apply( const game_play_operation& op, const game_play_object& obj, database& db )
   {
      // TODO:
   }

   void v8_game_engine::execute( const signed_block& block, database& db )
   {
      try {
         //wlog("Start execute in game engine...");
         v8::Locker locker(my->GetIsolate());
         Isolate::Scope isolate_scope(my->GetIsolate());
         v8::HandleScope handle_scope(my->GetIsolate());
         v8::Local<v8::Context> context = v8::Local<v8::Context>::New(my->GetIsolate(), my->_context);
         v8::Context::Scope context_scope(context);
         v8::TryCatch try_catch( my->GetIsolate() );

         Local<Function> execute_func;
         Local<Value>  argv[3] ;

         auto play = context->Global()->Get( String::NewFromUtf8( my->GetIsolate(), "PLAY") );

         auto execute = play->ToObject()->Get( String::NewFromUtf8( my->GetIsolate(), "execute") );

         if(!execute->IsFunction()) {
            FC_CAPTURE_AND_THROW( game_play_failed_compile_script );
         } else {
            execute_func = Handle<Function>::Cast(execute);

            v8_database local_v8_database(&db);

            argv[0] = v8_database::New(my->GetIsolate(), &local_v8_database);
            argv[1] = Integer::New( my->GetIsolate(), block.block_num() );

            // Run the script to get the result.
            // wlog("Run the script to get the result...");
            Local<Value> result = execute_func->Call(context->Global(), 2, argv);

            if ( result.IsEmpty() )
            {
               FC_CAPTURE_AND_THROW(game_play_failed_run_script, ( v8_helper::ReportException( my->GetIsolate(), &try_catch) ));
            } else
            {
               //assert(!try_catch.HasCaught());
               //if (!result->IsUndefined()) {
               // TOOD: return the result
               //}
               auto v = v8_helper::json_to_cpp<variant>(my->GetIsolate(), result);
               // wlog("The result of the running of script is ${s}", ( "s",  v ));
               if ( v.is_numeric() && v.as_int64() == 0 )
               {
                  // wlog("Nothing is done...");
               } else
               {
                  FC_ASSERT( v.is_object() );
                  // TOOD: verify result true or false
               }
            }
         }
         // wlog("End running the script in game engine...");
       }
       catch( const fc::exception& e )
       {
          // wlog( "error executing game contract  ${game_id}\n ${e}", ("game_id", game_id)("e",e.to_detail_string()) );
          /* Get by game, and create if null and modify if exists
          auto game_stat_obj = db.get_game_status( game_id );
          
          
          
          if( !game_stat.valid() ) game_stat = game_status( game_id );
          game_stat->block_number = block_num;
          game_stat->last_error = e;
          pending_state->store_game_status( *game_stat );
         */
       }
   }
} } // bts::graphene
