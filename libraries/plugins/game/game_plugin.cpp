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
#include <graphene/chain/game_object.hpp>
#include <graphene/time/time.hpp>

#include <graphene/utilities/key_conversion.hpp>

#include <fc/smart_ref_impl.hpp>
#include <fc/thread/thread.hpp>

#include <iostream>

namespace graphene { namespace game_plugin {

    namespace detail
    {


        class game_plugin_impl
        {
        public:
            game_plugin_impl(game_plugin& _plugin)
            : _self( _plugin )
            { }
            virtual ~game_plugin_impl();


            /** this method is called as a callback after a block is applied
             *  and the game contract will be executed.
             */
            void execute( const signed_block& b );

            graphene::chain::database& database()
            {
                return _self.database();
            }

            game_plugin& _self;
            flat_set<account_id_type> _tracked_accounts;
        };

        game_plugin_impl::~game_plugin_impl()
        {
            return;
        }

        void game_plugin_impl::execute( const signed_block& b )
        { try {
            /*
            wlog("Start executing in game client at block ${b}", ("b", block_num));
            auto games = blockchain->get_games("", -1);

            for ( const auto& g : games)
            {
                try {
                    auto v8_game_engine = get_v8_engine( g.name );
                    wlog("Start execute the game ${g}", ("g", g.name));
                    v8_game_engine->execute( g.id, blockchain, block_num, pending_state );
                }
                catch (const game_engine_not_found& e)
                {
                    wlog("game engine note found, failed to init for unknown reason during chain execution");
                }
            }
             */
        } FC_CAPTURE_AND_RETHROW( (b) ) }
    } // end namespace detail



}}

using namespace graphene::game_plugin;
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

   database().on_game_execution.connect( [&]( const signed_block& b){ my->execute(b); } );

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
